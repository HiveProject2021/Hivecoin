// Copyright (c) 2017-2019 The Raven Core developers
// Copyright (c) 2020-2021 The Hive Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef HIVECOIN_MASTERNODE_PROTOCOL_H
#define HIVECOIN_MASTERNODE_PROTOCOL_H

#include "amount.h"
#include "tinyformat.h"
#include "masternodetypes.h"

#include <string>
#include <set>
#include <map>
#include <unordered_map>
#include <list>

#define HVN_H 114
#define HVN_V 118
#define HVN_N 110
#define HVN_Q 113
#define HVN_T 116
#define HVN_O 111

//等定 MASTERNODE
#define HVN_MN 120
//等定 VOTE
#define HVN_VT 121

#define DEFAULT_UNITS 0
#define DEFAULT_REISSUABLE 1
#define DEFAULT_HAS_IPFS 0
#define DEFAULT_IPFS ""

#define MIN_MASTERNODE_LENGTH 3
#define MAX_MASTERNODE_LENGTH 32

#define OWNER_TAG "!"
#define OWNER_LENGTH 1
#define OWNER_UNITS 0
#define OWNER_MASTERNODE_AMOUNT 1 * COIN

#define QUALIFIER_MASTERNODE_MIN_AMOUNT 1 * COIN
#define QUALIFIER_MASTERNODE_MAX_AMOUNT 10 * COIN
#define QUALIFIER_MASTERNODE_UNITS 0

#define MASTERNODE_NEW_STRING "new_masternode"
#define MASTERNODE_UPDATE_STRING "update_masternode"

#define MINIMUM_REWARDS_PAYOUT_HEIGHT 60

class CScript;
class CDataStream;
class CTransaction;
class CTxOut;
class Coin;
class CWallet;
class CReserveKey;
class CWalletTx;
struct CMasterNodeOutputEntry;
class CCoinControl;
struct CBlockMasterNodeUndo;
class COutput;

// 2500 * 82 Bytes == 205 KB (kilobytes) of memory
#define MAX_CACHE_MASTERNODES_SIZE 2500

// Create map that store that state of current updated transaction that the mempool as accepted.
// If an masternode name is in this map, any other update transactions wont be accepted into the mempool
extern std::map<uint256, std::string> mapUpdatedTx;
extern std::map<std::string, uint256> mapUpdatedMasterNodes;

class CMasterNodes {
public:
    std::map<std::pair<std::string, std::string>, CAmount> mapMasterNodesAddressAmount; // pair < MasterNode Name , Address > -> Quantity of tokens in the address

    // Dirty, Gets wiped once flushed to database
    std::map<std::string, CNewMasterNode> mapUpdatedMasterNodeData; // MasterNode Name -> New MasterNode Data

    CMasterNodes(const CMasterNodes& masternodes) {
        this->mapMasterNodesAddressAmount = masternodes.mapMasterNodesAddressAmount;
        this->mapUpdatedMasterNodeData = masternodes.mapUpdatedMasterNodeData;
    }

    CMasterNodes& operator=(const CMasterNodes& other) {
        mapMasterNodesAddressAmount = other.mapMasterNodesAddressAmount;
        mapUpdatedMasterNodeData = other.mapUpdatedMasterNodeData;
        return *this;
    }

    CMasterNodes() {
        SetNull();
    }

    void SetNull() {
        mapMasterNodesAddressAmount.clear();
        mapUpdatedMasterNodeData.clear();
    }
};

struct ErrorReport {

    enum ErrorType {
        NotSetError = 0,
        InvalidQualifierName = 1,
        EmptyString = 2,
        LengthToLarge = 3,
        InvalidSubExpressionFormula = 4,
        InvalidSyntax = 5,
        MasterNodeDoesntExist = 6,
        FailedToVerifyAgainstAddress = 7,
        EmptySubExpression = 8,
        UnknownOperator = 9,
        ParenthesisParity = 10,
        VariableNotFound = 11
    };

    ErrorType type = ErrorType::NotSetError;
    std::string strDevData;
    std::vector<std::string> vecUserData;
};

std::string GetUserErrorString(const ErrorReport& report);

