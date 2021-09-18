echo 'Please read  https://github.com/HiveProject2021/Hivecoin/blob/main/doc/build-osx.md '
echo 'Run this script under macOS  '

HIVE_ROOT=$(pwd)
# build db4
CFLAGS="-Wno-error=implicit-function-declaration"  ./contrib/install_db4.sh .

BDB_PREFIX="${HIVE_ROOT}/db4"
export BDB_PREFIX='${BDB_PREFIX}'

# Configure Hive Core to use our own-built instance of BDB
cd $HIVE_ROOT
./autogen.sh
./configure BDB_LIBS="-L${BDB_PREFIX}/lib -ldb_cxx-4.8" BDB_CFLAGS="-I${BDB_PREFIX}/include" --enable-cxx --disable-shared --disable-tests --disable-gui-tests
make -j4
make deploy
