// Copyright (c) 2017-2020 The OLDNAMENEEDKEEP__Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <regex>
#include <script/script.h>
#include <version.h>
#include <streams.h>
#include <primitives/transaction.h>
#include <iostream>
#include <script/standard.h>
#include <util.h>
#include <chainparams.h>
#include <base58.h>
#include <validation.h>
#include <txmempool.h>
#include <tinyformat.h>
#include <wallet/wallet.h>
#include <boost/algorithm/string.hpp>
#include <consensus/validation.h>
#include <rpc/protocol.h>
#include <net.h>
#include "masternodes.h"
#include "masternodedb.h"
#include "masternodetypes.h"
#include "protocol.h"
#include "wallet/coincontrol.h"
#include "utilmoneystr.h"
#include "coins.h"
#include "wallet/wallet.h"
#include "LibBoolEE.h"

#define SIX_MONTHS 15780000 // Six months worth of seconds

#define OFFSET_THREE 3
#define OFFSET_FOUR 4
#define OFFSET_TWENTY_THREE 23


std::map<uint256, std::string> mapUpdatedTx;
std::map<std::string, uint256> mapUpdatedMasterNodes;

// excluding owner tag ('!')
static const auto MAX_NAME_LENGTH = 31;
static const auto MAX_CHANNEL_NAME_LENGTH = 12;

// min lengths are expressed by quantifiers
static const std::regex ROOT_NAME_CHARACTERS("^[A-Z0-9._]{3,}$");

static const std::regex HIVE_NAMES("^RVN$|^RAVEN$|^RAVENCOIN$|^#RVN$|^#RAVEN$|^#RAVENCOIN$|^HVN$|^HIVE$|^HIVECOIN$|^#HVN$|^#HIVE$|^#HIVECOIN$");

bool IsRootNameValid(const std::string& name)
{
    return std::regex_match(name, ROOT_NAME_CHARACTERS)
        && !std::regex_match(name, HIVE_NAMES);
}

bool IsNameValidBeforeTag(const std::string& name)
{
    std::vector<std::string> parts;
    boost::split(parts, name, boost::is_any_of(SUB_NAME_DELIMITER));

    if (!IsRootNameValid(parts.front())) return false;

    if (parts.size() > 1)
    {
        for (unsigned long i = 1; i < parts.size(); i++)
        {
            if (!IsSubNameValid(parts[i])) return false;
        }
    }

    return true;
}

bool IsMasterNodeNameValid(const std::string& name)
{
    MasterNodeType _masternodeType;
    std::string _error;
    return IsMasterNodeNameValid(name, _masternodeType, _error);
}

std::string GetParentName(const std::string& name)
{
    return name;
}

bool CNewMasterNode::IsNull() const
{
    return strName == "";
}

CNewMasterNode::CNewMasterNode(const CNewMasterNode& masternode)
{
    this->strName = masternode.strName;
    this->hostName = masternode.hostName;
    this->hostPort = masternode.hostPort;
    this->addressOwner = masternode.addressOwner;
    this->addressVote = masternode.addressVote;
    this->regHeight = masternode.regHeight;
    this->lastPayHeight = masternode.lastPayHeight;
    this->nextPayHeight = masternode.nextPayHeight;
    this->addressPayTo = masternode.addressPayTo;
}

CNewMasterNode& CNewMasterNode::operator=(const CNewMasterNode& masternode)
{
    this->strName = masternode.strName;
    this->hostName = masternode.hostName;
    this->hostPort = masternode.hostPort;
    this->addressOwner = masternode.addressOwner;
    this->addressVote = masternode.addressVote;
    this->regHeight = masternode.regHeight;
    this->lastPayHeight = masternode.lastPayHeight;
    this->nextPayHeight = masternode.nextPayHeight;
    this->addressPayTo = masternode.addressPayTo;
    return *this;
}

std::string CNewMasterNode::ToString()
{
    std::stringstream ss;
    ss << "Printing an masternode" << "\n";
    ss << "strName : " << strName << "\n";
    ss << "host: " << hostName << "\n";
    ss << "hostPort : " << hostPort << "\n";
    ss << "addressOwner : " << addressOwner << "\n";
    ss << "addressVote : " << addressVote << "\n";
    ss << "regHeight : " << std::to_string(regHeight) << "\n";
    ss << "lastPayHeight : " << std::to_string(lastPayHeight) << "\n";
    ss << "nextPayHeight : " << std::to_string(nextPayHeight) << "\n";
    ss << "addressPayTo : " << addressPayTo << "\n";
    ss << "amount : " << nAmount << "\n";
    ss << "amount : " << nAmount << "\n";
    ss << "amount : " << nAmount << "\n";
    ss << "amount : " << nAmount << "\n";

    return ss.str();
}

CNewMasterNode::CNewMasterNode(const std::string& strName, const std::string& hostName, const std::string& hostPort, const std::string& addressOwner, const std::string& addressVote, const std::string& regHeight, const std::string& lastPayHeight, const std::string& nextPayHeight, const std::string& addressPayTo)
{
    this->SetNull();
	this->strName = strName;
    this->hostName = hostName;
    this->hostPort = hostPort;
    this->addressOwner = addressOwner;
    this->addressVote = addressVote;
    this->regHeight = int8_t(regHeight);
    this->lastPayHeight = int8_t(lastPayHeight);
    this->nextPayHeight = int8_t(nextPayHeight);
    this->addressPayTo = addressPayTo;
}

//未完成.
CNewMasterNode::CNewMasterNode(const std::string& strName)
{
    this->SetNull();
	this->strName = strName;
    this->hostName = hostName;
    this->hostPort = hostPort;
    this->addressOwner = addressOwner;
    this->addressVote = addressVote;
    this->regHeight = regHeight;
    this->lastPayHeight = lastPayHeight;
    this->nextPayHeight = nextPayHeight;
    this->addressPayTo = addressPayTo;
}

CDatabasedMasterNodeData::CDatabasedMasterNodeData(const CNewMasterNode& masternode, const int& nHeight, const uint256& blockHash)
{
    this->SetNull();
    this->masternode = masternode;
    this->nHeight = nHeight;
    this->blockHash = blockHash;
}

CDatabasedMasterNodeData::CDatabasedMasterNodeData()
{
    this->SetNull();
}

/**
 * Constructs a CScript that carries the masternode name and other information and adds to to the end of the given script
 * @param dest - The destination that the masternode will belong to
 * @param script - This script needs to be a pay to address script
 */
void CNewMasterNode::ConstructTransaction(CScript& script) const
{
    CDataStream ssMasterNode(SER_NETWORK, PROTOCOL_VERSION);
    ssMasterNode << *this;

    std::vector<unsigned char> vchMessage;
    vchMessage.push_back(HVN_H); // h
    vchMessage.push_back(HVN_V); // v
    vchMessage.push_back(HVN_N); // n
    vchMessage.push_back(HVN_Q); // q

    vchMessage.insert(vchMessage.end(), ssMasterNode.begin(), ssMasterNode.end());
    script << OP_HVN_MASTERNODE << ToByteVector(vchMessage) << OP_DROP;
}

void CNewMasterNode::ConstructOwnerTransaction(CScript& script) const
{
    CDataStream ssOwner(SER_NETWORK, PROTOCOL_VERSION);
    ssOwner << std::string(this->strName + OWNER_TAG);

    std::vector<unsigned char> vchMessage;
    vchMessage.push_back(HVN_H); // h
    vchMessage.push_back(HVN_V); // v
    vchMessage.push_back(HVN_N); // n
    vchMessage.push_back(HVN_O); // o

    vchMessage.insert(vchMessage.end(), ssOwner.begin(), ssOwner.end());
    script << OP_HVN_MASTERNODE << ToByteVector(vchMessage) << OP_DROP;
}

bool MasterNodeFromTransaction(const CTransaction& tx, CNewMasterNode& masternode, std::string& strAddress)
{
    // Check to see if the transaction is an new masternode create tx
    if (!tx.IsNewMasterNode())
        return false;

    // Get the scriptPubKey from the last tx in vout
    CScript scriptPubKey = tx.vout[tx.vout.size() - 1].scriptPubKey;

    return MasterNodeFromScript(scriptPubKey, masternode, strAddress);
}

bool TransferMasterNodeFromScript(const CScript& scriptPubKey, CMasterNodeTransfer& masternodeTransfer, std::string& strAddress)
{
    int nStartingIndex = 0;
    if (!IsScriptTransferMasterNode(scriptPubKey, nStartingIndex)) {
        return false;
    }

    CTxDestination destination;
    ExtractDestination(scriptPubKey, destination);

    strAddress = EncodeDestination(destination);

    std::vector<unsigned char> vchTransferMasterNode;

    if (AreTransferScriptsSizeDeployed()) {
        // Before kawpow activation we used the hardcoded 31 to find the data
        // This created a bug where large transfers scripts would fail to serialize.
        // This fixes that create (https://github.com/HiveProject2021/Hivecoin/creates/752)
        // TODO, after the kawpow fork goes active, we should be able to remove this if/else statement and just use this line.
        vchTransferMasterNode.insert(vchTransferMasterNode.end(), scriptPubKey.begin() + nStartingIndex, scriptPubKey.end());
    } else {
        vchTransferMasterNode.insert(vchTransferMasterNode.end(), scriptPubKey.begin() + 31, scriptPubKey.end());
    }

    CDataStream ssMasterNode(vchTransferMasterNode, SER_NETWORK, PROTOCOL_VERSION);

    try {
        ssMasterNode >> masternodeTransfer;
    } catch(std::exception& e) {
        error("Failed to get the transfer masternode from the stream: %s", e.what());
        return false;
    }

    return true;
}

bool MasterNodeFromScript(const CScript& scriptPubKey, CNewMasterNode& masternodeNew, std::string& strAddress)
{
    int nStartingIndex = 0;
    if (!IsScriptNewMasterNode(scriptPubKey, nStartingIndex))
        return false;

    CTxDestination destination;
    ExtractDestination(scriptPubKey, destination);

    strAddress = EncodeDestination(destination);

    std::vector<unsigned char> vchNewMasterNode;
    vchNewMasterNode.insert(vchNewMasterNode.end(), scriptPubKey.begin() + nStartingIndex, scriptPubKey.end());
    CDataStream ssMasterNode(vchNewMasterNode, SER_NETWORK, PROTOCOL_VERSION);

    try {
        ssMasterNode >> masternodeNew;
    } catch(std::exception& e) {
        error("Failed to get the masternode from the stream: %s", e.what());
        return false;
    }

    return true;
}

bool MasterNodeNullDataFromScript(const CScript& scriptPubKey, CNullMasterNodeTxData& masternodeData, std::string& strAddress)
{
    if (!scriptPubKey.IsNullMasterNodeTxDataScript()) {
        return false;
    }

    CTxDestination destination;
    ExtractDestination(scriptPubKey, destination);

    strAddress = EncodeDestination(destination);

    std::vector<unsigned char> vchMasterNodeData;
    vchMasterNodeData.insert(vchMasterNodeData.end(), scriptPubKey.begin() + OFFSET_TWENTY_THREE, scriptPubKey.end());
    CDataStream ssData(vchMasterNodeData, SER_NETWORK, PROTOCOL_VERSION);

    try {
        ssData >> masternodeData;
    } catch(std::exception& e) {
        error("Failed to get the null masternode tx data from the stream: %s", e.what());
        return false;
    }

    return true;
}

bool GlobalMasterNodeNullDataFromScript(const CScript& scriptPubKey, CNullMasterNodeTxData& masternodeData)
{
    if (!scriptPubKey.IsNullGlobalRestrictionMasterNodeTxDataScript()) {
        return false;
    }

    std::vector<unsigned char> vchMasterNodeData;
    vchMasterNodeData.insert(vchMasterNodeData.end(), scriptPubKey.begin() + OFFSET_FOUR, scriptPubKey.end());
    CDataStream ssData(vchMasterNodeData, SER_NETWORK, PROTOCOL_VERSION);

    try {
        ssData >> masternodeData;
    } catch(std::exception& e) {
        error("Failed to get the global restriction masternode tx data from the stream: %s", e.what());
        return false;
    }

    return true;
}

bool MasterNodeNullVerifierDataFromScript(const CScript& scriptPubKey, CNullMasterNodeTxVerifierString& verifierData)
{
    if (!scriptPubKey.IsNullMasterNodeVerifierTxDataScript()) {
        return false;
    }

    std::vector<unsigned char> vchMasterNodeData;
    vchMasterNodeData.insert(vchMasterNodeData.end(), scriptPubKey.begin() + OFFSET_THREE, scriptPubKey.end());
    CDataStream ssData(vchMasterNodeData, SER_NETWORK, PROTOCOL_VERSION);

    try {
        ssData >> verifierData;
    } catch(std::exception& e) {
        error("Failed to get the verifier string from the stream: %s", e.what());
        return false;
    }

    return true;
}

//! Call VerifyNewMasterNode if this function returns true
bool CTransaction::IsNewMasterNode() const
{
    // New MasterNode transaction will always have at least three outputs.
    // 1. Owner Token output
    // 2. Create MasterNode output
    // 3. HVN Deposit Fee
    if (vout.size() < 3) {
        return false;
    }

    // Check for the masternodes data CTxOut. This will always be the last output in the transaction
    if (!CheckCreateDataTx(vout[vout.size() - 1]))
        return false;

    // Check to make sure the owner masternode is created
    if (!CheckOwnerDataTx(vout[vout.size() - 2]))
        return false;

    // Don't overlap with IsNewUniqueMasterNode()
    CScript script = vout[vout.size() - 1].scriptPubKey;
    if (IsScriptNewUniqueMasterNode(script)|| IsScriptNewRestrictedMasterNode(script))
        return false;

    return true;
}

bool CTransaction::GetVerifierStringFromTx(CNullMasterNodeTxVerifierString& verifier, std::string& strError, bool& fNotFound) const
{
    fNotFound = false;
    bool found = false;
    int count = 0;
    for (auto out : vout) {
        if (out.scriptPubKey.IsNullMasterNodeVerifierTxDataScript()) {
            count++;

            if (count > 1) {
                strError = _("Multiple verifier strings found in transaction");
                return false;
            }
            if (!MasterNodeNullVerifierDataFromScript(out.scriptPubKey, verifier)) {
                strError = _("Failed to get verifier string from output: ") + out.ToString();
                return false;
            }

            found = true;
        }
    }

    // Set error message, for if it returns false
    if (!found) {
        fNotFound = true;
        strError = _("Verifier string not found");
    }

    return found && count == 1;
}

bool CTransaction::GetVerifierStringFromTx(CNullMasterNodeTxVerifierString& verifier, std::string& strError) const
{
    bool fNotFound = false;
    return GetVerifierStringFromTx(verifier, strError, fNotFound);
}

bool CTransaction::CheckAddingTagDepositFee(const int& count) const
{
    // check for burn outpoint )
    bool fDepositOutpointFound = false;
    for (auto out : vout) {
        if (CheckCreateDepositTx(out, MasterNodeType::NULL_ADD_QUALIFIER, count)) {
            fDepositOutpointFound = true;
            break;
        }
    }

   return fDepositOutpointFound;
}

CMasterNodeTransfer::CMasterNodeTransfer(const std::string& strMasterNodeName, const CAmount& nAmount, const std::string& message, const int64_t& nExpireTime)
{
    SetNull();
    this->strName = strMasterNodeName;
    this->nAmount = nAmount;
    this->message = message;
    if (!message.empty()) {
        if (nExpireTime) {
            this->nExpireTime = nExpireTime;
        } else {
            this->nExpireTime = 0;
        }
    }
}

bool CMasterNodeTransfer::IsValid(std::string& strError) const
{
    // Don't use this function with any sort of consensus checks
    // All of these checks are run with ContextualCheckTransferMasterNode also

    strError = "";

    if (!IsMasterNodeNameValid(std::string(strName))) {
        strError = "Invalid parameter: masternode_name must only consist of valid characters and have a size between 3 and 30 characters. See help for more details.";
        return false;
    }

    // this function is only being called in createrawtranasction, so it is fine to have a contextual check here
    // if this gets called anywhere else, we will need to move this to a Contextual function
    if (nAmount <= 0) {
        strError = "Invalid parameter: masternode amount can't be equal to or less than zero.";
        return false;
    }

    if (message.empty() && nExpireTime > 0) {
        strError = "Invalid parameter: masternode transfer expiration time requires a message to be attached to the transfer";
        return false;
    }

    if (nExpireTime < 0) {
        strError = "Invalid parameter: expiration time must be a positive value";
        return false;
    }

    if (message.size() && !CheckEncoded(message, strError)) {
        return false;
    }

    return true;
}

bool CMasterNodeTransfer::ContextualCheckAgainstVerifyString(CMasterNodesCache *masternodeCache, const std::string& address, std::string& strError) const
{
    // Get the verifier string
    CNullMasterNodeTxVerifierString verifier;
    if (!masternodeCache->GetMasterNodeVerifierStringIfExists(this->strName, verifier, true)) {
        // This shouldn't ever happen, but if it does we need to know
        strError = _("Verifier String doesn't exist for masternode: ") + this->strName;
        return false;
    }

    if (!ContextualCheckVerifierString(masternodeCache, verifier.verifier_string, address, strError))
        return false;

    return true;
}

void CMasterNodeTransfer::ConstructTransaction(CScript& script) const
{
    CDataStream ssTransfer(SER_NETWORK, PROTOCOL_VERSION);
    ssTransfer << *this;

    std::vector<unsigned char> vchMessage;
    vchMessage.push_back(HVN_H); // h
    vchMessage.push_back(HVN_V); // v
    vchMessage.push_back(HVN_N); // n
    vchMessage.push_back(HVN_T); // t

    vchMessage.insert(vchMessage.end(), ssTransfer.begin(), ssTransfer.end());
    script << OP_HVN_MASTERNODE << ToByteVector(vchMessage) << OP_DROP;
}

bool CMasterNodesCache::AddTransferMasterNode(const CMasterNodeTransfer& transferMasterNode, const std::string& address, const COutPoint& out, const CTxOut& txOut)
{
    AddToMasterNodeBalance(transferMasterNode.strName, address, transferMasterNode.nAmount);

    // Add to cache so we can save to database
    CMasterNodeCacheNewTransfer newTransfer(transferMasterNode, address, out);

    if (setNewTransferMasterNodesToRemove.count(newTransfer))
        setNewTransferMasterNodesToRemove.erase(newTransfer);

    setNewTransferMasterNodesToAdd.insert(newTransfer);

    return true;
}

void CMasterNodesCache::AddToMasterNodeBalance(const std::string& strName, const std::string& address, const CAmount& nAmount)
{
    if (fMasterNodeIndex) {
        auto pair = std::make_pair(strName, address);
        // Add to map address -> amount map

        // Get the best amount
        if (!GetBestMasterNodeAddressAmount(*this, strName, address))
            mapMasterNodesAddressAmount.insert(make_pair(pair, 0));

        // Add the new amount to the balance
        if (IsMasterNodeNameAnOwner(strName))
            mapMasterNodesAddressAmount.at(pair) = OWNER_MASTERNODE_AMOUNT;
        else
            mapMasterNodesAddressAmount.at(pair) += nAmount;
    }
}

