Hive Core integration/staging tree
==================================

https://Hivecoin.org

What is Hivecoin?
-----------------

Hivecoin is an experimental digital currency that enables instant payments to
anyone, anywhere in the world. Hivecoin uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money and asset are carried
out collectively by the network.

License
-------

Hive Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/HiveProject2021/Hivecoin/tags) are created
regularly to indicate new official, stable release versions of Hive Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

Testnet is now up and running and available to use during development. There is an issue when connecting to the testnet that requires the use of the -maxtipage parameter in order to connect to the test network initially. After the initial launch the -maxtipage parameter is not required.

Use this command to initially start Hived on the testnet. <code>./Hived -testnet -maxtipage=259200</code>

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

There are also [regression and integration tests](/test), written
in Python, that are run automatically on the build server.
These tests can be run (if the [test dependencies](/test) are installed) with: `test/functional/test_runner.py`

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

About Hivecoin
--------------

A digital peer to peer network for the facilitation of asset transfer.

Hive stands for equality and collaboration.

The Hivecoin project is launched based on the hard work and continuous effort of Bitcoin & Ravencoin developers who made over 14,000 commits over the life to date of the Bitcoin and Ravencoin project. We are eternally grateful to you for your efforts and diligence in making a secure network and for their support of free and open source software development.  The Hivecoin experiment is made on the foundation you built.

Thank you to the Bitcoin developers.

Thank you to the Ravencoin developers.

Abstract
--------

Hivecoin aims to implement a blockchain which is optimized specifically for the use case of transferring assets such as securities from one holder to another. Based on the extensive development and testing of Bitcoin and Ravencoin, Hivecoin is built on a fork of the Ravencoin code. In order to commemorate the efforts made by Ravencoinon Bitcoin, Hivecoin inherited the same number of coins and block time as Ravencoinon.Hivecoin is free and open source and will be issued and mined transparently with no pre-mine, developer allocation or any other similar set aside. Hivecoin is intended to prioritize user control, privacy and censorship resistance and be jurisdiction agnostic while allowing simple optional additional features for users based on need.

A blockchain is a ledger showing the value of something and allowing it to be transferred to someone else. Of all the possible uses for blockchains, the reporting of who owns what is one of the core uses of the technology.  This is why the first and most successful use case for blockchain technology to date has been Bitcoin.

Hivecoin is designed to be a use case specific blockchain designed to efficiently handle one specific function: the transfer of assets from one party to another.

Bitcoin is and always should be focused on its goals of being a better form of money. Bitcoin developers will unlikely prioritize improvements or features which are specifically beneficial to the facilitation of token transfers.  One goal of the Hivecoin project is to see if a use case specific blockchain and development effort can create code which can either improve existing structures like Bitcoin or provide advantages for specific use cases.

In the new global economy, borders and jurisdictions will be less relevant as more assets are tradable and trade across borders is increasingly frictionless. In an age where people can move significant amounts of wealth instantly using Bitcoin, global consumers will likely demand the same efficiency for their securities and similar asset holdings.

For such a global system to work it will need to be independent of regulatory jurisdictions.  This is not due to ideological belief but practicality: if the rails for blockchain asset transfer are not censorship resistance and jurisdiction agnostic, any given jurisdiction may be in conflict with another.  In legacy systems, wealth was generally confined in the jurisdiction of the holder and therefore easy to control based on the policies of that jurisdiction. Because of the global nature of blockchain technology any protocol level ability to control wealth would potentially place jurisdictions in conflict and will not be able to operate fairly.


Test
