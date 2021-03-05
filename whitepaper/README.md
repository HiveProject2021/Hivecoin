<h1 align="center">
  Hivecoin: A Peer to Peer Electronic System Integrating Smart Contracts<br>
</h1>


<p align="center">
Jiyun Wang<br>
Zhen Liu<br>
http://www.hivecoin.org<br>
3rd March 2021
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

The solution is to create a bitcoin-like system that is fully asset aware. A system being asset aware provides two major advantages. First, it allows the client and RPC commands to protect the asset from being destroyed accidentally. Second, it allows a single native client to issue, track, and transfer the assets.
Lastly, to provide security for the underlying assets, the bitcoin-like system functions only with a market value, a strong mining community, and wide distribution.  
  
**Assets**  
Assets are tokens that can be issued by users of the Hive protocol without the need to be mined. Users of the Hive protocol create these assets and decide their purpose and rules independent of the protocol. These assets or tokens exist on the Hivecoin blockchain and could be whatever name, denomination or purpose selected by the creators of each asset, coin, or token. The tokens are transferable and move with the same ease as bitcoin, or other similarly functioning cryptocurrencies. In Hivecoin, an asset is just a limited quantity of a unique symbol, and transferable to any Hivecoin address. Assets have been available for some time on other platforms such as Open Assets, Mastercoin, Counterparty, and as an ERC20[7] or ERC223 [8] token on Ethereum [9]. Assets created on the Hive protocol have several advantages: they are easier to use, tightly integrated with a native coin, and secured with fair POW mining & POS and open source code not run by a centralized organization.

**Uses for Assets**  
Assets or tokens can be used for anything the creator’s imagination can conjure. The ideas presented here are a sampling.

**Representing real world custodied physical or digital assets to tokens**
*  Gold bars
*  Silver coins
*  Physical Euros
*  Artwork
*  Land Deeds
*  DC Comics Presents #26
*  Energy credits (Electricity, Wood, Gas, Oil, Wind)

**Representing a share of a project**
*  Securities tokens: stock or shares of a company where the shares are represented by a token
rather than a physical stock certificate
*  Securities or partnership interests with the built-in ability to pay rewards in HVN (legal in
many free market countries)
*  Tokens which represent a coop, limited partnership, royalty sharing or profit sharing platform
*  A token which represents a crowd-funded item with the ability to transfer or resell the item

**Representing virtual goods**
*  Tickets to an event such as a Baltimore Hives game with the ability to resell
*  A license to allow an activity
*  An access token to use a service
*  In-game currency and items, transferable outside of the game platform

**Representing a credit**
*  Gift cards
*  Airline miles
*  Reward points

Satoshi Nakamoto described bitcoin as an implementation of Wei Dai’s bmoney [10], designed to afford users more control, security, and privacy than more centralized systems. A design with the potential to prevent violence and discrimination, given the holder of bitcoin remains private. Hivecoin aims to continue this implementation by focusing on assets other than cash, providing a platform that users can easily issue assets they control under the rules they establish on a secure blockchain.

## 4. Hivecoin Launch and Algorithm

Hivecoin was announced on February 25, 2021[11] and released binaries for mining on April 1, 2021,[12] the respective ninth anniversary of the announcement and launch of Bitcoin. Hivecoin is the bitcoin-like system that will allow users to issue and integrate assets into its blockchain. This will be accomplished in phases which build upon each other.

\- In progress

Create a platform like Bitcoin with a new mining algorithm, KAWPOW[13], invented by the Ravencoin developers, intended to prevent immediate dominance by mining pools, and future dominance by ASIC mining equipment.

Launch the token with no pre-mine and a fair launch to widely distribute the tokens.

Allow the mining rate to increase and the value of the HVN token to naturally grow and gradually disburse to holders that understand the value of the platform.

Utilize proof of work mining, not because it burns a scarce resource of electricity, or the requirement of computer hardware, but instead focuses on the most valuable part of the "work" which is building an ever-larger and time-based wall that protects user data from future tampering and censorship with every new layer.

