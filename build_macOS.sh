#!/bin/bash

set -o errexit -o nounset

# Hold on to current directory
project_dir=$(pwd)

# Output macOS version
sw_vers

# Update platform
echo "Updating platform..."
brew update

# Install p7zip for packaging
brew install p7zip

# Install npm appdmg if you want to create custom dmg files with it
# npm install -g appdmg

# Install Qt
echo "Installing Qt..."
brew install qt

# Add Qt binaries to path
PATH=/usr/local/opt/qt/bin/:${PATH}

# Build your app
echo "Building YourApp..."
cd ${project_dir}
qmake -config release
make

# Build and run your tests here

# Package your app
echo "Packaging YourApp..."
cd ${project_dir}
find avlibs/bin -name \*.dylib -exec cp {} Re-play.app/Contents/Resources \;

echo "Creating dmg archive..."
macdeployqt Re-play.app -qmldir=ui -dmg
mv Re-play.dmg "Re-play.macos.x64.portable.dmg"

# You can use the appdmg command line app to create your dmg file if
# you want to use a custom background and icon arrangement. I'm still
# working on this for my apps, myself. If you want to do this, you'll
# remove the -dmg option above.
# appdmg json-path YourApp_${TRAVIS_TAG}.dmg

echo "Packaging zip archive..."
7z a Re-play.macos.x64.portable.zip "Re-play.macos.x64.portable.dmg"

echo "Done!"

exit 0