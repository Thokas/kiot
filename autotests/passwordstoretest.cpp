// SPDX-FileCopyrightText: 2026 Thomas Kastenholz
// SPDX-License-Identifier: LGPL-2.1-or-later

// PasswordStore without a wallet: testenv.h points the session bus nowhere, as on a machine without
// KWallet or Secret Service. The MQTT password then has to stay usable and must not get lost.

#include <QTest>

#include "Transport/passwordstore.h"
#include "testenv.h"

using KIOTShared::Transport::PasswordStore;

static KiotTest::IsolatedEnvironment s_env;

namespace
{
const QString host = QStringLiteral("broker.lan");
const QString user = QStringLiteral("kiot");

struct ReadResult {
    bool called = false;
    QString password;
};
} // namespace

class PasswordStoreTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testPasswordInFileIsUsedRightAway()
    {
        KiotTest::writeGeneralConfig({{QStringLiteral("host"), host}, {QStringLiteral("user"), user}, {QStringLiteral("password"), QStringLiteral("secret")}});

        ReadResult result;
        PasswordStore::read(user, host, this, [&result](const QString &password) {
            result.called = true;
            result.password = password;
        });
        // from the file, without waiting for the wallet
        QVERIFY(result.called);
        QCOMPARE(result.password, QStringLiteral("secret"));
    }

    void testWriteWithoutWalletKeepsPasswordInFile()
    {
        KiotTest::writeGeneralConfig({{QStringLiteral("host"), host}, {QStringLiteral("user"), user}});

        bool called = false;
        bool stored = true;
        PasswordStore::write(user, host, QStringLiteral("new-secret"), this, [&](bool ok) {
            called = true;
            stored = ok;
        });
        QTRY_VERIFY(called);
        QVERIFY(!stored);
        QCOMPARE(KiotTest::generalConfigOnDisk().readEntry("password"), QStringLiteral("new-secret"));

        // and the next start reads it from there
        ReadResult result;
        PasswordStore::read(user, host, this, [&result](const QString &password) {
            result.called = true;
            result.password = password;
        });
        QVERIFY(result.called);
        QCOMPARE(result.password, QStringLiteral("new-secret"));
    }

    void testNoPasswordAnywhere()
    {
        KiotTest::writeGeneralConfig({{QStringLiteral("host"), host}, {QStringLiteral("user"), user}});

        ReadResult result;
        PasswordStore::read(user, host, this, [&result](const QString &password) {
            result.called = true;
            result.password = password;
        });
        // the wallet's answer is asynchronous, and an unreachable wallet means no password
        QTRY_VERIFY(result.called);
        QVERIFY(result.password.isEmpty());
    }

    void testPlaceholderIsNotAPassword()
    {
        const QString placeholder = QStringLiteral("mqtt-password-here");
        KiotTest::writeGeneralConfig({{QStringLiteral("host"), host}, {QStringLiteral("user"), user}, {QStringLiteral("password"), placeholder}});

        ReadResult result;
        PasswordStore::read(user, host, this, [&result](const QString &password) {
            result.called = true;
            result.password = password;
        });
        QTRY_VERIFY(result.called);
        QVERIFY(result.password.isEmpty());
        QCOMPARE(KiotTest::generalConfigOnDisk().readEntry("password"), placeholder);
    }

    void testPasswordOfAnotherAccountIsNotHandedOut()
    {
        KiotTest::writeGeneralConfig({{QStringLiteral("host"), host}, {QStringLiteral("user"), user}, {QStringLiteral("password"), QStringLiteral("secret")}});

        ReadResult result;
        PasswordStore::read(QStringLiteral("someone-else"), host, this, [&result](const QString &password) {
            result.called = true;
            result.password = password;
        });
        QTRY_VERIFY(result.called);
        QVERIFY(result.password.isEmpty());
    }

    void testEmptyPasswordWithoutWallet()
    {
        KiotTest::writeGeneralConfig({{QStringLiteral("host"), host}, {QStringLiteral("user"), user}, {QStringLiteral("password"), QStringLiteral("old")}});

        bool called = false;
        PasswordStore::write(user, host, QString(), this, [&](bool) {
            called = true;
        });
        QTRY_VERIFY(called);
        // clearing the password also clears the copy in the file
        QVERIFY(KiotTest::generalConfigOnDisk().readEntry("password").isEmpty());
    }
};

QTEST_GUILESS_MAIN(PasswordStoreTest)

#include "passwordstoretest.moc"
