// Copyright (c) 2011-2014 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Raven Core developers
// Copyright (c) 2020-2021 The Hive Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef HIVE_QT_HIVEADDRESSVALIDATOR_H
#define HIVE_QT_HIVEADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class HiveAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit HiveAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** Hive address widget validator, checks for a valid hive address.
 */
class HiveAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit HiveAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // HIVE_QT_HIVEADDRESSVALIDATOR_H
