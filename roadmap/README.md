# Hivecoin Roadmap

### Phase 1 - (Complete)

Hivecoin (HVN) is a Proof of Work & Proof of Service coin built on the Bitcoin UTXO model. As with other Bitcoin derivatives, HVN coins are distributed to persons augmenting the Hive network by mining Hive and Master node.
*  x1000 coin distribution (21 Billion Total)
*  10x faster blocks (1 per minute)
*  In app CPU verification, with GPU specific PoW decentralised mining
*  Dark Gravity Wave difficulty adjustment (180 block average)
*  Addresses start with H... for regular addresses
*  Network Port: 9767
*  RPC Port: 9766

### Phase 2 - Assets (Complete)

#### ASIC Resistance

ASIC Resistance - A published commitment to continual attempts at ASIC resistance. The community has forked to KAWPOW (a variant of ethash and progpow) to maximise the ASIC resistance by reducing the potential efficiency increase of ASICs by requiring the feature set and capabilities within over the counter consumer graphics cards. We are not anticipating future forks to change the algorithm as the current algorithm allows a fair distribution of HVN via PoW to the community.

#### Asset Support

Hivecoin will be a hard fork that extends Hive to include the ability to issue and transfer assets. Hive will be extended to allow issuing, reissuing, and transfer of assets. Assets can be reissuable or limited to a set supply at the point of issuance. The cost to create assets will be 500 HVN to create any qty of an asset. Each asset name must be unique. Asset names will be limited to A-Z and 0-9, '_' and '.' and must be at least three characters long. The '.' and the '_' cannot be the first, or the last character, or be consecutive.  

The HVN used to issue assets will be sent to a burn address, which will reduce the amount of HVN available. 

Asset transfers require the standard HVN transaction fees for transfer from one address to another.

#### Metadata

Metadata about the token can be stored in IPFS.

#### Rewards

Reward capabilities will be added to allow payment (in HVN) to all holders of an asset. Payments of HVN would be distributed to all asset holders pro rata. This is useful for paying dividends, dividing payments, or rewarding a group of token holders.

#### Block Size

Hive may increase the blocksize from 2 MB to X MB to allow for more on-chain transactions.

### Phase 3 - Rewards

Rewards allow payment in HVN to asset holders.

[More on rewards...](./rewards/README.md)

### Phase 4 - Unique Assets (Complete)

Once created, assets can be made unique for a cost of 5 HVN. Only non-divisible assets can be made unique. This moves an asset to a UTXO and associates a unique identifier with the txid. From this point the asset can be moved from one address to another and can be traced back to its origin. Only the issuer of the original asset can make an asset unique.  
The costs to make unique assets will be sent to a burn address.  

### Phase 5 - Messaging

Messaging to token holders by authorized senders will be layered on top of the Phase 4 unique assets.

[More on messaging...](./messaging/README.md)  
[More on preventing message spam...](./messaging-antispam/README.md)  
[More on IPFS...](./ipfs/README.md)  

### Phase 6 - Voting

Voting will be accomplished by creating and distributing parallel tokens to token holders. These tokens can be sent to HVN addresses to record a vote.

[More on voting...](./voting/README.md)

### Phase 7 - Master node system(Developing)

The expected release of master node system will be approximately seven months after the release of HVN. The master node does not participate in mining activities, and the block reward of the master node network is 45%. To become a master node, users must have 1,000,000 HVN as a "margin." The deposit is always fully controlled by the holder, and the holder can freely transfer it. After moving the deposit, the corresponding master node will go offline and stop receiving block rewards.

### Phase 8 - Decentralized Budget Management System(Developing)

The expected release of decentralized Budget Management System will be approximately 12 months after the release of HVN.10% of the HVN has been separated from the mining block. Before the completion of this module, this part of the HVN directly funded the Hive core development team, after the completion of this module, the master node will vote to determine the use of 10% HVN.

### Phase 9 - Simplified Smart Contract

The expected release of simplified Smart Contract will be approximately 24 months after the release of HVN.
