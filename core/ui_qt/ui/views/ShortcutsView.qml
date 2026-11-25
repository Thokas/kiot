import QtQuick 2.15
import QtQuick.Controls 2.15 as QQC2
import QtQuick.Layouts
import QtQuick.Dialogs

Flickable {
    id: root
    contentWidth: width
    contentHeight: contentColumn.height
    clip: true

    property var settingsManager: null
    property string section: "Shortcuts"

    // --- INTERNE DIALOGER ---
    QQC2.Dialog {
        id: newShortcutDialog
        title: i18n("Create New Shortcut")
        standardButtons: QQC2.Dialog.Ok | QQC2.Dialog.Cancel
        
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            
            GridLayout {
                columns: 2
                columnSpacing: 10
                rowSpacing: 10
                
                QQC2.Label { text: i18n("Shortcut ID:") }
                QQC2.TextField {
                    id: newShortcutNameField
                    placeholderText: i18nc("@info:placeholder %1 are examples", "e.g., %1", "myShortcut, customAction")
                    Layout.fillWidth: true
                }
                
                QQC2.Label { text: i18n("Display Name:") }
                QQC2.TextField {
                    id: newShortcutDisplayNameField
                    placeholderText: i18nc("@info:placeholder %1 are examples", "e.g., %1", "Do a thing, Custom Action")
                    Layout.fillWidth: true
                }
            }
        }
        
        onAccepted: {
            if (newShortcutNameField.text.trim() !== "" && settingsManager) {
                var shortcutId = newShortcutNameField.text.trim()
                var displayName = newShortcutDisplayNameField.text.trim() || shortcutId
                settingsManager.saveNestedConfigValue("Shortcuts", shortcutId, "Name", displayName)
                
                newShortcutNameField.text = ""
                newShortcutDisplayNameField.text = ""
            }
        }
        onRejected: {
            newShortcutNameField.text = ""
            newShortcutDisplayNameField.text = ""
        }
    }

    QQC2.Dialog {
        id: deleteShortcutDialog
        title: i18n("Delete Shortcut")
        standardButtons: QQC2.Dialog.Yes | QQC2.Dialog.No
        property string shortcutId: ""

        ColumnLayout {
            QQC2.Label {
                text: i18n("Are you sure you want to delete this shortcut?")
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
            QQC2.Label {
                text: deleteShortcutDialog.shortcutId ? i18n("Shortcut: %1", deleteShortcutDialog.shortcutId) : ""
                wrapMode: Text.WordWrap
                Layout.fillWidth: true
            }
        }

        onAccepted: {
            if (deleteShortcutDialog.shortcutId && settingsManager) {
                settingsManager.deleteNestedConfig("Shortcuts", deleteShortcutDialog.shortcutId)
            }
            deleteShortcutDialog.shortcutId = ""
        }
        onRejected: {
            deleteShortcutDialog.shortcutId = ""
        }
    }

    // --- HOVEDINNHOLD ---
    ColumnLayout {
        id: contentColumn
        width: parent.width
        spacing: 10

        QQC2.Label {
            text: i18n("Shortcuts")
            font.bold: true
            font.pixelSize: 16
            Layout.fillWidth: true
        }

        QQC2.Button {
            text: i18n("Create New Shortcut")
            onClicked: newShortcutDialog.open() // Opens the internal dialog directly!
            Layout.fillWidth: true
        }

        property var shortcutKeys: {
            var sectionData = settingsManager ? settingsManager.configSections[section] : {}
            if (!sectionData) return []
            return Object.keys(sectionData).sort()
        }

        Connections {
            target: settingsManager
            enabled: settingsManager !== null
            
            function onConfigSectionsChanged() {
                contentColumn.shortcutKeys = contentColumn.shortcutKeys
            }
        }

        Repeater {
            model: contentColumn.shortcutKeys
            
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 5
                
                property string shortcutId: modelData
                property var shortcutData: {
                    var sectionData = settingsManager ? settingsManager.configSections[section] : {}
                    return sectionData ? sectionData[shortcutId] : {}
                }
                
                QQC2.Button {
                    text: i18n("Delete")
                    onClicked: {
                        deleteShortcutDialog.shortcutId = shortcutId
                        deleteShortcutDialog.open() // Setter ID og åpner den interne dialogen direkte!
                    }
                    Layout.alignment: Qt.AlignRight
                }
                
                GridLayout {
                    columns: 2
                    columnSpacing: 10
                    rowSpacing: 10
                    Layout.fillWidth: true
                    
                    QQC2.Label { text: i18n("Name:"); Layout.alignment: Qt.AlignRight }
                    QQC2.TextField {
                        text: shortcutData["Name"] || ""
                        onEditingFinished: {
                            if (settingsManager) {
                                settingsManager.saveNestedConfigValue("Shortcuts", shortcutId, "Name", text)
                            }
                        }
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }
}