bool CMasterNodesCache::TrySpendCoin(const COutPoint& out, const CTxOut& txOut)
{
    // Placeholder strings that will get set if you successfully get the transfer or masternode from the script
    std::string address = "";
    std::string masternodeName = "";
    CAmount nAmount = -1;

    // Get the masternode tx data
    int nType = -1;
    bool fIsOwner = false;
    if (txOut.scriptPubKey.IsMasterNodeScript(nType, fIsOwner)) {

        // Get the New MasterNode or Transfer MasterNode from the scriptPubKey
        if (nType == TX_NEW_MASTERNODE && !fIsOwner) {
            CNewMasterNode masternode;
            if (MasterNodeFromScript(txOut.scriptPubKey, masternode, address)) {
                masternodeName = masternode.strName;
                nAmount = masternode.nAmount;
            }
        } else if (nType == TX_TRANSFER_MASTERNODE) {
            CMasterNodeTransfer transfer;
            if (TransferMasterNodeFromScript(txOut.scriptPubKey, transfer, address)) {
                masternodeName = transfer.strName;
                nAmount = transfer.nAmount;
            }
        } else if (nType == TX_NEW_MASTERNODE && fIsOwner) {
            if (!OwnerMasterNodeFromScript(txOut.scriptPubKey, masternodeName, address))
                return error("%s : ERROR Failed to get owner masternode from the OutPoint: %s", __func__,
                             out.ToString());
            nAmount = OWNER_MASTERNODE_AMOUNT;
        } else if (nType == TX_UPDATE_MASTERNODE) {
            CUpdateMasterNode update;
            if (UpdateMasterNodeFromScript(txOut.scriptPubKey, update, address)) {
                masternodeName = update.strName;
                nAmount = update.nAmount;
            }
        }
    } else {
        // If it isn't an masternode tx return true, we only fail if an error occurs
        return true;
    }

    // If we got the address and the masternodeName, proceed to remove it from the database, and in memory objects
    if (address != "" && masternodeName != "") {
        if (fMasterNodeIndex && nAmount > 0) {
            CMasterNodeCacheSpendMasterNode spend(masternodeName, address, nAmount);
            if (GetBestMasterNodeAddressAmount(*this, masternodeName, address)) {
                auto pair = make_pair(masternodeName, address);
                if (mapMasterNodesAddressAmount.count(pair))
                    mapMasterNodesAddressAmount.at(pair) -= nAmount;

                if (mapMasterNodesAddressAmount.at(pair) < 0)
                    mapMasterNodesAddressAmount.at(pair) = 0;

                // Update the cache so we can save to database
                vSpentMasterNodes.push_back(spend);
            }
        }
    } else {
        return error("%s : ERROR Failed to get masternode from the OutPoint: %s", __func__, out.ToString());
    }

    return true;
}

bool CMasterNodesCache::ContainsMasterNode(const CNewMasterNode& masternode)
{
    return CheckIfMasterNodeExists(masternode.strName);
}

bool CMasterNodesCache::ContainsMasterNode(const std::string& masternodeName)
{
    return CheckIfMasterNodeExists(masternodeName);
}

bool CMasterNodesCache::UndoMasterNodeCoin(const Coin& coin, const COutPoint& out)
{
    std::string strAddress = "";
    std::string masternodeName = "";
    CAmount nAmount = 0;

    // Get the masternode tx from the script
    int nType = -1;
    bool fIsOwner = false;
    if(coin.out.scriptPubKey.IsMasterNodeScript(nType, fIsOwner)) {

        if (nType == TX_NEW_MASTERNODE && !fIsOwner) {
            CNewMasterNode masternode;
            if (!MasterNodeFromScript(coin.out.scriptPubKey, masternode, strAddress)) {
                return error("%s : Failed to get masternode from script while trying to undo masternode spend. OutPoint : %s",
                             __func__,
                             out.ToString());
            }
            masternodeName = masternode.strName;

            nAmount = masternode.nAmount;
        } else if (nType == TX_TRANSFER_MASTERNODE) {
            CMasterNodeTransfer transfer;
            if (!TransferMasterNodeFromScript(coin.out.scriptPubKey, transfer, strAddress))
                return error(
                        "%s : Failed to get transfer masternode from script while trying to undo masternode spend. OutPoint : %s",
                        __func__,
                        out.ToString());

            masternodeName = transfer.strName;
            nAmount = transfer.nAmount;
        } else if (nType == TX_NEW_MASTERNODE && fIsOwner) {
            std::string ownerName;
            if (!OwnerMasterNodeFromScript(coin.out.scriptPubKey, ownerName, strAddress))
                return error(
                        "%s : Failed to get owner masternode from script while trying to undo masternode spend. OutPoint : %s",
                        __func__, out.ToString());
            masternodeName = ownerName;
            nAmount = OWNER_MASTERNODE_AMOUNT;
        } else if (nType == TX_UPDATE_MASTERNODE) {
            CUpdateMasterNode update;
            if (!UpdateMasterNodeFromScript(coin.out.scriptPubKey, update, strAddress))
                return error(
                        "%s : Failed to get update masternode from script while trying to undo masternode spend. OutPoint : %s",
                        __func__, out.ToString());
            masternodeName = update.strName;
            nAmount = update.nAmount;
        }
    }

    if (masternodeName == "" || strAddress == "" || nAmount == 0)
        return error("%s : MasterNodeName, Address or nAmount is invalid., MasterNode Name: %s, Address: %s, Amount: %d", __func__, masternodeName, strAddress, nAmount);

    if (!AddBackSpentMasterNode(coin, masternodeName, strAddress, nAmount, out))
        return error("%s : Failed to add back the spent masternode. OutPoint : %s", __func__, out.ToString());

    return true;
}

//! Changes Memory Only
bool CMasterNodesCache::AddBackSpentMasterNode(const Coin& coin, const std::string& masternodeName, const std::string& address, const CAmount& nAmount, const COutPoint& out)
{
    if (fMasterNodeIndex) {
        // Update the masternodes address balance
        auto pair = std::make_pair(masternodeName, address);

        // Get the map address amount from database if the map doesn't have it already
        if (!GetBestMasterNodeAddressAmount(*this, masternodeName, address))
            mapMasterNodesAddressAmount.insert(std::make_pair(pair, 0));

        mapMasterNodesAddressAmount.at(pair) += nAmount;
    }

    // Add the undoAmount to the vector so we know what changes are dirty and what needs to be saved to database
    CMasterNodeCacheUndoMasterNodeAmount undoAmount(masternodeName, address, nAmount);
    vUndoMasterNodeAmount.push_back(undoAmount);

    return true;
}

//! Changes Memory Only
bool CMasterNodesCache::UndoTransfer(const CMasterNodeTransfer& transfer, const std::string& address, const COutPoint& outToRemove)
{
    if (fMasterNodeIndex) {
        // Make sure we are in a valid state to undo the transfer of the masternode
        if (!GetBestMasterNodeAddressAmount(*this, transfer.strName, address))
            return error("%s : Failed to get the masternodes address balance from the database. MasterNode : %s Address : %s",
                         __func__, transfer.strName, address);

        auto pair = std::make_pair(transfer.strName, address);
        if (!mapMasterNodesAddressAmount.count(pair))
            return error(
                    "%s : Tried undoing a transfer and the map of address amount didn't have the masternode address pair. MasterNode : %s Address : %s",
                    __func__, transfer.strName, address);

        if (mapMasterNodesAddressAmount.at(pair) < transfer.nAmount)
            return error(
                    "%s : Tried undoing a transfer and the map of address amount had less than the amount we are trying to undo. MasterNode : %s Address : %s",
                    __func__, transfer.strName, address);

        // Change the in memory balance of the masternode at the address
        mapMasterNodesAddressAmount[pair] -= transfer.nAmount;
    }

    return true;
}

//! Changes Memory Only
bool CMasterNodesCache::RemoveNewMasterNode(const CNewMasterNode& masternode, const std::string address)
{
    if (!CheckIfMasterNodeExists(masternode.strName))
        return error("%s : Tried removing an masternode that didn't exist. MasterNode Name : %s", __func__, masternode.strName);

    CMasterNodeCacheNewMasterNode newMasterNode(masternode, address, 0 , uint256());

    if (setNewMasterNodesToAdd.count(newMasterNode))
        setNewMasterNodesToAdd.erase(newMasterNode);

    setNewMasterNodesToRemove.insert(newMasterNode);

    if (fMasterNodeIndex)
        mapMasterNodesAddressAmount[std::make_pair(masternode.strName, address)] = 0;

    return true;
}

//! Changes Memory Only
bool CMasterNodesCache::AddNewMasterNode(const CNewMasterNode& masternode, const std::string address, const int& nHeight, const uint256& blockHash)
{
    if(CheckIfMasterNodeExists(masternode.strName))
        return error("%s: Tried adding new masternode, but it already existed in the set of masternodes: %s", __func__, masternode.strName);

    CMasterNodeCacheNewMasterNode newMasterNode(masternode, address, nHeight, blockHash);

    if (setNewMasterNodesToRemove.count(newMasterNode))
        setNewMasterNodesToRemove.erase(newMasterNode);

    setNewMasterNodesToAdd.insert(newMasterNode);

    if (fMasterNodeIndex) {
        // Insert the masternode into the assests address amount map
        mapMasterNodesAddressAmount[std::make_pair(masternode.strName, address)] = masternode.nAmount;
    }

    return true;
}

//! Changes Memory Only
bool CMasterNodesCache::AddOwnerMasterNode(const std::string& masternodesName, const std::string address)
{
    // Update the cache
    CMasterNodeCacheNewOwner newOwner(masternodesName, address);

    if (setNewOwnerMasterNodesToRemove.count(newOwner))
        setNewOwnerMasterNodesToRemove.erase(newOwner);

    setNewOwnerMasterNodesToAdd.insert(newOwner);

    if (fMasterNodeIndex) {
        // Insert the masternode into the assests address amount map
        mapMasterNodesAddressAmount[std::make_pair(masternodesName, address)] = OWNER_MASTERNODE_AMOUNT;
    }

    return true;
}

//! Changes Memory Only
bool CMasterNodesCache::RemoveOwnerMasterNode(const std::string& masternodesName, const std::string address)
{
    // Update the cache
    CMasterNodeCacheNewOwner newOwner(masternodesName, address);
    if (setNewOwnerMasterNodesToAdd.count(newOwner))
        setNewOwnerMasterNodesToAdd.erase(newOwner);

    setNewOwnerMasterNodesToRemove.insert(newOwner);

    if (fMasterNodeIndex) {
        auto pair = std::make_pair(masternodesName, address);
        mapMasterNodesAddressAmount[pair] = 0;
    }

    return true;
}

//! Changes Memory Only
bool CMasterNodesCache::RemoveTransfer(const CMasterNodeTransfer &transfer, const std::string &address, const COutPoint &out)
{
    if (!UndoTransfer(transfer, address, out))
        return error("%s : Failed to undo the transfer", __func__);

    CMasterNodeCacheNewTransfer newTransfer(transfer, address, out);
    if (setNewTransferMasterNodesToAdd.count(newTransfer))
        setNewTransferMasterNodesToAdd.erase(newTransfer);

    setNewTransferMasterNodesToRemove.insert(newTransfer);

    return true;
}

