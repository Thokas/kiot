// SPDX-FileCopyrightText: 2026 Thomas Kastenholz
// SPDX-License-Identifier: LGPL-2.1-or-later

// The German catalogs as the build compiles them (po/de, ki18n_install): they load, and the daemon's C++
// and QML strings and the KCM's come out in German, each from its own catalog.
//
// gettext ignores the language asked for while the messages locale is C (or C.UTF-8, as glibc treats it the
// same), which is all a bare container has; a desktop session always has a real one. Without en_US.UTF-8 or
// de_DE.UTF-8 the test skips, unless KIOT_TRANSLATION_TESTS=required (CI generates en_US.UTF-8).

#include <KLocalizedQmlContext>
#include <KLocalizedString>
#include <QQmlComponent>
#include <QQmlEngine>
#include <QTest>

#include <clocale>

class TranslationsTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void initTestCase()
    {
        if (!std::setlocale(LC_MESSAGES, "en_US.UTF-8") && !std::setlocale(LC_MESSAGES, "de_DE.UTF-8")) {
            if (qEnvironmentVariable("KIOT_TRANSLATION_TESTS") == QLatin1String("required")) {
                QFAIL("neither en_US.UTF-8 nor de_DE.UTF-8 is available, and KIOT_TRANSLATION_TESTS=required");
            }
            QSKIP("neither en_US.UTF-8 nor de_DE.UTF-8 is available: gettext won't translate in the C locale");
        }
        // as main() sets it for the daemon
        KLocalizedString::setApplicationDomain(QByteArrayLiteral("kiot"));
        KLocalizedString::addDomainLocaleDir(QByteArrayLiteral("kiot"), QStringLiteral(KIOT_LOCALE_DIR));
        KLocalizedString::addDomainLocaleDir(QByteArrayLiteral("kcm_kiot"), QStringLiteral(KIOT_LOCALE_DIR));
        KLocalizedString::setLanguages({QStringLiteral("de")});
        QVERIFY2(KLocalizedString::isApplicationTranslatedInto(QStringLiteral("de")), KIOT_LOCALE_DIR);
    }

    void testDaemon()
    {
        QCOMPARE(i18nc("@action:inmenu", "Quit"), QStringLiteral("Beenden"));
        QCOMPARE(i18nc("@item:inmenu MQTT connection", "Status: Connecting"), QStringLiteral("Status: Verbindung wird hergestellt"));
        QCOMPARE(i18nc("@title:window %1 is the application name", "%1 Settings", QStringLiteral("kiot")), QStringLiteral("kiot-Einstellungen"));
        QCOMPARE(i18nc("@info notification", "Please configure your MQTT settings"), QStringLiteral("Bitte richten Sie Ihre MQTT-Verbindung ein"));
    }

    void testQml()
    {
        // the settings window's views, through the context MainWindow::setupQml() gives its engine
        QQmlEngine engine;
        KLocalization::setupLocalizedContext(&engine);
        QQmlComponent component(&engine);
        component.setData(R"(import QtQml
QtObject {
    property string label: i18n("Are you sure you want to delete this script?")
    property string withArgument: i18n("Script: %1", "browser")
    property string example: i18nc("@info:placeholder %1 are examples", "e.g., %1", "mdi:web")
})",
                          QUrl());
        std::unique_ptr<QObject> object(component.create());
        QVERIFY2(object, qPrintable(component.errorString()));
        QCOMPARE(object->property("label").toString(), QStringLiteral("Möchten Sie dieses Skript wirklich löschen?"));
        QCOMPARE(object->property("withArgument").toString(), QStringLiteral("Skript: browser"));
        QCOMPARE(object->property("example").toString(), QStringLiteral("z. B. mdi:web"));
    }

    void testSystemSettingsModule()
    {
        QCOMPARE(i18nd("kcm_kiot", "More Settings…"), QStringLiteral("Weitere Einstellungen …"));
        QCOMPARE(i18nd("kcm_kiot", "Show system tray:"), QStringLiteral("Im Systemabschnitt anzeigen:"));
        // and not from the daemon's catalog
        QCOMPARE(i18nd("kcm_kiot", "Quit"), QStringLiteral("Quit"));
    }
};

QTEST_GUILESS_MAIN(TranslationsTest)

#include "translationstest.moc"
