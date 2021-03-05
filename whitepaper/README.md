<h1 align="center">
  Hivecoin: A Peer to Peer Electronic System Integrating Simplified Smart Contracts<br>
</h1>


<p align="center">
Jiyun Wang<br>
Zhen Liu<br>
http://www.hivecoin.org<br>
5 March 2021
</p>
Thank you to the Bitcoin & Ravencoin founder and developers. The Hivecoin project is launched based on the hard work and continuous effort of Bitcoin & Ravencoin developers who made over 14,000 commits over the life to date of the Bitcoin and Ravencoin project. We are eternally grateful to you for your efforts and diligence in making a secure network and for their support of free and open source software development.  The Hivecoin experiment is made on the foundation you built.

> **Abstract**. Hivecoin is a blockchain and platform optimized for a simplified smart contract, such as creating assets, trading assets, mortgage assets, community autonomy, etc. Based on the extensive development and testing of the UTXO model of the Bitcoin protocol, Hivecoin is built on a fork of the Ravencoin & Bitcoin code. The key change is the addition of a simplified version of the smart contract platform for asset, voting, mortgaging, and decentralized finance. Hivecoin is free and open source. 45% of Hivecoin (HVN) are fairly issued and mined publicly and transparently using Proof of Work (POW) using the KAWPOW algorithm which was created for resistance to ASC miners. The other 45% of Hivecoin (HVN) is allocated to all Hivecoin master nodes, which is required to mortgage 100,000 HVN to become a master node. 5% of Hivecoin (HVN) is allocated to developers, and the remaining 5% is mainly to establish a healthy and complete Hivecoin community ecological environment to achieve a certain degree of community autonomy. Hivecoin is intended to prioritize security, user control, privacy, and censorship resistance. It is open to use and development in any jurisdiction, while allowing simple additional features for users based on need.

## 1. Introduction

A blockchain is a ledger showing the quantity of something controlled by a user. It enables one to transfer control of that digital representation to someone else. Of the many possible uses for blockchain technology, the reporting of who owns what is one of its core functions. This is likely why the first, and to date most successful, use case for blockchain technology has been Bitcoin, which was announced by Satoshi Nakamoto on October 31, 2008[1].


The Ethereum ERC20, ERC223 and ERC721 protocols and other projects show tokenized assets that use another blockchain can be created with a wide variety of purposes and structures. Tokens offer several advantages to traditional shares or other participation mechanisms, e.g. faster transfer speed, increased user control and censorship resistance, and a reduction or elimination of the need for a trusted third party.

Bitcoin also has the ability to act as a token by using projects such as Omnilayer, RSK or Counterparty. However, neither Bitcoin nor Ethereum is specifically designed to promote the ownership of other assets, and users and development teams usually prioritize other functions.

Hivecoin has created a simplified version of smart contracts, which makes it easier to write smart contracts. Because of the simplified smart contracts, many additional restrictions have been added, which makes smart contracts more secure. For example, assets. Issuance, transaction, mortgage, and how to distribute the generated interest will become very simple. One of the goals of the Hive protocol is to create a use case-centric blockchain and development work that can create code to provide advantages for specific use cases , And contribute to open source code that can be used by Bitcoin or other projects.

If the global economy is affected by participants using various blockchains, the way in which today's capital markets operate may also change. As more assets become more tradable and cross-border trade continues to grow, borders and jurisdictions may become less important.

## 2. Background Tokens and Other Assets

On January 3, 2009, Bitcoin was launched as a peer-to-peer electronic cash system. Years later, after reaching a significant level of security, people realized that assets can be "created" on top of the "Bitcoin" blockchain or embedded in the Bitcoin blockchain. By creating a secure, signed, immutable Bitcoin transaction, new assets can be added to the Bitcoin blockchain, which also carries information about asset issuance and transfer.

Several projects have added tokens to the Bitcoin blockchain. First is JR Willett's Mastercoin [2], followed by Counterparty [3] and other projects. A type of protocol developed to facilitate the creation of assets on the Bitcoin blockchain is called colored coins [4], because they mark Bitcoin transactions with special transactions in OP_RETURN [5], just like in the Bitcoin protocol The comment field is the same.

