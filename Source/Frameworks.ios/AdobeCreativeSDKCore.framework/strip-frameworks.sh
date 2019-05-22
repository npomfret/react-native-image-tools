################################################################################
#
# Copyright 2015 Realm Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
################################################################################

# This script strips all non-valid architectures from dynamic libraries in
# the application's `Frameworks` directory.
#
# The following environment variables are required:
#
# BUILT_PRODUCTS_DIR
# FRAMEWORKS_FOLDER_PATH
# VALID_ARCHS
# EXPANDED_CODE_SIGN_IDENTITY

echo "BUILT_PRODUCTS_DIR: $BUILT_PRODUCTS_DIR"
echo "FRAMEWORKS_FOLDER_PATH: $FRAMEWORKS_FOLDER_PATH"
echo "VALID_ARCHS: $VALID_ARCHS"
echo "EXPANDED_CODE_SIGN_IDENTITY: $EXPANDED_CODE_SIGN_IDENTITY"


# Signs a framework with the provided identity
code_sign() {
  # Use the current code_sign_identitiy
  echo "Code Signing $1 with Identity ${EXPANDED_CODE_SIGN_IDENTITY_NAME}"
  echo "/usr/bin/codesign --force --sign ${EXPANDED_CODE_SIGN_IDENTITY} --preserve-metadata=identifier,entitlements $1"
  /usr/bin/codesign --force --sign ${EXPANDED_CODE_SIGN_IDENTITY} --preserve-metadata=identifier,entitlements "$1"
}

# Set working directory to product’s embedded frameworks 
cd "${BUILT_PRODUCTS_DIR}/${FRAMEWORKS_FOLDER_PATH}"

if [ "$ACTION" = "install" ]; then
  echo "Copy .bcsymbolmap files to .xcarchive"
  find . -name '*.bcsymbolmap' -type f -exec mv {} "${CONFIGURATION_BUILD_DIR}" \;
else
  # Delete *.bcsymbolmap files from framework bundle unless archiving
  find . -name '*.bcsymbolmap' -type f -exec rm -rf "{}" +\;
fi

echo "Stripping frameworks"

for file in $(find . -type f -perm +111); do
  # Skip non-dynamic libraries
  if ! [[ "$(file "$file")" == *"dynamically linked shared library"* ]]; then
    continue
  fi

  FRAMEWORK_DIR=`dirname $file`

  plist_updated=""

  # remove CFBundleSupportedPlatforms from framework
  plistval=$(/usr/libexec/PlistBuddy -c 'print ":CFBundleSupportedPlatforms"' "$FRAMEWORK_DIR/Info.plist" 2>/dev/null || printf '')
  echo "CFBundleSupportedPlatforms for $FRAMEWORK_DIR/Info.plist"
  echo "Bundle Info: $plistval"
  if [[ "$plistval" != "" ]]; then
    /usr/libexec/PlistBuddy -c "Delete :CFBundleSupportedPlatforms" "$FRAMEWORK_DIR/Info.plist"
    echo "Removing CFBundleSupportedPlatforms from framework plist: $FRAMEWORK_DIR/Info.plist"
    plist_updated="true"
  fi

  # remove CFBundleSupportedPlatforms from all embedded bundle plists
  for BUNDLE in $(find "$FRAMEWORK_DIR" -name '*.bundle' -type d); do
    for BUNDLE_PLIST in $(find "$BUNDLE" -name 'Info.plist' -type f -d 1); do
      plistval=$(/usr/libexec/PlistBuddy -c 'print ":CFBundleSupportedPlatforms"' "$BUNDLE_PLIST" 2>/dev/null || printf '')
      echo "CFBundleSupportedPlatforms for $FRAMEWORK_DIR/Info.plist"
      echo "Bundle Info: $plistval"
      if [[ "$plistval" != "" ]]; then
        /usr/libexec/PlistBuddy -c "Delete :CFBundleSupportedPlatforms" "$BUNDLE_PLIST"
        echo "Removing CFBundleSupportedPlatforms from embedded bundle plist: $BUNDLE_PLIST"
        plist_updated="true"
      fi
    done
  done

  # Get architectures for current file
  echo "Current architectures for file: ${file}"
  lipo -info "${file}"
  archs="$(lipo -info "${file}" | rev | cut -d ':' -f1 | rev)"
  stripped=""
  for arch in $archs; do
    if ! [[ "${VALID_ARCHS}" == *"$arch"* ]]; then
      # Strip non-valid architectures in-place
      lipo -remove "$arch" -output "$file" "$file" || exit 1
      stripped="$stripped $arch"
    fi
  done
  if [[ "$stripped" != "" || "$plist_updated" != "" ]]; then
    echo "Stripped $file of architectures:$stripped"
    if [ "${CODE_SIGNING_REQUIRED}" == "YES" ]; then
      code_sign "${file}"
    fi
  fi
done
