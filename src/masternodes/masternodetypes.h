// Copyright (c) 2019 The OLDNAMENEEDKEEP__Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVECOIN_NEWMASTERNODE_H
#define HIVECOIN_NEWMASTERNODE_H

#include <string>
#include <sstream>
#include <list>
#include <unordered_map>
#include "amount.h"
#include "script/standard.h"
#include "primitives/transaction.h"

#define MAX_UNIT 8
#define MIN_UNIT 0

class CMasterNodesCache;

enum class MasterNodeType
{
    ROOT = 10
};

enum class QualifierType
{
    REMOVE_QUALIFIER = 0,
    ADD_QUALIFIER = 1
};

enum class RestrictedType
{
    UNFREEZE_ADDRESS = 0,
    FREEZE_ADDRESS= 1,
    GLOBAL_UNFREEZE = 2,
    GLOBAL_FREEZE = 3
};

int IntFromMasterNodeType(MasterNodeType type);
MasterNodeType MasterNodeTypeFromInt(int nType);

const char IPFS_SHA2_256 = 0x12;
const char TXID_NOTIFIER = 0x54;
const char IPFS_SHA2_256_LEN = 0x20;

template <typename Stream, typename Operation>
bool ReadWriteMasterNodeHash(Stream &s, Operation ser_action, std::string &strIPFSHash)
{
    // assuming 34-byte IPFS SHA2-256 decoded hash (0x12, 0x20, 32 more bytes)
    if (ser_action.ForRead())
    {
        strIPFSHash = "";
        if (!s.empty() and s.size() >= 33) {
            char _sha2_256;
            ::Unserialize(s, _sha2_256);
            std::basic_string<char> hash;
            ::Unserialize(s, hash);

            std::ostringstream os;

            // If it is an ipfs hash, we put the Q and the m 'Qm' at the front
            if (_sha2_256 == IPFS_SHA2_256)
                os << IPFS_SHA2_256 << IPFS_SHA2_256_LEN;

            os << hash.substr(0, 32); // Get the 32 bytes of data
            strIPFSHash = os.str();
            return true;
        }
    }
    else
    {
        if (strIPFSHash.length() == 34) {
            ::Serialize(s, IPFS_SHA2_256);
            ::Serialize(s, strIPFSHash.substr(2));
            return true;
        } else if (strIPFSHash.length() == 32) {
            ::Serialize(s, TXID_NOTIFIER);
            ::Serialize(s, strIPFSHash);
            return true;
        }
    }
    return false;
};

class CNewMasterNode
{
public:
    std::string strName; // MAX 31 Bytes
    CAmount nAmount;     // 8 Bytes
    int8_t units;        // 1 Byte
    int8_t nReissuable;  // 1 Byte
    int8_t nHasIPFS;     // 1 Byte
    std::string strIPFSHash; // MAX 40 Bytes

    CNewMasterNode()
    {
        SetNull();
    }

    CNewMasterNode(const std::string& strName, const CAmount& nAmount, const int& units, const int& nReissuable, const int& nHasIPFS, const std::string& strIPFSHash);
    CNewMasterNode(const std::string& strName, const CAmount& nAmount);

    CNewMasterNode(const CNewMasterNode& masternode);
    CNewMasterNode& operator=(const CNewMasterNode& masternode);

    void SetNull()
    {
        strName= "";
        nAmount = 0;
        units = int8_t(MAX_UNIT);
        nReissuable = int8_t(0);
        nHasIPFS = int8_t(0);
        strIPFSHash = "";
    }

    bool IsNull() const;
    std::string ToString();

    void ConstructTransaction(CScript& script) const;
    void ConstructOwnerTransaction(CScript& script) const;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(strName);
        READWRITE(nAmount);
        READWRITE(units);
        READWRITE(nReissuable);
        READWRITE(nHasIPFS);
        if (nHasIPFS == 1) {
            ReadWriteMasterNodeHash(s, ser_action, strIPFSHash);
        }
    }
};

class MasterNodeComparator
{
public:
    bool operator()(const CNewMasterNode& s1, const CNewMasterNode& s2) const
    {
        return s1.strName < s2.strName;
    }
};

