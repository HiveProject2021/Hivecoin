// Copyright (c) 2017-2019 The Raven Core developers
// Copyright (c) 2020-2021 The Hive Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVE_MASTERNODEDB_H
#define HIVE_MASTERNODEDB_H

#include "fs.h"
#include "serialize.h"

#include <string>
#include <map>
#include <dbwrapper.h>

const int8_t MASTERNODE_UNDO_INCLUDES_VERIFIER_STRING = -1;

class CNewMasterNode;
class uint256;
class COutPoint;
class CDatabasedMasterNodeData;

struct CBlockMasterNodeUndo
{
    bool fChangedIPFS;
    bool fChangedUnits;
    std::string strIPFS;
    int32_t nUnits;
    int8_t version;
    bool fChangedVerifierString;
    std::string verifierString;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(fChangedUnits);
        READWRITE(fChangedIPFS);
        READWRITE(strIPFS);
        READWRITE(nUnits);
        if (ser_action.ForRead()) {
            if (!s.empty() and s.size() >= 1) {
                int8_t nVersionCheck;
                ::Unserialize(s, nVersionCheck);

                if (nVersionCheck == MASTERNODE_UNDO_INCLUDES_VERIFIER_STRING) {
                    ::Unserialize(s, fChangedVerifierString);
                    ::Unserialize(s, verifierString);
                }
                version = nVersionCheck;
            }
        } else {
            ::Serialize(s, MASTERNODE_UNDO_INCLUDES_VERIFIER_STRING);
            ::Serialize(s, fChangedVerifierString);
            ::Serialize(s, verifierString);
        }
    }
};

/** Access to the block database (blocks/index/) */
class CMasterNodesDB : public CDBWrapper
{
public:
    explicit CMasterNodesDB(size_t nCacheSize, bool fMemory = false, bool fWipe = false);

    CMasterNodesDB(const CMasterNodesDB&) = delete;
    CMasterNodesDB& operator=(const CMasterNodesDB&) = delete;

    // Write to database functions
    bool WriteMasterNodeData(const CNewMasterNode& masternode, const int nHeight, const uint256& blockHash);
    bool WriteMasterNodeAddressQuantity(const std::string& masternodeName, const std::string& address, const CAmount& quantity);
    bool WriteAddressMasterNodeQuantity(const std::string& address, const std::string& masternodeName, const CAmount& quantity);
    bool WriteBlockUndoMasterNodeData(const uint256& blockhash, const std::vector<std::pair<std::string, CBlockMasterNodeUndo> >& masternodeUndoData);
    bool WriteUpdatedMempoolState();

    // Read from database functions
    bool ReadMasterNodeData(const std::string& strName, CNewMasterNode& masternode, int& nHeight, uint256& blockHash);
    bool ReadMasterNodeAddressQuantity(const std::string& masternodeName, const std::string& address, CAmount& quantity);
    bool ReadAddressMasterNodeQuantity(const std::string& address, const std::string& masternodeName, CAmount& quantity);
    bool ReadBlockUndoMasterNodeData(const uint256& blockhash, std::vector<std::pair<std::string, CBlockMasterNodeUndo> >& masternodeUndoData);
    bool ReadUpdatedMempoolState();

    // Erase from database functions
    bool EraseMasterNodeData(const std::string& masternodeName);
    bool EraseMyMasterNodeData(const std::string& masternodeName);
    bool EraseMasterNodeAddressQuantity(const std::string &masternodeName, const std::string &address);
    bool EraseAddressMasterNodeQuantity(const std::string &address, const std::string &masternodeName);

    // Helper functions
    bool LoadMasterNodes();
    bool MasterNodeDir(std::vector<CDatabasedMasterNodeData>& masternodes, const std::string filter, const size_t count, const long start);
    bool MasterNodeDir(std::vector<CDatabasedMasterNodeData>& masternodes);

    bool AddressDir(std::vector<std::pair<std::string, CAmount> >& vecMasterNodeAmount, int& totalEntries, const bool& fGetTotal, const std::string& address, const size_t count, const long start);
    bool MasterNodeAddressDir(std::vector<std::pair<std::string, CAmount> >& vecAddressAmount, int& totalEntries, const bool& fGetTotal, const std::string& masternodeName, const size_t count, const long start);
};


#endif //HIVE_MASTERNODEDB_H
