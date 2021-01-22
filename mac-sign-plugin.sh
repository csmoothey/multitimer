#!/bin/bash
#
# USAGE: ./sign_plugin.sh <path_to_dylib.xpl> <bundle id>
#
# The path to your plugin is a full file path to the .xpl file on disk, unzipped and ready to run.
#
# The bundle ID is a company bundle ID, e.g. com.acme.epic-plugin-5.  The final component on the
# bundle ID can basically be unique and made up, e.g. it does not have to match anything INSIDE
# your dylib because there's no plist for a dylib.
#
# To use this script you must have a valid certificate with public/private key pair for app signing
# for your copmany that is not expired, and dev tools from 10.14 or 10.15.  
#
#------- CODE SIGNING AND NOTARIZATION UTILS FOR MAC --------

die() {
	echo $1
	exit 1
}

# CONFIGURATION - YOU NEED TO FILL THIS STUFF IN!

# This should be the name of your app distribution certifiate, e.g.
# "Developer ID Application: Acme Co"
developer_id=
# This should be the name of the login keychain password item that
# contains the "app password" you got from Apple.  The name here
# doesn't have to be codesigning, it's the item name you pick when you
# stash the passwod
app_password=@keychain:codesigning
# This is your Apple ID you log in with, e.g. it might be your email address
username=

# notarize <plugin path> <bundle id>
code_sign() {
	echo "Code signing $1"
	local plg_path=$1
	local bundle_id=$2
	local tool_results="$plg_path".txt
	local plg_path_zip="$plg_path".zip
	rm -rf "$plg_path_zip"
	codesign --force --timestamp --options=runtime --deep \
		-s "$developer_id" \
		"$plg_path"
	zip -rq --symlink "$plg_path_zip" "$plg_path"
	echo xcrun altool --notarize-app --file "$plg_path_zip" --username $username --password $app_password --primary-bundle-id $bundle_id
	xcrun altool --notarize-app --file "$plg_path_zip" --username $username --password $app_password --primary-bundle-id $bundle_id --output-format xml > "$tool_results"
	rm -rf "$plg_path_zip"
	uuid=`/usr/libexec/PlistBuddy -c "Print :notarization-upload:RequestUUID" "$tool_results"`
	rm "$tool_results"
}

#wait_code_sign <plugin_path> <uuid>
wait_code_sign() {
	local uuid=$2
	local plg_path=$1
	local plist="$plg_path".plist
	echo "Checking notarization of $plg_path - $uuid"
	echo xcrun altool --notarization-info $uuid --username $username --password $app_password
	while true; do
		xcrun altool --notarization-info $uuid --username $username --password $app_password --output-format xml > "$plist" || die "could not get notarization info"
		echo `/usr/libexec/PlistBuddy -c "Print :notarization-info:Status" "$plist"`
    	if [ "`/usr/libexec/PlistBuddy -c "Print :notarization-info:Status" "$plist"`" != "in progress" ]; then
      		break
    	fi
		sleep 30
	done

	echo `/usr/libexec/PlistBuddy -c "Print :notarization-info:LogFileURL" "$plist"`
	rm "$plist"
}


#---------------------- MAIN SCRIPT --------------------------

if [ $# -ne 2 ]; then
	die "Usage: $0 <plugin path> <bundle ID>"
fi

plugin_path=$1
bundle_id=$2

# notarize binaries
code_sign "$plugin_path" $bundle_id
uuid_p=$uuid
echo "Will wait 20 seconds before polling Apple for notarization"
sleep 20
wait_code_sign "$plugin_path" $uuid_p
