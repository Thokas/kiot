// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "VdfParser.h"
#include <QRegularExpression>

QVariantMap VdfParser::parseFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QVariantMap();
    }
    
    QTextStream stream(&file);
    return parseBlock(stream);
}

QVariantMap VdfParser::parseString(const QString &content)
{
    QString mutableContent = content;
    QTextStream stream(&mutableContent);
    return parseBlock(stream);
}

QString VdfParser::cleanToken(const QString &token)
{
    QString trimmed = token.trimmed();
    // Remove the quotes around strings if there are any
    if (trimmed.startsWith('"') && trimmed.endsWith('"') && trimmed.length() >= 2) {
        trimmed = trimmed.mid(1, trimmed.length() - 2);
    }
    return trimmed;
}

QVariantMap VdfParser::parseBlock(QTextStream &stream)
{
    QVariantMap result;
    QString keyToken = "";

    // Regex to extract either "quoted strings" or plain words
    static const QRegularExpression tokenRegex(R"("(?:[^"\\]|\\.)*"|\S+)");

    while (!stream.atEnd()) {
        QString line = stream.readLine().trimmed();

        // Skip empty lines and C++ style comments
        if (line.isEmpty() || line.startsWith("//")) {
            continue;
        }

        // If we hit a closing brace, this section/block is done
        if (line == "}") {
            break;
        }

        // Get all tokens on the line (e.g. `"appid"` and `"1623730"`)
        QRegularExpressionMatchIterator it = tokenRegex.globalMatch(line);
        QStringList tokens;
        while (it.hasNext()) {
            tokens.append(cleanToken(it.next().captured(0)));
        }

        if (tokens.isEmpty()) {
            continue;
        }

        // Case 1: The start of a new sub-block (e.g. "AppState" { or InstalledDepots {)
        if (tokens.size() == 1) {
            if (tokens[0] == "{") {
                // We already have the key (e.g. "appstate"), parse the subsection recursively!
                if (!keyToken.isEmpty()) {
                    result[keyToken.toLower()] = parseBlock(stream);
                    keyToken.clear();
                }
            } else {
                // This is the key of an upcoming block or value
                keyToken = tokens[0];
            }
        }
        // Case 2: Key and value on the same line (e.g. "appid" "1623730")
        else if (tokens.size() >= 2) {
            QString key = tokens[0].toLower();   // Make the key case-insensitive
            QString value = tokens[1];

            // Check whether the next token on the same line was a '{' (e.g. "InstalledDepots" {)
            if (value == "{") {
                result[key] = parseBlock(stream);
            } else {
                result[key] = value;
            }
            keyToken.clear();
        }
    }

    return result;
}