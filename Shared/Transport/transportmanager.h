#pragma once

#include <QObject>
#include <QMqttClient>
#include <QTimer>

#include "KIOTShared/kiotshared_export.h"

namespace KIOTShared {
namespace Transport {

class KIOT_SHARED_EXPORT TransportManager : public QObject
{
    Q_OBJECT

public:
    explicit TransportManager(QObject *parent = nullptr);
    ~TransportManager() override;
    
    // Safe way for plugins/other modules to get a reference to the client
    static QMqttClient *mqttClient()
    {
        if(s_self && s_self->m_client)
            return s_self->m_client;
        return nullptr;
    }
    void doConnect();
    void doDisconnect();

Q_SIGNALS:
    void connectionStateChanged(QMqttClient::ClientState state);
    void mqttConfigMissing();

public Q_SLOTS:
    void handleStateChanged(QMqttClient::ClientState state);

private:
    static TransportManager *s_self;
    QMqttClient *m_client;
    QTimer *reconnectTimer = nullptr;
    bool m_passwordRead = false;
    bool m_connectRequested = false;
    bool initiateMqttClient();
};

} // namespace Transport
} // namespace KIOTShared