The advantage of embedding assets in the Bitcoin blockchain is a high degree of security. Many people believe that Bitcoin is the most secure blockchain because of the large amount of distributed mining capabilities that can protect each block through "highly difficult hashing" [6]. Since distributed Bitcoin nodes recognize the level of work required to create difficult hashes, it is almost impossible to rewrite or modify the blockchain without the prohibitively high mining investment. To tamper with the Bitcoin blockchain and rewrite or modify its ledger will require a huge effort by investors at the level of a nation-state.

The disadvantage of adding smart contracts to the Bitcoin blockchain is that the Bitcoin rules originally written must be followed, and Bitcoin nodes do not know that the smart contract is being embedded. This means that Bitcoin transactions must be used for every smart contract, and It must send enough bitcoins to be considered a valid transaction, even if the main purpose of the transaction is to send assets. This is very inconvenient, but the main disadvantage is that the bitcoins are spent without knowing the operation of the embedded smart contract Bitcoin customers of Bitcoin may destroy the asset. For example, a private key holder of Bitcoin holding assets in a smart contract may accidentally send the Bitcoin to an exchange or wallet and lose these assets. Part of the solution to this problem is to create a special address format for assets in smart contracts, but it does not prevent errors that may destroy assets. It just provides more clues, indicating that assets are embedded in transactions in smart contracts.

Other token standards (such as ERC20, ERC721, and ERC223) are built on Ethereum or other blockchains that support smart contracts. There is another problem when using these smart contracts. Since the Ethereum network itself cannot recognize these smart contract tokens, it currently cannot prevent certain common problems. Smart contracts may confuse users because there may be multiple ERC20 tokens with the same name. The only difference between contracts with the same contract is the hash of the contract.

## 3. Full Asset Aware Protocol Level System

The solution is to create a Bitcoin-like system that fully understands the assets. An asset-aware system has two main advantages. First, it allows clients and RPC commands to manage assets and protect assets from accidental destruction. Second, it allows a single native client or mobile wallet to issue, track and transfer assets. Finally, in order to provide the security of basic assets, Bitcoin-like systems can only be operated with market value, strong mining communities and widely distributed master nodes.
  
**Assets**  
Assets can be issued by users of the Hive protocol or generated by Hive asset mortgages, rather than generated by mining. The users of the Hive protocol create these assets and determine their purpose and rules independently of the protocol. These assets or tokens exist on the Hivecoin blockchain and can be any name, denomination or purpose chosen by the creator of each asset, coin or token. Tokens are transferable and are as easy to move as Bitcoin or other cryptocurrencies with similar functions. In Hivecoin, assets are only a limited number of unique symbols, and can be transferred to any Hivecoin address. Assets created using the Hive protocol have multiple advantages: easy to use, tightly integrated with native coins, and protected by fair POW and POS, and open source codes run by decentralized autonomous organizations.

**Uses for Assets**  
Assets or tokens can be used for anything the creator's imagination can think of. The idea presented here is an example.

**Representing real world custodied physical or digital assets to tokens**
* Gold bars
* Silver coins
* Physical currency
* Artwork
* Land deed
* Copyright Information
* Medical information
* Educational information
* Logistics information

**Representing a share of a project**
* Securities tokens: stocks of companies whose stocks are represented by tokens, not physical stock certificates
* Securities or partnership interests with built-in ability to pay in the form of HVN
* Tokens representing cooperatives, limited partnerships, royalty-sharing or profit-sharing platforms
* Token, representing a crowdfunding project, capable of transferring or reselling the project

**Representing virtual goods**
* Event tickets, such as the Baltimore Hives game with resale capability
* Permit to allow activities
* Use the service's access token
* Currency and items in the game can be transferred outside the game platform

**Representing a credit**
* Gift card
* Air mileage
* Reward points

## 4. Hivecoin Launch and Algorithm

Hivecoin was announced on March 5, 2021[7] and released binaries for mining on April 1, 2021,[8] the respective ninth anniversary of the announcement and launch of Bitcoin. Hivecoin is the bitcoin-like system that will allow users to issue and integrate assets into its blockchain. This will be accomplished in phases which build upon each other.

\- In progress

Create a platform like Bitcoin with a new mining algorithm, KAWPOW[9], invented by the Ravencoin developers, intended to prevent immediate dominance by mining pools, and future dominance by ASIC mining equipment.

