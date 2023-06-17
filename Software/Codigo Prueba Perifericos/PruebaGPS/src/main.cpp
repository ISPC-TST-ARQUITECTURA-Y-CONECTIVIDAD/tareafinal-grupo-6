#include <Arduino.h>
#include <SoftwareSerial.h>//incluimos libreria SoftwareSerial
#include <TinyGPS.h>//incluimos libreria TinyGPS
int timeOffset = -3;  // Ajuste horario en horas (ejemplo: -3 para GMT-3 Arg)
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
delay(5000);
    }
  }
}
