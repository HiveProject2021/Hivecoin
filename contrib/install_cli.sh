 #!/usr/bin/env bash

 # Execute this file to install the hive cli tools into your path on OS X

 CURRENT_LOC="$( cd "$(dirname "$0")" ; pwd -P )"
 LOCATION=${CURRENT_LOC%Hive-Qt.app*}

 # Ensure that the directory to symlink to exists
 sudo mkdir -p /usr/local/bin

 # Create symlinks to the cli tools
 sudo ln -s ${LOCATION}/Hive-Qt.app/Contents/MacOS/hived /usr/local/bin/hived
 sudo ln -s ${LOCATION}/Hive-Qt.app/Contents/MacOS/hive-cli /usr/local/bin/hive-cli