Utilize proof of stake, not because it burns a scarce resource of electricity, or the requirement of computer hardware, but instead focuses on the most valuable part of the "work" which is building an ever-larger and time-based wall that protects user data from future tampering and censorship with every new layer.

Using proof of stake, Hivecoin can create as many online master nodes as possible. These master nodes will provide timely and fast data transmission services, rather than simply hiding behind the firewall. And these master nodes can participate in voting and participate in the community Autonomous, and decide where 5% of Hivecoin marketing expenses will be spent.

## 5. Asset Issuance & Transfer

Token names are guaranteed unique. The first to issue a token with a given name is the owner of that token project.

The issuer of a token burns 500 HVN and must provide a unique token name. The issuer determines the quantity issued, the number of decimal places, and whether they will be allowed to issue more of the same token in the future.

Allow the issuance of other tokens using similar method as Mastercoin, Counterparty, or CoinSpark[14].

Tightly integrate assets with the GUI wallet and create new RPC calls, which provides intuitive asset management. Easily issue new assets, report current balances, and transfer to other users.

The combination of open source and crypto enables interests to be aligned in ways that traditional structures cannot.

Fair and open source crypto projects can replace bosses, rulers, employees & corporate structure with aligned interests & economic choice for participants.

So, in some cases, whether one is selflessly, or selfishly motivated, open source may be a better model for many new and interesting types of projects than other structures. Hivecoin will allow projects to issue tokens to represent co-ops, corporations or partnerships.

Co-ops, for example, are a common organization form in which employees and participants are owners. Large organizations such as Credit Agricole, REI, Land O' Lakes, Ace Hardware, Co-op Kobe, Sunkist and Ocean Spray are structured as co-ops. Despite offering many advantages to participants, co-ops are sometimes difficult to structure and maintain. Tokenizing co-op interests opens many new ways this structure can be used to allocate resources and capital. Since the rules for each token can be changed by each issuer and the record keeping is done on the Hivecoin blockchain with the work distributed, organizations can adapt and deploy a variety of participation structures.

In addition, since the tokens can be made either unique, limited, or fungible by the issuer, token project managers will be able to have categories of token holders such as “Class A Shareholders”, “Lifetime social club members”, “Benefactors”, or “Holders of __ in game item”.

Tokens allow easier issuance of small scale public offerings.

“In the future the size distribution of multinationals will approach that of local business. The phase change between these states may be quite rapid as telecom and transport costs pass through a "melting point", creating a wide variety of new multinational small businesses, and industries to support those businesses.” Nick Szabo, Secure Property Titles with Owner Authority, 1998[15].

This also could decrease fraud, Economist Dr. Robert Shapiro noted significant evidence of Wall St. fraud which can be tied to custody issues (Patrick Byrne, PhD [16]).

Only an open protocol will work in a global economy where there are multiple jurisdictions, each with complex and conflicting regulations.

## 6. Rewards

Allow the payment of rewards in the native token. With a single command the reward, denominated in HVN, is automatically divided evenly and sent pro-rata to the holders of the asset.

Example:

A young child, in a country that permits it, could create a token that represents a lemonade stand business. Suppose she creates 10,000 LEMONADE tokens. These tokens could be used to raise funds for the lemonade stand at AUD$0.01 per LEMONADE token allowing her to raise AUD$ to build her business. These tokens can be sold and transferred easily by the owners. Suppose the lemonade stand does extraordinarily well because the neighborhood is invested in this entrepreneurial project. Now our fictional eight-year-old wants to reward those who believed in her project. With one command, she can send profits - denominated in any value HVN may have - to LEMONADE token holders. There could even be new holders of LEMONADE tokens that she’s never met. The built-in ease of use should allow anyone, anywhere in the world to do so on a mobile phone, or computer running Windows, Mac, or Linux.

For such a global system to work it will need to be independent of regulatory jurisdictions. This is not due to ideological belief, but practicality: if the rails for blockchain asset transfer are not censorship resistant and jurisdiction agnostic, any given jurisdiction may conflict with another. In legacy systems, wealth was generally confined in the jurisdiction of the holder and therefore easy to control based on the policies of that jurisdiction. Because of the global nature of blockchain technology, any protocol level ability to control wealth will potentially place jurisdictions in conflict and will not be able to operate fairly.