class CDatabasedMasterNodeData
{
public:
    CNewMasterNode masternode;
    int nHeight;
    uint256 blockHash;

    CDatabasedMasterNodeData(const CNewMasterNode& masternode, const int& nHeight, const uint256& blockHash);
    CDatabasedMasterNodeData();

    void SetNull()
    {
        masternode.SetNull();
        nHeight = -1;
        blockHash = uint256();
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(masternode);
        READWRITE(nHeight);
        READWRITE(blockHash);
    }
};

class CMasterNodeTransfer
{
public:
    std::string strName;
    CAmount nAmount;
    std::string message;
    int64_t nExpireTime;

    CMasterNodeTransfer()
    {
        SetNull();
    }

    void SetNull()
    {
        nAmount = 0;
        strName = "";
        message = "";
        nExpireTime = 0;
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(strName);
        READWRITE(nAmount);
        bool validIPFS = ReadWriteMasterNodeHash(s, ser_action, message);
        if (validIPFS) {
            if (ser_action.ForRead()) {
                if (!s.empty() && s.size() >= sizeof(int64_t)) {
                    ::Unserialize(s, nExpireTime);
                }
            } else {
                if (nExpireTime != 0) {
                    ::Serialize(s, nExpireTime);
                }
            }
        }

    }

    CMasterNodeTransfer(const std::string& strMasterNodeName, const CAmount& nAmount, const std::string& message = "", const int64_t& nExpireTime = 0);
    bool IsValid(std::string& strError) const;
    void ConstructTransaction(CScript& script) const;
    bool ContextualCheckAgainstVerifyString(CMasterNodesCache *masternodeCache, const std::string& address, std::string& strError) const;
};

class CUpdateMasterNode
{
public:
    std::string strName;
    CAmount nAmount;
    int8_t nUnits;
    int8_t nReissuable;
    std::string strIPFSHash;

    CUpdateMasterNode()
    {
        SetNull();
    }

    void SetNull()
    {
        nAmount = 0;
        strName = "";
        nUnits = 0;
        nReissuable = 1;
        strIPFSHash = "";
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(strName);
        READWRITE(nAmount);
        READWRITE(nUnits);
        READWRITE(nReissuable);
        ReadWriteMasterNodeHash(s, ser_action, strIPFSHash);
    }

    CUpdateMasterNode(const std::string& strMasterNodeName, const CAmount& nAmount, const int& nUnits, const int& nReissuable, const std::string& strIPFSHash);
    void ConstructTransaction(CScript& script) const;
    bool IsNull() const;
};

class CNullMasterNodeTxData {
public:
    std::string masternode_name;
    int8_t flag; // on/off but could be used to determine multiple options later on

    CNullMasterNodeTxData()
    {
        SetNull();
    }

    void SetNull()
    {
        flag = -1;
        masternode_name = "";
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(masternode_name);
        READWRITE(flag);
    }

    CNullMasterNodeTxData(const std::string& strMasterNodename, const int8_t& nFlag);
    bool IsValid(std::string& strError, CMasterNodesCache& masternodeCache, bool fForceCheckPrimaryMasterNodeExists) const;
    void ConstructTransaction(CScript& script) const;
    void ConstructGlobalRestrictionTransaction(CScript &script) const;
};

class CNullMasterNodeTxVerifierString {

public:
    std::string verifier_string;

    CNullMasterNodeTxVerifierString()
    {
        SetNull();
    }

    void SetNull()
    {
        verifier_string ="";
    }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
    inline void SerializationOp(Stream& s, Operation ser_action)
    {
        READWRITE(verifier_string);
    }

    CNullMasterNodeTxVerifierString(const std::string& verifier);
    void ConstructTransaction(CScript& script) const;
};

/** THESE ARE ONLY TO BE USED WHEN ADDING THINGS TO THE CACHE DURING CONNECT AND DISCONNECT BLOCK */
struct CMasterNodeCacheNewMasterNode
{
    CNewMasterNode masternode;
    std::string address;
    uint256 blockHash;
    int blockHeight;

