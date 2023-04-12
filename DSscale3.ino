//BIBLIOTECAs
#include <HX711.h>
#include <PushButton.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>

//PINOS
#define pinB1DT 53
#define pinB1SCK 52
#define pinB2DT 51
#define pinB2SCK 50
#define pinBotao1 49
#define pinBotao2 48
#define pinB1low 30
#define pinB1high 31
#define pinB2low 32
#define pinB2high 33


#define monitor false

//INSTANCIAR OBJETOS
HX711 balanca1;
HX711 balanca2;
PushButton botao1(pinBotao1);
PushButton botao2(pinBotao2);
LiquidCrystal lcd(24,25,26,27,28,29);

byte keyRow[4]={2,3,4,5};
byte keyCol[4]={6,7,8,9};
const char keys[4][4]={{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
Keypad keypad=Keypad(makeKeymap(keys),keyRow,keyCol,4,4);

//DECLARAR VARIAVEIS
float pesagemB1=0;
float pesagemB2=0;
float pesoB1low=100;
float pesoB1high=200;
float pesoB2low=100;
float pesoB2high=200;
float escalaB1=-221;
float escalaB2=-221;

//SETUP
void setup() {

  if(monitor){Serial.begin(57600);}

  lcd.begin(16,2);

  EEPROM.get(0, escalaB1);
  //EEPROM.get(4, escalaB2);

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
      lcd.setCursor(0, 0);
      lcd.print("B1_L:");
      pesoB1low=GetNumber();
      break;
    
      case 'B':
      lcd.setCursor(0, 0);
      lcd.print("B1_H:");
      pesoB1high=GetNumber();
      break;

      case 'C':
      lcd.setCursor(0, 0);
      lcd.print("B2_L:");
      pesoB2low=GetNumber();
      break;

      case 'D':
      lcd.setCursor(0, 0);
      lcd.print("B2_H:");
      pesoB2high=GetNumber();
      break;

      case '*':
      lcd.clear();
      lcd.setCursor(0, 0);

      lcd.print(escalaB1);
      lcd.setCursor(0, 1);
      lcd.print(" g B1?");


      int calibrar=GetNumber();
      balanca1.power_down();
      balanca1.power_up();        
      escalaB1= escalaB1 + balanca1.get_units(5)/calibrar;
      lcd.print(escalaB1);
      balanca1.set_scale(escalaB1);
      balanca1.tare();
      EEPROM.put(0,escalaB1);
      delay(2000);
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