class CMasterNodesCache : public CMasterNodes
{
private:
    bool AddBackSpentMasterNode(const Coin& coin, const std::string& masternodeName, const std::string& address, const CAmount& nAmount, const COutPoint& out);
    void AddToMasterNodeBalance(const std::string& strName, const std::string& address, const CAmount& nAmount);
    bool UndoTransfer(const CMasterNodeTransfer& transfer, const std::string& address, const COutPoint& outToRemove);
public :
    //! These are memory only containers that show dirty entries that will be databased when flushed
    std::vector<CMasterNodeCacheUndoMasterNodeAmount> vUndoMasterNodeAmount;
    std::vector<CMasterNodeCacheSpendMasterNode> vSpentMasterNodes;

    //! New MasterNodes Caches
    std::set<CMasterNodeCacheNewMasterNode> setNewMasterNodesToRemove;
    std::set<CMasterNodeCacheNewMasterNode> setNewMasterNodesToAdd;

    //! New Update Caches
    std::set<CMasterNodeCacheUpdateMasterNode> setNewUpdateToRemove;
    std::set<CMasterNodeCacheUpdateMasterNode> setNewUpdateToAdd;

    //! Ownership MasterNodes Caches
    std::set<CMasterNodeCacheNewOwner> setNewOwnerMasterNodesToAdd;
    std::set<CMasterNodeCacheNewOwner> setNewOwnerMasterNodesToRemove;

    //! Restricted Global MasterNode Caches
    std::set<CMasterNodeCacheRestrictedGlobal> setNewRestrictedGlobalToAdd;
    std::set<CMasterNodeCacheRestrictedGlobal> setNewRestrictedGlobalToRemove;

    //! Restricted MasterNodes Verifier Caches
    std::set<CMasterNodeCacheRestrictedVerifiers> setNewRestrictedVerifierToAdd;
    std::set<CMasterNodeCacheRestrictedVerifiers> setNewRestrictedVerifierToRemove;

    //! Root Qualifier Address Map
    std::map<CMasterNodeCacheRootQualifierChecker, std::set<std::string> > mapRootQualifierAddressesAdd;
    std::map<CMasterNodeCacheRootQualifierChecker, std::set<std::string> > mapRootQualifierAddressesRemove;

    CMasterNodesCache() : CMasterNodes()
    {
        SetNull();
        ClearDirtyCache();
    }

    CMasterNodesCache(const CMasterNodesCache& cache) : CMasterNodes(cache)
    {
        //! Copy dirty cache also
        this->vSpentMasterNodes = cache.vSpentMasterNodes;
        this->vUndoMasterNodeAmount = cache.vUndoMasterNodeAmount;

        //! Create Caches
        this->setNewMasterNodesToRemove = cache.setNewMasterNodesToRemove;
        this->setNewMasterNodesToAdd = cache.setNewMasterNodesToAdd;

        //! Update Caches
        this->setNewUpdateToRemove = cache.setNewUpdateToRemove;
        this->setNewUpdateToAdd = cache.setNewUpdateToAdd;

        //! Owner Caches
        this->setNewOwnerMasterNodesToAdd = cache.setNewOwnerMasterNodesToAdd;
        this->setNewOwnerMasterNodesToRemove = cache.setNewOwnerMasterNodesToRemove;

    }

    CMasterNodesCache& operator=(const CMasterNodesCache& cache)
    {
        this->mapMasterNodesAddressAmount = cache.mapMasterNodesAddressAmount;
        this->mapUpdatedMasterNodeData = cache.mapUpdatedMasterNodeData;

        //! Copy dirty cache also
        this->vSpentMasterNodes = cache.vSpentMasterNodes;
        this->vUndoMasterNodeAmount = cache.vUndoMasterNodeAmount;

        //! Create Caches
        this->setNewMasterNodesToRemove = cache.setNewMasterNodesToRemove;
        this->setNewMasterNodesToAdd = cache.setNewMasterNodesToAdd;

        //! Update Caches
        this->setNewUpdateToRemove = cache.setNewUpdateToRemove;
        this->setNewUpdateToAdd = cache.setNewUpdateToAdd;

        //! Owner Caches
        this->setNewOwnerMasterNodesToAdd = cache.setNewOwnerMasterNodesToAdd;
        this->setNewOwnerMasterNodesToRemove = cache.setNewOwnerMasterNodesToRemove;

        return *this;
    }

