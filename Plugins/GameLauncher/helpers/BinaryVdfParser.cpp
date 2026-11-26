// SPDX-FileCopyrightText: 2026 Odd Østlie <theoddpirate@gmail.com>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "BinaryVdfParser.h"
#include <QDebug>

QString BinaryVdfParser::readNullTerminatedString(QDataStream &stream)
{
    QByteArray bytes;
    char ch;
    while (!stream.atEnd()) {
        stream.readRawData(&ch, 1);
        if (ch == 0x00) {
            break;
        }
        bytes.append(ch);
    }
    return QString::fromUtf8(bytes);
}

QVariantMap BinaryVdfParser::parseMap(QDataStream &stream)
{
    QVariantMap result;

    while (!stream.atEnd()) {
        quint8 type;
        stream >> type;

        // 0x08 means we have reached the end of this block
        if (type == 0x08) {
            break;
        }

        QString key = readNullTerminatedString(stream);

        switch (type) {
        case 0x00: { // Subsection / Map
            result[key] = parseMap(stream);
            break;
        }
        case 0x01: { // String
            QString val = readNullTerminatedString(stream);
            result[key] = val;
            break;
        }
        case 0x02: { // 32-bit Integer
            quint32 val;
            stream >> val;
            result[key] = val;
            break;
        }
        case 0x07: { // 64-bit Integer
            quint64 val;
            stream >> val;
            result[key] = val;
            break;
        }
        default:
            // Unknown type, stop parsing this block to avoid corruption
            return result;
        }
    }

    return result;
}

QVariantMap BinaryVdfParser::parseShortcutsFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return QVariantMap();
    }

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    // The first byte must be 0x00 (root block)
    quint8 firstByte;
    stream >> firstByte;
    if (firstByte != 0x00) {
        return QVariantMap();
    }

    // Read the root name (usually "shortcuts")
    QString rootName = readNullTerminatedString(stream);

    QVariantMap rootMap;
    rootMap[rootName] = parseMap(stream);

    file.close();
    return rootMap;
}