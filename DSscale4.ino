//BIBLIOTECAs
#include <HX711.h>
#include <PushButton.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>

//PINOS
#define pinB1DT 2
#define pinB1SCK 3
#define pinB2DT 10
#define pinB2SCK 11
#define pinBotao1 30
#define pinBotao2 32
#define pinB1low 34
#define pinB1high 36
#define pinB2low 35
#define pinB2high 37


#define monitor false

//INSTANCIAR OBJETOS
HX711 balanca1;
HX711 balanca2;
PushButton botao1(pinBotao1);
PushButton botao2(pinBotao2);
LiquidCrystal_I2C lcd(0x27,16,2);

byte keyRow[4]={53,51,49,47};
byte keyCol[4]={45,43,41,39};
const char keys[4][4]={{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
Keypad keypad=Keypad(makeKeymap(keys),keyRow,keyCol,4,4);

//DECLARAR VARIAVEIS
float pesagemB1=0;
float pesagemB2=0;
int pesoB1low=100;
int pesoB1high=200;
int pesoB2low=100;
int pesoB2high=200;
float escalaB1=-221;
float escalaB2=-221;

//SETUP
void setup() {

  if(monitor){Serial.begin(57600);}

  lcd.init();
  lcd.backlight();

  EEPROM.get(0, escalaB1);
  EEPROM.get(4, escalaB2);
  EEPROM.get(8, pesoB1low);
  EEPROM.get(12, pesoB1high);
  EEPROM.get(16, pesoB2low);
  EEPROM.get(20, pesoB2high);

  balanca1.begin(pinB1DT, pinB1SCK);
  balanca1.set_scale(escalaB1);
  balanca1.tare();
  if(monitor){Serial.println("Setup balanca1 finalizado");}
  //lcd.print("Setup balanca1 finalizado");
  
  balanca2.begin(pinB2DT, pinB2SCK);
  balanca2.set_scale(escalaB2);
  balanca2.tare();
  if(monitor){Serial.println("Setup balanca2 finalizado");}
  //lcd.print("Setup balanca2 finalizado");
  
  pinMode(pinB1low, OUTPUT);
  pinMode(pinB1high, OUTPUT);
  pinMode(pinB2low, OUTPUT);
  pinMode(pinB2high, OUTPUT);
}

//LOOP
void loop() {
  
  //TECLADO
  botao1.button_loop();
  botao2.button_loop();
  char key=keypad.getKey();

  if(key!=NO_KEY){
    lcd.setCursor(10, 0);
    lcd.print(key);  
  }



  //DEFINIR VALORES
  if(key!=NO_KEY){
    switch (key)
    {
      case 'A':
      lcd.clear();
      lcd.print("B1_L ");
      lcd.print(pesoB1low);
      lcd.print(":");
      pesoB1low=GetNumber();
      EEPROM.put(8, pesoB1low);
      break;
    
      case 'B':
      lcd.clear();
      lcd.print("B1_H ");
      lcd.print(pesoB1high);
      lcd.print(":");
      pesoB1high=GetNumber();
      EEPROM.put(12, pesoB1high);
      break;

      case 'C':
      lcd.clear();
      lcd.print("B2_L ");
      lcd.print(pesoB2low);
      lcd.print(":");
      pesoB2low=GetNumber();
      EEPROM.put(16, pesoB2low);
      break;

      case 'D':
      lcd.clear();
      lcd.print("B2_H ");
      lcd.print(pesoB2high);
      lcd.print(":");
      pesoB2high=GetNumber();
      EEPROM.put(20, pesoB2high);
      break;

      case '*':
        lcd.clear();
        lcd.print("1: CalibraB1");
        lcd.setCursor(0, 1);
        lcd.print("2: CalibraB2");
        while(key != '#') 
        {
          if (key == '1')
          {
              lcd.clear();
              balanca1.power_down();
              delay(1000);
              balanca1.power_up();
              delay(1000);
              balanca1.set_scale();
              delay(1000);
              balanca1.tare(); 
              lcd.print(escalaB1);
              lcd.setCursor(0, 1);
              delay(1000);
              lcd.print("Calibre g:");
              int calibrar1=GetNumber();
              escalaB1= balanca1.get_units(5)/calibrar1;
              lcd.setCursor(8, 0);
              lcd.print(escalaB1);
              delay(1000);
              balanca1.set_scale(escalaB1);
              EEPROM.put(0,escalaB1);
              delay(2000);
              lcd.clear();
              lcd.print("Retire o peso");
              lcd.setCursor(0, 1);
              lcd.print("# para sair...");
          } else if (key == '2')
          {
              lcd.clear();
              balanca2.power_down();
              delay(1000);
              balanca2.power_up();
              delay(1000);
              balanca2.set_scale();
              delay(1000);
              balanca2.tare(); 
              lcd.print(escalaB2);
              lcd.setCursor(0, 1);
              delay(1000);
              lcd.print("Calibre g:");
              int calibrar2=GetNumber(); 
              escalaB2= balanca2.get_units(5)/calibrar2;
              lcd.setCursor(8, 0);
              lcd.print(escalaB2);
              delay(1000);
              balanca2.set_scale(escalaB2);
              EEPROM.put(4,escalaB2);
              delay(2000);
              lcd.clear();
              lcd.print("Retire o peso");
              lcd.setCursor(0, 1);
              lcd.print("# para sair...");
          }

          key = keypad.getKey();
        }

      break;
        
    }

  }

  
  //TARA
  if(botao1.pressed()) {
    balanca1.power_up();  
    balanca1.tare();
    if(monitor){Serial.println("Tara balança 1");}
    lcd.setCursor(10, 0);
    lcd.print("Tara");
    delay(100);
  }
  
if(botao2.pressed()) {
    balanca2.power_up();  
    balanca2.tare();
    if(monitor){Serial.println("Tara balança 2");}
    lcd.setCursor(10, 1);
    lcd.print("Tara");
    delay(100);   
  }
  

  delay(100);
  pesagemB1 = balanca1.get_units(5);
  pesagemB2 = balanca2.get_units(5);

  if(monitor){
    Serial.print("Balança 1: ");
    Serial.println(pesagemB1, 3);

    Serial.print("Balança 2: ");
    Serial.println(pesagemB2, 3);
  }

  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("B1:");
  lcd.print(pesagemB1, 0);

  lcd.setCursor(0,1);
  lcd.print("B2:");
  lcd.print(pesagemB2, 0);

if(pesagemB1>pesoB1low){digitalWrite(pinB1low, LOW); lcd.setCursor(14, 0); lcd.print("L"); } else {digitalWrite(pinB1low, HIGH);}
if(pesagemB1>=pesoB1high){digitalWrite(pinB1high, LOW);lcd.setCursor(15, 0); lcd.print("H");} else {digitalWrite(pinB1high, HIGH);}

if(pesagemB2>pesoB2low){digitalWrite(pinB2low, LOW); lcd.setCursor(14, 1); lcd.print("L"); } else {digitalWrite(pinB2low, HIGH);}
if(pesagemB2>=pesoB2high){digitalWrite(pinB2high, LOW);lcd.setCursor(15, 1); lcd.print("H");} else {digitalWrite(pinB2high, HIGH);}


}

//FUNCOES

//TECLADO GetNumber()
 int GetNumber()
{
   int num = 0;
   char key = keypad.getKey();
   while(key != '#')
   {
      switch (key)
      {
         case NO_KEY:
            break;

         case '0': case '1': case '2': case '3': case '4':
         case '5': case '6': case '7': case '8': case '9':
            lcd.print(key);
            num = num * 10 + (key - '0');
            break;

         case '*':
            num = 0;
            lcd.clear();
            break;
      }

      key = keypad.getKey();
   }

   return num;
} 
