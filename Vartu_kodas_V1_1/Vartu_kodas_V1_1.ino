#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();

// Versija 1.1 Varikliu valdymas su draivais.

/////////// ISEJIMAI/IEJIMAI ///////////////


int Vartu_mygtuko_pin_A = A3;
int Vartu_mygtuko_pin_B = A4;

int dir1 = 7;
int dir2 = 5;
int pwm_pin = 9;


int lempa = 3;
int imtuvo_maitinimas = A1;
int garazas  = 8;
int garazo_sensoriaus_pin = A7;

//int Varteliu_variklio_pin_C = 5;   //5
//int Varteliu_variklio_pin_D = 4;   //4


int kodas = 0;
int busena = 0;
int auto_uzdarymas=0;

//int varteliu_kodas = 8932;
int Vartu_kodas = 8929;


int ledState = LOW;             
long previousMillis = 0;       
 

long off_time = 500;
int on_time = 1000;
int off_time1=off_time;

long Vartu_atidarymo_laikas = 34000;
long Vartu_uzdarymo_laikas = 34000;

long Vartu_atidarymo_greitejimo_laikas = 3;
long Vartu_atidarymo_letejimo_laikas = 50;

long Vartu_uzdarymo_greitejimo_laikas = 3;
long Vartu_uzdarymo_letejimo_laikas = 50;

long Varteliu_atidarymo_laikas = 30000;
long Varteliu_uzdarymo_laikas = 27000;

long Lempos_veikimo_laikas = 3000;
long laukimo_laikas = 15000;

long papildomas_laikas = 15000;

int letejimas = 1;
int nepilnas=0;
long likes_laikas=0;
int papildomas=1;
int suletejimo_V = 180;
int startavimo_greitis = 50;

int atidaryti = 0;

unsigned long laikas = 0;
long praejes = 0;
long reseto_intervalas = 60; //minutes
void(* resetFunc) (void) = 0;//declare reset function at address 0 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySwitch.enableReceive(0);  // Receiver on interrupt 0 => that is pin #2

  pinMode(Vartu_mygtuko_pin_A, INPUT);
  pinMode(Vartu_mygtuko_pin_B, INPUT);
  pinMode(garazo_sensoriaus_pin, INPUT);    
  pinMode(dir1,OUTPUT);
  pinMode(dir2,OUTPUT);
  pinMode(pwm_pin,OUTPUT);
  pinMode(lempa, OUTPUT);
  pinMode(garazas, OUTPUT);
  pinMode(imtuvo_maitinimas, OUTPUT);
//  pinMode(Varteliu_variklio_pin_C, OUTPUT);
//  pinMode(Varteliu_variklio_pin_D, OUTPUT);
   motor_stop();
   delay(1000);
  digitalWrite(imtuvo_maitinimas,LOW);
  digitalWrite(garazas,LOW);
 for(int i=1; i<=2; i++){
   digitalWrite(lempa,HIGH);
   delay(500);
   digitalWrite(lempa,LOW);
   delay(500);
 }
 digitalWrite(imtuvo_maitinimas,HIGH);
  
  Serial.println("Vartu kodas 1.1");
  Serial.println("Varikliu valdymas su draivais(Nauji dravai BTS7960). Atnaujinta 2022-11-19");

 // TCCR1A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20);
 // TCCR1B = TCCR1B & 0b11111000 | 0x02;
    
    
  int mygtukasA = digitalRead(Vartu_mygtuko_pin_A);
   int mygtukasB = digitalRead(Vartu_mygtuko_pin_B);

   if(mygtukasB == HIGH){
    Serial.println("Vartai atidaryti");
    atidaryti = 1;
   }
   else if(mygtukasA == HIGH){
    Serial.println("Vartai uzdaryti");
    atidaryti = 0;
   // busena = 2;
   }
   else if(mygtukasA == LOW && mygtukasB == LOW){
    Serial.println("Vartai be pozicijos");
    atidaryti = 1;
   }
  // busena = 2;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (mySwitch.available()) {
    kodas =  mySwitch.getReceivedValue();
    
    Serial.print("Received ");
    Serial.print(kodas);
    Serial.print(" / ");
    Serial.print( mySwitch.getReceivedBitlength() );
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println( mySwitch.getReceivedProtocol() );

    mySwitch.resetAvailable();
   int mygtukasA = digitalRead(Vartu_mygtuko_pin_A);
   int mygtukasB = digitalRead(Vartu_mygtuko_pin_B);

  if(mygtukasB == HIGH){ atidaryti = 1;}
  else if(mygtukasA == HIGH){atidaryti = 0;}
  else if(mygtukasA == LOW && mygtukasB == LOW){atidaryti = 1;}
   
   if(kodas == Vartu_kodas){
      if(atidaryti == 0){
        int garazo_sensorius = analogRead(garazo_sensoriaus_pin);
        if(garazo_sensorius>=1010){
          pinMode(2,OUTPUT);
          digitalWrite(garazas,HIGH);
          delay(1000);
          digitalWrite(garazas,LOW);
          delay(100);
        }
        busena = 2;
      }else{
        busena = 3;
      }
       Serial.println("A mygtukas");
         pinMode(2,OUTPUT);
      // digitalWrite(2,HIGH);
     }
     
    kodas = 0;
  }

