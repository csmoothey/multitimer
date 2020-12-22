#include <stdio.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include "soundcore.h"
#include "XPLMUtilities.h"
#include "volumeid.h"
#include "shared.h"
#include "soundid.h"

#define RIFF_ID 0x46464952			// 'RIFF'
#define FMT_ID  0x20746D66			// 'FMT '
#define DATA_ID 0x61746164			// 'DATA'
#define SWAP_32(value)                 \
        (((((unsigned short)value)<<8) & 0xFF00)   | \
         ((((unsigned short)value)>>8) & 0x00FF))
#define SWAP_16(value)                     \
        (((((unsigned int)value)<<24) & 0xFF000000)  | \
         ((((unsigned int)value)<< 8) & 0x00FF0000)  | \
         ((((unsigned int)value)>> 8) & 0x0000FF00)  | \
         ((((unsigned int)value)>>24) & 0x000000FF))
#define FAIL(X) { XPLMDebugString(X); free(mem); return 0; }

typedef struct {
	short		format;				// PCM = 1, not sure what other values are legal.
	short		num_channels;
	int			sample_rate;
	int			byte_rate;
	short		block_align;
	short		bits_per_sample;
} format_info;

typedef struct {
	int			id;
	int			size;
} chunk_header;

static ALCdevice *	sndDev = NULL;
static ALCcontext * sndCtx = NULL;
static ALuint sndSrc = 0;
static ALuint	sndBuffers[5] = {0, 0, 0, 0, 0};

static char * find_chunk(char * file_begin, char * file_end, int desired_id, int swapped)
{
	while (file_begin < file_end)
	{
		chunk_header * h = (chunk_header *)file_begin;
		if (h->id == desired_id && !swapped)
			return file_begin + sizeof(chunk_header);
		if (h->id == SWAP_32(desired_id) && swapped)
			return file_begin + sizeof(chunk_header);
		int chunk_size = swapped ? SWAP_32(h->size) : h->size;
		char * next = file_begin + chunk_size + sizeof(chunk_header);
		if (next > file_end || next <= file_begin)
			return NULL;
		file_begin = next;
	}
	return NULL;
}

// Given a chunk, find its end by going back to the header.
static char * chunk_end(char * chunk_start, int swapped)
{
	chunk_header * h = (chunk_header *)(chunk_start - sizeof(chunk_header));
	return chunk_start + (swapped ? SWAP_32(h->size) : h->size);
}


static ALuint loadWave(const char * file_name)
{
	// First: we open the file and copy it into a single large memory buffer for processing.

	FILE * fi = fopen(file_name, "rb");
	if (fi == NULL)
	{
		XPLMDebugString("WAVE file load failed - could not open.\n");
		return 0;
	}
	fseek(fi, 0, SEEK_END);
	int file_size = ftell(fi);
	fseek(fi, 0, SEEK_SET);
	char * mem = (char*)malloc(file_size);
	if (mem == NULL)
	{
		XPLMDebugString("WAVE file load failed - could not allocate memory.\n");
		fclose(fi);
		return 0;
	}
	if (fread(mem, 1, file_size, fi) != file_size)
	{
		XPLMDebugString("WAVE file load failed - could not read file.\n");
		free(mem);
		fclose(fi);
		return 0;
	}
	fclose(fi);
	char * mem_end = mem + file_size;

	// Second: find the RIFF chunk.  Note that by searching for RIFF both normal
	// and reversed, we can automatically determine the endian swap situation for
	// this file regardless of what machine we are on.

	int swapped = 0;
	char * riff = find_chunk(mem, mem_end, RIFF_ID, 0);
	if (riff == NULL)
	{
		riff = find_chunk(mem, mem_end, RIFF_ID, 1);
		if (riff)
			swapped = 1;
		else
			FAIL("Could not find RIFF chunk in wave file.\n")
	}

	// The wave chunk isn't really a chunk at all. :-(  It's just a "WAVE" tag 
	// followed by more chunks.  This strikes me as totally inconsistent, but
	// anyway, confirm the WAVE ID and move on.

	if (riff[0] != 'W' ||
		riff[1] != 'A' ||
		riff[2] != 'V' ||
		riff[3] != 'E')
		FAIL("Could not find WAVE signature in wave file.\n")

		char * format = find_chunk(riff + 4, chunk_end(riff, swapped), FMT_ID, swapped);
	if (format == NULL)
		FAIL("Could not find FMT  chunk in wave file.\n")

		// Find the format chunk, and swap the values if needed.  This gives us our real format.

		format_info * fmt = (format_info *)format;
	if (swapped)
	{
		fmt->format = SWAP_16(fmt->format);
		fmt->num_channels = SWAP_16(fmt->num_channels);
		fmt->sample_rate = SWAP_32(fmt->sample_rate);
		fmt->byte_rate = SWAP_32(fmt->byte_rate);
		fmt->block_align = SWAP_16(fmt->block_align);
		fmt->bits_per_sample = SWAP_16(fmt->bits_per_sample);
	}

	// Reject things we don't understand...expand this code to support weirder audio formats.

	if (fmt->format != 1) FAIL("Wave file is not PCM format data.\n")
		if (fmt->num_channels != 1 && fmt->num_channels != 2) FAIL("Must have mono or stereo sound.\n")
			if (fmt->bits_per_sample != 8 && fmt->bits_per_sample != 16) FAIL("Must have 8 or 16 bit sounds.\n")
				char * data = find_chunk(riff + 4, chunk_end(riff, swapped), DATA_ID, swapped);
	if (data == NULL)
		FAIL("I could not find the DATA chunk.\n")

		int sample_size = fmt->num_channels * fmt->bits_per_sample / 8;
	int data_bytes = (int)(chunk_end(data, swapped) - data);
	int data_samples = data_bytes / sample_size;

	// If the file is swapped and we have 16-bit audio, we need to endian-swap the audio too or we'll 
	// get something that sounds just astoundingly bad!

	if (fmt->bits_per_sample == 16 && swapped)
	{
		short * ptr = (short *)data;
		int words = data_samples * fmt->num_channels;
		while (words--)
		{
			*ptr = SWAP_16(*ptr);
			++ptr;
		}
	}

	// Finally, the OpenAL crud.  Build a new OpenAL buffer and send the data to OpenAL, passing in
	// OpenAL format enums based on the format chunk.

	ALuint buf_id = 0;
	alGenBuffers(1, &buf_id);
	if (buf_id == 0) FAIL("Could not generate buffer id.\n");

	alBufferData(buf_id, fmt->bits_per_sample == 16 ?
		(fmt->num_channels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16) :
		(fmt->num_channels == 2 ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8),
		data, data_bytes, fmt->sample_rate);
	free(mem);
	return buf_id;
}