bool CMasterNodesCache::DumpCacheToDatabase()
{
    try {
        bool dirty = false;
        std::string message;

        // Remove new masternodes from the database
        for (auto newMasterNode : setNewMasterNodesToRemove) {
            pmasternodesCache->Erase(newMasterNode.masternode.strName);
            if (!pmasternodesdb->EraseMasterNodeData(newMasterNode.masternode.strName)) {
                dirty = true;
                message = "_Failed Erasing New MasterNode Data from database";
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }

            if (!prestricteddb->EraseVerifier(newMasterNode.masternode.strName)) {
                dirty = true;
                message = "_Failed Erasing verifier of new masternode removal data from database";
            }

            if (fMasterNodeIndex) {
                if (!pmasternodesdb->EraseMasterNodeAddressQuantity(newMasterNode.masternode.strName, newMasterNode.address)) {
                    dirty = true;
                    message = "_Failed Erasing Address Balance from database";
                }

                if (!pmasternodesdb->EraseAddressMasterNodeQuantity(newMasterNode.address, newMasterNode.masternode.strName)) {
                    dirty = true;
                    message = "_Failed Erasing New MasterNode Address Balance from AddressMasterNode database";
                }
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }
        }

        // Add the new masternodes to the database
        for (auto newMasterNode : setNewMasterNodesToAdd) {
            pmasternodesCache->Put(newMasterNode.masternode.strName, CDatabasedMasterNodeData(newMasterNode.masternode, newMasterNode.blockHeight, newMasterNode.blockHash));
            if (!pmasternodesdb->WriteMasterNodeData(newMasterNode.masternode, newMasterNode.blockHeight, newMasterNode.blockHash)) {
                dirty = true;
                message = "_Failed Writing New MasterNode Data to database";
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }

            if (fMasterNodeIndex) {
                if (!pmasternodesdb->WriteMasterNodeAddressQuantity(newMasterNode.masternode.strName, newMasterNode.address,
                                                          newMasterNode.masternode.nAmount)) {
                    dirty = true;
                    message = "_Failed Writing Address Balance to database";
                }

                if (!pmasternodesdb->WriteAddressMasterNodeQuantity(newMasterNode.address, newMasterNode.masternode.strName,
                                                          newMasterNode.masternode.nAmount)) {
                    dirty = true;
                    message = "_Failed Writing Address Balance to database";
                }
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }
        }

        if (fMasterNodeIndex) {
            // Remove the new owners from database
            for (auto ownerMasterNode : setNewOwnerMasterNodesToRemove) {
                if (!pmasternodesdb->EraseMasterNodeAddressQuantity(ownerMasterNode.masternodeName, ownerMasterNode.address)) {
                    dirty = true;
                    message = "_Failed Erasing Owner Address Balance from database";
                }

                if (!pmasternodesdb->EraseAddressMasterNodeQuantity(ownerMasterNode.address, ownerMasterNode.masternodeName)) {
                    dirty = true;
                    message = "_Failed Erasing New Owner Address Balance from AddressMasterNode database";
                }

                if (dirty) {
                    return error("%s : %s", __func__, message);
                }
            }

            // Add the new owners to database
            for (auto ownerMasterNode : setNewOwnerMasterNodesToAdd) {
                auto pair = std::make_pair(ownerMasterNode.masternodeName, ownerMasterNode.address);
                if (mapMasterNodesAddressAmount.count(pair) && mapMasterNodesAddressAmount.at(pair) > 0) {
                    if (!pmasternodesdb->WriteMasterNodeAddressQuantity(ownerMasterNode.masternodeName, ownerMasterNode.address,
                                                              mapMasterNodesAddressAmount.at(pair))) {
                        dirty = true;
                        message = "_Failed Writing Owner Address Balance to database";
                    }

                    if (!pmasternodesdb->WriteAddressMasterNodeQuantity(ownerMasterNode.address, ownerMasterNode.masternodeName,
                                                              mapMasterNodesAddressAmount.at(pair))) {
                        dirty = true;
                        message = "_Failed Writing Address Balance to database";
                    }

                    if (dirty) {
                        return error("%s : %s", __func__, message);
                    }
                }
            }

            // Undo the transfering by updating the balances in the database

            for (auto undoTransfer : setNewTransferMasterNodesToRemove) {
                auto pair = std::make_pair(undoTransfer.transfer.strName, undoTransfer.address);
                if (mapMasterNodesAddressAmount.count(pair)) {
                    if (mapMasterNodesAddressAmount.at(pair) == 0) {
                        if (!pmasternodesdb->EraseMasterNodeAddressQuantity(undoTransfer.transfer.strName,
                                                                  undoTransfer.address)) {
                            dirty = true;
                            message = "_Failed Erasing Address Quantity from database";
                        }

                        if (!pmasternodesdb->EraseAddressMasterNodeQuantity(undoTransfer.address,
                                                                  undoTransfer.transfer.strName)) {
                            dirty = true;
                            message = "_Failed Erasing UndoTransfer Address Balance from AddressMasterNode database";
                        }

                        if (dirty) {
                            return error("%s : %s", __func__, message);
                        }
                    } else {
                        if (!pmasternodesdb->WriteMasterNodeAddressQuantity(undoTransfer.transfer.strName,
                                                                  undoTransfer.address,
                                                                  mapMasterNodesAddressAmount.at(pair))) {
                            dirty = true;
                            message = "_Failed Writing updated Address Quantity to database when undoing transfers";
                        }

                        if (!pmasternodesdb->WriteAddressMasterNodeQuantity(undoTransfer.address,
                                                                  undoTransfer.transfer.strName,
                                                                  mapMasterNodesAddressAmount.at(pair))) {
                            dirty = true;
                            message = "_Failed Writing Address Balance to database";
                        }

                        if (dirty) {
                            return error("%s : %s", __func__, message);
                        }
                    }
                }
            }


            // Save the new transfers by updating the quantity in the database
            for (auto newTransfer : setNewTransferMasterNodesToAdd) {
                auto pair = std::make_pair(newTransfer.transfer.strName, newTransfer.address);
                // During init and reindex it disconnects and verifies blocks, can create a state where vNewTransfer will contain transfers that have already been spent. So if they aren't in the map, we can skip them.
                if (mapMasterNodesAddressAmount.count(pair)) {
                    if (!pmasternodesdb->WriteMasterNodeAddressQuantity(newTransfer.transfer.strName, newTransfer.address,
                                                              mapMasterNodesAddressAmount.at(pair))) {
                        dirty = true;
                        message = "_Failed Writing new address quantity to database";
                    }

                    if (!pmasternodesdb->WriteAddressMasterNodeQuantity(newTransfer.address, newTransfer.transfer.strName,
                                                              mapMasterNodesAddressAmount.at(pair))) {
                        dirty = true;
                        message = "_Failed Writing Address Balance to database";
                    }

                    if (dirty) {
                        return error("%s : %s", __func__, message);
                    }
                }
            }
        }

        for (auto newUpdate : setNewUpdateToAdd) {
            auto update_name = newUpdate.update.strName;
            auto pair = make_pair(update_name, newUpdate.address);
            if (mapUpdatedMasterNodeData.count(update_name)) {
                if(!pmasternodesdb->WriteMasterNodeData(mapUpdatedMasterNodeData.at(update_name), newUpdate.blockHeight, newUpdate.blockHash)) {
                    dirty = true;
                    message = "_Failed Writing update masternode data to database";
                }

                if (dirty) {
                    return error("%s : %s", __func__, message);
                }

                pmasternodesCache->Erase(update_name);

                if (fMasterNodeIndex) {

                    if (mapMasterNodesAddressAmount.count(pair) && mapMasterNodesAddressAmount.at(pair) > 0) {
                        if (!pmasternodesdb->WriteMasterNodeAddressQuantity(pair.first, pair.second,
                                                                  mapMasterNodesAddressAmount.at(pair))) {
                            dirty = true;
                            message = "_Failed Writing update masternode quantity to the address quantity database";
                        }

                        if (!pmasternodesdb->WriteAddressMasterNodeQuantity(pair.second, pair.first,
                                                                  mapMasterNodesAddressAmount.at(pair))) {
                            dirty = true;
                            message = "_Failed Writing Address Balance to database";
                        }

                        if (dirty) {
                            return error("%s, %s", __func__, message);
                        }
                    }
                }
            }
        }

        for (auto undoUpdate : setNewUpdateToRemove) {
            // In the case the the create and update are both being removed
            // we can skip this call because the removal of the create should remove all data pertaining the to masternode
            // Fixes the create where the update data will write over the removed masternode meta data that was removed above
            CNewMasterNode masternode(undoUpdate.update.strName, 0);
            CMasterNodeCacheNewMasterNode testNewMasterNodeCache(masternode, "", 0 , uint256());
            if (setNewMasterNodesToRemove.count(testNewMasterNodeCache)) {
                continue;
            }

            auto update_name = undoUpdate.update.strName;
            if (mapUpdatedMasterNodeData.count(update_name)) {
                if(!pmasternodesdb->WriteMasterNodeData(mapUpdatedMasterNodeData.at(update_name), undoUpdate.blockHeight, undoUpdate.blockHash)) {
                    dirty = true;
                    message = "_Failed Writing undo update masternode data to database";
                }

                if (fMasterNodeIndex) {
                    auto pair = make_pair(undoUpdate.update.strName, undoUpdate.address);
                    if (mapMasterNodesAddressAmount.count(pair)) {
                        if (mapMasterNodesAddressAmount.at(pair) == 0) {
                            if (!pmasternodesdb->EraseMasterNodeAddressQuantity(update_name, undoUpdate.address)) {
                                dirty = true;
                                message = "_Failed Erasing Address Balance from database";
                            }

                            if (!pmasternodesdb->EraseAddressMasterNodeQuantity(undoUpdate.address, update_name)) {
                                dirty = true;
                                message = "_Failed Erasing UndoUpdate Balance from AddressMasterNode database";
                            }
                        } else {
                            if (!pmasternodesdb->WriteMasterNodeAddressQuantity(update_name, undoUpdate.address,
                                                                      mapMasterNodesAddressAmount.at(pair))) {
                                dirty = true;
                                message = "_Failed Writing the undo of update of masternode from database";
                            }

                            if (!pmasternodesdb->WriteAddressMasterNodeQuantity(undoUpdate.address, update_name,
                                                                      mapMasterNodesAddressAmount.at(pair))) {
                                dirty = true;
                                message = "_Failed Writing Address Balance to database";
                            }
                        }
                    }
                }

                if (dirty) {
                    return error("%s : %s", __func__, message);
                }

                pmasternodesCache->Erase(update_name);
            }
        }

        // Add new verifier strings for restricted masternodes
        for (auto newVerifier : setNewRestrictedVerifierToAdd) {
            auto masternodeName = newVerifier.masternodeName;
            if (!prestricteddb->WriteVerifier(masternodeName, newVerifier.verifier)) {
                dirty = true;
                message = "_Failed Writing restricted verifier to database";
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }

            pmasternodesVerifierCache->Erase(masternodeName);
        }

        // Undo verifier string for restricted masternodes
        for (auto undoVerifiers : setNewRestrictedVerifierToRemove) {
            auto masternodeName = undoVerifiers.masternodeName;

            // If we are undoing a update, we need to save back the old verifier string to database
            if (undoVerifiers.fUndoingRessiue) {
                if (!prestricteddb->WriteVerifier(masternodeName, undoVerifiers.verifier)) {
                    dirty = true;
                    message = "_Failed Writing undo restricted verifer to database";
                }
            } else {
                if (!prestricteddb->EraseVerifier(masternodeName)) {
                    dirty = true;
                    message = "_Failed Writing undo restricted verifer to database";
                }
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }

            pmasternodesVerifierCache->Erase(masternodeName);
        }

        // Add the new qualifier commands to the database
        for (auto newQualifierAddress : setNewQualifierAddressToAdd) {
            if (newQualifierAddress.type == QualifierType::REMOVE_QUALIFIER) {
                pmasternodesQualifierCache->Erase(newQualifierAddress.GetHash().GetHex());
                if (!prestricteddb->EraseAddressQualifier(newQualifierAddress.address, newQualifierAddress.masternodeName)) {
                    dirty = true;
                    message = "_Failed Erasing address qualifier from database";
                }
                if (fMasterNodeIndex && !dirty) {
                    if (!prestricteddb->EraseQualifierAddress(newQualifierAddress.address,
                                                              newQualifierAddress.masternodeName)) {
                        dirty = true;
                        message = "_Failed Erasing qualifier address from database";
                    }
                }
            } else if (newQualifierAddress.type == QualifierType::ADD_QUALIFIER) {
                pmasternodesQualifierCache->Put(newQualifierAddress.GetHash().GetHex(), 1);
                if (!prestricteddb->WriteAddressQualifier(newQualifierAddress.address, newQualifierAddress.masternodeName))
                {
                    dirty = true;
                    message = "_Failed Writing address qualifier to database";
                }
                if (fMasterNodeIndex & !dirty) {
                    if (!prestricteddb->WriteQualifierAddress(newQualifierAddress.address, newQualifierAddress.masternodeName))
                    {
                        dirty = true;
                        message = "_Failed Writing qualifier address to database";
                    }
                }
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }
        }

        // Undo the qualifier commands
        for (auto undoQualifierAddress : setNewQualifierAddressToRemove) {
            if (undoQualifierAddress.type == QualifierType::REMOVE_QUALIFIER) { // If we are undoing a removal, we write the data to database
                pmasternodesQualifierCache->Put(undoQualifierAddress.GetHash().GetHex(), 1);
                if (!prestricteddb->WriteAddressQualifier(undoQualifierAddress.address, undoQualifierAddress.masternodeName)) {
                    dirty = true;
                    message = "_Failed undoing a removal of a address qualifier  from database";
                }
                if (fMasterNodeIndex & !dirty) {
                    if (!prestricteddb->WriteQualifierAddress(undoQualifierAddress.address, undoQualifierAddress.masternodeName))
                    {
                        dirty = true;
                        message = "_Failed undoing a removal of a qualifier address from database";
                    }
                }
            } else if (undoQualifierAddress.type == QualifierType::ADD_QUALIFIER) { // If we are undoing an addition, we remove the data from the database
                pmasternodesQualifierCache->Erase(undoQualifierAddress.GetHash().GetHex());
                if (!prestricteddb->EraseAddressQualifier(undoQualifierAddress.address, undoQualifierAddress.masternodeName))
                {
                    dirty = true;
                    message = "_Failed undoing a addition of a address qualifier to database";
                }
                if (fMasterNodeIndex && !dirty) {
                    if (!prestricteddb->EraseQualifierAddress(undoQualifierAddress.address,
                                                              undoQualifierAddress.masternodeName)) {
                        dirty = true;
                        message = "_Failed undoing a addition of a qualifier address from database";
                    }
                }
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }
        }

        // Add new restricted address commands
        for (auto newRestrictedAddress : setNewRestrictedAddressToAdd) {
            if (newRestrictedAddress.type == RestrictedType::UNFREEZE_ADDRESS) {
                pmasternodesRestrictionCache->Erase(newRestrictedAddress.GetHash().GetHex());
                if (!prestricteddb->EraseRestrictedAddress(newRestrictedAddress.address, newRestrictedAddress.masternodeName)) {
                    dirty = true;
                    message = "_Failed Erasing restricted address from database";
                }
            } else if (newRestrictedAddress.type == RestrictedType::FREEZE_ADDRESS) {
                pmasternodesRestrictionCache->Put(newRestrictedAddress.GetHash().GetHex(), 1);
                if (!prestricteddb->WriteRestrictedAddress(newRestrictedAddress.address, newRestrictedAddress.masternodeName))
                {
                    dirty = true;
                    message = "_Failed Writing restricted address to database";
                }
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }
        }

        // Undo the qualifier addresses from database
        for (auto undoRestrictedAddress : setNewRestrictedAddressToRemove) {
            if (undoRestrictedAddress.type == RestrictedType::UNFREEZE_ADDRESS) { // If we are undoing an unfreeze, we need to freeze the address
                pmasternodesRestrictionCache->Put(undoRestrictedAddress.GetHash().GetHex(), 1);
                if (!prestricteddb->WriteRestrictedAddress(undoRestrictedAddress.address, undoRestrictedAddress.masternodeName)) {
                    dirty = true;
                    message = "_Failed undoing a removal of a restricted address from database";
                }
            } else if (undoRestrictedAddress.type == RestrictedType::FREEZE_ADDRESS) { // If we are undoing a freeze, we need to unfreeze the address
                pmasternodesRestrictionCache->Erase(undoRestrictedAddress.GetHash().GetHex());
                if (!prestricteddb->EraseRestrictedAddress(undoRestrictedAddress.address, undoRestrictedAddress.masternodeName))
                {
                    dirty = true;
                    message = "_Failed undoing a addition of a restricted address to database";
                }
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }
        }

        // Add new global restriction commands
        for (auto newGlobalRestriction : setNewRestrictedGlobalToAdd) {
            if (newGlobalRestriction.type == RestrictedType::GLOBAL_UNFREEZE) {
                pmasternodesGlobalRestrictionCache->Erase(newGlobalRestriction.masternodeName);
                if (!prestricteddb->EraseGlobalRestriction(newGlobalRestriction.masternodeName)) {
                    dirty = true;
                    message = "_Failed Erasing global restriction from database";
                }
            } else if (newGlobalRestriction.type == RestrictedType::GLOBAL_FREEZE) {
                pmasternodesGlobalRestrictionCache->Put(newGlobalRestriction.masternodeName, 1);
                if (!prestricteddb->WriteGlobalRestriction(newGlobalRestriction.masternodeName))
                {
                    dirty = true;
                    message = "_Failed Writing global restriction to database";
                }
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }
        }

        // Undo the global restriction commands
        for (auto undoGlobalRestriction : setNewRestrictedGlobalToRemove) {
            if (undoGlobalRestriction.type == RestrictedType::GLOBAL_UNFREEZE) { // If we are undoing an global unfreeze, we need to write a global freeze
                pmasternodesGlobalRestrictionCache->Put(undoGlobalRestriction.masternodeName, 1);
                if (!prestricteddb->WriteGlobalRestriction(undoGlobalRestriction.masternodeName)) {
                    dirty = true;
                    message = "_Failed undoing a global unfreeze of a restricted masternode from database";
                }
            } else if (undoGlobalRestriction.type == RestrictedType::GLOBAL_FREEZE) { // If we are undoing a global freeze, erase the freeze from the database
                pmasternodesGlobalRestrictionCache->Erase(undoGlobalRestriction.masternodeName);
                if (!prestricteddb->EraseGlobalRestriction(undoGlobalRestriction.masternodeName))
                {
                    dirty = true;
                    message = "_Failed undoing a global freeze of a restricted masternode to database";
                }
            }

            if (dirty) {
                return error("%s : %s", __func__, message);
            }
        }

        if (fMasterNodeIndex) {
            // Undo the masternode spends by updating there balance in the database
            for (auto undoSpend : vUndoMasterNodeAmount) {
                auto pair = std::make_pair(undoSpend.masternodeName, undoSpend.address);
                if (mapMasterNodesAddressAmount.count(pair)) {
                    if (!pmasternodesdb->WriteMasterNodeAddressQuantity(undoSpend.masternodeName, undoSpend.address,
                                                              mapMasterNodesAddressAmount.at(pair))) {
                        dirty = true;
                        message = "_Failed Writing updated Address Quantity to database when undoing spends";
                    }

                    if (!pmasternodesdb->WriteAddressMasterNodeQuantity(undoSpend.address, undoSpend.masternodeName,
                                                              mapMasterNodesAddressAmount.at(pair))) {
                        dirty = true;
                        message = "_Failed Writing Address Balance to database";
                    }

                    if (dirty) {
                        return error("%s : %s", __func__, message);
                    }
                }
            }


            // Save the masternodes that have been spent by erasing the quantity in the database
            for (auto spentMasterNode : vSpentMasterNodes) {
                auto pair = make_pair(spentMasterNode.masternodeName, spentMasterNode.address);
                if (mapMasterNodesAddressAmount.count(pair)) {
                    if (mapMasterNodesAddressAmount.at(pair) == 0) {
                        if (!pmasternodesdb->EraseMasterNodeAddressQuantity(spentMasterNode.masternodeName, spentMasterNode.address)) {
                            dirty = true;
                            message = "_Failed Erasing a Spent MasterNode, from database";
                        }

                        if (!pmasternodesdb->EraseAddressMasterNodeQuantity(spentMasterNode.address, spentMasterNode.masternodeName)) {
                            dirty = true;
                            message = "_Failed Erasing a Spent MasterNode from AddressMasterNode database";
                        }

                        if (dirty) {
                            return error("%s : %s", __func__, message);
                        }
                    } else {
                        if (!pmasternodesdb->WriteMasterNodeAddressQuantity(spentMasterNode.masternodeName, spentMasterNode.address,
                                                                  mapMasterNodesAddressAmount.at(pair))) {
                            dirty = true;
                            message = "_Failed Erasing a Spent MasterNode, from database";
                        }

                        if (!pmasternodesdb->WriteAddressMasterNodeQuantity(spentMasterNode.address, spentMasterNode.masternodeName,
                                                                  mapMasterNodesAddressAmount.at(pair))) {
                            dirty = true;
                            message = "_Failed Writing Address Balance to database";
                        }

                        if (dirty) {
                            return error("%s : %s", __func__, message);
                        }
                    }
                }
            }
        }

        ClearDirtyCache();

        return true;
    } catch (const std::runtime_error& e) {
        return error("%s : %s ", __func__, std::string("System error while flushing masternodes: ") + e.what());
    }
}

// This function will put all current cache data into the global pmasternodes cache.
//! Do not call this function on the pmasternodes pointer
bool CMasterNodesCache::Flush()
{

    if (!pmasternodes)
        return error("%s: Couldn't find pmasternodes pointer while trying to flush masternodes cache", __func__);

    try {
        for (auto &item : setNewMasterNodesToAdd) {
            if (pmasternodes->setNewMasterNodesToRemove.count(item))
                pmasternodes->setNewMasterNodesToRemove.erase(item);
            pmasternodes->setNewMasterNodesToAdd.insert(item);
        }

        for (auto &item : setNewMasterNodesToRemove) {
            if (pmasternodes->setNewMasterNodesToAdd.count(item))
                pmasternodes->setNewMasterNodesToAdd.erase(item);
            pmasternodes->setNewMasterNodesToRemove.insert(item);
        }

        for (auto &item : mapMasterNodesAddressAmount)
            pmasternodes->mapMasterNodesAddressAmount[item.first] = item.second;

        for (auto &item : mapUpdatedMasterNodeData)
            pmasternodes->mapUpdatedMasterNodeData[item.first] = item.second;

        for (auto &item : setNewOwnerMasterNodesToAdd) {
            if (pmasternodes->setNewOwnerMasterNodesToRemove.count(item))
                pmasternodes->setNewOwnerMasterNodesToRemove.erase(item);
            pmasternodes->setNewOwnerMasterNodesToAdd.insert(item);
        }

        for (auto &item : setNewOwnerMasterNodesToRemove) {
            if (pmasternodes->setNewOwnerMasterNodesToAdd.count(item))
                pmasternodes->setNewOwnerMasterNodesToAdd.erase(item);
            pmasternodes->setNewOwnerMasterNodesToRemove.insert(item);
        }

        for (auto &item : setNewTransferMasterNodesToAdd) {
            if (pmasternodes->setNewTransferMasterNodesToRemove.count(item))
                pmasternodes->setNewTransferMasterNodesToRemove.erase(item);
            pmasternodes->setNewTransferMasterNodesToAdd.insert(item);
        }

        for (auto &item : setNewTransferMasterNodesToRemove) {
            if (pmasternodes->setNewTransferMasterNodesToAdd.count(item))
                pmasternodes->setNewTransferMasterNodesToAdd.erase(item);
            pmasternodes->setNewTransferMasterNodesToRemove.insert(item);
        }

        for (auto &item : vSpentMasterNodes) {
            pmasternodes->vSpentMasterNodes.emplace_back(item);
        }

        for (auto &item : vUndoMasterNodeAmount) {
            pmasternodes->vUndoMasterNodeAmount.emplace_back(item);
        }

        return true;

    } catch (const std::runtime_error& e) {
        return error("%s : %s ", __func__, std::string("System error while flushing masternodes: ") + e.what());
    }
}

//! Get the amount of memory the cache is using
size_t CMasterNodesCache::DynamicMemoryUsage() const
{
    // TODO make sure this is accurate
    return memusage::DynamicUsage(mapMasterNodesAddressAmount) + memusage::DynamicUsage(mapUpdatedMasterNodeData);
}

//! Get an estimated size of the cache in bytes that will be needed inorder to save to database
size_t CMasterNodesCache::GetCacheSize() const
{
    // COutPoint: 32 bytes
    // CNewMasterNode: Max 80 bytes
    // CMasterNodeTransfer: MasterNode Name, CAmount ( 40 bytes)
    // CUpdateMasterNode: Max 80 bytes
    // CAmount: 8 bytes
    // MasterNode Name: Max 32 bytes
    // Address: 40 bytes
    // Block hash: 32 bytes
    // CTxOut: CAmount + CScript (105 + 8 = 113 bytes)

    size_t size = 0;

    size += (32 + 40 + 8) * vUndoMasterNodeAmount.size(); // MasterNode Name, Address, CAmount

    size += (40 + 40 + 32) * setNewTransferMasterNodesToRemove.size(); // CMasterNodeTrasnfer, Address, COutPoint
    size += (40 + 40 + 32) * setNewTransferMasterNodesToAdd.size(); // CMasterNodeTrasnfer, Address, COutPoint

    size += 72 * setNewOwnerMasterNodesToAdd.size(); // MasterNode Name, Address
    size += 72 * setNewOwnerMasterNodesToRemove.size(); // MasterNode Name, Address

    size += (32 + 40 + 8) * vSpentMasterNodes.size(); // MasterNode Name, Address, CAmount

    size += (80 + 40 + 32 + sizeof(int)) * setNewMasterNodesToAdd.size(); // CNewMasterNode, Address, Block hash, int
    size += (80 + 40 + 32 + sizeof(int)) * setNewMasterNodesToRemove.size(); // CNewMasterNode, Address, Block hash, int

    size += (80 + 40 + 32 + 32 + sizeof(int)) * setNewUpdateToAdd.size(); // CUpdateMasterNode, Address, COutPoint, Block hash, int
    size += (80 + 40 + 32 + 32 + sizeof(int)) * setNewUpdateToRemove.size(); // CUpdateMasterNode, Address, COutPoint, Block hash, int

    // TODO add the qualfier, and restricted sets into this calculation

    return size;
}

//! Get an estimated size of the cache in bytes that will be needed inorder to save to database
size_t CMasterNodesCache::GetCacheSizeV2() const
{
    // COutPoint: 32 bytes
    // CNewMasterNode: Max 80 bytes
    // CMasterNodeTransfer: MasterNode Name, CAmount ( 40 bytes)
    // CUpdateMasterNode: Max 80 bytes
    // CAmount: 8 bytes
    // MasterNode Name: Max 32 bytes
    // Address: 40 bytes
    // Block hash: 32 bytes
    // CTxOut: CAmount + CScript (105 + 8 = 113 bytes)

    size_t size = 0;
    size += memusage::DynamicUsage(vUndoMasterNodeAmount);
    size += memusage::DynamicUsage(setNewTransferMasterNodesToRemove);
    size += memusage::DynamicUsage(setNewTransferMasterNodesToAdd);
    size += memusage::DynamicUsage(setNewOwnerMasterNodesToAdd);
    size += memusage::DynamicUsage(setNewOwnerMasterNodesToRemove);
    size += memusage::DynamicUsage(vSpentMasterNodes);
    size += memusage::DynamicUsage(setNewMasterNodesToAdd);
    size += memusage::DynamicUsage(setNewMasterNodesToRemove);
    size += memusage::DynamicUsage(setNewUpdateToAdd);
    size += memusage::DynamicUsage(setNewUpdateToRemove);

    return size;
}

bool CheckCreateDepositTx(const CTxOut& txOut, const MasterNodeType& type, const int numberCreated)
{
    if (type == MasterNodeType::UPDATE || type == MasterNodeType::VOTE || type == MasterNodeType::OWNER || type == MasterNodeType::INVALID)
        return false;

    CAmount burnAmount = 0;
    std::string burnAddress = "";

    // Get the burn address and amount for the type of masternode
    burnAmount = GetDepositAmount(type);
    burnAddress = GetDepositAddress(type);

    // If issuing multiple (unique) masternodes need to burn for each
    burnAmount *= numberCreated;

    // Check if script satisfies the burn amount
    if (!(txOut.nValue == burnAmount))
        return false;

    // Extract the destination
    CTxDestination destination;
    if (!ExtractDestination(txOut.scriptPubKey, destination))
        return false;

    // Verify destination is valid
    if (!IsValidDestination(destination))
        return false;

    // Check destination address is the burn address
    auto strDestination = EncodeDestination(destination);
    if (!(strDestination == burnAddress))
        return false;

    return true;
}

