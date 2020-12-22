#pragma once

float openSound(float elapsed, float elapsed_sim, int counter, void * ref);
void closeSound();
void playSound(int bufferId, int volumeId);

