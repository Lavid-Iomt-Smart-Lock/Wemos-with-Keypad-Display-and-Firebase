/*
 * COISAS REFERENTES AO FIREBASE
 */
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

// Configure with your credentials
// Credencial está em configuração do projeto, contas e serviços.
#define FIREBASE_HOST "ledd-f30cc.firebaseio.com"
#define FIREBASE_AUTH "7XsYCYGOp7vawpwb5iqy4XOymxQUH5jcNjJIYOHV"
#define WIFI_SSID "Wil_Wifi"
//#define WIFI_SSID "Claudio_Wifi"
#define WIFI_PASSWORD "PlsChangeMe@senhas0"

int countPass = 0;
bool access = false, state;

String path = "/";
FirebaseObject object = Firebase.get(path);
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
 * COISAS REFERENTES AO DISPLAY
 */
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
 
Adafruit_SSD1306 display(-1);//cria o objeto do display para i2c 

void introMessage(){
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("");
  display.println("");
  display.println("SMART");
  display.println("LOCK");
  display.display();
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin (9600);

  //DISPLAY
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);//Begin communication with the display in the right address
  delay(100);
  introMessage();

  //WIFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); //Connectos to home wifi using wifi name and password
  Serial.print("conectando");
  while (WiFi.status() != WL_CONNECTED) {//Wait until WIFI is connected
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("conectado: ");
  Serial.println(WiFi.localIP());
  Serial.println("IT IS CONNECTED MOTHERFUCKER");
 
  //FIREBASE
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();
  int count = 1;
  if (key != NO_KEY){
    Serial.println(key);
    password += key;
    countPass++;
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Password:");
    if (countPass == 1){
      display.setTextSize(2);
      display.print("*");  
    }
    else if(countPass == 2){
      display.setTextSize(2);
      display.print("* *");
    }
    else if(countPass == 3){
      display.setTextSize(2);
      display.print("* * *");
    }
    else if(countPass == 4){
      display.setTextSize(2);
      display.print("* * * *");
    }
    //display.setTextSize(2);
    //display.print(key);
    display.display();

    if (!strcmp(password.c_str(), "1234") && countPass == 4){
      Serial.println ("Access released!");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.print("You may come in");
      display.display();
      countPass = 0; //Zera contador de digitos pressionados
      password = ""; //Reseta senha
      access = true; //Libera acesso a porta
      Firebase.setBool("access", access); //Envia para o firebase a informação de que o acesso foi liberado
      
      //state = Firebase.getBool("access");
      Serial.print("Current port state: ");
      Serial.println(Firebase.getBool("access"));
      delay (1000);
      introMessage();
    }
    else if (countPass == 4){
      Serial.println("Try 4 digits again!");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.setTextSize(1);
      display.println("Wrong Password!");
      display.setTextSize(1);
      display.println("Try again!");
      display.display();
      countPass = 0;
      password = "";
      access = false;
      Firebase.setBool("access", access);

      //state = Firebase.getBool("access");
      Serial.print("Current port state: ");
      Serial.println(Firebase.getBool("access"));
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
