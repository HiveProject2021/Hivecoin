#!/usr/bin/env bash

OS=${1}
GITHUB_WORKSPACE=${2}
GITHUB_REF=${3}

if [[ ! ${OS} || ! ${GITHUB_WORKSPACE} ]]; then
    echo "Error: Invalid options"
    echo "Usage: ${0} <operating system> <github workspace path>"
    exit 1
fi
echo "----------------------------------------"
echo "OS: ${OS}"
echo "----------------------------------------"

if [[ ${OS} == "arm32v7-disable-wallet" || ${OS} == "linux-disable-wallet" ]]; then
    OS=`echo ${OS} | cut -d"-" -f1`
fi

if [[ ${GITHUB_REF} =~ "release" ]]; then
    echo "----------------------------------------"
    echo "Building Dependencies for ${OS}"
    echo "----------------------------------------"

    cd depends
    if [[ ${OS} == "windows" ]]; then
        make HOST=x86_64-w64-mingw32 -j2
    elif [[ ${OS} == "osx" ]]; then
        cd ${GITHUB_WORKSPACE}
        curl -O https://hive-build-resources.s3.amazonaws.com/osx/MacOSX10.11.sdk.tar.gz
        mkdir -p ${GITHUB_WORKSPACE}/depends/SDKs
        cd ${GITHUB_WORKSPACE}/depends/SDKs && tar -zxf ${GITHUB_WORKSPACE}/MacOSX10.11.sdk.tar.gz
        cd ${GITHUB_WORKSPACE}/depends && make HOST=x86_64-apple-darwin14 -j2
    elif [[ ${OS} == "linux" || ${OS} == "linux-disable-wallet" ]]; then
        make HOST=x86_64-linux-gnu -j2
    elif [[ ${OS} == "arm32v7" || ${OS} == "arm32v7-disable-wallet" ]]; then
        make HOST=arm-linux-gnueabihf -j2
    fi
else
    echo "----------------------------------------"
    echo "Retrieving Dependencies for ${OS}"
    echo "----------------------------------------"

    cd /tmp
    curl -O https://hive-build-resources.s3.amazonaws.com/${OS}/hive-${OS}-dependencies.tar.gz
    curl -O https://hive-build-resources.s3.amazonaws.com/${OS}/SHASUM
    if [[ $(sha256sum -c /tmp/SHASUM) ]]; then
        cd ${GITHUB_WORKSPACE}/depends
        tar zxvf /tmp/hive-${OS}-dependencies.tar.gz
    else
        echo "SHASUM doesn't match"
        exit 1
    fi
fi