bool CheckCreateDepositTx(const CTxOut& txOut, const MasterNodeType& type)
{
    return CheckCreateDepositTx(txOut, type, 1);
}

bool CheckUpdateDepositTx(const CTxOut& txOut)
{
    // Check the first transaction and verify that the correct HVN Amount
    if (txOut.nValue != GetUpdateMasterNodeDepositAmount())
        return false;

    // Extract the destination
    CTxDestination destination;
    if (!ExtractDestination(txOut.scriptPubKey, destination))
        return false;

    // Verify destination is valid
    if (!IsValidDestination(destination))
        return false;

    // Check destination address is the correct burn address
    if (EncodeDestination(destination) != GetParams().UpdateMasterNodeDepositAddress())
        return false;

    return true;
}

bool CheckCreateDataTx(const CTxOut& txOut)
{
    // Verify 'hvnq' is in the transaction
    CScript scriptPubKey = txOut.scriptPubKey;

    int nStartingIndex = 0;
    return IsScriptNewMasterNode(scriptPubKey, nStartingIndex);
}

bool CheckUpdateDataTx(const CTxOut& txOut)
{
    // Verify 'hvnr' is in the transaction
    CScript scriptPubKey = txOut.scriptPubKey;

    return IsScriptUpdateMasterNode(scriptPubKey);
}

bool CheckOwnerDataTx(const CTxOut& txOut)
{
    // Verify 'hvnq' is in the transaction
    CScript scriptPubKey = txOut.scriptPubKey;

    return IsScriptOwnerMasterNode(scriptPubKey);
}

bool CheckTransferOwnerTx(const CTxOut& txOut)
{
    // Verify 'hvnq' is in the transaction
    CScript scriptPubKey = txOut.scriptPubKey;

    return IsScriptTransferMasterNode(scriptPubKey);
}

bool IsScriptNewMasterNode(const CScript& scriptPubKey)
{
    int index = 0;
    return IsScriptNewMasterNode(scriptPubKey, index);
}

bool IsScriptNewMasterNode(const CScript& scriptPubKey, int& nStartingIndex)
{
    int nType = 0;
    bool fIsOwner =false;
    if (scriptPubKey.IsMasterNodeScript(nType, fIsOwner, nStartingIndex)) {
        return nType == TX_NEW_MASTERNODE && !fIsOwner;
    }
    return false;
}

bool IsScriptNewUniqueMasterNode(const CScript& scriptPubKey)
{
    int index = 0;
    return IsScriptNewUniqueMasterNode(scriptPubKey, index);
}

bool IsScriptNewUniqueMasterNode(const CScript &scriptPubKey, int &nStartingIndex)
{
    int nType = 0;
    bool fIsOwner = false;
    if (!scriptPubKey.IsMasterNodeScript(nType, fIsOwner, nStartingIndex))
        return false;

    CNewMasterNode masternode;
    std::string address;
    if (!MasterNodeFromScript(scriptPubKey, masternode, address))
        return false;

    MasterNodeType masternodeType;
    if (!IsMasterNodeNameValid(masternode.strName, masternodeType))
        return false;

    return MasterNodeType::UNIQUE == masternodeType;
}

bool IsScriptNewMsgChannelMasterNode(const CScript& scriptPubKey)
{
    int index = 0;
    return IsScriptNewMsgChannelMasterNode(scriptPubKey, index);
}

bool IsScriptNewMsgChannelMasterNode(const CScript &scriptPubKey, int &nStartingIndex)
{
    int nType = 0;
    bool fIsOwner = false;
    if (!scriptPubKey.IsMasterNodeScript(nType, fIsOwner, nStartingIndex))
        return false;

    CNewMasterNode masternode;
    std::string address;
    if (!MasterNodeFromScript(scriptPubKey, masternode, address))
        return false;

    MasterNodeType masternodeType;
    if (!IsMasterNodeNameValid(masternode.strName, masternodeType))
        return false;

    return MasterNodeType::MSGCHANNEL == masternodeType;
}

bool IsScriptOwnerMasterNode(const CScript& scriptPubKey)
{

    int index = 0;
    return IsScriptOwnerMasterNode(scriptPubKey, index);
}

bool IsScriptOwnerMasterNode(const CScript& scriptPubKey, int& nStartingIndex)
{
    int nType = 0;
    bool fIsOwner =false;
    if (scriptPubKey.IsMasterNodeScript(nType, fIsOwner, nStartingIndex)) {
        return nType == TX_NEW_MASTERNODE && fIsOwner;
    }

    return false;
}

bool IsScriptUpdateMasterNode(const CScript& scriptPubKey)
{
    int index = 0;
    return IsScriptUpdateMasterNode(scriptPubKey, index);
}

bool IsScriptUpdateMasterNode(const CScript& scriptPubKey, int& nStartingIndex)
{
    int nType = 0;
    bool fIsOwner =false;
    if (scriptPubKey.IsMasterNodeScript(nType, fIsOwner, nStartingIndex)) {
        return nType == TX_UPDATE_MASTERNODE;
    }

    return false;
}

bool IsScriptTransferMasterNode(const CScript& scriptPubKey)
{
    int index = 0;
    return IsScriptTransferMasterNode(scriptPubKey, index);
}

bool IsScriptTransferMasterNode(const CScript& scriptPubKey, int& nStartingIndex)
{
    int nType = 0;
    bool fIsOwner = false;
    if (scriptPubKey.IsMasterNodeScript(nType, fIsOwner, nStartingIndex)) {
        return nType == TX_TRANSFER_MASTERNODE;
    }

    return false;
}

bool IsScriptNewQualifierMasterNode(const CScript& scriptPubKey)
{
    int index = 0;
    return IsScriptNewQualifierMasterNode(scriptPubKey, index);
}

bool IsScriptNewQualifierMasterNode(const CScript &scriptPubKey, int &nStartingIndex)
{
    int nType = 0;
    bool fIsOwner = false;
    if (!scriptPubKey.IsMasterNodeScript(nType, fIsOwner, nStartingIndex))
        return false;

    CNewMasterNode masternode;
    std::string address;
    if (!MasterNodeFromScript(scriptPubKey, masternode, address))
        return false;

    MasterNodeType masternodeType;
    if (!IsMasterNodeNameValid(masternode.strName, masternodeType))
        return false;

    return MasterNodeType::QUALIFIER == masternodeType || MasterNodeType::SUB_QUALIFIER == masternodeType;
}

bool IsScriptNewRestrictedMasterNode(const CScript& scriptPubKey)
{
    int index = 0;
    return IsScriptNewRestrictedMasterNode(scriptPubKey, index);
}

bool IsScriptNewRestrictedMasterNode(const CScript &scriptPubKey, int &nStartingIndex)
{
    int nType = 0;
    bool fIsOwner = false;
    if (!scriptPubKey.IsMasterNodeScript(nType, fIsOwner, nStartingIndex))
        return false;

    CNewMasterNode masternode;
    std::string address;
    if (!MasterNodeFromScript(scriptPubKey, masternode, address))
        return false;

    MasterNodeType masternodeType;
    if (!IsMasterNodeNameValid(masternode.strName, masternodeType))
        return false;

    return MasterNodeType::RESTRICTED == masternodeType;
}


//! Returns a boolean on if the masternode exists
bool CMasterNodesCache::CheckIfMasterNodeExists(const std::string& name, bool fForceDuplicateCheck)
{
    // If we are reindexing, we don't know if an masternode exists when accepting blocks
    if (fReindex) {
        return true;
    }

    // Create objects that will be used to check the dirty cache
    CNewMasterNode masternode;
    masternode.strName = name;
    CMasterNodeCacheNewMasterNode cachedMasterNode(masternode, "", 0, uint256());

    // Check the dirty caches first and see if it was recently added or removed
    if (setNewMasterNodesToRemove.count(cachedMasterNode)) {
        return false;
    }

    // Check the dirty caches first and see if it was recently added or removed
    if (pmasternodes->setNewMasterNodesToRemove.count(cachedMasterNode)) {
        return false;
    }

    if (setNewMasterNodesToAdd.count(cachedMasterNode)) {
        if (fForceDuplicateCheck) {
            return true;
        }
        else {
            LogPrintf("%s : Found masternode %s in setNewMasterNodesToAdd but force duplicate check wasn't true\n", __func__, name);
        }
    }

    if (pmasternodes->setNewMasterNodesToAdd.count(cachedMasterNode)) {
        if (fForceDuplicateCheck) {
            return true;
        }
        else {
            LogPrintf("%s : Found masternode %s in setNewMasterNodesToAdd but force duplicate check wasn't true\n", __func__, name);
        }
    }

    // Check the cache, if it doesn't exist in the cache. Try and read it from database
    if (pmasternodesCache) {
        if (pmasternodesCache->Exists(name)) {
            if (fForceDuplicateCheck) {
                return true;
            }
            else {
                LogPrintf("%s : Found masternode %s in pmasternodesCache but force duplicate check wasn't true\n", __func__, name);
            }
        } else {
            if (pmasternodesdb) {
                CNewMasterNode readMasterNode;
                int nHeight;
                uint256 hash;
                if (pmasternodesdb->ReadMasterNodeData(name, readMasterNode, nHeight, hash)) {
                    pmasternodesCache->Put(readMasterNode.strName, CDatabasedMasterNodeData(readMasterNode, nHeight, hash));
                    if (fForceDuplicateCheck) {
                        return true;
                    }
                    else {
                        LogPrintf("%s : Found masternode %s in pmasternodesdb but force duplicate check wasn't true\n", __func__, name);
                    }
                }
            }
        }
    }
    return false;
}

bool CMasterNodesCache::GetMasterNodeMetaDataIfExists(const std::string &name, CNewMasterNode &masternode)
{
    int height;
    uint256 hash;
    return GetMasterNodeMetaDataIfExists(name, masternode, height, hash);
}

bool CMasterNodesCache::GetMasterNodeMetaDataIfExists(const std::string &name, CNewMasterNode &masternode, int& nHeight, uint256& blockHash)
{
    // Check the map that contains the updated masternode data. If it is in this map, it hasn't been saved to disk yet
    if (mapUpdatedMasterNodeData.count(name)) {
        masternode = mapUpdatedMasterNodeData.at(name);
        return true;
    }

    // Check the map that contains the updated masternode data. If it is in this map, it hasn't been saved to disk yet
    if (pmasternodes->mapUpdatedMasterNodeData.count(name)) {
        masternode = pmasternodes->mapUpdatedMasterNodeData.at(name);
        return true;
    }

    // Create objects that will be used to check the dirty cache
    CNewMasterNode tempMasterNode;
    tempMasterNode.strName = name;
    CMasterNodeCacheNewMasterNode cachedMasterNode(tempMasterNode, "", 0, uint256());

    // Check the dirty caches first and see if it was recently added or removed
    if (setNewMasterNodesToRemove.count(cachedMasterNode)) {
        LogPrintf("%s : Found in new masternodes to Remove - Returning False\n", __func__);
        return false;
    }

    // Check the dirty caches first and see if it was recently added or removed
    if (pmasternodes->setNewMasterNodesToRemove.count(cachedMasterNode)) {
        LogPrintf("%s : Found in new masternodes to Remove - Returning False\n", __func__);
        return false;
    }

    auto setIterator = setNewMasterNodesToAdd.find(cachedMasterNode);
    if (setIterator != setNewMasterNodesToAdd.end()) {
        masternode = setIterator->masternode;
        nHeight = setIterator->blockHeight;
        blockHash = setIterator->blockHash;
        return true;
    }

    setIterator = pmasternodes->setNewMasterNodesToAdd.find(cachedMasterNode);
    if (setIterator != pmasternodes->setNewMasterNodesToAdd.end()) {
        masternode = setIterator->masternode;
        nHeight = setIterator->blockHeight;
        blockHash = setIterator->blockHash;
        return true;
    }

    // Check the cache, if it doesn't exist in the cache. Try and read it from database
    if (pmasternodesCache) {
        if (pmasternodesCache->Exists(name)) {
            CDatabasedMasterNodeData data;
            data = pmasternodesCache->Get(name);
            masternode = data.masternode;
            nHeight = data.nHeight;
            blockHash = data.blockHash;
            return true;
        }
    }

    if (pmasternodesdb && pmasternodesCache) {
        CNewMasterNode readMasterNode;
        int height;
        uint256 hash;
        if (pmasternodesdb->ReadMasterNodeData(name, readMasterNode, height, hash)) {
            masternode = readMasterNode;
            nHeight = height;
            blockHash = hash;
            pmasternodesCache->Put(readMasterNode.strName, CDatabasedMasterNodeData(readMasterNode, height, hash));
            return true;
        }
    }

    LogPrintf("%s : Didn't find masternode meta data anywhere. Returning False\n", __func__);
    return false;
}

bool GetMasterNodeInfoFromScript(const CScript& scriptPubKey, std::string& strName, CAmount& nAmount)
{
    CMasterNodeOutputEntry data;
    if(!GetMasterNodeData(scriptPubKey, data))
        return false;

    strName = data.masternodeName;
    nAmount = data.nAmount;

    return true;
}

bool GetMasterNodeInfoFromCoin(const Coin& coin, std::string& strName, CAmount& nAmount)
{
    return GetMasterNodeInfoFromScript(coin.out.scriptPubKey, strName, nAmount);
}

bool GetMasterNodeData(const CScript& script, CMasterNodeOutputEntry& data)
{
    // Placeholder strings that will get set if you successfully get the transfer or masternode from the script
    std::string address = "";
    std::string masternodeName = "";

    int nType = 0;
    bool fIsOwner = false;
    if (!script.IsMasterNodeScript(nType, fIsOwner)) {
        return false;
    }

    txnouttype type = txnouttype(nType);

    // Get the New MasterNode or Transfer MasterNode from the scriptPubKey
    if (type == TX_NEW_MASTERNODE && !fIsOwner) {
        CNewMasterNode masternode;
        if (MasterNodeFromScript(script, masternode, address)) {
            data.type = TX_NEW_MASTERNODE;
            data.nAmount = masternode.nAmount;
            data.destination = DecodeDestination(address);
            data.masternodeName = masternode.strName;
            return true;
        } else if (MsgChannelMasterNodeFromScript(script, masternode, address)) {
            data.type = TX_NEW_MASTERNODE;
            data.nAmount = masternode.nAmount;
            data.destination = DecodeDestination(address);
            data.masternodeName = masternode.strName;
        } else if (QualifierMasterNodeFromScript(script, masternode, address)) {
            data.type = TX_NEW_MASTERNODE;
            data.nAmount = masternode.nAmount;
            data.destination = DecodeDestination(address);
            data.masternodeName = masternode.strName;
        } else if (RestrictedMasterNodeFromScript(script, masternode, address)) {
            data.type = TX_NEW_MASTERNODE;
            data.nAmount = masternode.nAmount;
            data.destination = DecodeDestination(address);
            data.masternodeName = masternode.strName;
        }
    } else if (type == TX_TRANSFER_MASTERNODE) {
        CMasterNodeTransfer transfer;
        if (TransferMasterNodeFromScript(script, transfer, address)) {
            data.type = TX_TRANSFER_MASTERNODE;
            data.nAmount = transfer.nAmount;
            data.destination = DecodeDestination(address);
            data.masternodeName = transfer.strName;
            data.message = transfer.message;
            data.expireTime = transfer.nExpireTime;
            return true;
        } else {
            LogPrintf("Failed to get transfer from script\n");
        }
    } else if (type == TX_NEW_MASTERNODE && fIsOwner) {
        if (OwnerMasterNodeFromScript(script, masternodeName, address)) {
            data.type = TX_NEW_MASTERNODE;
            data.nAmount = OWNER_MASTERNODE_AMOUNT;
            data.destination = DecodeDestination(address);
            data.masternodeName = masternodeName;
            return true;
        }
    } else if (type == TX_UPDATE_MASTERNODE) {
        CUpdateMasterNode update;
        if (UpdateMasterNodeFromScript(script, update, address)) {
            data.type = TX_UPDATE_MASTERNODE;
            data.nAmount = update.nAmount;
            data.destination = DecodeDestination(address);
            data.masternodeName = update.strName;
            return true;
        }
    }

    return false;
}

#ifdef ENABLE_WALLET
void GetAllAdministrativeMasterNodes(CWallet *pwallet, std::vector<std::string> &names, int nMinConf)
{
    if(!pwallet)
        return;

    GetAllMyMasterNodes(pwallet, names, nMinConf, true, true);
}

void GetAllMyMasterNodes(CWallet* pwallet, std::vector<std::string>& names, int nMinConf, bool fIncludeAdministrator, bool fOnlyAdministrator)
{
    if(!pwallet)
        return;

    std::map<std::string, std::vector<COutput> > mapMasterNodes;
    pwallet->AvailableMasterNodes(mapMasterNodes, true, nullptr, 1, MAX_MONEY, MAX_MONEY, 0, nMinConf); // Set the mincof, set the rest to the defaults

    for (auto item : mapMasterNodes) {
        bool isOwner = IsMasterNodeNameAnOwner(item.first);

        if (isOwner) {
            if (fOnlyAdministrator || fIncludeAdministrator)
                names.emplace_back(item.first);
        } else {
            if (fOnlyAdministrator)
                continue;
            names.emplace_back(item.first);
        }
    }
}
#endif

CAmount GetCreateMasterNodeDepositAmount()
{
    return GetParams().CreateMasterNodeDepositAmount();
}

CAmount GetUpdateMasterNodeDepositAmount()
{
    return GetParams().UpdateMasterNodeDepositAmount();
}

CAmount GetCreateSubMasterNodeDepositAmount()
{
    return GetParams().CreateSubMasterNodeDepositAmount();
}

CAmount GetCreateUniqueMasterNodeDepositAmount()
{
    return GetParams().CreateUniqueMasterNodeDepositAmount();
}

CAmount GetCreateMsgChannelMasterNodeDepositAmount()
{
    return GetParams().CreateMsgChannelMasterNodeDepositAmount();
}

CAmount GetCreateQualifierMasterNodeDepositAmount()
{
    return GetParams().CreateQualifierMasterNodeDepositAmount();
}

CAmount GetCreateSubQualifierMasterNodeDepositAmount()
{
    return GetParams().CreateSubQualifierMasterNodeDepositAmount();
}

CAmount GetCreateRestrictedMasterNodeDepositAmount()
{
    return GetParams().CreateRestrictedMasterNodeDepositAmount();
}

CAmount GetAddNullQualifierTagDepositAmount()
{
    return GetParams().AddNullQualifierTagDepositAmount();
}

CAmount GetDepositAmount(const int nType)
{
    return GetDepositAmount((MasterNodeType(nType)));
}

CAmount GetDepositAmount(const MasterNodeType type)
{
    switch (type) {
        case MasterNodeType::ROOT:
            return GetCreateMasterNodeDepositAmount();
        case MasterNodeType::SUB:
            return GetCreateSubMasterNodeDepositAmount();
        case MasterNodeType::MSGCHANNEL:
            return GetCreateMsgChannelMasterNodeDepositAmount();
        case MasterNodeType::OWNER:
            return 0;
        case MasterNodeType::UNIQUE:
            return GetCreateUniqueMasterNodeDepositAmount();
        case MasterNodeType::VOTE:
            return 0;
        case MasterNodeType::UPDATE:
            return GetUpdateMasterNodeDepositAmount();
        case MasterNodeType::QUALIFIER:
            return GetCreateQualifierMasterNodeDepositAmount();
        case MasterNodeType::SUB_QUALIFIER:
            return GetCreateSubQualifierMasterNodeDepositAmount();
        case MasterNodeType::RESTRICTED:
            return GetCreateRestrictedMasterNodeDepositAmount();
        case MasterNodeType::NULL_ADD_QUALIFIER:
            return GetAddNullQualifierTagDepositAmount();
        default:
            return 0;
    }
}

std::string GetDepositAddress(const int nType)
{
    return GetDepositAddress((MasterNodeType(nType)));
}