    //! Cache only undo functions
    bool RemoveNewMasterNode(const CNewMasterNode& masternode, const std::string address);
    bool RemoveOwnerMasterNode(const std::string& masternodesName, const std::string address);
    bool RemoveUpdateMasterNode(const CUpdateMasterNode& update, const std::string address, const COutPoint& out, const std::vector<std::pair<std::string, CBlockMasterNodeUndo> >& vUndoIPFS);
    bool UndoMasterNodeCoin(const Coin& coin, const COutPoint& out);
    

    //! Cache only add masternode functions
    bool AddNewMasterNode(const CNewMasterNode& masternode, const std::string address, const int& nHeight, const uint256& blockHash);
    bool AddOwnerMasterNode(const std::string& masternodesName, const std::string address);
    bool AddUpdateMasterNode(const CUpdateMasterNode& update, const std::string address, const COutPoint& out);
    
    //! Cache only validation functions
    bool TrySpendCoin(const COutPoint& out, const CTxOut& coin);

    //! Help functions
    bool ContainsMasterNode(const CNewMasterNode& masternode);
    bool ContainsMasterNode(const std::string& masternodeName);

    //! Returns true if an masternode with this name already exists
    bool CheckIfMasterNodeExists(const std::string& name, bool fForceDuplicateCheck = true);

    //! Returns true if an masternode with the name exists, and it was able to get the masternode metadata from database
    bool GetMasterNodeMetaDataIfExists(const std::string &name, CNewMasterNode &masternode);
    bool GetMasterNodeMetaDataIfExists(const std::string &name, CNewMasterNode &masternode, int& nHeight, uint256& blockHash);

    //! Returns true if the MasterNode Verifier String was found for an masternode_name, if fSkipTempCache is true, it will only search pmasternodes pointer and databases
    bool GetMasterNodeVerifierStringIfExists(const std::string &name, CNullMasterNodeTxVerifierString& verifier, bool fSkipTempCache = false);

    //! Return true if the address has the given qualifier assigned to it
    bool CheckForAddressQualifier(const std::string &qualifier_name, const std::string& address, bool fSkipTempCache = false);

    //! Return true if the address is marked as frozen
    bool CheckForAddressRestriction(const std::string &restricted_name, const std::string& address, bool fSkipTempCache = false);

    //! Return true if the restricted masternode is globally freezing trading
    bool CheckForGlobalRestriction(const std::string &restricted_name, bool fSkipTempCache = false);

    //! Calculate the size of the CMasterNodes (in bytes)
    size_t DynamicMemoryUsage() const;

    //! Get the size of the none databased cache
    size_t GetCacheSize() const;
    size_t GetCacheSizeV2() const;

    //! Flush all new cache entries into the pmasternodes global cache
    bool Flush();

    //! Write masternode cache data to database
    bool DumpCacheToDatabase();

    //! Clear all dirty cache sets, vetors, and maps
    void ClearDirtyCache() {

        vUndoMasterNodeAmount.clear();
        vSpentMasterNodes.clear();

        setNewMasterNodesToRemove.clear();
        setNewMasterNodesToAdd.clear();

        setNewUpdateToAdd.clear();
        setNewUpdateToRemove.clear();

        setNewOwnerMasterNodesToAdd.clear();
        setNewOwnerMasterNodesToRemove.clear();

        mapUpdatedMasterNodeData.clear();
        mapMasterNodesAddressAmount.clear();
    }

   std::string CacheToString() const {

       return strprintf(
               "vNewMasterNodesToRemove size : %d, vNewMasterNodesToAdd size : %d, vSpentMasterNodes : %d\n",
               setNewMasterNodesToRemove.size(), setNewMasterNodesToAdd.size(), 
               vSpentMasterNodes.size());
   }
};

//! Functions to be used to get access to the current burn amount required for specific masternode issuance transactions
CAmount GetCreateMasterNodeDepositAmount();
CAmount GetUpdateMasterNodeDepositAmount();
CAmount GetDepositAmount(const MasterNodeType type);
CAmount GetDepositAmount(const int nType);

//! Functions to be used to get access to the burn address for a given masternode type issuance
std::string GetDepositAddress(const MasterNodeType type);
std::string GetDepositAddress(const int nType);

