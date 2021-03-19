// Copyright (c) 2017-2019 The Raven Core developers
// Copyright (c) 2020-2021 The Hive Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "masternodetypes.h"
#include "hash.h"

int IntFromMasterNodeType(MasterNodeType type) {
    return (int)type;
}

MasterNodeType MasterNodeTypeFromInt(int nType) {
    return (MasterNodeType)nType;
}

uint256 CMasterNodeCacheQualifierAddress::GetHash() {
    return Hash(masternodeName.begin(), masternodeName.end(), address.begin(), address.end());
}

uint256 CMasterNodeCacheRestrictedAddress::GetHash() {
    return Hash(masternodeName.begin(), masternodeName.end(), address.begin(), address.end());
}

uint256 CMasterNodeCacheRootQualifierChecker::GetHash() {
    return Hash(rootMasterNodeName.begin(), rootMasterNodeName.end(), address.begin(), address.end());
}