std::string GetDepositAddress(const MasterNodeType type)
{
    switch (type) {
        case MasterNodeType::ROOT:
            return GetParams().CreateMasterNodeDepositAddress();
        case MasterNodeType::SUB:
            return GetParams().CreateSubMasterNodeDepositAddress();
        case MasterNodeType::MSGCHANNEL:
            return GetParams().CreateMsgChannelMasterNodeDepositAddress();
        case MasterNodeType::OWNER:
            return "";
        case MasterNodeType::UNIQUE:
            return GetParams().CreateUniqueMasterNodeDepositAddress();
        case MasterNodeType::VOTE:
            return "";
        case MasterNodeType::UPDATE:
            return GetParams().UpdateMasterNodeDepositAddress();
        case MasterNodeType::QUALIFIER:
            return GetParams().CreateQualifierMasterNodeDepositAddress();
        case MasterNodeType::SUB_QUALIFIER:
            return GetParams().CreateSubQualifierMasterNodeDepositAddress();
        case MasterNodeType::RESTRICTED:
            return GetParams().CreateRestrictedMasterNodeDepositAddress();
        case MasterNodeType::NULL_ADD_QUALIFIER:
            return GetParams().AddNullQualifierTagDepositAddress();
        default:
            return "";
    }
}

//! This will get the amount that an address for a certain masternode contains from the database if they cache doesn't already have it
bool GetBestMasterNodeAddressAmount(CMasterNodesCache& cache, const std::string& masternodeName, const std::string& address)
{
    if (fMasterNodeIndex) {
        auto pair = make_pair(masternodeName, address);

        // If the caches map has the pair, return true because the map already contains the best dirty amount
        if (cache.mapMasterNodesAddressAmount.count(pair))
            return true;

        // If the caches map has the pair, return true because the map already contains the best dirty amount
        if (pmasternodes->mapMasterNodesAddressAmount.count(pair)) {
            cache.mapMasterNodesAddressAmount[pair] = pmasternodes->mapMasterNodesAddressAmount.at(pair);
            return true;
        }

        // If the database contains the masternodes address amount, insert it into the database and return true
        CAmount nDBAmount;
        if (pmasternodesdb->ReadMasterNodeAddressQuantity(pair.first, pair.second, nDBAmount)) {
            cache.mapMasterNodesAddressAmount.insert(make_pair(pair, nDBAmount));
            return true;
        }
    }

    // The amount wasn't found return false
    return false;
}

#ifdef ENABLE_WALLET
//! sets _balances_ with the total quantity of each owned masternode
bool GetAllMyMasterNodeBalances(std::map<std::string, std::vector<COutput> >& outputs, std::map<std::string, CAmount>& amounts, const int confirmations, const std::string& prefix) {

    // Return false if no wallet was found to compute masternode balances
    if (!vpwallets.size())
        return false;

    // Get the map of masternodenames to outputs
    vpwallets[0]->AvailableMasterNodes(outputs, true, nullptr, 1, MAX_MONEY, MAX_MONEY, 0, confirmations);

    // Loop through all pairs of MasterNode Name -> vector<COutput>
    for (const auto& pair : outputs) {
        if (prefix.empty() || pair.first.find(prefix) == 0) { // Check for prefix
            CAmount balance = 0;
            for (auto txout : pair.second) { // Compute balance of masternode by summing all Available Outputs
                CMasterNodeOutputEntry data;
                if (GetMasterNodeData(txout.tx->tx->vout[txout.i].scriptPubKey, data))
                    balance += data.nAmount;
            }
            amounts.insert(std::make_pair(pair.first, balance));
        }
    }

    return true;
}

bool GetMyMasterNodeBalance(const std::string& name, CAmount& balance, const int& confirmations) {

    // Return false if no wallet was found to compute masternode balances
    if (!vpwallets.size())
        return false;

    // Get the map of masternodenames to outputs
    std::map<std::string, std::vector<COutput> > outputs;
    vpwallets[0]->AvailableMasterNodes(outputs, true, nullptr, 1, MAX_MONEY, MAX_MONEY, 0, confirmations);

    // Loop through all pairs of MasterNode Name -> vector<COutput>
    if (outputs.count(name)) {
        auto& ref = outputs.at(name);
        for (const auto& txout : ref) {
            CMasterNodeOutputEntry data;
            if (GetMasterNodeData(txout.tx->tx->vout[txout.i].scriptPubKey, data)) {
                balance += data.nAmount;
            }
        }
    }

    return true;
}
#endif

// 46 char base58 --> 34 char KAW compatible
std::string DecodeMasterNodeData(std::string encoded)
{
    if (encoded.size() == 46) {
        std::vector<unsigned char> b;
        DecodeBase58(encoded, b);
        return std::string(b.begin(), b.end());
    }

    else if (encoded.size() == 64 && IsHex(encoded)) {
        std::vector<unsigned char> vec = ParseHex(encoded);
        return std::string(vec.begin(), vec.end());
    }

    return "";

};

std::string EncodeMasterNodeData(std::string decoded)
{
    if (decoded.size() == 34) {
        return EncodeIPFS(decoded);
    }
    else if (decoded.size() == 32){
        return HexStr(decoded);
    }

    return "";
}

// 46 char base58 --> 34 char KAW compatible
std::string DecodeIPFS(std::string encoded)
{
    std::vector<unsigned char> b;
    DecodeBase58(encoded, b);
    return std::string(b.begin(), b.end());
};

// 34 char KAW compatible --> 46 char base58
std::string EncodeIPFS(std::string decoded){
    std::vector<char> charData(decoded.begin(), decoded.end());
    std::vector<unsigned char> unsignedCharData;
    for (char c : charData)
        unsignedCharData.push_back(static_cast<unsigned char>(c));
    return EncodeBase58(unsignedCharData);
};

#ifdef ENABLE_WALLET
bool CreateMasterNodeTransaction(CWallet* pwallet, CCoinControl& coinControl, const CNewMasterNode& masternode, const std::string& address, std::pair<int, std::string>& error, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRequired, std::string* verifier_string)
{
    std::vector<CNewMasterNode> masternodes;
    masternodes.push_back(masternode);
    return CreateMasterNodeTransaction(pwallet, coinControl, masternodes, address, error, wtxNew, reservekey, nFeeRequired, verifier_string);
}