void GetTxOutMasterNodeTypes(const std::vector<CTxOut>& vout, int& creates, int& updates, int& transfers, int& owners);

//! Check is an masternode name is valid, and being able to return the masternode type if needed
bool IsMasterNodeNameValid(const std::string& name);
bool IsMasterNodeNameValid(const std::string& name, MasterNodeType& masternodeType);
bool IsMasterNodeNameValid(const std::string& name, MasterNodeType& masternodeType, std::string& error);

//! Check if an unique tagname is valid
bool IsUniqueTagValid(const std::string& tag);

//! Check if an masternode is an owner
bool IsMasterNodeNameAnOwner(const std::string& name);

//! Check if an masternode is a restricted masternode
bool IsMasterNodeNameAnRestricted(const std::string& name);

//! Check if an masternode is a qualifier masternode or sub qualifier
bool IsMasterNodeNameAQualifier(const std::string& name, bool fOnlyQualifiers = false);

//! Check if an masternode is a sub qualifier
bool IsMasterNodeNameASubQualifier(const std::string& name);

//! Check if an masternode is a message channel
bool IsMasterNodeNameAnMsgChannel(const std::string& name);

bool IsMasterNodeNameARoot(const std::string& name);

//! Get the root name of an masternode
std::string GetParentName(const std::string& name); // Gets the parent name of a submasternode TEST/TESTSUB would return TEST

//! Get the owner token name belonging to a restricted masternode
std::string RestrictedNameToOwnerName(const std::string& name);

//! Build a unique masternode buy giving the root name, and the tag name (ROOT, TAG) => ROOT#TAG
std::string GetUniqueMasterNodeName(const std::string& parent, const std::string& tag);

//! Given a type, and an masternode name, return if that name is valid based on the type
bool IsTypeCheckNameValid(const MasterNodeType type, const std::string& name, std::string& error);

//! These types of masternode tx, have specific metadata at certain indexes in the transaction.
//! These functions pull data from the scripts at those indexes
bool MasterNodeFromTransaction(const CTransaction& tx, CNewMasterNode& masternode, std::string& strAddress);
bool OwnerFromTransaction(const CTransaction& tx, std::string& ownerName, std::string& strAddress);
bool UpdateMasterNodeFromTransaction(const CTransaction& tx, CUpdateMasterNode& update, std::string& strAddress);
bool UniqueMasterNodeFromTransaction(const CTransaction& tx, CNewMasterNode& masternode, std::string& strAddress);
bool MsgChannelMasterNodeFromTransaction(const CTransaction& tx, CNewMasterNode& masternode, std::string& strAddress);
bool QualifierMasterNodeFromTransaction(const CTransaction& tx, CNewMasterNode& masternode, std::string& strAddress);
bool RestrictedMasterNodeFromTransaction(const CTransaction& tx, CNewMasterNode& masternode, std::string& strAddress);

//! Get specific masternode type metadata from the given scripts
bool MasterNodeFromScript(const CScript& scriptPubKey, CNewMasterNode& masternode, std::string& strAddress);
bool OwnerMasterNodeFromScript(const CScript& scriptPubKey, std::string& masternodeName, std::string& strAddress);
bool UpdateMasterNodeFromScript(const CScript& scriptPubKey, CUpdateMasterNode& update, std::string& strAddress);
bool MasterNodeNullDataFromScript(const CScript& scriptPubKey, CNullMasterNodeTxData& masternodeData, std::string& strAddress);
bool MasterNodeNullVerifierDataFromScript(const CScript& scriptPubKey, CNullMasterNodeTxVerifierString& verifierData);

//! Check to make sure the script contains the burn transaction
bool CheckCreateDepositTx(const CTxOut& txOut, const MasterNodeType& type, const int numberCreated);
bool CheckCreateDepositTx(const CTxOut& txOut, const MasterNodeType& type);

// TODO, maybe remove this function and input that check into the CheckCreateDepositTx.
//! Check to make sure the script contains the update burn data
bool CheckUpdateDepositTx(const CTxOut& txOut);

