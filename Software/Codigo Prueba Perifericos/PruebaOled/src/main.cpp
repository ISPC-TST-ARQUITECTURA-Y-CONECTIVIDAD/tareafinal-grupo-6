#include <Arduino.h>
#include <Adafruit_SSD1306.h>//libreria necesaria para dsiplay
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64);//Especifico display

void setup() {
display.begin(SSD1306_SWITCHCAPVCC, 0x3c);//Inicio display con el puerto I2C por defecto
display.clearDisplay();//Borro display
display.setTextColor(WHITE, BLACK);//Seteo display
}

void loop() {
display.setCursor(0, 0); // Seteo cursor
display.println("ESP32_Iniciado");//inprimo texto
display.display();//Envio los datos
delay(1000);

display.setCursor(0, 10);
display.println("ESTO ES UNA PRUEBA");
display.display();
delay(1000);

display.setCursor(0, 20);
display.println("0123456789");
display.display();
delay(1000);

display.setCursor(0, 30);
display.println("!#$%&/()=/*-+");
display.display();
delay(1000);

display.setCursor(0, 40);
display.println("LINEA 5");
display.display();
delay(1000);

display.setCursor(0, 50);
display.println("linea 6");
display.display();
delay(5000);
display.clearDisplay();//Borro todo el texto
display.display();
delay(1000);
}
