//Last Edit: 14/07/2021

#include <IRremote.h>


#define RECV_PIN 6          
IRrecv irrecv(RECV_PIN);
decode_results results;

#define CODICE_ACCENSIONE 281559486         //10C8FF00
#define CODICE_SPEGNIMENTO 281607936        //10C841BE
#define CODICE_ONE_CUP 281600286            //10C8E11E

#define ON_OFF_BUTTON 2
#define ENERGY_RELE 4
#define LED_STRIP 11

bool stato_caffettiera = 0;
unsigned long timer_rele_switch = 0;
#define DELAY_ACCENSIONE_CAFFETTIERA 500 //ms
#define DELAY_PRESSIONE_TASTO 200 //ms

bool accendi_caffettiera = 0;
bool spegni_caffettiera = 0;
int tazze_da_versare = 0;

bool conta_tempo = 0;

unsigned long timer_caffettiera = 0;

//DELAY:
#define DELAY_ONE_CUP 10500 //ms



void setup() {
  Serial.begin(115200);
  
  Serial.println();
  Serial.println("Caffettiera Domotica by xTor3 - Software Version 2.0");
  
  irrecv.enableIRIn(); 
  
  pinMode(LED_BUILTIN ,OUTPUT);

  pinMode(ENERGY_RELE, OUTPUT);
  pinMode(ON_OFF_BUTTON, OUTPUT);
  pinMode(LED_STRIP, OUTPUT);
  
  digitalWrite(ENERGY_RELE, HIGH);
  digitalWrite(ON_OFF_BUTTON, HIGH);
  digitalWrite(LED_STRIP, LOW);

  Serial.println("Setup Complete");
  Serial.println();
}

void loop() {
  if(irrecv.decode(&results)){
    Serial.println();
    Serial.print("E' stato ricevuto: ");
    Serial.println(results.value, HEX);


    //On Command:
    if(results.value == CODICE_ACCENSIONE){
      Serial.println("Accensione Caffettiera");
      accendi_caffettiera = 1;
      timer_rele_switch = millis();
    }

    //Off Command:
    else if(results.value == CODICE_SPEGNIMENTO){
      Serial.println("Spegnimento Caffettiera");
      spegni_caffettiera = 1;
    }

    //1Cup Command:
    else if(results.value == CODICE_ONE_CUP){
      Serial.println("Preparo una Tazza di Caffé");
      accendi_caffettiera = 1;
      tazze_da_versare = 1;
    }
    
    irrecv.resume(); // Receive the next value
  }



  if(accendi_caffettiera && !stato_caffettiera){
    if(switch_coffe_maker_state()){
      Serial.println("Caffettiera Accesa");
      accendi_caffettiera = 0;
    } 
  }

  else if(spegni_caffettiera && stato_caffettiera){
    if(switch_coffe_maker_state()){
      Serial.println("Caffettiera Spenta");
      spegni_caffettiera = 0;
    } 
  }

  else if(stato_caffettiera && tazze_da_versare > 0){
    if(!conta_tempo){
      timer_caffettiera = millis();
      conta_tempo = 1;
    }
    switch(tazze_da_versare){
      case 1:
        if(versa_caffe(DELAY_ONE_CUP)){
          Serial.print("Ho finito di versare ");
          Serial.print(tazze_da_versare);
          if(tazze_da_versare == 1) Serial.println("Tazza");
          else Serial.println("Tazze");
          spegni_caffettiera = 1;
          tazze_da_versare = 0;
          conta_tempo = 0;
        }
        break;
    }
  }
  else if(!stato_caffettiera){
    tazze_da_versare = 0;
  }
  
}



bool versa_caffe(unsigned long timer_var){
  if((millis() - timer_caffettiera) >= timer_var){
    return 1;
  }
  else{
    return 0;
  }
}



bool switch_coffe_maker_state(void){
  if(!stato_caffettiera){
    digitalWrite(ENERGY_RELE, LOW);
    if((millis() - timer_rele_switch) >= DELAY_ACCENSIONE_CAFFETTIERA){
      digitalWrite(ON_OFF_BUTTON, LOW);
      if((millis() - timer_rele_switch) >= DELAY_ACCENSIONE_CAFFETTIERA + DELAY_PRESSIONE_TASTO){
        digitalWrite(ON_OFF_BUTTON, HIGH);
        timer_rele_switch = millis();
        stato_caffettiera = 1;
        return 1;     
      }
    }
    else return 0;
  }
  else{
    digitalWrite(ENERGY_RELE, HIGH);
    stato_caffettiera = 0;
    return 1;
  }
}
