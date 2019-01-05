/*
 * COISAS REFERENTES AO FIREBASE
 */
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Configure with your credentials
// Credencial está em configuração do projeto, contas e serviços.
#define FIREBASE_HOST "ledd-f30cc.firebaseio.com"
#define FIREBASE_AUTH "7XsYCYGOp7vawpwb5iqy4XOymxQUH5jcNjJIYOHV"
//#define WIFI_SSID "Wil_Wifi"
//#define WIFI_SSID "Claudio_Wifi"
//#define WIFI_PASSWORD "PlsChangeMe@senhas0"
#define WIFI_SSID "SudoApt-Get102"
#define WIFI_PASSWORD "J606165249"

int countPass = 0;
bool access = false, state;

//String path = "/access";
//FirebaseObject object = Firebase.get(path);
String password = "";

/*
 * COISAS REFERENTES AO KEYPAD
 */
#include <Keypad.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte rowPins[ROWS] = {D0, D3, D7, D1}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {D4, D5, D6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

/*
 * COISAS REFERENTES AO QR CODE E DISPLAY
 */

#include <qrcode.h>
#include <SSD1306.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "images.h"

SSD1306Wire  display(0x3c, D2, D1);
QRcode qrcode (&display);

void introMessage(){
  display.clear();
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 22, "SMART");
  display.drawString(64, 44, "LOCK");
  display.display();
}

void drawImageDemo() { //From the library examples
    // see http://blog.squix.org/2015/05/esp8266-nodemcu-how-to-create-xbm.html
    // on how to create xbm files
    display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);

  //DISPLAY
  display.init();//Begin communication with the display in the right address
  display.flipScreenVertically();
  qrcode.init();
  //qrcode.create("Teste");

  //WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //Connectos to home wifi using wifi name and password
  Serial.print("conectando");
  while (WiFi.status() != WL_CONNECTED) {//Wait until WIFI is connected
    display.clear();
    drawImageDemo();
    display.setTextAlignment(TEXT_ALIGN_RIGHT);
    display.drawString(10, 128, String(millis()));
    // write the buffer to the display
    display.display();
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("conectado: ");
  Serial.println(WiFi.localIP());
  Serial.println("IT IS CONNECTED MOTHERFUCKER");
 
  //FIREBASE
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  introMessage();
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();
  int count = 1;
  if (key != NO_KEY){
    Serial.println(key);
    password += key;
    countPass++;
    display.clear();
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 10, "Password");
    if (countPass == 1){
      display.setFont(ArialMT_Plain_16);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 40, "*");  
    }
    else if(countPass == 2){
      display.setFont(ArialMT_Plain_16);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 40, "*  *");
    }
    else if(countPass == 3){
      display.setFont(ArialMT_Plain_16);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 40, "*  *  *");
    }
    else if(countPass == 4){
      display.setFont(ArialMT_Plain_16);
      display.setTextAlignment(TEXT_ALIGN_CENTER);
      display.drawString(64, 40, "*  *  *  *");
    }

    display.display();

    if (!strcmp(password.c_str(), "1234") && countPass == 4){
      Serial.println ("Access released!");
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_16);
      display.drawString(0, 0, "You may come in!");
      display.display();
      countPass = 0; //Zera contador de digitos pressionados
      password = ""; //Reseta senha
      access = true; //Libera acesso a porta
      Firebase.set("/access/status", "true"); //Envia para o firebase a informação de que o acesso foi liberado
      
      //state = Firebase.getBool("access");
      Serial.print("Current port state: ");
      Serial.println(Firebase.getString("/access/status/"));
      delay (1000);
      introMessage();
    }
    else if (countPass == 4){
      Serial.println("Try 4 digits again!");
      display.clear();
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(ArialMT_Plain_16);
      display.drawString(0, 0, "Wrong password...");
      display.drawString(0, 20, "Try Again!");
      display.display();
      countPass = 0;
      password = "";
      access = false;
      Firebase.set("/access/status", "false");

      //state = Firebase.getBool("access");
      Serial.print("Current port state: ");
      Serial.println(Firebase.getString("/access/status/"));
      delay(1000);
      introMessage();
    }
  }

  /*
   * //Receber a variavel led do firebase
  led = Firebase.getBool("led");
  Serial.print("led: ");
  Serial.println(led);
  if(led == 1){
    digitalWrite(LED_BUILTIN ,HIGH);
  }else if(led == 0){
    digitalWrite(LED_BUILTIN ,LOW);
  }

  //sensor_random = sensor_random + 1.3;
  //Firebase.pushFloat("LDR", sensor_random);
  //delay (3000);
   */
}