Launch the token with no pre-mine and a fair launch to widely distribute the tokens.

Allow the mining rate to increase and the value of the HVN token to naturally grow and gradually disburse to holders that understand the value of the platform.

Utilize proof of work mining, not because it burns a scarce resource of electricity, or the requirement of computer hardware, but instead focuses on the most valuable part of the "work" which is building an ever-larger and time-based wall that protects user data from future tampering and censorship with every new layer.

Using proof of stake, Hivecoin can create as many online master nodes as possible. These master nodes will provide timely and fast data transmission services, rather than simply hiding behind the firewall. And these master nodes can participate in voting and participate in the community Autonomous, and decide where 5% of Hivecoin marketing expenses will be spent.

## 5. Asset Issuance & Transfer

The token name is unique. Whoever creates it first belongs to whoever owns it.

The issuer of the token will burn 500 HVN and must provide a unique token name. The issuer determines the number of issuances, the number of decimal places and whether they will be allowed to issue more of the same tokens in the future.

It is allowed to issue other tokens using methods similar to Mastercoin, Counterparty or CoinSpark [10].

Tightly integrate assets with GUI wallets to provide intuitive asset management. Easily issue new assets, display current balances and transfer to other users.

The combination of open source and cryptocurrency enables benefits to be carried out in ways that cannot be achieved by traditional centralized structures.

Fair and open source cryptocurrency projects can replace bosses, rulers, employees, and company structures with consistent interests and the economic choices of participants.

Therefore, in some cases, whether it is selfless dedication or selfish motivation, for many new and interesting types of projects, open source is a better model than other structures. Hivecoin will allow projects to issue tokens to represent cooperatives, companies or partnerships.

In addition, since the issuer can make the token a unique, restricted or fungible token, the token project manager will be able to have the category of token holders, such as "Class A Shareholders", "Lifetime Social Club Members" ", or "beneficiary".

Tokens make it easier to issue small public offerings.

## 6. Rewards

Allows to pay rewards with native tokens. With a single command, the rewards denominated in HVN will be automatically evenly distributed and sent to the asset holders in proportion.

example:
For example, a company raises some funds from the society and distributes the same amount of SHAREHOLDER tokens to investors. When these funds finally generate income and need to pay dividends to shareholders, it can pass an order to reduce the profits (with HVN may have Any value of) is sent to SHAREHOLDER token holders. The ease of use of the built-in GUI wallet should allow anyone anywhere in the world to operate on a mobile phone or computer running Windows, Mac or Linux.

## 7. Unique Tokens

Unique tokens allow token holders to create unique assets. Like the ERC721 token, the unique token is guaranteed to be unique, and there will only be one. The unique token can change ownership by sending the unique token to another user's address.

Some examples of unique tokens:

*Software developers can use the name of their software ICAMPUS to issue assets, and then assign a unique ID or license key to each ICAMPUS token. Game tokens can be transferred as a license transfer. Each token IAMPUS: 202131 and IAMPUS: 202132 is a unique token.

*Unique assets based on HVN can be tied to real-world assets. Create an asset named ASSET_NAME. Each gold coin or gold bar in the vault can be serialized and reviewed. You can create associated unique assets ASSET_NAME: 444322 and ASSET_NAME: 555994 to represent specific assets in the physical vault.

The public nature of the chain allows for complete transparency.

Some use cases for unique assets include:

*Software License
*Car information registration
*Student graduation certificate registration
*Token that allows communication on the channel (see messaging)

## 8. Messaging Stakeholders

A common problem with tokens/assets is that the token issuer cannot communicate with the token holder. This must be handled very carefully, because token holders do not always want to be identified. This communication should allow token holders to withdraw at any time. The mail system should only allow selected parties to use the mail channel so that it does not become a spam channel.


The messaging system uses unique tokens to allow communication on the main token channel. For example, the COMPANY token will have a ~COMPANY:Alert token, which allows alerts to be sent to all holders of COMPANY.

Communications, game developers, non-profit organizations, rights organizations, companies, and other entities will be able to issue tokens for specific users and then send messages to those users, but unlike email or other messaging services, messaging itself will only be The token holder activates so that the token can be transferred.

Messages sent by authorized senders to token holders will be layered based on unique assets. The only asset will act as a "talking stick", allowing the channel owner to send messages. The KAAAWWW agreement will be published separately, which contains more detailed information.