//--------------RAMYBES BUSENA--------------------------------------------------------
  if(busena == 0){
      Vartu_variklis(0);
       digitalWrite(lempa,LOW);
       ledState = LOW;
     //Serial.println("Ramybes busena");
     laikas = millis();
      if(laikas - praejes >= reseto_intervalas*60000){
        praejes = laikas;
       resetFunc(); 
      }
   }
//----------------VARTU ATIDARYMAS----------------------------------------------------
  else if(busena == 2){
    Serial.print("atidarom");
     int mygtukasB = digitalRead(Vartu_mygtuko_pin_B);
       if(mygtukasB == HIGH){
           busena=0;
           ledState = LOW;
          //break;
          atidaryti = 1;
           pinMode(2,INPUT);
        }
        else{
          
          for(int g = startavimo_greitis; g<=255; g++){
              int mygtukasB = digitalRead(Vartu_mygtuko_pin_B);
            if(mygtukasB==HIGH){
                     Vartu_variklis(0);
                      letejimas=0;
                      busena=0;
                      papildomas=0;
                      nepilnas=1;
                      likes_laikas=20000;
                      atidaryti =1;
                       pinMode(2,INPUT);
                     break;
              }
             else{
             Vartu_variklis(-g );
             letejimas=1;
              Lempa();
             delay(Vartu_atidarymo_greitejimo_laikas);
             }
           }
          
           for(int i = 0; i<=Vartu_atidarymo_laikas/2; i++){
            int mygtukasB = digitalRead(Vartu_mygtuko_pin_B);
                if(mygtukasB == HIGH){
                      i=Vartu_atidarymo_laikas;
                      
                     for(int g=255; g>=0; g--){
                      Vartu_variklis(-g);
                      delay(3);
                     }
           
                      Vartu_variklis(0);
                      letejimas=0;
                      
                     // nepilnas=1;
                    //  likes_laikas = Vartu_atidarymo_laikas - i +10000;
                      busena=0;
                      papildomas=0;
                      atidaryti =1;
                       pinMode(2,INPUT);
                     break;
                 }
                 else{
                   Vartu_variklis(-255);
                   papildomas=1;
                   letejimas=1;
                   Lempa();
                  delay(2);
                 }
           }
           if (letejimas == 1 ){
             for(int g=255; g>=suletejimo_V; g--){
               int mygtukasB = digitalRead(Vartu_mygtuko_pin_B);
                 if(mygtukasB == HIGH){
                     g=0;
                     Vartu_variklis( 0 );
                     papildomas=0;
                     atidaryti = 1;
                      pinMode(2,INPUT);
                    break;
                  }
                 else{
                   Vartu_variklis(-g);
                   papildomas=1;
                    Lempa();
                 }
                 delay(Vartu_atidarymo_letejimo_laikas); 
            }
         }
         if(papildomas ==1){
             for(int p = 0; p <= papildomas_laikas; p++){
                    int mygtukasB = digitalRead(Vartu_mygtuko_pin_B);
                   if(mygtukasB == LOW){
                        Vartu_variklis( -suletejimo_V );
                         Lempa();
                     }
                   else{
                        Vartu_variklis( 0);
                        busena=0;
                        p = papildomas_laikas;
                        nepilnas=0;
                        likes_laikas=0;
                        papildomas=0;
                        atidaryti = 1;
                         pinMode(2,INPUT);
                        break;
                  
                } 
                delay(1);
           }
         }
     
          for(int i=0;i<=Lempos_veikimo_laikas;i++){
            Lempa();
            delay(1);
          }
           busena=0;
          ledState = LOW;
          
          papildomas=1;
          atidaryti = 1;
           pinMode(2,INPUT);
   }
 }