## 7. Unique Tokens

Unique tokens allow token holders to create unique assets. Like ERC721 tokens, unique tokens are guaranteed to be unique and only one will exist. Unique tokens can change ownership by sending the unique token to another user’s address.

Some examples of unique tokens:

*  Imagine an art dealer issues the asset named ART. The dealer can then make unique ART assets by attaching a name or a serialized number to each piece of art. These unique tokens can be transferred to the new owner along with the artwork as a proof of authenticity. The tokens ART:MonaLisa and ART:VenusDeMilo are not fungible and represent distinct pieces of art.

*  A software developer can issue the asset with the name of their software ABCGAME, and then assign each ABCGAME token a unique id or license key. The game tokens could be transferred as the license transfers. Each token ABCGAME:398222 and ABCGAME:423655 are unique tokens.

*  In game assets. A game ZYX_GAME could create unique limited edition in-game assets that are owned and used by the game player. Example: ZYX_GAME:SwordOfTruth005 and ZYX_GAME:HammerOfThor These in game assets could then be kept, traded with other players via QR codes and wallets or uploaded into an upgrade or different version of a game.

*  HVN based unique assets can be tied to real world assets. Create an asset named GOLDVAULT. Each gold coin or gold bar in a vault can be serialized and audited. Associated unique assets GOLDVAULT:444322 and GOLDVAULT:555994 can be created to represent the specific assets in the physical gold vault. The public nature of the chain allows for full transparency.

Example:

The holder of the token CAR could issue a unique token for each car by including the VIN number.

Example: CAR:19UYA31581L000000

Some use cases for unique assets include:

*  Software licensing
*  Car registration
*  Proof of authenticity tokens to transfer along with items that could be counterfeited
*  A token that allows communication on a channel (see Messaging)

## 8. Messaging Stakeholders

A common problem with tokens/assets is that the token issuer cannot communicate with the token holders. This must be handled very carefully because the token holders do not always wish to be identified. The communication should allow the token holder to opt-out at any time. The message system should only allow select parties to use the message channel so that it is not a spam conduit.


The messaging system uses unique tokens to allow communication on the main token channel. For example, the COMPANY token would have a ~COMPANY:Alert token which allows alerts to be sent to all holders of COMPANY.

Newsletters, game developers, non-profits, activist organizations, corporations and other entities will be able to issue tokens for specific users and then message those users but unlike email or other messaging services, the messaging itself will be enabled only for token holders, thereby making the token transferable.

Messaging to token holders by authorized senders will be layered on top of the unique assets. The unique assets will act as a "talking stick" allowing messages to be sent by the channel owner. The KAAAWWW Protocol will be published with more information on this separately.

## 9. Voting

One of the problems, among many, with the existing US financial system is that all the shares are held in street name. In this age of fast communication, this makes holding a vote ridiculously difficult. A public company that issues shares on Nasdaq, as an example, will have to pay a quasi-monopoly company just to get the mailing addresses of their own shareholders at a given point in time. Then, a physical (dead tree) mailing must be sent out to shareholders with information on how to vote along with a proxy voting form.

By using the messaging system, the holders of a token can be notified of the vote, and by automatically issuing a VOTE token to every holder of a token, the vote can be automated from the client or through a web or mobile interface using the protocol built into Hivecoin.

Tokens are created to represent votes. Hivecoin will create an exact number of VOTE tokens and distribute them 1:1 to the token holders. These votes can be sent via the protocol to addresses that tally the votes. Because the voting tokens move the same way as assets, delegation of votes - sometimes known as delegative or liquid democracy[17] - is possible.

## 10. Privacy

<p align="center"><i>
It's a community where the threat of violence is impotent because violence is impossible, and violence is impossible because its participants cannot be linked to their true names or physical locations. (Wei Dai)
</i></p>  
Privacy is key in investments and tokens because financial systems function better when assets are fungible and can trade in a frictionless manner. The project should seek to strengthen privacy in any way possible as future technological improvements are made.

