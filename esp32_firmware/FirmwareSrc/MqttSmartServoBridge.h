#pragma once

#include <WiFi.h>
#include <PubSubClient.h>

class MqttSmartServoBridge
{
private:
    WiFiClient wifi;
    PubSubClient client;
    const char* subscribedTopic = nullptr;
    const char* client_id = nullptr;
    String lastMessage;
    bool newMessage = false;

public:
    MqttSmartServoBridge();

    // Lifecycle and Connections
    void begin(const char* ssid, const char* pass, const char* broker, const char* client_id);
    void loop();

    // MQTT Pub/Sub Actions
    void publish(const char* topic, const char* msg);
    void subscribe(const char* topic);

    // Message Accessors / Getters
    bool hasNewMessage();
    const char* getMessage();
};