//----------------VARTU UZDARYMAS-----------------------------------------------------
  else if(busena == 3){
        
        int mygtukasA = digitalRead(Vartu_mygtuko_pin_A);
       if(mygtukasA == HIGH){
           busena=0;
           ledState = LOW;
          
           atidaryti = 0;
            pinMode(2,INPUT);
           // break;
        }
        else{
          
           for(int g =startavimo_greitis; g<=255; g++){
            int mygtukasA = digitalRead(Vartu_mygtuko_pin_A);
            if(mygtukasA==HIGH){
                     Vartu_variklis(0);
                      letejimas=0;
                      busena=0;
                      papildomas=0;
                      nepilnas=1;
                      likes_laikas=20000;
                      atidaryti =0;
                      pinMode(2,INPUT);
                     break;
              }
             else{
             Vartu_variklis(g );
             letejimas=1;
              Lempa();
             delay(Vartu_uzdarymo_greitejimo_laikas);
             }
            
            
           }
           
           
           for(int i = 0; i<=Vartu_uzdarymo_laikas/2; i++){
            int mygtukasA = digitalRead(Vartu_mygtuko_pin_A);
                if(mygtukasA == HIGH){
                   i=Vartu_uzdarymo_laikas;
                   
                     for(int g=255; g>=0; g--){
                      Vartu_variklis(g);
                       delay(3);
                     }
           
                      Vartu_variklis(0);
                      letejimas=0;
                      //nepilnas=1;
                      //likes_laikas = Vartu_uzdarymo_laikas - i+10000;
                      busena=0;
                      papildomas=0;
                      atidaryti = 0;
                       pinMode(2,INPUT);
                     break;
                     
                 }
                 else{
                   Vartu_variklis(255);
                   papildomas=1;
                   letejimas=1;
                  Lempa();
                  delay(2);
                 }
           }
           if (letejimas == 1 ){
             for(int g=255; g>=250; g--){
               int mygtukasA = digitalRead(Vartu_mygtuko_pin_A);
                 if(mygtukasA == HIGH){
                     g=0;
                     Vartu_variklis( 0 );
                     papildomas=0;
                     atidaryti = 0;
                     pinMode(2,INPUT);
                    break;
                  }
                 else{
                   Vartu_variklis( g );
                   papildomas=1;
                    Lempa();
                 }
               delay(Vartu_uzdarymo_letejimo_laikas);  
            }
         }
         if(papildomas == 1){
             for(int p = 0; p <= papildomas_laikas; p++){
                    int mygtukasA = digitalRead(Vartu_mygtuko_pin_A);
                   if(mygtukasA == LOW){
                        Vartu_variklis( suletejimo_V);
                         Lempa();
                     }
                   else{
                        Vartu_variklis( 0);
                        busena=0;
                        p = papildomas_laikas;
                        nepilnas=0;
                        likes_laikas=0;
                        atidaryti=0;
                        pinMode(2,INPUT);
                        break;
                     }
                 delay(1);
               }
         }      

         for(int i=0;i<=Lempos_veikimo_laikas;i++){
            Lempa();
            delay(1);
          }
         
        busena=0;
        ledState = LOW;
        papildomas=1;
        pinMode(2,INPUT);
       }
       
  }


}
void Lempa(){
  
 unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis > off_time) {
    // save the last time you blinked the LED 
    previousMillis = currentMillis;
    
    if(ledState==LOW){
    ledState = HIGH;
    off_time=on_time; 
    }
    else{
     ledState = LOW;
     off_time=off_time1;
    }
   
  }
   digitalWrite(lempa, ledState); 

}
void motor_stop(){
  digitalWrite(dir1,LOW);
  digitalWrite(dir2,LOW);
  analogWrite(pwm_pin, 0);
}

void Vartu_variklis(int pwm){
  
  pwm = constrain(pwm,-255, 255);
      if(pwm>0){ 
        digitalWrite(dir1,HIGH);
        digitalWrite(dir2,LOW);
        analogWrite(pwm_pin,pwm);
      }
      else if(pwm==0){
        motor_stop();
      }
      
      else if(pwm<0){
        digitalWrite(dir1,LOW);
        digitalWrite(dir2,HIGH);
        analogWrite(pwm_pin,pwm*(-1));
      }     
  
}

