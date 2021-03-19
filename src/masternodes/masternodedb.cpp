// Copyright (c) 2017-2019 The Raven Core developers
// Copyright (c) 2020-2021 The Hive Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <util.h>
#include <consensus/params.h>
#include <script/ismine.h>
#include <tinyformat.h>
#include "masternodedb.h"
#include "masternodes.h"
#include "validation.h"

#include <boost/thread.hpp>

static const char MASTERNODE_FLAG = 'A';
static const char MASTERNODE_ADDRESS_QUANTITY_FLAG = 'B';
static const char ADDRESS_MASTERNODE_QUANTITY_FLAG = 'C';
static const char MY_MASTERNODE_FLAG = 'M';
static const char BLOCK_MASTERNODE_UNDO_DATA = 'U';
static const char MEMPOOL_UPDATED_TX = 'Z';

static size_t MAX_DATABASE_RESULTS = 50000;

CMasterNodesDB::CMasterNodesDB(size_t nCacheSize, bool fMemory, bool fWipe) : CDBWrapper(GetDataDir() / "masternodes", nCacheSize, fMemory, fWipe) {
}

bool CMasterNodesDB::WriteMasterNodeData(const CNewMasterNode &masternode, const int nHeight, const uint256& blockHash)
{
    CDatabasedMasterNodeData data(masternode, nHeight, blockHash);
    return Write(std::make_pair(MASTERNODE_FLAG, masternode.strName), data);
}

bool CMasterNodesDB::WriteMasterNodeAddressQuantity(const std::string &masternodeName, const std::string &address, const CAmount &quantity)
{
    return Write(std::make_pair(MASTERNODE_ADDRESS_QUANTITY_FLAG, std::make_pair(masternodeName, address)), quantity);
}

bool CMasterNodesDB::WriteAddressMasterNodeQuantity(const std::string &address, const std::string &masternodeName, const CAmount& quantity) {
    return Write(std::make_pair(ADDRESS_MASTERNODE_QUANTITY_FLAG, std::make_pair(address, masternodeName)), quantity);
}

bool CMasterNodesDB::ReadMasterNodeData(const std::string& strName, CNewMasterNode& masternode, int& nHeight, uint256& blockHash)
{

    CDatabasedMasterNodeData data;
    bool ret =  Read(std::make_pair(MASTERNODE_FLAG, strName), data);

    if (ret) {
        masternode = data.masternode;
        nHeight = data.nHeight;
        blockHash = data.blockHash;
    }

    return ret;
}

bool CMasterNodesDB::ReadMasterNodeAddressQuantity(const std::string& masternodeName, const std::string& address, CAmount& quantity)
{
    return Read(std::make_pair(MASTERNODE_ADDRESS_QUANTITY_FLAG, std::make_pair(masternodeName, address)), quantity);
}

bool CMasterNodesDB::ReadAddressMasterNodeQuantity(const std::string &address, const std::string &masternodeName, CAmount& quantity) {
    return Read(std::make_pair(ADDRESS_MASTERNODE_QUANTITY_FLAG, std::make_pair(address, masternodeName)), quantity);
}

bool CMasterNodesDB::EraseMasterNodeData(const std::string& masternodeName)
{
    return Erase(std::make_pair(MASTERNODE_FLAG, masternodeName));
}

bool CMasterNodesDB::EraseMyMasterNodeData(const std::string& masternodeName)
{
    return Erase(std::make_pair(MY_MASTERNODE_FLAG, masternodeName));
}

bool CMasterNodesDB::EraseMasterNodeAddressQuantity(const std::string &masternodeName, const std::string &address) {
    return Erase(std::make_pair(MASTERNODE_ADDRESS_QUANTITY_FLAG, std::make_pair(masternodeName, address)));
}

bool CMasterNodesDB::EraseAddressMasterNodeQuantity(const std::string &address, const std::string &masternodeName) {
    return Erase(std::make_pair(ADDRESS_MASTERNODE_QUANTITY_FLAG, std::make_pair(address, masternodeName)));
}

bool EraseAddressMasterNodeQuantity(const std::string &address, const std::string &masternodeName);

bool CMasterNodesDB::WriteBlockUndoMasterNodeData(const uint256& blockhash, const std::vector<std::pair<std::string, CBlockMasterNodeUndo> >& masternodeUndoData)
{
    return Write(std::make_pair(BLOCK_MASTERNODE_UNDO_DATA, blockhash), masternodeUndoData);
}

bool CMasterNodesDB::ReadBlockUndoMasterNodeData(const uint256 &blockhash, std::vector<std::pair<std::string, CBlockMasterNodeUndo> > &masternodeUndoData)
{
    // If it exists, return the read value.
    if (Exists(std::make_pair(BLOCK_MASTERNODE_UNDO_DATA, blockhash)))
           return Read(std::make_pair(BLOCK_MASTERNODE_UNDO_DATA, blockhash), masternodeUndoData);

    // If it doesn't exist, we just return true because we don't want to fail just because it didn't exist in the db
    return true;
}

