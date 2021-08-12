VERSION=4.3.2.5
rm -rf ./release-linux
mkdir release-linux

cp ./src/hived ./release-linux/
cp ./src/hive-cli ./release-linux/
cp ./src/qt/hive-qt ./release-linux/
cp ./Hivecoin_small.png ./release-linux/

cd ./release-linux/
strip hived
strip hive-cli
strip hive-qt

# prepare for packaging deb file.

mkdir hivecoin$VERSION
cd hivecoin$VERSION
mkdir -p DEBIAN
echo 'Package: hivecoin
Version: '$VERSION'
Section: base 
Priority: optional 
Architecture: all 
Depends:
Maintainer: HiveProject2021
Description: Hive coin wallet and service.
' > ./DEBIAN/control
mkdir -p ./usr/local/bin/
mv ../hived ./usr/local/bin/
mv ../hive-cli ./usr/local/bin/
mv ../hive-qt ./usr/local/bin/

# prepare for desktop shortcut
mkdir -p ./usr/share/icons/
mv ../Hivecoin_small.png ./usr/share/icons/
mkdir -p ./usr/share/applications/
echo '
#!/usr/bin/env xdg-open

[Desktop Entry]
Version=1.0
Type=Application
Terminal=false
Exec=/usr/local/bin/hive-qt
Name=hivecoin
Comment= hive coin wallet
Icon=/usr/share/icons/Hivecoin_small.png
' > ./usr/share/applications/hivecoin.desktop

cd ../
# build deb file.
dpkg-deb --build hivecoin$VERSION
