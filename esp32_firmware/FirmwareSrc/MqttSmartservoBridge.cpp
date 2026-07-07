#include "MqttSmartServoBridge.h"

// Constructor
MqttSmartServoBridge::MqttSmartServoBridge()
    : client(wifi)
{
}

// Lifecycle and Connections
void MqttSmartServoBridge::begin(const char* ssid, const char* pass, const char* broker, const char* client_id)
{
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    client.setServer(broker, 1883);

    client.setCallback([this](char* topic, byte* payload, unsigned int length)
    {
        char msg[50];

        for (int i = 0; i < length; i++)
            msg[i] = (char)payload[i];

        msg[length] = '\0';

        lastMessage = msg;
        newMessage = true;
    });

    this->client_id = client_id;
}

void MqttSmartServoBridge::loop()
{
    if (!client.connected())
    {
        while (!client.connected())
        {
            if (client.connect(client_id))
            {
                Serial.println("MQTT Connected");

                if (subscribedTopic != nullptr)
                {
                    client.subscribe(subscribedTopic);

                    Serial.print("Subscribed to: ");
                    Serial.println(subscribedTopic);
                }
            }
            delay(500);
        }
    }

    client.loop();
}

// MQTT Pub/Sub Actions
void MqttSmartServoBridge::publish(const char* topic, const char* msg)
{
    client.publish(topic, msg);
}

void MqttSmartServoBridge::subscribe(const char* topic)
{
    subscribedTopic = topic;
}

// Message Accessors / Getters
bool MqttSmartServoBridge::hasNewMessage()
{
    return newMessage;
}

const char* MqttSmartServoBridge::getMessage()
{
    newMessage = false;
    return lastMessage.c_str();
}