## 9. Voting

By using a messaging system, token holders can be notified of voting tokens, and by automatically issuing VOTE tokens to each holder of the token, it can be done automatically from the client or through the web or mobile interface using the protocol vote.

Create a token to represent voting. Hivecoin will create an accurate number of VOTE tokens and distribute them to token holders in a 1:1 ratio. These tickets can be sent by agreement to the address that counts the tickets. Since voting tokens flow in the same way as assets, voting can be delegated-sometimes referred to as representative democracy or liquid democracy [11].

## 10. Privacy

<p align="center"><i>
It's a community where the threat of violence is impotent because violence is impossible, and violence is impossible because its participants cannot be linked to their true names or physical locations. (Wei Dai)
</i></p>  
Privacy is the key to investment and tokens, because the financial system functions better when assets are interchangeable and can be traded frictionlessly. As future technology improves, the project should seek to enhance privacy in any way possible.

With the addition of functions such as messages, assets, and rewards, privacy will be protected in the same way that UTXO-based cryptocurrencies separate identities from public addresses.

"Because we desire privacy, we must ensure that each party to a transaction only knows the knowledge directly necessary for the transaction. Since any information can be spoken, we must ensure that as little information as possible is disclosed. In most cases, personal identity is not prominent.

When the basic mechanism of the transaction reveals my identity, I don't have any right to privacy. I cannot show myself selectively here. I must always show myself.

"Therefore, privacy in an open society requires an anonymous transaction system. So far, cash has been such a major system. An anonymous transaction system is not a secret transaction system. Anonymity systems enable individuals to disclose their identities when and only when needed. ; This is the essence of privacy.” (E. Hughes) [12].

## 11. Additional

Other projects can use this chain. Second layer solutions, particularly those being built for projects which share the code base of Bitcoin can be built on the Hivecoin project. RSK, the Lightning Network, confidential transactions, and other scalability improvements, etc. to various open source projects could benefit projects built on this platform.

## 12. Conclusion

Hivecoin is a platform coin built based on Bitcoin's UTXO [13] model. It is impractical to modify the Bitcoin code to add these features, but Hivecoin is a platform based on code branches and releasing newly mined HVN. Hivecoin will add a simplified version of smart contracts, assets, rewards, unique assets, messaging, voting, and master nodes. The functions of the Hive protocol will be rolled out in phases, and a hard fork upgrade will be carried out as planned. The code base is designed to allow users and developers to maintain a secure, decentralized, and tamper-proof network.

The Hivecoin project can also be used as a basis and starting point for projects, second-tier solutions, experiments, and business ideas, which may benefit from a Bitcoin-based code base (with adjustments) or native additional features added to the Hivecoin blockchain .

In open source, the power of the crowd can do more work than any one person or organization. People from all walks of life are welcome to contribute.

Thank you to the Ravencoin developers again ! [14].

---

References

[1] S. Nakamoto, “Bitcoin: A Peer-to-Peer Electronic Cash System https://bitcoin.org/bitcoin.pdf  
[2] https://bhiveewcoin.com/assets/Whitepapers/2ndBitcoinWhitepaper.pdf  
[3] https://counterparty.io/  
[4] https://en.bitcoin.it/wiki/Colored_Coins  
[5] https://en.bitcoin.it/wiki/OP_RETURN  
[6] https://bitcoinwisdom.com/bitcoin/difficulty  
[7] J. Wang, “Hivecoin: A Peer to Peer Electronic System Integrating Simplified Smart Contracts” https://hivecoin.medium.com/hivecoin-a-peer-to-peer-electronic-system-integrating-simplified-smart-contracts-a0ef597afd0f
[8] https://github.com/HiveProject2021/Hivecoin  
[9] https://github.com/RavenCommunity/kawpowminer 
[10] [http://coinspark.org/developers/assets-introduction/](http://coinspark.org/developers/assets-introduction/)  
[11] https://en.wikipedia.org/wiki/Delegative_democracy  
[12] E. Hughes https://www.activism.net/cypherpunk/manifesto.html  
[13] https://bitcoin.org/en/glossary/unspent-transaction-output 
[14] Ravencoin developers https://ravencoin.org/whitepaper/ 