//! create masternode scripts to make sure script meets the standards
bool CheckCreateDataTx(const CTxOut& txOut); // OP_HIVE_MASTERNODE HVNQ (That is a Q as in Que not an O)
bool CheckOwnerDataTx(const CTxOut& txOut);// OP_HIVE_MASTERNODE HVNO
bool CheckUpdateDataTx(const CTxOut& txOut);// OP_HIVE_MASTERNODE HVNR
bool CheckTransferOwnerTx(const CTxOut& txOut);// OP_HIVE_MASTERNODE HVNT

//! Check the Encoded hash and make sure it is either an IPFS hash or a OIP hash
bool CheckEncoded(const std::string& hash, std::string& strError);

//! Checks the amount and units, and makes sure that the amount uses the correct decimals
bool CheckAmountWithUnits(const CAmount& nAmount, const int8_t nUnits);

//! Check script and see if it matches the masternode issuance template
bool IsScriptNewMasterNode(const CScript& scriptPubKey);
bool IsScriptNewMasterNode(const CScript& scriptPubKey, int& nStartingIndex);

//! Check script and see if it matches the unquie issuance template
bool IsScriptNewUniqueMasterNode(const CScript& scriptPubKey);
bool IsScriptNewUniqueMasterNode(const CScript &scriptPubKey, int &nStartingIndex);

//! Check script and see if it matches the owner issuance template
bool IsScriptOwnerMasterNode(const CScript& scriptPubKey);
bool IsScriptOwnerMasterNode(const CScript& scriptPubKey, int& nStartingIndex);

//! Check script and see if it matches the update template
bool IsScriptUpdateMasterNode(const CScript& scriptPubKey);
bool IsScriptUpdateMasterNode(const CScript& scriptPubKey, int& nStartingIndex);

//! Check script and see if it matches the transfer masternode template
bool IsScriptTransferMasterNode(const CScript& scriptPubKey);
bool IsScriptTransferMasterNode(const CScript& scriptPubKey, int& nStartingIndex);

//! Check script and see if it matches the message channel issuance template
bool IsScriptNewMsgChannelMasterNode(const CScript& scriptPubKey);
bool IsScriptNewMsgChannelMasterNode(const CScript& scriptPubKey, int& nStartingIndex);

//! Check script and see if it matches the qualifier issuance template
bool IsScriptNewQualifierMasterNode(const CScript& scriptPubKey);
bool IsScriptNewQualifierMasterNode(const CScript &scriptPubKey, int &nStartingIndex);

//! Check script and see if it matches the restricted createance template
bool IsScriptNewRestrictedMasterNode(const CScript& scriptPubKey);
bool IsScriptNewRestrictedMasterNode(const CScript &scriptPubKey, int &nStartingIndex);

bool IsNewOwnerTxValid(const CTransaction& tx, const std::string& masternodeName, const std::string& address, std::string& errorMsg);

void GetAllAdministrativeMasterNodes(CWallet *pwallet, std::vector<std::string> &names, int nMinConf = 1);
void GetAllMyMasterNodes(CWallet* pwallet, std::vector<std::string>& names, int nMinConf = 1, bool fIncludeAdministrator = false, bool fOnlyAdministrator = false);

bool GetMasterNodeInfoFromCoin(const Coin& coin, std::string& strName, CAmount& nAmount);
bool GetMasterNodeInfoFromScript(const CScript& scriptPubKey, std::string& strName, CAmount& nAmount);

bool GetMasterNodeData(const CScript& script, CMasterNodeOutputEntry& data);

bool GetBestMasterNodeAddressAmount(CMasterNodesCache& cache, const std::string& masternodeName, const std::string& address);


//! Decode and Encode IPFS hashes, or OIP hashes
std::string DecodeMasterNodeData(std::string encoded);
std::string EncodeMasterNodeData(std::string decoded);
std::string DecodeIPFS(std::string encoded);
std::string EncodeIPFS(std::string decoded);

#ifdef ENABLE_WALLET

bool GetAllMyMasterNodeBalances(std::map<std::string, std::vector<COutput> >& outputs, std::map<std::string, CAmount>& amounts, const int confirmations = 0, const std::string& prefix = "");
bool GetMyMasterNodeBalance(const std::string& name, CAmount& balance, const int& confirmations);

