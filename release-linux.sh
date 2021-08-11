VERSION=4.3.2.5
rm -rf ./release-linux
mkdir release-linux

cp ./src/hived ./release-linux/
cp ./src/hive-cli ./release-linux/
cp ./src/qt/hive-qt ./release-linux/

cd ./release-linux/
strip hived
strip hive-cli
strip hive-qt
cd ../

zip -r hive-$VERSION-x86_64-linux-gnu.zip ./release-linux/
