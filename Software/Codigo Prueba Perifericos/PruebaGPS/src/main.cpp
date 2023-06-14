#include <Arduino.h>
#include <SoftwareSerial.h>//incluimos SoftwareSerial
#include <TinyGPS.h>//incluimos TinyGPS
//#include <TinyGPSPlus.h>
int timeOffset = -3;  // Ajuste horario en horas (ejemplo: -3 para GMT-3)
TinyGPS gps;//Declaramos el objeto gps
//Declaramos pines GPS
#define pinRX 23
#define pinTX 19
SoftwareSerial serialgps(pinRX,pinTX);
//Declaramos la variables para la obtención de datos
int year;
byte month, day, hour, minute, second, hundredths;
unsigned long chars;
unsigned short sentences, failed_checksum;

void setup()
{

Serial.begin(9600);//Iniciamos el puerto serie
serialgps.begin(9600);//Iniciamos el puerto serie del gps
//Imprimimos:
Serial.println("");
Serial.println("GPS Iniciado");
Serial.println(" ---Esperando señal del satelite--- ");
Serial.println("");
}

void loop()
{
while(serialgps.available()) 
{
int c = serialgps.read(); 
if(gps.encode(c)) 

{
float latitude, longitude;
gps.f_get_position(&latitude, &longitude);
Serial.print("Latitud/Longitud: "); 
Serial.print(latitude,5); 
Serial.print(", "); 
Serial.println(longitude,5);
gps.crack_datetime(&year,&month,&day,&hour,&minute,&second,&hundredths);

//Serial.print(" HORA: "); Serial.println(hour, DEC);
  //Serial.print("HORAAAA: "); Serial.println(hour);
  int hora = hour + timeOffset;
  int dia= day;
  Serial.print("HORA: "); Serial.println(hora);
  if (hora < 0){
    dia= day -1;
    Serial.print("DIA: "); Serial.println(dia); 
  }
    if (hora < 0) {
          hora += 24;
        } else if (hora >= 24) {
          hora -= 24;
        }
Serial.print("Fecha: "); Serial.print(dia); Serial.print("/"); 
Serial.print(month, DEC); Serial.print("/"); Serial.println(year);        
Serial.print("Hora: "); Serial.print(hora); Serial.print(":"); 
Serial.print(minute, DEC); Serial.print(":"); Serial.print(second, DEC); 
Serial.print("."); Serial.println(hundredths, DEC);
//Serial.print("Altitud (metros): "); Serial.println(gps.f_altitude()); 
Serial.print("Rumbo (grados): "); Serial.println(gps.f_course()); 
Serial.print("Velocidad(kmph): "); Serial.println(gps.f_speed_kmph());
Serial.print("Satelites: "); Serial.println(gps.satellites());
Serial.println();
gps.stats(&chars, &sentences, &failed_checksum);
delay(5000);
    }
  }
}
