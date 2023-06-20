#include <Arduino.h>
#define TINY_GSM_MODEM_SIM800
#include <SoftwareSerial.h>
SoftwareSerial SerialAT(5, 18);

#define TINY_GSM_USE_GPRS true
#include <TinyGsmClient.h>

const char apn[]      = "gprs.personal.com";
const char gprsUser[] = "gprs";
const char gprsPass[] = "gprs";

const char* broker = "brokergrupo6.ddns.net";
const char* topic = "/datos/traker/";
#include <PubSubClient.h>

TinyGsm       modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient  mqtt(client);

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.write(payload, len);
  Serial.println();
}

boolean mqttConnect() {
  Serial.print("Conectando con ");
  Serial.println(broker);
  boolean status = mqtt.connect("GsmClientTest");
  if (status == false) {
    Serial.println("fail");
    return false;
  }
  Serial.println("Conectado al Broker");
  mqtt.publish(topic, "Prueba de mqtt");
  mqtt.subscribe(topic);
  return mqtt.connected();
}

void setup() {
    Serial.begin(9600);
    delay(10);
    SerialAT.begin(9600);
    delay(6000);
    Serial.println("Iniciando modem...");
    modem.restart();
    Serial.print(F("Conectando con "));
    Serial.println(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println("Error en la conexion !!!!!!!");
    delay(10000);
    return;
    }
    Serial.println("Conexion Exitosa !!");
    if (modem.isGprsConnected()) { Serial.println("GPRS conectado"); }
    mqtt.setServer(broker, 1883);
    mqtt.setCallback(mqttCallback);
}

void loop() {
  if (!modem.isNetworkConnected()) {
    Serial.println("Network disconnected");
    if (!modem.waitForNetwork(180000L, true)) {
      Serial.println(" Error de conexion !!!!");
      delay(10000);
      return;
    }
    if (modem.isNetworkConnected()) {
      Serial.println("Network re-connected");
    }

#if TINY_GSM_USE_GPRS
    if (!modem.isGprsConnected()) {
      Serial.println("GPRS disconnected!");
      Serial.print(F("Connecting to "));
      Serial.print(apn);
      if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.println(" fail");
        delay(10000);
        return;
      }
      if (modem.isGprsConnected()) { Serial.println("GPRS reconnected"); }
    }
#endif
  }

  if (!mqtt.connected()) {
    uint32_t t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) { lastReconnectAttempt = 0; }
    }
    delay(100);
    return;
  }

  mqtt.loop();
}