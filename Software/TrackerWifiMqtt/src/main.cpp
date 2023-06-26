#include <Arduino.h>
#include <WiFi.h>
#include <Adafruit_SSD1306.h>//libreria necesaria para dsiplay
#include <SoftwareSerial.h>//incluimos libreria SoftwareSerial
#include <TinyGPS.h>//incluimos libreria TinyGPS
#include <PubSubClient.h>
//#include <MQTTClient.h> 
#define WIFI_SSID ""
#define WIFI_PASS ""
const char* BROKER_HOST="brokergrupo6.ddns.net";
const int BROKER_PORT=1883;
#define TOPIC "/datos/traker/"
String idtracker = "";
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64);//Especifico display
int timeOffset = -3;  // Ajuste horario en horas (ejemplo: -3 para GMT-3 Arg)
TinyGPS gps;//Declaramos el objeto gps
//Declaramos pines GPS
#define pinRX 23
#define pinTX 19
SoftwareSerial serialgps(pinRX,pinTX);

WiFiClient espClient;
PubSubClient client(espClient);
//Declaramos la variables para la obtención de datos
int year;
byte month, day, hour, minute, second, hundredths;
unsigned long chars;
unsigned short sentences, failed_checksum;

void conectarWifi(){
  Serial.print("Conectando a WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

 
  //client.setCallback(onMessage);
}

//Configuramos la conexion al server mqtt
void reconnectMQTT() {
  // Loop hasta que estemos conectados al servidor MQTT
  while (!client.connected()) {
    Serial.println("Intentando conexión MQTT...");

    // Intentar conectarse al servidor MQTT
    if (client.connect("cliente")) {
      Serial.println("Conectado al servidor MQTT");

      // Suscribirse a los temas necesarios después de la conexión exitosa
      //client.subscribe("tema_suscripcion");
    } else {
      Serial.print("Error de conexión MQTT, rc=");
      Serial.print(client.state());
      Serial.println(" Reintentando en 5 segundos...");

      // Esperar 5 segundos antes de intentar nuevamente
      delay(5000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
display.begin(SSD1306_SWITCHCAPVCC, 0x3c);//Inicio display con el puerto I2C por defecto
display.clearDisplay();//Borro display
display.setTextColor(WHITE, BLACK);//Seteo display
Serial.begin(9600);//Iniciamos el puerto serie
serialgps.begin(9600);//Iniciamos el puerto serie del gps
Serial.println("");
Serial.println("GPS Iniciado");
Serial.println("---Esperando señal Satelital---");
Serial.println("");
display.setCursor(0, 0); // Seteo cursor
display.println("GPS Iniciado");//inprimo texto
display.setCursor(0, 10);
display.println("--Esperando senal--");
display.display();//Envio los datos
conectarWifi();
client.setServer(BROKER_HOST, BROKER_PORT);
delay(2000);
}

void loop() {
if (WiFi.status() != WL_CONNECTED){
  conectarWifi();
}
if (!client.connected()) {
    reconnectMQTT();
  }
while(serialgps.available()) 
{
int c = serialgps.read(); 
if(gps.encode(c)) 

{
float latitude, longitude;
gps.f_get_position(&latitude, &longitude); //obtenemos lat y long, luego mostramos por serial
Serial.print("Latitud/Longitud: "); 
Serial.print(latitude,5); 
Serial.print(", "); 
Serial.println(longitude,5);

gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths); //obtenemos los datos
  int hora = hour + timeOffset; //seteamos el uso horario Arg
  int dia= day;
  if (hora < 0){ //si hora <0 significa que en Arg todavia no paso medianoche, entonces ajustamos el dia local
    dia= day -1;
    
  }
  //realizamos el ajuste de la hora Arg mediante + o -
    if (hora < 0) {
          hora += 24;
        } else if (hora >= 24) {
          hora -= 24;
        }
//mostramos los datos
Serial.print("Fecha: "); Serial.print(dia); Serial.print("/"); 
Serial.print(month, DEC); Serial.print("/"); Serial.println(year);        
Serial.print("Hora: "); Serial.print(hora); Serial.print(":"); 
Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC); 
Serial.print("."); Serial.println(hundredths, DEC);
Serial.print("Rumbo (grados): "); Serial.println(gps.f_course()); 
Serial.print("Velocidad(kmph): "); Serial.println(gps.f_speed_kmph());
Serial.print("Satelites: "); Serial.println(gps.satellites());
Serial.println();
gps.stats(&chars, &sentences, &failed_checksum);

//Muestro datos en diplay Oled
display.clearDisplay();//Borro display
display.setCursor(0, 0);
display.println("Lat: "+String(latitude,5));
//display.setCursor(0, 10);
display.println("Long: "+String(longitude,5));
display.display();//Envio los datos
//display.setCursor(0, 20);
display.println("Fecha: "+String(dia)+"/"+String(month)+"/"+String(year));
//display.setCursor(0, 30);
display.println("Hora: "+String(hora)+":"+String(minute)+":"+String(second));
//display.setCursor(0, 40);
display.println("Rumbo grados:"+String(gps.f_course())); 
display.println("Vel(km/h): "+String(gps.f_speed_kmph()));
display.println("Satelites: "+String(gps.satellites()));
display.display();//Envio los datos

//Construimos el arreglo y enviamos los datos por MQTT
String dato=String(dia)+String(month)+String(year)+","+String(hora)+String(minute)+","+idtracker+","+String(latitude,5)+","+String(longitude,5)+","+String(gps.f_speed_kmph());
client.publish(TOPIC, dato.c_str());

delay(5000);
   }
  }
  

}