    CMasterNodeCacheNewMasterNode(const CNewMasterNode& masternode, const std::string& address, const int& blockHeight, const uint256& blockHash)
    {
        this->masternode = masternode;
        this->address = address;
        this->blockHash = blockHash;
        this->blockHeight = blockHeight;
    }

    bool operator<(const CMasterNodeCacheNewMasterNode& rhs) const
    {
        return masternode.strName < rhs.masternode.strName;
    }
};

struct CMasterNodeCacheUpdateMasterNode
{
    CUpdateMasterNode update;
    std::string address;
    COutPoint out;
    uint256 blockHash;
    int blockHeight;


    CMasterNodeCacheUpdateMasterNode(const CUpdateMasterNode& update, const std::string& address, const COutPoint& out, const int& blockHeight, const uint256& blockHash)
    {
        this->update = update;
        this->address = address;
        this->out = out;
        this->blockHash = blockHash;
        this->blockHeight = blockHeight;
    }

    bool operator<(const CMasterNodeCacheUpdateMasterNode& rhs) const
    {
        return out < rhs.out;
    }

};

struct CMasterNodeCacheNewTransfer
{
    CMasterNodeTransfer transfer;
    std::string address;
    COutPoint out;

    CMasterNodeCacheNewTransfer(const CMasterNodeTransfer& transfer, const std::string& address, const COutPoint& out)
    {
        this->transfer = transfer;
        this->address = address;
        this->out = out;
    }

    bool operator<(const CMasterNodeCacheNewTransfer& rhs ) const
    {
        return out < rhs.out;
    }
};

struct CMasterNodeCacheNewOwner
{
    std::string masternodeName;
    std::string address;

    CMasterNodeCacheNewOwner(const std::string& masternodeName, const std::string& address)
    {
        this->masternodeName = masternodeName;
        this->address = address;
    }

    bool operator<(const CMasterNodeCacheNewOwner& rhs) const
    {

        return masternodeName < rhs.masternodeName;
    }
};

struct CMasterNodeCacheUndoMasterNodeAmount
{
    std::string masternodeName;
    std::string address;
    CAmount nAmount;

    CMasterNodeCacheUndoMasterNodeAmount(const std::string& masternodeName, const std::string& address, const CAmount& nAmount)
    {
        this->masternodeName = masternodeName;
        this->address = address;
        this->nAmount = nAmount;
    }
};

struct CMasterNodeCacheSpendMasterNode
{
    std::string masternodeName;
    std::string address;
    CAmount nAmount;

    CMasterNodeCacheSpendMasterNode(const std::string& masternodeName, const std::string& address, const CAmount& nAmount)
    {
        this->masternodeName = masternodeName;
        this->address = address;
        this->nAmount = nAmount;
    }
};

struct CMasterNodeCacheQualifierAddress {
    std::string masternodeName;
    std::string address;
    QualifierType type;

    CMasterNodeCacheQualifierAddress(const std::string &masternodeName, const std::string &address, const QualifierType &type) {
        this->masternodeName = masternodeName;
        this->address = address;
        this->type = type;
    }

    bool operator<(const CMasterNodeCacheQualifierAddress &rhs) const {
        return masternodeName < rhs.masternodeName || (masternodeName == rhs.masternodeName && address < rhs.address);
    }

    uint256 GetHash();
};

struct CMasterNodeCacheRootQualifierChecker {
    std::string rootMasterNodeName;
    std::string address;

    CMasterNodeCacheRootQualifierChecker(const std::string &masternodeName, const std::string &address) {
        this->rootMasterNodeName = masternodeName;
        this->address = address;
    }

    bool operator<(const CMasterNodeCacheRootQualifierChecker &rhs) const {
        return rootMasterNodeName < rhs.rootMasterNodeName || (rootMasterNodeName == rhs.rootMasterNodeName && address < rhs.address);
    }

    uint256 GetHash();
};

struct CMasterNodeCacheRestrictedAddress
{
    std::string masternodeName;
    std::string address;
    RestrictedType type;

    CMasterNodeCacheRestrictedAddress(const std::string& masternodeName, const std::string& address, const RestrictedType& type)
    {
        this->masternodeName = masternodeName;
        this->address = address;
        this->type = type;
    }