bool CMasterNodesDB::WriteUpdatedMempoolState()
{
    return Write(MEMPOOL_UPDATED_TX, mapUpdatedMasterNodes);
}

bool CMasterNodesDB::ReadUpdatedMempoolState()
{
    mapUpdatedMasterNodes.clear();
    mapUpdatedTx.clear();
    // If it exists, return the read value.
    bool rv = Read(MEMPOOL_UPDATED_TX, mapUpdatedMasterNodes);
    if (rv) {
        for (auto pair : mapUpdatedMasterNodes)
            mapUpdatedTx.insert(std::make_pair(pair.second, pair.first));
    }
    return rv;
}

bool CMasterNodesDB::LoadMasterNodes()
{
    std::unique_ptr<CDBIterator> pcursor(NewIterator());

    pcursor->Seek(std::make_pair(MASTERNODE_FLAG, std::string()));

    // Load masternodes
    while (pcursor->Valid()) {
        boost::this_thread::interruption_point();
        std::pair<char, std::string> key;
        if (pcursor->GetKey(key) && key.first == MASTERNODE_FLAG) {
            CDatabasedMasterNodeData data;
            if (pcursor->GetValue(data)) {
                pmasternodesCache->Put(data.masternode.strName, data);
                pcursor->Next();

                // Loaded enough from database to have in memory.
                // No need to load everything if it is just going to be removed from the cache
                if (pmasternodesCache->Size() == (pmasternodesCache->MaxSize() / 2))
                    break;
            } else {
                return error("%s: failed to read masternode", __func__);
            }
        } else {
            break;
        }
    }


    if (fMasterNodeIndex) {
        std::unique_ptr<CDBIterator> pcursor3(NewIterator());
        pcursor3->Seek(std::make_pair(MASTERNODE_ADDRESS_QUANTITY_FLAG, std::make_pair(std::string(), std::string())));

        // Load mapMasterNodeAddressAmount
        while (pcursor3->Valid()) {
            boost::this_thread::interruption_point();
            std::pair<char, std::pair<std::string, std::string> > key; // <MasterNode Name, Address> -> Quantity
            if (pcursor3->GetKey(key) && key.first == MASTERNODE_ADDRESS_QUANTITY_FLAG) {
                CAmount value;
                if (pcursor3->GetValue(value)) {
                    pmasternodes->mapMasterNodesAddressAmount.insert(
                            std::make_pair(std::make_pair(key.second.first, key.second.second), value));
                    if (pmasternodes->mapMasterNodesAddressAmount.size() > MAX_CACHE_MASTERNODES_SIZE)
                        break;
                    pcursor3->Next();
                } else {
                    return error("%s: failed to read my address quantity from database", __func__);
                }
            } else {
                break;
            }
        }
    }

    return true;
}

bool CMasterNodesDB::MasterNodeDir(std::vector<CDatabasedMasterNodeData>& masternodes, const std::string filter, const size_t count, const long start)
{
    FlushStateToDisk();

    std::unique_ptr<CDBIterator> pcursor(NewIterator());
    pcursor->Seek(std::make_pair(MASTERNODE_FLAG, std::string()));

    auto prefix = filter;
    bool wildcard = prefix.back() == '*';
    if (wildcard)
        prefix.pop_back();

    size_t skip = 0;
    if (start >= 0) {
        skip = start;
    }
    else {
        // compute table size for backwards offset
        long table_size = 0;
        while (pcursor->Valid()) {
            boost::this_thread::interruption_point();

            std::pair<char, std::string> key;
            if (pcursor->GetKey(key) && key.first == MASTERNODE_FLAG) {
                if (prefix == "" ||
                    (wildcard && key.second.find(prefix) == 0) ||
                    (!wildcard && key.second == prefix)) {
                    table_size += 1;
                }
            }
            pcursor->Next();
        }
        skip = table_size + start;
        pcursor->SeekToFirst();
    }


    size_t loaded = 0;
    size_t offset = 0;

    // Load masternodes
    while (pcursor->Valid() && loaded < count) {
        boost::this_thread::interruption_point();

        std::pair<char, std::string> key;
        if (pcursor->GetKey(key) && key.first == MASTERNODE_FLAG) {
            if (prefix == "" ||
                    (wildcard && key.second.find(prefix) == 0) ||
                    (!wildcard && key.second == prefix)) {
                if (offset < skip) {
                    offset += 1;
                }
                else {
                    CDatabasedMasterNodeData data;
                    if (pcursor->GetValue(data)) {
                        masternodes.push_back(data);
                        loaded += 1;
                    } else {
                        return error("%s: failed to read masternode", __func__);
                    }
                }
            }
            pcursor->Next();
        } else {
            break;
        }
    }

    return true;
}