bool CreateMasterNodeTransaction(CWallet* pwallet, CCoinControl& coinControl, const std::vector<CNewMasterNode> masternodes, const std::string& address, std::pair<int, std::string>& error, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRequired, std::string* verifier_string)
{
    std::string change_address = EncodeDestination(coinControl.destChange);

    auto currentActiveMasterNodeCache = GetCurrentMasterNodeCache();
    // Validate the masternodes data
    std::string strError;
    for (auto masternode : masternodes) {
        if (!ContextualCheckNewMasterNode(currentActiveMasterNodeCache, masternode, strError)) {
            error = std::make_pair(RPC_INVALID_PARAMETER, strError);
            return false;
        }
    }

    if (!change_address.empty()) {
        CTxDestination destination = DecodeDestination(change_address);
        if (!IsValidDestination(destination)) {
            error = std::make_pair(RPC_INVALID_ADDRESS_OR_KEY, std::string("Invalid Hive address: ") + change_address);
            return false;
        }
    } else {
        // no coin control: send change to newly generated address
        CKeyID keyID;
        std::string strFailReason;
        if (!pwallet->CreateNewChangeAddress(reservekey, keyID, strFailReason)) {
            error = std::make_pair(RPC_WALLET_KEYPOOL_RAN_OUT, strFailReason);
            return false;
        }

        change_address = EncodeDestination(keyID);
        coinControl.destChange = DecodeDestination(change_address);
    }

    MasterNodeType masternodeType;
    std::string parentName;
    for (auto masternode : masternodes) {
        if (!IsMasterNodeNameValid(masternode.strName, masternodeType)) {
            error = std::make_pair(RPC_INVALID_PARAMETER, "MasterNode name not valid");
            return false;
        }
        if (masternodes.size() > 1 && masternodeType != MasterNodeType::UNIQUE) {
            error = std::make_pair(RPC_INVALID_PARAMETER, "Only unique masternodes can be created in bulk.");
            return false;
        }
        std::string parent = GetParentName(masternode.strName);
        if (parentName.empty())
            parentName = parent;
        if (parentName != parent) {
            error = std::make_pair(RPC_INVALID_PARAMETER, "All masternodes must have the same parent.");
            return false;
        }
    }

    // Assign the correct burn amount and the correct burn address depending on the type of masternode issuance that is happening
    CAmount burnAmount = GetDepositAmount(masternodeType) * masternodes.size();
    CScript scriptPubKey = GetScriptForDestination(DecodeDestination(GetDepositAddress(masternodeType)));

    CAmount curBalance = pwallet->GetBalance();

    // Check to make sure the wallet has the HVN required by the burnAmount
    if (curBalance < burnAmount) {
        error = std::make_pair(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");
        return false;
    }

    if (pwallet->GetBroadcastTransactions() && !g_connman) {
        error = std::make_pair(RPC_CLIENT_P2P_DISABLED, "Error: Peer-to-peer functionality missing or disabled");
        return false;
    }

    LOCK2(cs_main, pwallet->cs_wallet);

    // Create and send the transaction
    std::string strTxError;
    std::vector<CRecipient> vecSend;
    int nChangePosRet = -1;
    bool fSubtractFeeFromAmount = false;

    CRecipient recipient = {scriptPubKey, burnAmount, fSubtractFeeFromAmount};
    vecSend.push_back(recipient);

    // If the masternode is a submasternode or unique masternode. We need to send the ownertoken change back to ourselfs
    if (masternodeType == MasterNodeType::SUB || masternodeType == MasterNodeType::UNIQUE || masternodeType == MasterNodeType::MSGCHANNEL) {
        // Get the script for the destination address for the masternodes
        CScript scriptTransferOwnerMasterNode = GetScriptForDestination(DecodeDestination(change_address));

        CMasterNodeTransfer masternodeTransfer(parentName + OWNER_TAG, OWNER_MASTERNODE_AMOUNT);
        masternodeTransfer.ConstructTransaction(scriptTransferOwnerMasterNode);
        CRecipient rec = {scriptTransferOwnerMasterNode, 0, fSubtractFeeFromAmount};
        vecSend.push_back(rec);
    }

    // If the masternode is a sub qualifier. We need to send the token parent change back to ourselfs
    if (masternodeType == MasterNodeType::SUB_QUALIFIER) {
        // Get the script for the destination address for the masternodes
        CScript scriptTransferQualifierMasterNode = GetScriptForDestination(DecodeDestination(change_address));

        CMasterNodeTransfer masternodeTransfer(parentName, OWNER_MASTERNODE_AMOUNT);
        masternodeTransfer.ConstructTransaction(scriptTransferQualifierMasterNode);
        CRecipient rec = {scriptTransferQualifierMasterNode, 0, fSubtractFeeFromAmount};
        vecSend.push_back(rec);
    }

    // Get the owner outpoints if this is a submasternode or unique masternode
    if (masternodeType == MasterNodeType::SUB || masternodeType == MasterNodeType::UNIQUE || masternodeType == MasterNodeType::MSGCHANNEL) {
        // Verify that this wallet is the owner for the masternode, and get the owner masternode outpoint
        for (auto masternode : masternodes) {
            if (!VerifyWalletHasMasterNode(parentName + OWNER_TAG, error)) {
                return false;
            }
        }
    }

    // Get the owner outpoints if this is a sub_qualifier masternode
    if (masternodeType == MasterNodeType::SUB_QUALIFIER) {
        // Verify that this wallet is the owner for the masternode, and get the owner masternode outpoint
        for (auto masternode : masternodes) {
            if (!VerifyWalletHasMasterNode(parentName, error)) {
                return false;
            }
        }
    }

    if (masternodeType == MasterNodeType::RESTRICTED) {
        // Restricted masternodes require the ROOT! token to be sent with the issuance
        CScript scriptTransferOwnerMasterNode = GetScriptForDestination(DecodeDestination(change_address));

        // Create a transaction that sends the ROOT owner token (e.g. $TOKEN requires TOKEN!)
        std::string strStripped = parentName.substr(1, parentName.size() - 1);

        // Verify that this wallet is the owner for the masternode, and get the owner masternode outpoint
        if (!VerifyWalletHasMasterNode(strStripped + OWNER_TAG, error)) {
            return false;
        }

        CMasterNodeTransfer masternodeTransfer(strStripped + OWNER_TAG, OWNER_MASTERNODE_AMOUNT);
        masternodeTransfer.ConstructTransaction(scriptTransferOwnerMasterNode);

        CRecipient ownerRec = {scriptTransferOwnerMasterNode, 0, fSubtractFeeFromAmount};
        vecSend.push_back(ownerRec);

        // Every restricted masternode issuance must have a verifier string
        if (!verifier_string) {
            error = std::make_pair(RPC_INVALID_PARAMETER, "Error: Verifier string not found");
            return false;
        }

        // Create the masternode null data transaction that will get added to the create transaction
        CScript verifierScript;
        CNullMasterNodeTxVerifierString verifier(*verifier_string);
        verifier.ConstructTransaction(verifierScript);

        CRecipient rec = {verifierScript, 0, false};
        vecSend.push_back(rec);
    }

    if (!pwallet->CreateTransactionWithMasterNodes(vecSend, wtxNew, reservekey, nFeeRequired, nChangePosRet, strTxError, coinControl, masternodes, DecodeDestination(address), masternodeType)) {
        if (!fSubtractFeeFromAmount && burnAmount + nFeeRequired > curBalance)
            strTxError = strprintf("Error: This transaction requires a transaction fee of at least %s", FormatMoney(nFeeRequired));
        error = std::make_pair(RPC_WALLET_ERROR, strTxError);
        return false;
    }
    return true;
}

bool CreateUpdateMasterNodeTransaction(CWallet* pwallet, CCoinControl& coinControl, const CUpdateMasterNode& updateMasterNode, const std::string& address, std::pair<int, std::string>& error, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRequired, std::string* verifier_string)
{
    // Create transaction variables
    std::string strTxError;
    std::vector<CRecipient> vecSend;
    int nChangePosRet = -1;
    bool fSubtractFeeFromAmount = false;

    // Create masternode variables
    std::string masternode_name = updateMasterNode.strName;
    std::string change_address = EncodeDestination(coinControl.destChange);

    // Get the masternode type
    MasterNodeType masternode_type = MasterNodeType::INVALID;
    IsMasterNodeNameValid(masternode_name, masternode_type);

    // Check that validitity of the address
    if (!IsValidDestinationString(address)) {
        error = std::make_pair(RPC_INVALID_ADDRESS_OR_KEY, std::string("Invalid Hive address: ") + address);
        return false;
    }

    // Build the change address
    if (!change_address.empty()) {
        CTxDestination destination = DecodeDestination(change_address);
        if (!IsValidDestination(destination)) {
            error = std::make_pair(RPC_INVALID_ADDRESS_OR_KEY, std::string("Invalid Hive address: ") + change_address);
            return false;
        }
    } else {
        CKeyID keyID;
        std::string strFailReason;
        if (!pwallet->CreateNewChangeAddress(reservekey, keyID, strFailReason)) {
            error = std::make_pair(RPC_WALLET_KEYPOOL_RAN_OUT, strFailReason);
            return false;
        }

        change_address = EncodeDestination(keyID);
        coinControl.destChange = DecodeDestination(change_address);
    }

    // Check the masternodes name
    if (!IsMasterNodeNameValid(masternode_name)) {
        error = std::make_pair(RPC_INVALID_PARAMS, std::string("Invalid masternode name: ") + masternode_name);
        return false;
    }

    // Check to make sure this isn't an owner token
    if (IsMasterNodeNameAnOwner(masternode_name)) {
        error = std::make_pair(RPC_INVALID_PARAMS, std::string("Owner MasterNodes are not able to be updated"));
        return false;
    }

    // pmasternodes and pmasternodesCache need to be initialized
    auto currentActiveMasterNodeCache = GetCurrentMasterNodeCache();
    if (!currentActiveMasterNodeCache) {
        error = std::make_pair(RPC_DATABASE_ERROR, std::string("pmasternodes isn't initialized"));
        return false;
    }

    // Fail if the masternode cache isn't initialized
    if (!pmasternodesCache) {
        error = std::make_pair(RPC_DATABASE_ERROR,
                               std::string("pmasternodesCache isn't initialized"));
        return false;
    }

    // Check to make sure that the update masternode data is valid
    std::string strError;
    if (!ContextualCheckUpdateMasterNode(currentActiveMasterNodeCache, updateMasterNode, strError)) {
        error = std::make_pair(RPC_VERIFY_ERROR,
                               std::string("Failed to create update masternode object. Error: ") + strError);
        return false;
    }

    // strip of the first character of the masternode name, this is used for restricted masternodes only
    std::string stripped_masternode_name = masternode_name.substr(1, masternode_name.size() - 1);

    // If we are reissuing a restricted masternode, check to see if we have the root owner token $TOKEN check for TOKEN!
    if (masternode_type == MasterNodeType::RESTRICTED) {
        // Verify that this wallet is the owner for the masternode, and get the owner masternode outpoint
        if (!VerifyWalletHasMasterNode(stripped_masternode_name + OWNER_TAG, error)) {
            return false;
        }
    } else {
        // Verify that this wallet is the owner for the masternode, and get the owner masternode outpoint
        if (!VerifyWalletHasMasterNode(masternode_name + OWNER_TAG, error)) {
            return false;
        }
    }

    // Check the wallet balance
    CAmount curBalance = pwallet->GetBalance();

    // Get the current burn amount for issuing an masternode
    CAmount burnAmount = GetUpdateMasterNodeDepositAmount();

    // Check to make sure the wallet has the HVN required by the burnAmount
    if (curBalance < burnAmount) {
        error = std::make_pair(RPC_WALLET_INSUFFICIENT_FUNDS, "Insufficient funds");
        return false;
    }

    if (pwallet->GetBroadcastTransactions() && !g_connman) {
        error = std::make_pair(RPC_CLIENT_P2P_DISABLED, "Error: Peer-to-peer functionality missing or disabled");
        return false;
    }

    // Get the script for the destination address for the masternodes
    CScript scriptTransferOwnerMasterNode = GetScriptForDestination(DecodeDestination(change_address));

    if (masternode_type == MasterNodeType::RESTRICTED) {
        CMasterNodeTransfer masternodeTransfer(stripped_masternode_name + OWNER_TAG, OWNER_MASTERNODE_AMOUNT);
        masternodeTransfer.ConstructTransaction(scriptTransferOwnerMasterNode);
    } else {
        CMasterNodeTransfer masternodeTransfer(masternode_name + OWNER_TAG, OWNER_MASTERNODE_AMOUNT);
        masternodeTransfer.ConstructTransaction(scriptTransferOwnerMasterNode);
    }

    if (masternode_type == MasterNodeType::RESTRICTED) {
        // If we are changing the verifier string, check to make sure the new address meets the new verifier string rules
        if (verifier_string) {
            if (updateMasterNode.nAmount > 0) {
                std::string strError = "";
                ErrorReport report;
                if (!ContextualCheckVerifierString(pmasternodes, *verifier_string, address, strError, &report)) {
                    error = std::make_pair(RPC_INVALID_PARAMETER, strError);
                    return false;
                }
            } else {
                // If we aren't adding any masternodes but we are changing the verifier string, Check to make sure the verifier string parses correctly
                std::string strError = "";
                if (!ContextualCheckVerifierString(pmasternodes, *verifier_string, "", strError)) {
                    error = std::make_pair(RPC_INVALID_PARAMETER, strError);
                    return false;
                }
            }
        } else {
            // If the user is reissuing more masternodes, and they aren't changing the verifier string, check it against the current verifier string
            if (updateMasterNode.nAmount > 0) {
                CNullMasterNodeTxVerifierString verifier;
                if (!pmasternodes->GetMasterNodeVerifierStringIfExists(updateMasterNode.strName, verifier)) {
                    error = std::make_pair(RPC_DATABASE_ERROR, "Failed to get the masternodes cache pointer");
                    return false;
                }

                std::string strError = "";
                if (!ContextualCheckVerifierString(pmasternodes, verifier.verifier_string, address, strError)) {
                    error = std::make_pair(RPC_INVALID_PARAMETER, strError);
                    return false;
                }
            }
        }

        // Every restricted masternode issuance must have a verifier string
        if (verifier_string) {
            // Create the masternode null data transaction that will get added to the create transaction
            CScript verifierScript;
            CNullMasterNodeTxVerifierString verifier(*verifier_string);
            verifier.ConstructTransaction(verifierScript);

            CRecipient rec = {verifierScript, 0, false};
            vecSend.push_back(rec);
        }
    }

    // Get the script for the burn address
    CScript scriptPubKeyDeposit = GetScriptForDestination(DecodeDestination(GetParams().UpdateMasterNodeDepositAddress()));

    // Create and send the transaction
    CRecipient recipient = {scriptPubKeyDeposit, burnAmount, fSubtractFeeFromAmount};
    CRecipient recipient2 = {scriptTransferOwnerMasterNode, 0, fSubtractFeeFromAmount};
    vecSend.push_back(recipient);
    vecSend.push_back(recipient2);
    if (!pwallet->CreateTransactionWithUpdateMasterNode(vecSend, wtxNew, reservekey, nFeeRequired, nChangePosRet, strTxError, coinControl, updateMasterNode, DecodeDestination(address))) {
        if (!fSubtractFeeFromAmount && burnAmount + nFeeRequired > curBalance)
            strTxError = strprintf("Error: This transaction requires a transaction fee of at least %s", FormatMoney(nFeeRequired));
        error = std::make_pair(RPC_WALLET_ERROR, strTxError);
        return false;
    }
    return true;
}


// nullMasterNodeTxData -> Use this for freeze/unfreeze an address or adding a qualifier to an address
// nullGlobalRestrictionData -> Use this to globally freeze/unfreeze a restricted masternode.
bool CreateTransferMasterNodeTransaction(CWallet* pwallet, const CCoinControl& coinControl, const std::vector< std::pair<CMasterNodeTransfer, std::string> >vTransfers, const std::string& changeAddress, std::pair<int, std::string>& error, CWalletTx& wtxNew, CReserveKey& reservekey, CAmount& nFeeRequired, std::vector<std::pair<CNullMasterNodeTxData, std::string> >* nullMasterNodeTxData, std::vector<CNullMasterNodeTxData>* nullGlobalRestrictionData)
{
    // Initialize Values for transaction
    std::string strTxError;
    std::vector<CRecipient> vecSend;
    int nChangePosRet = -1;
    bool fSubtractFeeFromAmount = false;

    // Check for a balance before processing transfers
    CAmount curBalance = pwallet->GetBalance();
    if (curBalance == 0) {
        error = std::make_pair(RPC_WALLET_INSUFFICIENT_FUNDS, std::string("This wallet doesn't contain any HVN, transfering an masternode requires a network fee"));
        return false;
    }

    // Check for peers and connections
    if (pwallet->GetBroadcastTransactions() && !g_connman) {
        error = std::make_pair(RPC_CLIENT_P2P_DISABLED, "Error: Peer-to-peer functionality missing or disabled");
        return false;
    }

    // Loop through all transfers and create scriptpubkeys for them
    for (auto transfer : vTransfers) {
        std::string address = transfer.second;
        std::string masternode_name = transfer.first.strName;
        std::string message = transfer.first.message;
        CAmount nAmount = transfer.first.nAmount;
        int64_t expireTime = transfer.first.nExpireTime;

        if (!IsValidDestinationString(address)) {
            error = std::make_pair(RPC_INVALID_ADDRESS_OR_KEY, std::string("Invalid Hive address: ") + address);
            return false;
        }
        auto currentActiveMasterNodeCache = GetCurrentMasterNodeCache();
        if (!currentActiveMasterNodeCache) {
            error = std::make_pair(RPC_DATABASE_ERROR, std::string("pmasternodes isn't initialized"));
            return false;
        }

        if (!VerifyWalletHasMasterNode(masternode_name, error)) // Sets error if it fails
            return false;

        // If it is an ownership transfer, make a quick check to make sure the amount is 1
        if (IsMasterNodeNameAnOwner(masternode_name)) {
            if (nAmount != OWNER_MASTERNODE_AMOUNT) {
                error = std::make_pair(RPC_INVALID_PARAMS, std::string(
                        _("When transferring an 'Ownership MasterNode' the amount must always be 1. Please try again with the amount of 1")));
                return false;
            }
        }

        // If the masternode is a restricted masternode, check the verifier script
        if(IsMasterNodeNameAnRestricted(masternode_name)) {
            std::string strError = "";

            // Check for global restriction
            if (pmasternodes->CheckForGlobalRestriction(transfer.first.strName, true)) {
                error = std::make_pair(RPC_INVALID_PARAMETER, _("Unable to transfer restricted masternode, this restricted masternode has been globally frozen"));
                return false;
            }

            if (!transfer.first.ContextualCheckAgainstVerifyString(pmasternodes, address, strError)) {
                error = std::make_pair(RPC_INVALID_PARAMETER, strError);
                return false;
            }

            if (!coinControl.masternodeDestChange.empty()) {
                std::string change_address = EncodeDestination(coinControl.masternodeDestChange);
                // If this is a transfer of a restricted masternode, check the destination address against the verifier string
                CNullMasterNodeTxVerifierString verifier;
                if (!pmasternodes->GetMasterNodeVerifierStringIfExists(masternode_name, verifier)) {
                    error = std::make_pair(RPC_DATABASE_ERROR, _("Unable to get restricted masternodes verifier string. Database out of sync. Reindex required"));
                    return false;
                }

                if (!ContextualCheckVerifierString(pmasternodes, verifier.verifier_string, change_address, strError)) {
                    error = std::make_pair(RPC_DATABASE_ERROR, std::string(_("Change address can not be sent to because it doesn't have the correct qualifier tags ") + strError));
                    return false;
                }
            }
        }

        // Get the script for the burn address
        CScript scriptPubKey = GetScriptForDestination(DecodeDestination(address));

        // Update the scriptPubKey with the transfer masternode information
        CMasterNodeTransfer masternodeTransfer(masternode_name, nAmount, message, expireTime);
        masternodeTransfer.ConstructTransaction(scriptPubKey);

        CRecipient recipient = {scriptPubKey, 0, fSubtractFeeFromAmount};
        vecSend.push_back(recipient);
    }

    // If masternodeTxData is not nullptr, the user wants to add some OP_HVN_MASTERNODE data transactions into the transaction
    if (nullMasterNodeTxData) {
        std::string strError = "";
        int nAddTagCount = 0;
        for (auto pair : *nullMasterNodeTxData) {

            if (IsMasterNodeNameAQualifier(pair.first.masternode_name)) {
                if (!VerifyQualifierChange(*pmasternodes, pair.first, pair.second, strError)) {
                    error = std::make_pair(RPC_INVALID_REQUEST, strError);
                    return false;
                }
                if (pair.first.flag == (int)QualifierType::ADD_QUALIFIER)
                    nAddTagCount++;
            } else if (IsMasterNodeNameAnRestricted(pair.first.masternode_name)) {
                if (!VerifyRestrictedAddressChange(*pmasternodes, pair.first, pair.second, strError)) {
                    error = std::make_pair(RPC_INVALID_REQUEST, strError);
                    return false;
                }
            }

            CScript dataScript = GetScriptForNullMasterNodeDataDestination(DecodeDestination(pair.second));
            pair.first.ConstructTransaction(dataScript);

            CRecipient recipient = {dataScript, 0, false};
            vecSend.push_back(recipient);
        }

        // Add the burn recipient for adding tags to addresses
        if (nAddTagCount) {
            CScript addTagDepositScript = GetScriptForDestination(DecodeDestination(GetDepositAddress(MasterNodeType::NULL_ADD_QUALIFIER)));
            CRecipient addTagDepositRecipient = {addTagDepositScript, GetDepositAmount(MasterNodeType::NULL_ADD_QUALIFIER) * nAddTagCount, false};
            vecSend.push_back(addTagDepositRecipient);
        }
    }

    // nullGlobalRestiotionData, the user wants to add OP_HVN_MASTERNODE OP_HVN_MASTERNODE OP_HVN_MASTERNODES data transaction to the transaction
    if (nullGlobalRestrictionData) {
        std::string strError = "";
        for (auto dataObject : *nullGlobalRestrictionData) {

            if (!VerifyGlobalRestrictedChange(*pmasternodes, dataObject, strError)) {
                error = std::make_pair(RPC_INVALID_REQUEST, strError);
                return false;
            }

            CScript dataScript;
            dataObject.ConstructGlobalRestrictionTransaction(dataScript);
            CRecipient recipient = {dataScript, 0, false};
            vecSend.push_back(recipient);
        }
    }

    // Create and send the transaction
    if (!pwallet->CreateTransactionWithTransferMasterNode(vecSend, wtxNew, reservekey, nFeeRequired, nChangePosRet, strTxError, coinControl)) {
        if (!fSubtractFeeFromAmount && nFeeRequired > curBalance) {
            error = std::make_pair(RPC_WALLET_ERROR, strprintf("Error: This transaction requires a transaction fee of at least %s", FormatMoney(nFeeRequired)));
            return false;
        }
        error = std::make_pair(RPC_TRANSACTION_ERROR, strTxError);
        return false;
    }
    return true;
}

bool SendMasterNodeTransaction(CWallet* pwallet, CWalletTx& transaction, CReserveKey& reserveKey, std::pair<int, std::string>& error, std::string& txid)
{
    CValidationState state;
    if (!pwallet->CommitTransaction(transaction, reserveKey, g_connman.get(), state)) {
        error = std::make_pair(RPC_WALLET_ERROR, strprintf("Error: The transaction was rejected! Reason given: %s", state.GetRejectReason()));
        return false;
    }

    txid = transaction.GetHash().GetHex();
    return true;
}

bool VerifyWalletHasMasterNode(const std::string& masternode_name, std::pair<int, std::string>& pairError)
{
    CWallet* pwallet;
    if (vpwallets.size() > 0)
        pwallet = vpwallets[0];
    else {
        pairError = std::make_pair(RPC_WALLET_ERROR, strprintf("Wallet not found. Can't verify if it contains: %s", masternode_name));
        return false;
    }

    std::vector<COutput> vCoins;
    std::map<std::string, std::vector<COutput> > mapMasterNodeCoins;
    pwallet->AvailableMasterNodes(mapMasterNodeCoins);

    if (mapMasterNodeCoins.count(masternode_name))
        return true;

    pairError = std::make_pair(RPC_INVALID_REQUEST, strprintf("Wallet doesn't have masternode: %s", masternode_name));
    return false;
}

#endif

// Return true if the amount is valid with the units passed in
bool CheckAmountWithUnits(const CAmount& nAmount, const int8_t nUnits)
{
    return nAmount % int64_t(pow(10, (MAX_UNIT - nUnits))) == 0;
}

bool CheckEncoded(const std::string& hash, std::string& strError) {
    std::string encodedStr = EncodeMasterNodeData(hash);
    if (encodedStr.substr(0, 2) == "Qm" && encodedStr.size() == 46) {
        return true;
    }

    if (AreMessagesDeployed()) {
        if (IsHex(encodedStr) && encodedStr.length() == 64) {
            return true;
        }
    }

    strError = _("Invalid parameter: ipfs_hash is not valid, or txid hash is not the right length");

    return false;
}

void GetTxOutMasterNodeTypes(const std::vector<CTxOut>& vout, int& creates, int& updates, int& transfers, int& owners)
{
    for (auto out: vout) {
        int type;
        bool fIsOwner;
        if (out.scriptPubKey.IsMasterNodeScript(type, fIsOwner)) {
            if (type == TX_NEW_MASTERNODE && !fIsOwner)
                creates++;
            else if (type == TX_NEW_MASTERNODE && fIsOwner)
                owners++;
            else if (type == TX_TRANSFER_MASTERNODE)
                transfers++;
            else if (type == TX_UPDATE_MASTERNODE)
                updates++;
        }
    }
}

bool ParseMasterNodeScript(CScript scriptPubKey, uint160 &hashBytes, std::string &masternodeName, CAmount &masternodeAmount) {
    int nType;
    bool fIsOwner;
    int _nStartingPoint;
    std::string _strAddress;
    bool isMasterNode = false;
    if (scriptPubKey.IsMasterNodeScript(nType, fIsOwner, _nStartingPoint)) {
        if (nType == TX_NEW_MASTERNODE) {
            if (fIsOwner) {
                if (OwnerMasterNodeFromScript(scriptPubKey, masternodeName, _strAddress)) {
                    masternodeAmount = OWNER_MASTERNODE_AMOUNT;
                    isMasterNode = true;
                } else {
                    LogPrintf("%s : Couldn't get new owner masternode from script: %s", __func__, HexStr(scriptPubKey));
                }
            } else {
                CNewMasterNode masternode;
                if (MasterNodeFromScript(scriptPubKey, masternode, _strAddress)) {
                    masternodeName = masternode.strName;
                    masternodeAmount = masternode.nAmount;
                    isMasterNode = true;
                } else {
                    LogPrintf("%s : Couldn't get new masternode from script: %s", __func__, HexStr(scriptPubKey));
                }
            }
        } else if (nType == TX_UPDATE_MASTERNODE) {
            CUpdateMasterNode masternode;
            if (UpdateMasterNodeFromScript(scriptPubKey, masternode, _strAddress)) {
                masternodeName = masternode.strName;
                masternodeAmount = masternode.nAmount;
                isMasterNode = true;
            } else {
                LogPrintf("%s : Couldn't get update masternode from script: %s", __func__, HexStr(scriptPubKey));
            }
        } else if (nType == TX_TRANSFER_MASTERNODE) {
            CMasterNodeTransfer masternode;
            if (TransferMasterNodeFromScript(scriptPubKey, masternode, _strAddress)) {
                masternodeName = masternode.strName;
                masternodeAmount = masternode.nAmount;
                isMasterNode = true;
            } else {
                LogPrintf("%s : Couldn't get transfer masternode from script: %s", __func__, HexStr(scriptPubKey));
            }
        } else {
            LogPrintf("%s : Unsupported masternode type: %s", __func__, nType);
        }
    } else {
//        LogPrintf("%s : Found no masternode in script: %s", __func__, HexStr(scriptPubKey));
    }
    if (isMasterNode) {
//        LogPrintf("%s : Found masternodes in script at address %s : %s (%s)", __func__, _strAddress, masternodeName, masternodeAmount);
        hashBytes = uint160(std::vector <unsigned char>(scriptPubKey.begin()+3, scriptPubKey.begin()+23));
        return true;
    }
    return false;
}

CNullMasterNodeTxData::CNullMasterNodeTxData(const std::string &strMasterNodename, const int8_t &nFlag)
{
    SetNull();
    this->masternode_name = strMasterNodename;
    this->flag = nFlag;
}

bool CNullMasterNodeTxData::IsValid(std::string &strError, CMasterNodesCache &masternodeCache, bool fForceCheckPrimaryMasterNodeExists) const
{
    MasterNodeType type;
    if (!IsMasterNodeNameValid(masternode_name, type)) {
        strError = _("MasterNode name is not valid");
        return false;
    }

    if (type != MasterNodeType::QUALIFIER && type != MasterNodeType::SUB_QUALIFIER && type != MasterNodeType::RESTRICTED) {
        strError = _("MasterNode must be a qualifier, sub qualifier, or a restricted masternode");
        return false;
    }

    if (flag != 0 || flag != 1) {
        strError = _("Flag must be 1 or 0");
        return false;
    }

    if (fForceCheckPrimaryMasterNodeExists) {
        if (!masternodeCache.CheckIfMasterNodeExists(masternode_name)) {
            strError = _("MasterNode doesn't exist: ") + masternode_name;
            return false;
        }
    }

    return true;
}

void CNullMasterNodeTxData::ConstructTransaction(CScript &script) const
{
    CDataStream ssMasterNodeTxData(SER_NETWORK, PROTOCOL_VERSION);
    ssMasterNodeTxData << *this;

    std::vector<unsigned char> vchMessage;
    vchMessage.insert(vchMessage.end(), ssMasterNodeTxData.begin(), ssMasterNodeTxData.end());
    script << ToByteVector(vchMessage);
}

void CNullMasterNodeTxData::ConstructGlobalRestrictionTransaction(CScript &script) const
{
    CDataStream ssMasterNodeTxData(SER_NETWORK, PROTOCOL_VERSION);
    ssMasterNodeTxData << *this;

    std::vector<unsigned char> vchMessage;
    vchMessage.insert(vchMessage.end(), ssMasterNodeTxData.begin(), ssMasterNodeTxData.end());
    script << OP_HVN_MASTERNODE << OP_RESERVED << OP_RESERVED << ToByteVector(vchMessage);
}

CNullMasterNodeTxVerifierString::CNullMasterNodeTxVerifierString(const std::string &verifier)
{
    SetNull();
    this->verifier_string = verifier;
}

void CNullMasterNodeTxVerifierString::ConstructTransaction(CScript &script) const
{
    CDataStream ssMasterNodeTxData(SER_NETWORK, PROTOCOL_VERSION);
    ssMasterNodeTxData << *this;

    std::vector<unsigned char> vchMessage;
    vchMessage.insert(vchMessage.end(), ssMasterNodeTxData.begin(), ssMasterNodeTxData.end());
    script << OP_HVN_MASTERNODE << OP_RESERVED << ToByteVector(vchMessage);
}

bool CMasterNodesCache::GetMasterNodeVerifierStringIfExists(const std::string &name, CNullMasterNodeTxVerifierString& verifierString, bool fSkipTempCache)
{

    /** There are circumstances where a blocks transactions could be changing an masternodes verifier string, While at the
     * same time a transaction is added to the same block that is trying to transfer the masternodes who verifier string is
     * changing.
     * Depending on the ordering of these two transactions. The verifier string used to verify the validity of the
     * transaction could be different.
     * To fix this all restricted masternode transfer validation checks will use only the latest connect block tips caches
     * and databases to validate it. This allows for masternode transfers and verify string change transactions to be added in the same block
     * without failing validation
    **/

    // Create objects that will be used to check the dirty cache
    CMasterNodeCacheRestrictedVerifiers tempCacheVerifier {name, ""};

    auto setIterator = setNewRestrictedVerifierToRemove.find(tempCacheVerifier);
    // Check the dirty caches first and see if it was recently added or removed
    if (!fSkipTempCache && setIterator != setNewRestrictedVerifierToRemove.end()) {
        if (setIterator->fUndoingRessiue) {
            verifierString.verifier_string = setIterator->verifier;
            return true;
        }
        return false;
    }

    setIterator = pmasternodes->setNewRestrictedVerifierToRemove.find(tempCacheVerifier);
    // Check the dirty caches first and see if it was recently added or removed
    if (setIterator != pmasternodes->setNewRestrictedVerifierToRemove.end()) {
        if (setIterator->fUndoingRessiue) {
            verifierString.verifier_string = setIterator->verifier;
            return true;
        }
        return false;
    }

    setIterator = setNewRestrictedVerifierToAdd.find(tempCacheVerifier);
    if (!fSkipTempCache && setIterator != setNewRestrictedVerifierToAdd.end()) {
        verifierString.verifier_string = setIterator->verifier;
        return true;
    }

    setIterator = pmasternodes->setNewRestrictedVerifierToAdd.find(tempCacheVerifier);
    if (setIterator != pmasternodes->setNewRestrictedVerifierToAdd.end()) {
        verifierString.verifier_string = setIterator->verifier;
        return true;
    }

    // Check the cache, if it doesn't exist in the cache. Try and read it from database
    if (pmasternodesVerifierCache) {
        if (pmasternodesVerifierCache->Exists(name)) {
            verifierString = pmasternodesVerifierCache->Get(name);
            return true;
        }
    }

    if (prestricteddb) {
        std::string verifier;
        if (prestricteddb->ReadVerifier(name, verifier)) {
            verifierString.verifier_string = verifier;
            if (pmasternodesVerifierCache)
                pmasternodesVerifierCache->Put(name, verifierString);
            return true;
        }
    }

    return false;
}

bool CMasterNodesCache::CheckForAddressQualifier(const std::string &qualifier_name, const std::string& address, bool fSkipTempCache)
{
    /** There are circumstances where a blocks transactions could be removing or adding a qualifier to an address,
     * While at the same time a transaction is added to the same block that is trying to transfer to the same address.
     * Depending on the ordering of these two transactions. The qualifier database used to verify the validity of the
     * transactions could be different.
     * To fix this all restricted masternode transfer validation checks will use only the latest connect block tips caches
     * and databases to validate it. This allows for masternode transfers and address qualifier transactions to be added in the same block
     * without failing validation
    **/

    // Create cache object that will be used to check the dirty caches
    CMasterNodeCacheQualifierAddress cachedQualifierAddress(qualifier_name, address, QualifierType::ADD_QUALIFIER);

    // Check the dirty caches first and see if it was recently added or removed
    auto setIterator = setNewQualifierAddressToRemove.find(cachedQualifierAddress);
    if (!fSkipTempCache &&setIterator != setNewQualifierAddressToRemove.end()) {
        // Undoing a remove qualifier command, means that we are adding the qualifier to the address
        return setIterator->type == QualifierType::REMOVE_QUALIFIER;
    }


    setIterator = pmasternodes->setNewQualifierAddressToRemove.find(cachedQualifierAddress);
    if (setIterator != pmasternodes->setNewQualifierAddressToRemove.end()) {
        // Undoing a remove qualifier command, means that we are adding the qualifier to the address
        return setIterator->type == QualifierType::REMOVE_QUALIFIER;
    }

    setIterator = setNewQualifierAddressToAdd.find(cachedQualifierAddress);
    if (!fSkipTempCache && setIterator != setNewQualifierAddressToAdd.end()) {
        // Return true if we are adding the qualifier, and false if we are removing it
        return setIterator->type == QualifierType::ADD_QUALIFIER;
    }


    setIterator = pmasternodes->setNewQualifierAddressToAdd.find(cachedQualifierAddress);
    if (setIterator != pmasternodes->setNewQualifierAddressToAdd.end()) {
        // Return true if we are adding the qualifier, and false if we are removing it
        return setIterator->type == QualifierType::ADD_QUALIFIER;
    }

    auto tempCache = CMasterNodeCacheRootQualifierChecker(qualifier_name, address);
    if (!fSkipTempCache && mapRootQualifierAddressesAdd.count(tempCache)){
        if (mapRootQualifierAddressesAdd[tempCache].size()) {
            return true;
        }
    }

    if (pmasternodes->mapRootQualifierAddressesAdd.count(tempCache)) {
        if (pmasternodes->mapRootQualifierAddressesAdd[tempCache].size()) {
            return true;
        }
    }

    // Check the cache, if it doesn't exist in the cache. Try and read it from database
    if (pmasternodesQualifierCache) {
        if (pmasternodesQualifierCache->Exists(cachedQualifierAddress.GetHash().GetHex())) {
            return true;
        }
    }

    if (prestricteddb) {

        // Check for exact qualifier, and add to cache if it exists
        if (prestricteddb->ReadAddressQualifier(address, qualifier_name)) {
            pmasternodesQualifierCache->Put(cachedQualifierAddress.GetHash().GetHex(), 1);
            return true;
        }

        // Look for sub qualifiers
        if (prestricteddb->CheckForAddressRootQualifier(address, qualifier_name)){
            return true;
        }
    }

    return false;
}


bool CMasterNodesCache::CheckForAddressRestriction(const std::string &restricted_name, const std::string& address, bool fSkipTempCache)
{
    /** There are circumstances where a blocks transactions could be removing or adding a restriction to an address,
     * While at the same time a transaction is added to the same block that is trying to transfer from that address.
     * Depending on the ordering of these two transactions. The address restriction database used to verify the validity of the
     * transactions could be different.
     * To fix this all restricted masternode transfer validation checks will use only the latest connect block tips caches
     * and databases to validate it. This allows for masternode transfers and address restriction transactions to be added in the same block
     * without failing validation
    **/

    // Create cache object that will be used to check the dirty caches (type, doesn't matter in this search)
    CMasterNodeCacheRestrictedAddress cachedRestrictedAddress(restricted_name, address, RestrictedType::FREEZE_ADDRESS);

    // Check the dirty caches first and see if it was recently added or removed
    auto setIterator = setNewRestrictedAddressToRemove.find(cachedRestrictedAddress);
    if (!fSkipTempCache && setIterator != setNewRestrictedAddressToRemove.end()) {
        // Undoing a unfreeze, means that we are adding back a freeze
        return setIterator->type == RestrictedType::UNFREEZE_ADDRESS;
    }

    setIterator = pmasternodes->setNewRestrictedAddressToRemove.find(cachedRestrictedAddress);
    if (setIterator != pmasternodes->setNewRestrictedAddressToRemove.end()) {
        // Undoing a unfreeze, means that we are adding back a freeze
        return setIterator->type == RestrictedType::UNFREEZE_ADDRESS;
    }

    setIterator = setNewRestrictedAddressToAdd.find(cachedRestrictedAddress);
    if (!fSkipTempCache && setIterator != setNewRestrictedAddressToAdd.end()) {
        // Return true if we are freezing the address
        return setIterator->type == RestrictedType::FREEZE_ADDRESS;
    }

    setIterator = pmasternodes->setNewRestrictedAddressToAdd.find(cachedRestrictedAddress);
    if (setIterator != pmasternodes->setNewRestrictedAddressToAdd.end()) {
        // Return true if we are freezing the address
        return setIterator->type == RestrictedType::FREEZE_ADDRESS;
    }

    // Check the cache, if it doesn't exist in the cache. Try and read it from database
    if (pmasternodesRestrictionCache) {
        if (pmasternodesRestrictionCache->Exists(cachedRestrictedAddress.GetHash().GetHex())) {
            return true;
        }
    }

    if (prestricteddb) {
        if (prestricteddb->ReadRestrictedAddress(address, restricted_name)) {
            if (pmasternodesRestrictionCache) {
                pmasternodesRestrictionCache->Put(cachedRestrictedAddress.GetHash().GetHex(), 1);
            }
            return true;
        }
    }

    return false;
}

bool CMasterNodesCache::CheckForGlobalRestriction(const std::string &restricted_name, bool fSkipTempCache)
{
    /** There are circumstances where a blocks transactions could be freezing all masternode transfers. While at
     * the same time a transaction is added to the same block that is trying to transfer the same masternode that is being
     * frozen.
     * Depending on the ordering of these two transactions. The global restriction database used to verify the validity of the
     * transactions could be different.
     * To fix this all restricted masternode transfer validation checks will use only the latest connect block tips caches
     * and databases to validate it. This allows for masternode transfers and global restriction transactions to be added in the same block
     * without failing validation
    **/

    // Create cache object that will be used to check the dirty caches (type, doesn't matter in this search)
    CMasterNodeCacheRestrictedGlobal cachedRestrictedGlobal(restricted_name, RestrictedType::GLOBAL_FREEZE);

    // Check the dirty caches first and see if it was recently added or removed
    auto setIterator = setNewRestrictedGlobalToRemove.find(cachedRestrictedGlobal);
    if (!fSkipTempCache && setIterator != setNewRestrictedGlobalToRemove.end()) {
        // Undoing a removal of a global unfreeze, means that is will become frozen
        return setIterator->type == RestrictedType::GLOBAL_UNFREEZE;
    }

    setIterator = pmasternodes->setNewRestrictedGlobalToRemove.find(cachedRestrictedGlobal);
    if (setIterator != pmasternodes->setNewRestrictedGlobalToRemove.end()) {
        // Undoing a removal of a global unfreeze, means that is will become frozen
        return setIterator->type == RestrictedType::GLOBAL_UNFREEZE;
    }

    setIterator = setNewRestrictedGlobalToAdd.find(cachedRestrictedGlobal);
    if (!fSkipTempCache && setIterator != setNewRestrictedGlobalToAdd.end()) {
        // Return true if we are adding a freeze command
        return setIterator->type == RestrictedType::GLOBAL_FREEZE;
    }

    setIterator = pmasternodes->setNewRestrictedGlobalToAdd.find(cachedRestrictedGlobal);
    if (setIterator != pmasternodes->setNewRestrictedGlobalToAdd.end()) {
        // Return true if we are adding a freeze command
        return setIterator->type == RestrictedType::GLOBAL_FREEZE;
    }

    // Check the cache, if it doesn't exist in the cache. Try and read it from database
    if (pmasternodesGlobalRestrictionCache) {
        if (pmasternodesGlobalRestrictionCache->Exists(cachedRestrictedGlobal.masternodeName)) {
            return true;
        }
    }

    if (prestricteddb) {
        if (prestricteddb->ReadGlobalRestriction(restricted_name)) {
            if (pmasternodesGlobalRestrictionCache)
                pmasternodesGlobalRestrictionCache->Put(cachedRestrictedGlobal.masternodeName, 1);
            return true;
        }
    }

    return false;
}

void ExtractVerifierStringQualifiers(const std::string& verifier, std::set<std::string>& qualifiers)
{
    std::string s(verifier);

    std::regex regexSearch = std::regex(R"([A-Z0-9_.]+)");
    std::smatch match;

    while (std::regex_search(s,match,regexSearch)) {
        for (auto str : match)
            qualifiers.insert(str);
        s = match.suffix().str();
    }
}

std::string GetStrippedVerifierString(const std::string& verifier)
{
    // Remove all white spaces from the verifier string
    std::string str_without_whitespaces = LibBoolEE::removeWhitespaces(verifier);

    // Remove all '#' from the verifier string
    std::string str_without_qualifier_tags = LibBoolEE::removeCharacter(str_without_whitespaces, QUALIFIER_CHAR);

    return str_without_qualifier_tags;
}

bool CheckVerifierString(const std::string& verifier, std::set<std::string>& setFoundQualifiers, std::string& strError, ErrorReport* errorReport)
{
    // If verifier string is true, always return true
    if (verifier == "true") {
        return true;
    }

    // If verifier string is empty, return false
    if (verifier.empty()) {
        strError = _("Verifier string can not be empty. To default to true, use \"true\"");
        if (errorReport) {
            errorReport->type = ErrorReport::ErrorType::EmptyString;
            errorReport->strDevData = "bad-txns-null-verifier-empty";
        }
        return false;
    }

    // Remove all white spaces, and # from the string as this is how it will be stored in database, and in the script
    std::string strippedVerifier = GetStrippedVerifierString(verifier);

    // Check the stripped size to make sure it isn't over 80
    if (strippedVerifier.length() > 80){
        strError = _("Verifier string has length greater than 80 after whitespaces and '#' are removed");
        if (errorReport) {
            errorReport->type = ErrorReport::ErrorType::LengthToLarge;
            errorReport->strDevData = "bad-txns-null-verifier-length-greater-than-max-length";
            errorReport->vecUserData.emplace_back(strippedVerifier);
        }
        return false;
    }

    // Extract the qualifiers from the verifier string
    ExtractVerifierStringQualifiers(strippedVerifier, setFoundQualifiers);

    // Create an object that stores if an address contains a qualifier
    LibBoolEE::Vals vals;

    // If the check address is empty

    // set all qualifiers in the verifier to true
    for (auto qualifier : setFoundQualifiers) {

        std::string edited_qualifier;

        // Qualifer string was stripped above, so we need to add back the #
        edited_qualifier = QUALIFIER_CHAR + qualifier;

        if (!IsQualifierNameValid(edited_qualifier)) {
            strError = "bad-txns-null-verifier-invalid-masternode-name-" + qualifier;
            if (errorReport) {
                errorReport->type = ErrorReport::ErrorType::InvalidQualifierName;
                errorReport->vecUserData.emplace_back(edited_qualifier);
                errorReport->strDevData = "bad-txns-null-verifier-invalid-masternode-name-" + qualifier;
            }
            return false;
        }

        vals.insert(std::make_pair(qualifier, true));
    }

    try {
        LibBoolEE::resolve(verifier, vals, errorReport);
        return true;
    } catch (const std::runtime_error& run_error) {
        if (errorReport) {
            if (errorReport->type == ErrorReport::ErrorType::NotSetError) {
                errorReport->type = ErrorReport::ErrorType::InvalidSyntax;
                errorReport->vecUserData.emplace_back(run_error.what());
                errorReport->strDevData = "bad-txns-null-verifier-failed-syntax-check";
            }
        }
        strError = "bad-txns-null-verifier-failed-syntax-check";
        return error("%s : Verifier string failed to resolve. Please check string syntax - exception: %s\n", __func__, run_error.what());
    }
}

bool VerifyNullMasterNodeDataFlag(const int& flag, std::string& strError)
{
    // Check the flag
    if (flag != 0 && flag != 1) {
        strError = "bad-txns-null-data-flag-must-be-0-or-1";
        return false;
    }

    return true;
}

bool VerifyQualifierChange(CMasterNodesCache& cache, const CNullMasterNodeTxData& data, const std::string& address, std::string& strError)
{
    // Check the flag
    if (!VerifyNullMasterNodeDataFlag(data.flag, strError))
        return false;

    // Check to make sure we only allow changes to the current status
    bool fHasQualifier = cache.CheckForAddressQualifier(data.masternode_name, address, true);
    QualifierType type = data.flag ? QualifierType::ADD_QUALIFIER : QualifierType::REMOVE_QUALIFIER;
    if (type == QualifierType::ADD_QUALIFIER) {
        if (fHasQualifier) {
            strError = "bad-txns-null-data-add-qualifier-when-already-assigned";
            return false;
        }
    } else if (type == QualifierType::REMOVE_QUALIFIER) {
        if (!fHasQualifier) {
            strError = "bad-txns-null-data-removing-qualifier-when-not-assigned";
            return false;
        }
    }

    return true;
}

bool VerifyRestrictedAddressChange(CMasterNodesCache& cache, const CNullMasterNodeTxData& data, const std::string& address, std::string& strError)
{
    // Check the flag
    if (!VerifyNullMasterNodeDataFlag(data.flag, strError))
        return false;

    // Get the current status of the masternode and the given address
    bool fIsFrozen = cache.CheckForAddressRestriction(data.masternode_name, address, true);

    // Assign the type based on the data
    RestrictedType type = data.flag ? RestrictedType::FREEZE_ADDRESS : RestrictedType::UNFREEZE_ADDRESS;

    if (type == RestrictedType::FREEZE_ADDRESS) {
        if (fIsFrozen) {
            strError = "bad-txns-null-data-freeze-address-when-already-frozen";
            return false;
        }
    } else if (type == RestrictedType::UNFREEZE_ADDRESS) {
        if (!fIsFrozen) {
            strError = "bad-txns-null-data-unfreeze-address-when-not-frozen";
            return false;
        }
    }

    return true;
}

bool VerifyGlobalRestrictedChange(CMasterNodesCache& cache, const CNullMasterNodeTxData& data, std::string& strError)
{
    // Check the flag
    if (!VerifyNullMasterNodeDataFlag(data.flag, strError))
        return false;

    // Get the current status of the masternode globally
    bool fIsGloballyFrozen = cache.CheckForGlobalRestriction(data.masternode_name, true);

    // Assign the type based on the data
    RestrictedType type = data.flag ? RestrictedType::GLOBAL_FREEZE : RestrictedType::GLOBAL_UNFREEZE;

    if (type == RestrictedType::GLOBAL_FREEZE) {
        if (fIsGloballyFrozen) {
            strError = "bad-txns-null-data-global-freeze-when-already-frozen";
            return false;
        }
    } else if (type == RestrictedType::GLOBAL_UNFREEZE) {
        if (!fIsGloballyFrozen) {
            strError = "bad-txns-null-data-global-unfreeze-when-not-frozen";
            return false;
        }
    }

    return true;
}

////////////////


bool CheckVerifierMasterNodeTxOut(const CTxOut& txout, std::string& strError)
{
    CNullMasterNodeTxVerifierString verifier;
    if (!MasterNodeNullVerifierDataFromScript(txout.scriptPubKey, verifier)) {
        strError = "bad-txns-null-verifier-data-serialization";
        return false;
    }

    // All restricted verifiers should have white spaces stripped from the data before it is added to a script
    if ((int)verifier.verifier_string.find_first_of(' ') != -1) {
        strError = "bad-txns-null-verifier-data-contained-whitespaces";
        return false;
    }

    // All restricted verifiers should have # stripped from that data before it is added to a script
    if ((int)verifier.verifier_string.find_first_of('#') != -1) {
        strError = "bad-txns-null-verifier-data-contained-qualifier-character-#";
        return false;
    }

    std::set<std::string> setFoundQualifiers;
    if (!CheckVerifierString(verifier.verifier_string, setFoundQualifiers, strError))
        return false;

    return true;
}
///////////////
bool ContextualCheckNullMasterNodeTxOut(const CTxOut& txout, CMasterNodesCache* masternodeCache, std::string& strError, std::vector<std::pair<std::string, CNullMasterNodeTxData>>* myNullMasterNodeData)
{
    // Get the data from the script
    CNullMasterNodeTxData data;
    std::string address;
    if (!MasterNodeNullDataFromScript(txout.scriptPubKey, data, address)) {
        strError = "bad-txns-null-masternode-data-serialization";
        return false;
    }

    // Validate the tx data against the cache, and database
    if (masternodeCache) {
        if (IsMasterNodeNameAQualifier(data.masternode_name)) {
            if (!VerifyQualifierChange(*masternodeCache, data, address, strError)) {
                return false;
            }

        } else if (IsMasterNodeNameAnRestricted(data.masternode_name)) {
            if (!VerifyRestrictedAddressChange(*masternodeCache, data, address, strError))
                return false;
        } else {
            strError = "bad-txns-null-masternode-data-on-non-restricted-or-qualifier-masternode";
            return false;
        }
    }

#ifdef ENABLE_WALLET
    if (myNullMasterNodeData && vpwallets.size()) {
        if (IsMine(*vpwallets[0], DecodeDestination(address)) & ISMINE_ALL) {
            myNullMasterNodeData->emplace_back(std::make_pair(address, data));
        }
    }
#endif
    return true;
}

bool ContextualCheckGlobalMasterNodeTxOut(const CTxOut& txout, CMasterNodesCache* masternodeCache, std::string& strError)
{
    // Get the data from the script
    CNullMasterNodeTxData data;
    if (!GlobalMasterNodeNullDataFromScript(txout.scriptPubKey, data)) {
        strError = "bad-txns-null-global-masternode-data-serialization";
        return false;
    }

    // Validate the tx data against the cache, and database
    if (masternodeCache) {
        if (!VerifyGlobalRestrictedChange(*masternodeCache, data, strError))
            return false;
    }
    return true;
}

bool ContextualCheckVerifierMasterNodeTxOut(const CTxOut& txout, CMasterNodesCache* masternodeCache, std::string& strError)
{
    CNullMasterNodeTxVerifierString verifier;
    if (!MasterNodeNullVerifierDataFromScript(txout.scriptPubKey, verifier)) {
        strError = "bad-txns-null-verifier-data-serialization";
        return false;
    }

    if (masternodeCache) {
        std::string strError = "";
        std::string address = "";
        std::string strVerifier = verifier.verifier_string;
        if (!ContextualCheckVerifierString(masternodeCache, strVerifier, address, strError))
            return false;
    }

    return true;
}

bool ContextualCheckVerifierString(CMasterNodesCache* cache, const std::string& verifier, const std::string& check_address, std::string& strError, ErrorReport* errorReport)
{
    // If verifier is set to true, return true
    if (verifier == "true")
        return true;

    // Check against the non contextual changes first
    std::set<std::string> setFoundQualifiers;
    if (!CheckVerifierString(verifier, setFoundQualifiers, strError, errorReport))
        return false;

    // Loop through each qualifier and make sure that the masternode exists
    for(auto qualifier : setFoundQualifiers) {
        std::string search = QUALIFIER_CHAR + qualifier;
        if (!cache->CheckIfMasterNodeExists(search, true)) {
            if (errorReport) {
                errorReport->type = ErrorReport::ErrorType::MasterNodeDoesntExist;
                errorReport->vecUserData.emplace_back(search);
                errorReport->strDevData = "bad-txns-null-verifier-contains-non-created-qualifier";
            }
            strError = "bad-txns-null-verifier-contains-non-created-qualifier";
            return false;
        }
    }

    // If we got this far, and the check_address is empty. The CheckVerifyString method already did the syntax checks
    // No need to do any more checks, as it will fail because the check_address is empty
    if (check_address.empty())
        return true;

    // Create an object that stores if an address contains a qualifier
    LibBoolEE::Vals vals;

    // Add the qualifiers into the vals object
    for (auto qualifier : setFoundQualifiers) {
        std::string search = QUALIFIER_CHAR + qualifier;

        // Check to see if the address contains the qualifier
        bool has_qualifier = cache->CheckForAddressQualifier(search, check_address, true);

        // Add the true or false value into the vals
        vals.insert(std::make_pair(qualifier, has_qualifier));
    }

    try {
        bool ret = LibBoolEE::resolve(verifier, vals, errorReport);
        if (!ret) {
            if (errorReport) {
                if (errorReport->type == ErrorReport::ErrorType::NotSetError) {
                    errorReport->type = ErrorReport::ErrorType::FailedToVerifyAgainstAddress;
                    errorReport->vecUserData.emplace_back(check_address);
                    errorReport->strDevData = "bad-txns-null-verifier-address-failed-verification";
                }
            }

            error("%s : The address %s failed to verify against: %s. Is null %d", __func__, check_address, verifier, errorReport ? 0 : 1);
            strError = "bad-txns-null-verifier-address-failed-verification";
        }
        return ret;

    } catch (const std::runtime_error& run_error) {

        if (errorReport) {
            if (errorReport->type == ErrorReport::ErrorType::NotSetError) {
                errorReport->type = ErrorReport::ErrorType::InvalidSyntax;
            }

            errorReport->vecUserData.emplace_back(run_error.what());
            errorReport->strDevData = "bad-txns-null-verifier-failed-contexual-syntax-check";
        }

        strError = "bad-txns-null-verifier-failed-contexual-syntax-check";
        return error("%s : Verifier string failed to resolve. Please check string syntax - exception: %s\n", __func__, run_error.what());
    }
}

bool ContextualCheckTransferMasterNode(CMasterNodesCache* masternodeCache, const CMasterNodeTransfer& transfer, const std::string& address, std::string& strError)
{
    strError = "";
    MasterNodeType masternodeType;
    if (!IsMasterNodeNameValid(transfer.strName, masternodeType)) {
        strError = "Invalid parameter: masternode_name must only consist of valid characters and have a size between 3 and 30 characters. See help for more details.";
        return false;
    }

    if (transfer.nAmount <= 0) {
        strError = "Invalid parameter: masternode amount can't be equal to or less than zero.";
        return false;
    }

    if (AreMessagesDeployed()) {
        // This is for the current testnet6 only.
        if (transfer.nAmount <= 0) {
            strError = "Invalid parameter: masternode amount can't be equal to or less than zero.";
            return false;
        }

        if (transfer.message.empty() && transfer.nExpireTime > 0) {
            strError = "Invalid parameter: masternode transfer expiration time requires a message to be attached to the transfer";
            return false;
        }

        if (transfer.nExpireTime < 0) {
            strError = "Invalid parameter: expiration time must be a positive value";
            return false;
        }

        if (transfer.message.size() && !CheckEncoded(transfer.message, strError)) {
            return false;
        }
    }

    // If the transfer is a message channel masternode. Check to make sure that it is UNIQUE_MASTERNODE_AMOUNT
    if (masternodeType == MasterNodeType::MSGCHANNEL) {
        if (!AreMessagesDeployed()) {
            strError = "bad-txns-transfer-msgchannel-before-messaging-is-active";
            return false;
        }
    }

    if (masternodeType == MasterNodeType::RESTRICTED) {
        if (!AreRestrictedMasterNodesDeployed()) {
            strError = "bad-txns-transfer-restricted-before-it-is-active";
            return false;
        }

        if (masternodeCache) {
            if (masternodeCache->CheckForGlobalRestriction(transfer.strName, true)) {
                strError = "bad-txns-transfer-restricted-masternode-that-is-globally-restricted";
                return false;
            }
        }


        std::string strError = "";
        if (!transfer.ContextualCheckAgainstVerifyString(masternodeCache, address, strError)) {
            error("%s : %s", __func__, strError);
            return false;
        }
    }

    // If the transfer is a qualifier channel masternode.
    if (masternodeType == MasterNodeType::QUALIFIER || masternodeType == MasterNodeType::SUB_QUALIFIER) {
        if (!AreRestrictedMasterNodesDeployed()) {
            strError = "bad-txns-transfer-qualifier-before-it-is-active";
            return false;
        }
    }
    return true;
}

bool CheckNewMasterNode(const CNewMasterNode& masternode, std::string& strError)
{
    strError = "";

    MasterNodeType masternodeType;
    if (!IsMasterNodeNameValid(std::string(masternode.strName), masternodeType)) {
        strError = _("Invalid parameter: masternode_name must only consist of valid characters and have a size between 3 and 30 characters. See help for more details.");
        return false;
    }

    if (masternodeType == MasterNodeType::UNIQUE || masternodeType == MasterNodeType::MSGCHANNEL) {
        if (masternode.units != UNIQUE_MASTERNODE_UNITS) {
            strError = _("Invalid parameter: units must be ") + std::to_string(UNIQUE_MASTERNODE_UNITS);
            return false;
        }
        if (masternode.nAmount != UNIQUE_MASTERNODE_AMOUNT) {
            strError = _("Invalid parameter: amount must be ") + std::to_string(UNIQUE_MASTERNODE_AMOUNT);
            return false;
        }
        if (masternode.nReissuable != 0) {
            strError = _("Invalid parameter: reissuable must be 0");
            return false;
        }
    }

    if (masternodeType == MasterNodeType::QUALIFIER || masternodeType == MasterNodeType::SUB_QUALIFIER) {
        if (masternode.units != QUALIFIER_MASTERNODE_UNITS) {
            strError = _("Invalid parameter: units must be ") + std::to_string(QUALIFIER_MASTERNODE_UNITS);
            return false;
        }
        if (masternode.nAmount < QUALIFIER_MASTERNODE_MIN_AMOUNT || masternode.nAmount > QUALIFIER_MASTERNODE_MAX_AMOUNT) {
            strError = _("Invalid parameter: amount must be between ") + std::to_string(QUALIFIER_MASTERNODE_MIN_AMOUNT) + " - " + std::to_string(QUALIFIER_MASTERNODE_MAX_AMOUNT);
            return false;
        }
        if (masternode.nReissuable != 0) {
            strError = _("Invalid parameter: reissuable must be 0");
            return false;
        }
    }

    if (IsMasterNodeNameAnOwner(std::string(masternode.strName))) {
        strError = _("Invalid parameters: masternode_name can't have a '!' at the end of it. See help for more details.");
        return false;
    }

    if (masternode.nAmount <= 0) {
        strError = _("Invalid parameter: masternode amount can't be equal to or less than zero.");
        return false;
    }

    if (masternode.nAmount > MAX_MONEY) {
        strError = _("Invalid parameter: masternode amount greater than max money: ") + std::to_string(MAX_MONEY / COIN);
        return false;
    }

    if (masternode.units < 0 || masternode.units > 8) {
        strError = _("Invalid parameter: units must be between 0-8.");
        return false;
    }

    if (!CheckAmountWithUnits(masternode.nAmount, masternode.units)) {
        strError = _("Invalid parameter: amount must be divisible by the smaller unit assigned to the masternode");
        return false;
    }

    if (masternode.nReissuable != 0 && masternode.nReissuable != 1) {
        strError = _("Invalid parameter: reissuable must be 0 or 1");
        return false;
    }

    if (masternode.nHasIPFS != 0 && masternode.nHasIPFS != 1) {
        strError = _("Invalid parameter: has_ipfs must be 0 or 1.");
        return false;
    }

    return true;
}

bool ContextualCheckNewMasterNode(CMasterNodesCache* masternodeCache, const CNewMasterNode& masternode, std::string& strError, bool fCheckMempool)
{
    if (!AreMasterNodesDeployed() && !fUnitTest) {
        strError = "bad-txns-new-masternode-when-masternodes-is-not-active";
        return false;
    }

    if (!CheckNewMasterNode(masternode, strError))
        return false;

    // Check our current cache to see if the masternode has been created yet
    if (masternodeCache->CheckIfMasterNodeExists(masternode.strName, true)) {
        strError = std::string(_("Invalid parameter: masternode_name '")) + masternode.strName + std::string(_("' has already been used"));
        return false;
    }

    // Check the mempool
    if (fCheckMempool) {
        if (mempool.mapMasterNodeToHash.count(masternode.strName)) {
            strError = _("MasterNode with this name is already in the mempool");
            return false;
        }
    }

    // Check the ipfs hash as it changes when messaging goes active
    if (masternode.nHasIPFS && masternode.strIPFSHash.size() != 34) {
        if (!AreMessagesDeployed()) {
            strError = _("Invalid parameter: ipfs_hash must be 46 characters. Txid must be valid 64 character hash");
            return false;
        } else {
            if (masternode.strIPFSHash.size() != 32) {
                strError = _("Invalid parameter: ipfs_hash must be 46 characters. Txid must be valid 64 character hash");
                return false;
            }
        }
    }

    if (masternode.nHasIPFS) {
        if (!CheckEncoded(masternode.strIPFSHash, strError))
            return false;
    }

    return true;
}

bool CheckUpdateMasterNode(const CUpdateMasterNode& masternode, std::string& strError)
{
    strError = "";

    if (masternode.nAmount < 0 || masternode.nAmount >= MAX_MONEY) {
        strError = _("Unable to update masternode: amount must be 0 or larger");
        return false;
    }

    if (masternode.nUnits > MAX_UNIT || masternode.nUnits < -1) {
        strError = _("Unable to update masternode: unit must be between 8 and -1");
        return false;
    }

    /// -------- TESTNET ONLY ---------- ///
    // Testnet has a couple blocks that have invalid nUpdate values before constriants were created
    bool fSkip = false;
    if (GetParams().NetworkIDString() == CBaseChainParams::TESTNET) {
        if (masternode.strName == "GAMINGWEB" && masternode.nReissuable == 109) {
            fSkip = true;
        } else if (masternode.strName == "UINT8" && masternode.nReissuable == -47) {
            fSkip = true;
        }
    }
    /// -------- TESTNET ONLY ---------- ///

    if (!fSkip && masternode.nReissuable != 0 && masternode.nReissuable != 1) {
        strError = _("Unable to update masternode: reissuable must be 0 or 1");
        return false;
    }

    MasterNodeType type;
    IsMasterNodeNameValid(masternode.strName, type);

    if (type == MasterNodeType::RESTRICTED) {
        // TODO Add checks for restricted masternode if we can come up with any
    }

    return true;
}

bool ContextualCheckUpdateMasterNode(CMasterNodesCache* masternodeCache, const CUpdateMasterNode& update_masternode, std::string& strError, const CTransaction& tx)
{
    // We are using this just to get the strAddress
    CUpdateMasterNode update;
    std::string strAddress;
    if (!UpdateMasterNodeFromTransaction(tx, update, strAddress)) {
        strError = "bad-txns-update-masternode-contextual-check";
        return false;
    }

    // hun non contextual checks
    if (!CheckUpdateMasterNode(update_masternode, strError))
        return false;

    // Check previous masternode data with the updatesd data
    CNewMasterNode prev_masternode;
    if (!masternodeCache->GetMasterNodeMetaDataIfExists(update_masternode.strName, prev_masternode)) {
        strError = _("Unable to update masternode: masternode_name '") + update_masternode.strName + _("' doesn't exist in the database");
        return false;
    }

    if (!prev_masternode.nReissuable) {
        // Check to make sure the masternode can be updated
        strError = _("Unable to update masternode: reissuable is set to false");
        return false;
    }

    if (prev_masternode.nAmount + update_masternode.nAmount > MAX_MONEY) {
        strError = _("Unable to update masternode: masternode_name '") + update_masternode.strName +
                   _("' the amount trying to update is to large");
        return false;
    }

    if (!CheckAmountWithUnits(update_masternode.nAmount, prev_masternode.units)) {
        strError = _("Unable to update masternode: amount must be divisible by the smaller unit assigned to the masternode");
        return false;
    }

    if (update_masternode.nUnits < prev_masternode.units && update_masternode.nUnits != -1) {
        strError = _("Unable to update masternode: unit must be larger than current unit selection");
        return false;
    }

    // Check the ipfs hash
    if (update_masternode.strIPFSHash != "" && update_masternode.strIPFSHash.size() != 34 && (AreMessagesDeployed() && update_masternode.strIPFSHash.size() != 32)) {
        strError = _("Invalid parameter: ipfs_hash must be 34 bytes, Txid must be 32 bytes");
        return false;
    }

    if (update_masternode.strIPFSHash != "") {
        if (!CheckEncoded(update_masternode.strIPFSHash, strError))
            return false;
    }

    if (IsMasterNodeNameAnRestricted(update_masternode.strName)) {
        CNullMasterNodeTxVerifierString new_verifier;
        bool fNotFound = false;

        // Try and get the verifier string if it was changed
        if (!tx.GetVerifierStringFromTx(new_verifier, strError, fNotFound)) {
            // If it return false for any other reason besides not being found, fail the transaction check
            if (!fNotFound) {
                return false;
            }
        }

        if (update_masternode.nAmount > 0) {
            // If it wasn't found, get the current verifier and validate against it
            if (fNotFound) {
                CNullMasterNodeTxVerifierString current_verifier;
                if (masternodeCache->GetMasterNodeVerifierStringIfExists(update_masternode.strName, current_verifier)) {
                    if (!ContextualCheckVerifierString(masternodeCache, current_verifier.verifier_string, strAddress, strError))
                        return false;
                } else {
                    // This should happen, but if it does. The wallet needs to shutdown,
                    // TODO, remove this after restricted masternodes have been tested in testnet for some time, and this hasn't happened yet. It this has happened. Investigation is required by the dev team
                    error("%s : failed to get verifier string from a restricted masternode, this shouldn't happen, database is out of sync. Reindex required. Please report this is to development team masternode name: %s, txhash : %s",__func__, update_masternode.strName, tx.GetHash().GetHex());
                    strError = "failed to get verifier string from a restricted masternode, database is out of sync. Reindex required. Please report this is to development team";
                    return false;
                }
            } else {
                if (!ContextualCheckVerifierString(masternodeCache, new_verifier.verifier_string, strAddress, strError))
                    return false;
            }
        }
    }


    return true;
}

bool ContextualCheckUpdateMasterNode(CMasterNodesCache* masternodeCache, const CUpdateMasterNode& update_masternode, std::string& strError)
{
    // hun non contextual checks
    if (!CheckUpdateMasterNode(update_masternode, strError))
        return false;

    // Check previous masternode data with the updatesd data
    if (masternodeCache) {
        CNewMasterNode prev_masternode;
        if (!masternodeCache->GetMasterNodeMetaDataIfExists(update_masternode.strName, prev_masternode)) {
            strError = _("Unable to update masternode: masternode_name '") + update_masternode.strName +
                       _("' doesn't exist in the database");
            return false;
        }

        if (!prev_masternode.nReissuable) {
            // Check to make sure the masternode can be updated
            strError = _("Unable to update masternode: reissuable is set to false");
            return false;
        }

        if (prev_masternode.nAmount + update_masternode.nAmount > MAX_MONEY) {
            strError = _("Unable to update masternode: masternode_name '") + update_masternode.strName +
                       _("' the amount trying to update is to large");
            return false;
        }

        if (!CheckAmountWithUnits(update_masternode.nAmount, prev_masternode.units)) {
            strError = _("Unable to update masternode: amount must be divisible by the smaller unit assigned to the masternode");
            return false;
        }

        if (update_masternode.nUnits < prev_masternode.units && update_masternode.nUnits != -1) {
            strError = _("Unable to update masternode: unit must be larger than current unit selection");
            return false;
        }
    }

    // Check the ipfs hash
    if (update_masternode.strIPFSHash != "" && update_masternode.strIPFSHash.size() != 34 && (AreMessagesDeployed() && update_masternode.strIPFSHash.size() != 32)) {
        strError = _("Invalid parameter: ipfs_hash must be 34 bytes, Txid must be 32 bytes");
        return false;
    }

    if (update_masternode.strIPFSHash != "") {
        if (!CheckEncoded(update_masternode.strIPFSHash, strError))
            return false;
    }

    return true;
}

bool ContextualCheckUniqueMasterNodeTx(CMasterNodesCache* masternodeCache, std::string& strError, const CTransaction& tx)
{
    for (auto out : tx.vout)
    {
        if (IsScriptNewUniqueMasterNode(out.scriptPubKey))
        {
            CNewMasterNode masternode;
            std::string strAddress;
            if (!MasterNodeFromScript(out.scriptPubKey, masternode, strAddress)) {
                strError = "bad-txns-create-unique-serialization-failed";
                return false;
            }

            if (!ContextualCheckUniqueMasterNode(masternodeCache, masternode, strError))
                return false;
        }
    }

    return true;
}

bool ContextualCheckUniqueMasterNode(CMasterNodesCache* masternodeCache, const CNewMasterNode& unique_masternode, std::string& strError)
{
    if (!ContextualCheckNewMasterNode(masternodeCache, unique_masternode, strError))
        return false;

    return true;
}

std::string GetUserErrorString(const ErrorReport& report)
{
    switch (report.type) {
        case ErrorReport::ErrorType::NotSetError: return _("Error not set");
        case ErrorReport::ErrorType::InvalidQualifierName: return _("Invalid Qualifier Name: ") + report.vecUserData[0];
        case ErrorReport::ErrorType::EmptyString: return _("Verifier string is empty");
        case ErrorReport::ErrorType::LengthToLarge: return _("Length is to large. Please use a smaller length");
        case ErrorReport::ErrorType::InvalidSubExpressionFormula: return _("Invalid expressions in verifier string: ") + report.vecUserData[0];
        case ErrorReport::ErrorType::InvalidSyntax: return _("Invalid syntax: ") + report.vecUserData[0];
        case ErrorReport::ErrorType::MasterNodeDoesntExist: return _("MasterNode doesn't exist: ") + report.vecUserData[0];
        case ErrorReport::ErrorType::FailedToVerifyAgainstAddress: return _("This address doesn't contain the correct tags to pass the verifier string check: ") + report.vecUserData[0];
        case ErrorReport::ErrorType::EmptySubExpression: return _("The verifier string has two operators without a tag between them");
        case ErrorReport::ErrorType::UnknownOperator: return _("The symbol: '") + report.vecUserData[0] + _("' is not a valid character in the expression: ") + report.vecUserData[1];
        case ErrorReport::ErrorType::ParenthesisParity: return _("Every '(' must have a corresponding ')' in the expression: ") + report.vecUserData[0];
        case ErrorReport::ErrorType::VariableNotFound: return _("Variable is not allow in the expression: '") + report.vecUserData[0] + "'";;
        default:
            return _("Error not set");
    }
}