float openSound(float elapsed, float elapsed_sim, int counter, void * ref) {
	// Open the sound device.
	sndDev = alcOpenDevice(NULL);
	if (sndDev == NULL) {
		XPLMDebugString("MultiTimer: Error opening sound device");
		return 0.0f;
	}

	// Create the context.
	ALCcontext * prevSndCtx = alcGetCurrentContext();
	sndCtx = alcCreateContext(sndDev, NULL);
	if (sndCtx == NULL) {
		XPLMDebugString("MultiTimer: Error creating sound context");
		if (prevSndCtx != NULL) {
			alcMakeContextCurrent(prevSndCtx);
		}
		alcCloseDevice(sndDev);
		return 0.0f;
	}
	alcMakeContextCurrent(sndCtx);
	alGetError();
	alGenSources(1, &sndSrc);
	ALuint e = alGetError();
	if (e != AL_NO_ERROR) {
		char charbuf[64];
		sprintf(charbuf, "Multitimer: Error creating source = %d\n", e);
		XPLMDebugString(charbuf);
		if (prevSndCtx != NULL) {
			alcMakeContextCurrent(prevSndCtx);
		}
		alcDestroyContext(sndCtx);
		alcCloseDevice(sndDev);
		return 0.0f;
	}
	alSourcef(sndSrc, AL_PITCH, 1.0f);
	alSourcei(sndSrc, AL_LOOPING, 0);
	ALfloat    zero[3] = { 0 };
	alSourcefv(sndSrc, AL_POSITION, zero);
	alSourcefv(sndSrc, AL_VELOCITY, zero);

	char pluginDir[2048];
	getPluginDir(pluginDir);
	char buf[2048];

	strcpy(buf, pluginDir);
	strcat(buf, "sixtyseconds.wav");
	sndBuffers[SOUND_ID_60_SECONDS] = loadWave(buf);

	strcpy(buf, pluginDir);
	strcat(buf, "thirtyseconds.wav");
	sndBuffers[SOUND_ID_30_SECONDS] = loadWave(buf);

	strcpy(buf, pluginDir);
	strcat(buf, "fifteenseconds.wav");
	sndBuffers[SOUND_ID_15_SECONDS] = loadWave(buf);

	strcpy(buf, pluginDir);
	strcat(buf, "fiveseconds.wav");
	sndBuffers[SOUND_ID_5_SECONDS] = loadWave(buf);

	strcpy(buf, pluginDir);
	strcat(buf, "mark.wav");
	sndBuffers[SOUND_ID_MARK] = loadWave(buf);
	if (prevSndCtx != NULL) {		alcMakeContextCurrent(prevSndCtx);	}	return 0.0f;}

void closeSound() {
	if (sndSrc) {
		alDeleteSources(1, &sndSrc);
	}
	if (sndCtx != NULL) {
		alcDestroyContext(sndCtx);
	}
	for (int i = 0; i < (sizeof(sndBuffers) / sizeof(sndBuffers[0])); i++) {
		if (sndBuffers[i]) {
			alDeleteBuffers(1, &sndBuffers[i]);
		}
	}
	if (sndDev != NULL) {
		alcCloseDevice(sndDev);
	}
}

void playSound(int bufferId, int volumeId) {
	if (volumeId == VOL_ID_OFF || sndSrc == 0 || sndBuffers[bufferId] == 0) {
		return;
	}
	ALCcontext * prevSndCtx = alcGetCurrentContext();
	alcMakeContextCurrent(sndCtx);
	alSourcei(sndSrc, AL_BUFFER, sndBuffers[bufferId]);
	float volf[3] = { 1.0f / (2.5f * 2.5f), 1.0f / 2.5f, 1.0f};
	alSourcef(sndSrc, AL_GAIN, volf[volumeId-1]);
	ALint srcState = AL_PLAYING;
	alGetSourcei(sndSrc, AL_SOURCE_STATE, &srcState);
	if (srcState != AL_PLAYING) {
		alGetError();
		alSourcePlay(sndSrc);
		ALuint e = alGetError();
		if (e != AL_NO_ERROR) {
			char charbuf[64];
			sprintf(charbuf, "Multitimer: Error playing sound = %d\n", e);
			XPLMDebugString(charbuf);
		}
	}
	if (prevSndCtx) {
		alcMakeContextCurrent(prevSndCtx);
	}
}