bool CMasterNodesDB::AddressDir(std::vector<std::pair<std::string, CAmount> >& vecMasterNodeAmount, int& totalEntries, const bool& fGetTotal, const std::string& address, const size_t count, const long start)
{
    FlushStateToDisk();

    std::unique_ptr<CDBIterator> pcursor(NewIterator());
    pcursor->Seek(std::make_pair(ADDRESS_MASTERNODE_QUANTITY_FLAG, std::make_pair(address, std::string())));

    if (fGetTotal) {
        totalEntries = 0;
        while (pcursor->Valid()) {
            boost::this_thread::interruption_point();

            std::pair<char, std::pair<std::string, std::string> > key;
            if (pcursor->GetKey(key) && key.first == ADDRESS_MASTERNODE_QUANTITY_FLAG && key.second.first == address) {
                totalEntries++;
            }
            pcursor->Next();
        }
        return true;
    }

    size_t skip = 0;
    if (start >= 0) {
        skip = start;
    }
    else {
        // compute table size for backwards offset
        long table_size = 0;
        while (pcursor->Valid()) {
            boost::this_thread::interruption_point();

            std::pair<char, std::pair<std::string, std::string> > key;
            if (pcursor->GetKey(key) && key.first == ADDRESS_MASTERNODE_QUANTITY_FLAG && key.second.first == address) {
                table_size += 1;
            }
            pcursor->Next();
        }
        skip = table_size + start;
        pcursor->SeekToFirst();
    }


    size_t loaded = 0;
    size_t offset = 0;

    // Load masternodes
    while (pcursor->Valid() && loaded < count && loaded < MAX_DATABASE_RESULTS) {
        boost::this_thread::interruption_point();

        std::pair<char, std::pair<std::string, std::string> > key;
        if (pcursor->GetKey(key) && key.first == ADDRESS_MASTERNODE_QUANTITY_FLAG && key.second.first == address) {
                if (offset < skip) {
                    offset += 1;
                }
                else {
                    CAmount amount;
                    if (pcursor->GetValue(amount)) {
                        vecMasterNodeAmount.emplace_back(std::make_pair(key.second.second, amount));
                        loaded += 1;
                    } else {
                        return error("%s: failed to Address MasterNode Quanity", __func__);
                    }
                }
            pcursor->Next();
        } else {
            break;
        }
    }

    return true;
}

// Can get to total count of addresses that belong to a certain masternode_name, or get you the list of all address that belong to a certain masternode_name
bool CMasterNodesDB::MasterNodeAddressDir(std::vector<std::pair<std::string, CAmount> >& vecAddressAmount, int& totalEntries, const bool& fGetTotal, const std::string& masternodeName, const size_t count, const long start)
{
    FlushStateToDisk();

    std::unique_ptr<CDBIterator> pcursor(NewIterator());
    pcursor->Seek(std::make_pair(MASTERNODE_ADDRESS_QUANTITY_FLAG, std::make_pair(masternodeName, std::string())));

    if (fGetTotal) {
        totalEntries = 0;
        while (pcursor->Valid()) {
            boost::this_thread::interruption_point();

            std::pair<char, std::pair<std::string, std::string> > key;
            if (pcursor->GetKey(key) && key.first == MASTERNODE_ADDRESS_QUANTITY_FLAG && key.second.first == masternodeName) {
                totalEntries += 1;
            }
            pcursor->Next();
        }
        return true;
    }

    size_t skip = 0;
    if (start >= 0) {
        skip = start;
    }
    else {
        // compute table size for backwards offset
        long table_size = 0;
        while (pcursor->Valid()) {
            boost::this_thread::interruption_point();

            std::pair<char, std::pair<std::string, std::string> > key;
            if (pcursor->GetKey(key) && key.first == MASTERNODE_ADDRESS_QUANTITY_FLAG && key.second.first == masternodeName) {
                table_size += 1;
            }
            pcursor->Next();
        }
        skip = table_size + start;
        pcursor->SeekToFirst();
    }

    size_t loaded = 0;
    size_t offset = 0;

    // Load masternodes
    while (pcursor->Valid() && loaded < count && loaded < MAX_DATABASE_RESULTS) {
        boost::this_thread::interruption_point();

        std::pair<char, std::pair<std::string, std::string> > key;
        if (pcursor->GetKey(key) && key.first == MASTERNODE_ADDRESS_QUANTITY_FLAG && key.second.first == masternodeName) {
            if (offset < skip) {
                offset += 1;
            }
            else {
                CAmount amount;
                if (pcursor->GetValue(amount)) {
                    vecAddressAmount.emplace_back(std::make_pair(key.second.second, amount));
                    loaded += 1;
                } else {
                    return error("%s: failed to MasterNode Address Quanity", __func__);
                }
            }
            pcursor->Next();
        } else {
            break;
        }
    }

    return true;
}

bool CMasterNodesDB::MasterNodeDir(std::vector<CDatabasedMasterNodeData>& masternodes)
{
    return CMasterNodesDB::MasterNodeDir(masternodes, "*", MAX_SIZE, 0);
}
