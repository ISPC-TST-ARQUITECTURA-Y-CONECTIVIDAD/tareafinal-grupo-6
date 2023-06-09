#include <Arduino.h>
#define TINY_GSM_MODEM_SIM800
#include <SoftwareSerial.h>
#include <Adafruit_SSD1306.h>//libreria necesaria para dsiplay
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64);//Especifico display
#include <TinyGPS.h> //incluimos libreria TinyGPS
int timeOffset = -3; // Ajuste horario en horas (ejemplo: -3 para GMT-3 Arg)
TinyGPS gps;         // Declaramos el objeto gps
int year;
byte month, day, hour, minute, second, hundredths;
unsigned long chars;
unsigned short sentences, failed_checksum;
String idtracker = "1";
float lat_b = 0;
float lon_b = 0;

SoftwareSerial SerialAT(5, 18);
SoftwareSerial serialgps(15, 2);

#include <TinyGsmClient.h>

const char apn[] = "gprs.personal.com";
const char gprsUser[] = "gprs";
const char gprsPass[] = "gprs";
const char *broker = "brokergrupo6.ddns.net";
const char *topic = "/datos/traker/";
#include <PubSubClient.h>

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);

uint32_t lastReconnectAttempt = 0;

void mqttCallback(char *topic, byte *payload, unsigned int len)
{}

boolean mqttConnect()
{
  Serial.print("Conectando con ");
  Serial.println(broker);
  boolean status = mqtt.connect("GsmClientTest");
  if (status == false)
  {
    Serial.println("fail");
    return false;
  }
  Serial.println("Conectado al Broker");
  mqtt.subscribe(topic);
  return mqtt.connected();
}

void red_gsm()
{
  Serial.println("Network disconnected");
  if (!modem.waitForNetwork(180000L, true))
  {
    Serial.println(" Error de conexion !!!!");
    delay(10000);
    return;
  }
  if (modem.isNetworkConnected())
  {
    Serial.println("Network re-connected");
  }
}

void gprs_gsm()
{
  Serial.println("GPRS disconnected!");
  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  if (modem.isGprsConnected())
  {
    Serial.println("GPRS reconnected");
  }
}

void broker_gsm()
{
  uint32_t t = millis();
  if (t - lastReconnectAttempt > 10000L)
  {
    lastReconnectAttempt = t;
    if (mqttConnect())
    {
      lastReconnectAttempt = 0;
    }
  }
  delay(100);
  return;
}

void setup()
{
  Serial.begin(9600);
  SerialAT.begin(9600);
  serialgps.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  Serial.println("Iniciando modem...");
  modem.restart();
  delay(6000);
  display.setCursor(0, 0);
  display.println(F("Conectando con "));
  display.display();
  delay(1000);
  display.setCursor(0, 10);
  display.println(apn);
  display.display();
  delay(1000);


  //Serial.print(F("Conectando con "));
  //Serial.println(apn);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass))
  {
    Serial.println("Error en la conexion !!!!!!!");
    delay(10000);
    return;
  }
  Serial.println("Conexion Exitosa !!");
  if (modem.isGprsConnected())
  {
    Serial.println("GPRS conectado");
  }
  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
  delay(600);
  gprs_gsm();
  delay(600);
  broker_gsm();
  delay(600);
}

void loop()
{
  
  if (!modem.isNetworkConnected())
  {
    red_gsm();
  }
  if (!modem.isGprsConnected())
  {
    gprs_gsm();
  }
  if (!mqtt.connected())
  {
    broker_gsm();
  }
  mqtt.loop();
  while (serialgps.available())
  {
    {
      int c = serialgps.read();
      if (gps.encode(c))
      {
        float latitude, longitude;
        gps.f_get_position(&latitude, &longitude);                                     // obtenemos lat y long, luego mostramos por serial
        gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &hundredths); // obtenemos los datos
        int hora = hour + timeOffset;                                                  // seteamos el uso horario Arg
        int dia = day;
        if (hora < 0)
        { // si hora <0 significa que en Arg todavia no paso medianoche, entonces ajustamos el dia local
          dia = day - 1;
        }
        if (hora < 0)
        {
          hora += 24;
        }
        else if (hora >= 24)
        {
          hora -= 24;
        }
        float lat_a = round(latitude * 1000.0) / 1000.0;
        float lon_a = round(longitude * 1000.0) / 1000.0;
        if ((lat_a != lat_b) || (lon_a != lon_b)) {
          String dato = String(year) + String("-") + String(month) + String("-") + String(dia) + "," +
                        String(hora) + String(":") + String(minute) + String(":") + String(second) + "," +
                        idtracker + "," +
                        String(latitude, 5) + "," +
                        String(longitude, 5) + "," +
                        String(gps.f_speed_kmph());
          mqtt.publish(topic, dato.c_str());
          delay(3000);
          lat_b = round(latitude * 1000.0) / 1000.0;
          lon_b = round(longitude * 1000.0) / 1000.0;
        }
        delay(1000);
      }
    }
  }
}