    bool operator<(const CMasterNodeCacheRestrictedAddress& rhs) const
    {
        return masternodeName < rhs.masternodeName || (masternodeName == rhs.masternodeName && address < rhs.address);
    }

    uint256 GetHash();
};

struct CMasterNodeCacheRestrictedGlobal
{
    std::string masternodeName;
    RestrictedType type;

    CMasterNodeCacheRestrictedGlobal(const std::string& masternodeName, const RestrictedType& type)
    {
        this->masternodeName = masternodeName;
        this->type = type;
    }

    bool operator<(const CMasterNodeCacheRestrictedGlobal& rhs) const
    {
        return masternodeName < rhs.masternodeName;
    }
};

struct CMasterNodeCacheRestrictedVerifiers
{
    std::string masternodeName;
    std::string verifier;
    bool fUndoingRessiue;

    CMasterNodeCacheRestrictedVerifiers(const std::string& masternodeName, const std::string& verifier)
    {
        this->masternodeName = masternodeName;
        this->verifier = verifier;
        fUndoingRessiue = false;
    }

    bool operator<(const CMasterNodeCacheRestrictedVerifiers& rhs) const
    {
        return masternodeName < rhs.masternodeName;
    }
};

// Least Recently Used Cache
template<typename cache_key_t, typename cache_value_t>
class CLRUCache
{
public:
    typedef typename std::pair<cache_key_t, cache_value_t> key_value_pair_t;
    typedef typename std::list<key_value_pair_t>::iterator list_iterator_t;

    CLRUCache(size_t max_size) : maxSize(max_size)
    {
    }
    CLRUCache()
    {
        SetNull();
    }

    void Put(const cache_key_t& key, const cache_value_t& value)
    {
        auto it = cacheItemsMap.find(key);
        cacheItemsList.push_front(key_value_pair_t(key, value));
        if (it != cacheItemsMap.end())
        {
            cacheItemsList.erase(it->second);
            cacheItemsMap.erase(it);
        }
        cacheItemsMap[key] = cacheItemsList.begin();

        if (cacheItemsMap.size() > maxSize)
        {
            auto last = cacheItemsList.end();
            last--;
            cacheItemsMap.erase(last->first);
            cacheItemsList.pop_back();
        }
    }

    void Erase(const cache_key_t& key)
    {
        auto it = cacheItemsMap.find(key);
        if (it != cacheItemsMap.end())
        {
            cacheItemsList.erase(it->second);
            cacheItemsMap.erase(it);
        }
    }

    const cache_value_t& Get(const cache_key_t& key)
    {
        auto it = cacheItemsMap.find(key);
        if (it == cacheItemsMap.end())
        {
            throw std::range_error("There is no such key in cache");
        }
        else
        {
            cacheItemsList.splice(cacheItemsList.begin(), cacheItemsList, it->second);
            return it->second->second;
        }
    }

    bool Exists(const cache_key_t& key) const
    {
        return cacheItemsMap.find(key) != cacheItemsMap.end();
    }

    size_t Size() const
    {
        return cacheItemsMap.size();
    }


    void Clear()
    {
        cacheItemsMap.clear();
        cacheItemsList.clear();
    }

    void SetNull()
    {
        maxSize = 0;
        Clear();
    }

    size_t MaxSize() const
    {
        return maxSize;
    }


    void SetSize(const size_t size)
    {
        maxSize = size;
    }

   const std::unordered_map<cache_key_t, list_iterator_t>& GetItemsMap()
    {
        return cacheItemsMap;
    };

    const std::list<key_value_pair_t>& GetItemsList()
    {
        return cacheItemsList;
    };


    CLRUCache(const CLRUCache& cache)
    {
        this->cacheItemsList = cache.cacheItemsList;
        this->cacheItemsMap = cache.cacheItemsMap;
        this->maxSize = cache.maxSize;
    }

private:
    std::list<key_value_pair_t> cacheItemsList;
    std::unordered_map<cache_key_t, list_iterator_t> cacheItemsMap;
    size_t maxSize;
};

#endif //HIVECOIN_NEWMASTERNODE_H