//! Creates new masternode issuance transaction
bool CreateMasterNodeTransaction(CWallet* pwallet, CCoinControl& coinControl, const CNewMasterNode& masternode, const std::string& address, std::pair<int, std::string>& error, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRequired, std::string* verifier_string = nullptr);
bool CreateMasterNodeTransaction(CWallet* pwallet, CCoinControl& coinControl, const std::vector<CNewMasterNode> masternodes, const std::string& address, std::pair<int, std::string>& error, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRequired, std::string* verifier_string = nullptr);

//! Create a update masternode transaction
bool CreateUpdateMasterNodeTransaction(CWallet* pwallet, CCoinControl& coinControl, const CUpdateMasterNode& masternode, const std::string& address, std::pair<int, std::string>& error, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRequired, std::string* verifier_string = nullptr);


//! Create a transfer masternode transaction
bool CreateTransferMasterNodeTransaction(CWallet* pwallet, const CCoinControl& coinControl, const std::vector< std::pair<CMasterNodeTransfer, std::string> >vTransfers, const std::string& changeAddress, std::pair<int, std::string>& error, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRequired, std::vector<std::pair<CNullMasterNodeTxData, std::string> >* nullMasterNodeTxData = nullptr, std::vector<CNullMasterNodeTxData>* nullGlobalRestrictionData = nullptr);

//! Send any type of masternode transaction to the network
bool SendMasterNodeTransaction(CWallet* pwallet, CWalletTx& transaction, CReserveKey& reserveKey, std::pair<int, std::string>& error, std::string& txid);

/** Verifies that this wallet owns the give masternode */
bool VerifyWalletHasMasterNode(const std::string& masternode_name, std::pair<int, std::string>& pairError);
#endif

/** Helper method for extracting address bytes, masternode name and amount from an masternode script */
bool ParseMasterNodeScript(CScript scriptPubKey, uint160 &hashBytes, std::string &masternodeName, CAmount &masternodeAmount);

/** Helper method for extracting #TAGS from a verifier string */
void ExtractVerifierStringQualifiers(const std::string& verifier, std::set<std::string>& qualifiers);
bool CheckVerifierString(const std::string& verifier, std::set<std::string>& setFoundQualifiers, std::string& strError, ErrorReport* errorReport = nullptr);
std::string GetStrippedVerifierString(const std::string& verifier);

/** Helper methods that validate changes to null masternode data transaction databases */
bool VerifyNullMasterNodeDataFlag(const int& flag, std::string& strError);
bool VerifyQualifierChange(CMasterNodesCache& cache, const CNullMasterNodeTxData& data, const std::string& address, std::string& strError);
bool VerifyRestrictedAddressChange(CMasterNodesCache& cache, const CNullMasterNodeTxData& data, const std::string& address, std::string& strError);
bool VerifyGlobalRestrictedChange(CMasterNodesCache& cache, const CNullMasterNodeTxData& data, std::string& strError);

//// Non Contextual Check functions
bool CheckVerifierMasterNodeTxOut(const CTxOut& txout, std::string& strError);
bool CheckNewMasterNode(const CNewMasterNode& masternode, std::string& strError);
bool CheckUpdateMasterNode(const CUpdateMasterNode& masternode, std::string& strError);

//// Contextual Check functions
bool ContextualCheckNullMasterNodeTxOut(const CTxOut& txout, CMasterNodesCache* masternodeCache, std::string& strError, std::vector<std::pair<std::string, CNullMasterNodeTxData>>* myNullMasterNodeData = nullptr);
bool ContextualCheckVerifierMasterNodeTxOut(const CTxOut& txout, CMasterNodesCache* masternodeCache, std::string& strError);
bool ContextualCheckVerifierString(CMasterNodesCache* cache, const std::string& verifier, const std::string& check_address, std::string& strError, ErrorReport* errorReport = nullptr);
bool ContextualCheckNewMasterNode(CMasterNodesCache* masternodeCache, const CNewMasterNode& masternode, std::string& strError, bool fCheckMempool = false);
bool ContextualCheckUpdateMasterNode(CMasterNodesCache* masternodeCache, const CUpdateMasterNode& update_masternode, std::string& strError, const CTransaction& tx);
bool ContextualCheckUpdateMasterNode(CMasterNodesCache* masternodeCache, const CUpdateMasterNode& update_masternode, std::string& strError);

#endif //HIVECOIN_MASTERNODE_PROTOCOL_H