As capabilities like messaging, assets, and rewards are added, privacy will be preserved in the same way that UTXO based cryptocurrencies separate identity from public addresses.

“Since we desire privacy, we must ensure that each party to a transaction have knowledge only of that which is directly necessary for that transaction. Since any information can be spoken of, we must ensure that we reveal as little as possible. In most cases personal identity is not salient.  
... When my identity is revealed by the underlying mechanism of the transaction, I have no privacy. I cannot here selectively reveal myself; I must always reveal myself.  
“Therefore, privacy in an open society requires anonymous transaction systems. Until now, cash has been the primary such system. An anonymous transaction system is not a secret transaction system. An anonymous system empowers individuals to reveal their identity when desired and only when desired; this is the essence of privacy.” (E. Hughes)[18].

## 11. Additional

Other projects can use this chain. Second layer solutions, particularly those being built for projects which share the code base of Bitcoin can be built on the Hivecoin project. RSK, the Lightning Network, confidential transactions, and other scalability improvements, etc. to various open source projects could benefit projects built on this platform.

## 12. Conclusion

Hivecoin is a platform coin built on the UTXO[19] model of Bitcoin. Modifying Bitcoin code to add these capabilities is not practical, but Hivecoin is a platform built from a code fork and issuing newly mined HVN. Hivecoin will be adding assets, rewards, unique assets, messaging, and voting. The Hive protocol’s capabilities will be rolled out in phases which will be done as a planned hard fork upgrade. The code base is designed allow users and developers to maintain a secure, decentralized, and tamper resistant network.

The Hivecoin project can also serve as a base and starting point for projects, second layer solutions, experiments, and business ideas which might benefit from either the Bitcoin-based code base with adjustments or the native additional features added to the Hivecoin blockchain.

_The Inuit, Tlinglit, Tahitian, Chukchi, Sioux, the Haida, and many others call Hive the magical keeper of secrets, the trickster, friend of the First Men and Creator of the World - an idea or force able to shift, change, and create something from nothing. In open source, the power of the crowd can accomplish much more than any one person or organization. All are welcome to contribute._

---

References

[1] S. Nakamoto, “Bitcoin: A Peer-to-Peer Electronic Cash System https://bitcoin.org/bitcoin.pdf  
[2] https://bhiveewcoin.com/assets/Whitepapers/2ndBitcoinWhitepaper.pdf  
[3] https://counterparty.io/  
[4] https://en.bitcoin.it/wiki/Colored_Coins  
[5] https://en.bitcoin.it/wiki/OP_RETURN  
[6] https://bitcoinwisdom.com/bitcoin/difficulty  
[7] https://theethereum.wiki/w/index.php/ERC20_Token_Standard  
[8] https://github.com/Dexaran/ERC223-token-standard  
[9] https://www.ethereum.org/  
[10] W. Dei, “B-Money” [http://www.weidai.com/bmoney.txt](http://www.weidai.com/bmoney.txt)  
[11] B. Fenton, “Hivecoin: A digital peer to peer network for the facilitation of asset transfers.” https://medium.com/@hivecoin/hivecoin-4683cd00f83c  
[12] https://github.com/HiveProject/Hivecoin  
[13] T. Black, J. Weight “X16R” Algorithm White Paper https://hivecoin.org/wp-content/uploads/2018/03/X16R-Whitepaper.pdf  
[14] [http://coinspark.org/developers/assets-introduction/](http://coinspark.org/developers/assets-introduction/)  
[15] N. Szabo, “Secure Property Titles with Owner Authority” [http://nakamotoinstitute.org/secure-property-titles/#selection-7.7-7.50](http://nakamotoinstitute.org/secure-property-titles/#selection-7.7-7.50)  
[16] https://www.forbes.com/2008/09/23/naked-shorting-trades-oped-cx_pb_0923byrne.html#63076e102e6c  
[17] https://en.wikipedia.org/wiki/Delegative_democracy  
[18] E. Hughes https://www.activism.net/cypherpunk/manifesto.html  
[19] https://bitcoin.org/en/glossary/unspent-transaction-output  
