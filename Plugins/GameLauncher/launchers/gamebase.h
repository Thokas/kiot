// SPDX-FileCopyrightText: 2025 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once
#include <KIOTShared/kiotshared.h>
#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>
#include <QFileSystemWatcher>
#include <QProcess>


#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QString>
#include <QTextStream>
#include <QVariantMap>
#include <QTimer>
#include <QApplication>
#include <QByteArray>
#include <QLoggingCategory>
#include <QProcess>
#include <QCollator>
#include <QLocale>

using KIOTShared::PlatformHelper;

struct GameData {
    QString launcher;     // "Steam", "Heroic", "Lutris", etc.
    QString gameId;       // AppID or internal ID
    QString gameName;     // e.g. "Skyrim Special Edition"
    QString displayName;
    QString runner;       // e.g. "Proton GE-Proton9-2", "lutris-f wine"
    
    QString installPath;  // Where the game's files are (.exe)
    QString prefixPath;   // E.g. ~/.local/share/steam/steamapps/compatdata/489830/pfx
    QString exePath;      // Main exe file of the game
    QString iconPath;    // Path to the icon
    QString envVariables;
    QString launchOptions;
};

class GameBase : public QObject
{
    Q_OBJECT

public:
    explicit GameBase(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~GameBase() = default;

    // === Pure Virtuals / May be overridden by subclasses ===
    virtual QString launcherName() const = 0;
    virtual bool isLauncherInstalled() const = 0;
    virtual QMap<QString, GameData> scanGames() = 0;

    // Shared function to get the cached games
    QMap<QString, GameData> getGameList() const { return m_games; }


Q_SIGNALS:
    void gamesUpdated();

protected:
    QMap<QString, GameData> m_games;
    QFileSystemWatcher m_watcher; // Cloned into each subclass to watch the relevant config files
};