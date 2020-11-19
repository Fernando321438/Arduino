#include <LiquidCrystal.h>
const int rs = 12, en = 13, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Piedini PWM 2560 2,3,4,5,6,7,8,9,10,11,12,13,44,45,46

//--------------------------------------------------------------------------------------------------------
//------- Classe per Variabili Ore-Minuti ----------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

//----------------- Dichiarazione
class OreMin{
    private:
    public:
        OreMin(int ora,int minuti);
      OreMin();
        int ora;
        int minuti;
      int seriale;
        int SerialeMinuti();
};

//----------------- Costruttore con parametri
OreMin::OreMin(int parOra,int parMinuti){
    if (parOra<0)  parOra=0;
    if (parOra>23) parOra=23;
    if (parMinuti<0) parMinuti=0;
    if (parMinuti>59) parMinuti=59;
    ora=parOra;
    minuti=parMinuti;
    seriale=ora*60+minuti;
}
//----------------- Costruttore senza parametri
OreMin::OreMin(){
    ora=0;
    minuti=0;
    seriale=0;
};

//----------------- Calcolo Seriale minuti giornalieri
int OreMin::SerialeMinuti(){
    int x= ora*60+minuti;
    seriale=x;
    return x;
}



//------------------ Dichiarazione Orari e Segnali Digitali con PWM
//--- Alba
OreMin AlbaFase1(6,0);
OreMin AlbaFase2(7,30);
OreMin AlbaFase3(9, 0);
#define pinAlba 3            //Mega 2, Uno 3   

//--- Giorno
OreMin GiornoFase1 ( 7,30);
OreMin GiornoFase2 ( 9,00);
OreMin GiornoFase3 (17, 0);
OreMin GiornoFase4 (18,30);
#define pinGiorno 3

//--- Tramonto
OreMin TramontoFase1 (17, 0);
OreMin TramontoFase2 (18,30);
OreMin TramontoFase3 (20, 0);
#define pinTramonto 4

//--- Notte
OreMin NotteFase1 (18,30);
OreMin NotteFase2 (20, 0);
OreMin NotteFase3 ( 6, 0);
OreMin NotteFase4 ( 7,30);
#define pinNotte 5

//-------------------- Dichiarazione Orari e Segnali Digitali con On_OFF
//--- Relé GiornoA
OreMin ReleGA_ON  ( 8, 0);
OreMin ReleGA_OFF (18, 0);
#define pinReleGA   8      //Mega 14, Uno 8

//--- Relé GiornoB
OreMin ReleGB_ON  ( 8,15);
OreMin ReleGB_OFF (17,30);
#define pinReleGB   15

//--- Relé NotteA
OreMin ReleNA_ON  (19, 0);
OreMin ReleNA_OFF ( 7, 0);
#define pinReleNA   2     // Mega 16, Uno 2

//--- Relé NotteB
OreMin ReleNB_ON  (19, 0);
OreMin ReleNB_OFF ( 6,30);
#define pinReleNB   17

//--- Relé Stelle
OreMin ReleStelle_ON  (18,30);
OreMin ReleStelle_OFF ( 4, 0);
#define pinReleStelle   18

//--- Relé Cometa
OreMin ReleCometa_ON  (21, 0);
OreMin ReleCometa_OFF ( 5, 0);
#define pinReleCometa   19

//--- Fuoco Giorno
OreMin FuocoGiorno_ON  ( 8,00);
OreMin FuocoGiorno_OFF (18,00);
#define pinFuocoGiorno  9         //6 + 7 + 8      //Mega 6, Uno 9    //3-Pin consecutivi PWM

//--- Fuoco Notte
OreMin FuocoNotte_ON  (19, 0);
OreMin FuocoNotte_OFF ( 6,30);
#define pinFuocoNotte   9         //9 + 10 + 11   //3-Pin consecutivi PWM

//------------------------ Dichiarazione Potenziometro Temporizzazione
int valTempoMax=1;
#define pinTempoMax A0       


//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//              VARIABILI
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------

unsigned long millisCiclo=0;
unsigned long millisCicloPrecedente=0;
unsigned long maxMillisCiclo =0;

OreMin OraCorrente ;  



//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//              SETUP
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void setup(){

    Serial.begin(9600);
  
    lcd.begin(16, 2);
    wellcome();
    dispMenu();
  
    
    //------------------------ Gestione Alba
    pinMode (pinAlba, OUTPUT);

    //------------------------ Gestione Giorno PWM
    pinMode (pinGiorno, OUTPUT);    
    
    //------------------------ Gestione Tramonto PWM
    pinMode (pinTramonto, OUTPUT);

    //------------------------ Gestione Notte PWM
    pinMode (pinNotte, OUTPUT);    


    //------------------------ Gestione Relé Giorno A
    pinMode (pinReleGA, OUTPUT);    

    //------------------------ Gestione Relé Giorno B
    pinMode (pinReleGB, OUTPUT);    

    //------------------------ Gestione Relé Notte A
    pinMode (pinReleNA, OUTPUT);    

    //------------------------ Gestione Relé Notte B
    pinMode (pinReleNB, OUTPUT);    

    //------------------------ Gestione Relé Stelle
    pinMode (pinReleStelle, OUTPUT);    

    //------------------------ Gestione Relé Cometa
    pinMode (pinReleStelle, OUTPUT);    

    //------------------------ Fuoco Giorno
    pinMode (pinFuocoGiorno+0, OUTPUT);
    pinMode (pinFuocoGiorno+1, OUTPUT);
    pinMode (pinFuocoGiorno+2, OUTPUT);

    //------------------------ Fuoco Notte
    pinMode (pinFuocoNotte+0, OUTPUT);
    pinMode (pinFuocoNotte+1, OUTPUT);
    pinMode (pinFuocoNotte+2, OUTPUT);

    //------------------------ Temporizzatore Max
    pinMode (pinTempoMax, INPUT);

}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//              LOOP
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
void loop() {
  
    LeggiTempoMax();
    LeggiOraCorrente();
    CalcolaOraCorrente();

    controlloReleGA();
    controlloReleNA();
  
    controlloAlba();
    controlloFuocoGiorno();
    
    dispOraCorrente();
  
    /*
    Serial.print(valTempoMax);
    Serial.print(", ");
    Serial.print(maxMillisCiclo);
    Serial.print(", ");
    Serial.print(millisCiclo);
    Serial.print(" => ");
    Serial.print(OraCorrente.ora);
    Serial.print(":");
    Serial.print(OraCorrente.minuti);    
    Serial.print(" (");
    Serial.print(OraCorrente.seriale);     
    Serial.print(")");
    Serial.println();
    */
    delay(100);     // Da Commentare quando si carica il software sulla scheda Arduino (solo per simulazione)
    
}


//--------------------------------------------------------------------------------------------
void wellcome(){
  lcd.clear();
  lcd.print("  Centralina");
  lcd.setCursor(0,1);
  lcd.print (" Presepio v1.0");
  delay (2000);
}

//--------------------------------------------------------------------------------------------
void dispMenu(){
  lcd.clear();
  lcd.print ("Giornata: -- min");
  lcd.setCursor(0,1);
  lcd.print ("Orario:   --:--  ");
  delay (2000);

}

//--------------------------------------------------------------------------------------------
void dispDurata( int Val){
  lcd.setCursor(10,0);
  if  (Val<10) lcd.print("0");
  lcd.print (Val);
}

//--------------------------------------------------------------------------------------------
void dispOraCorrente(){
    
  lcd.setCursor(10,1);
  if  (OraCorrente.ora<10) lcd.print("0");
  lcd.print (OraCorrente.ora);
  lcd.setCursor(13,1);
  if  (OraCorrente.minuti<10) lcd.print("0");
  lcd.print (OraCorrente.minuti);  
}

//--------------------------------------------------------------------------------------------
void LeggiTempoMax(){
    
  unsigned int x=analogRead(pinTempoMax);
    static int oldMaxMin=-1;
    
    valTempoMax=map(x,0,1000,1,30);
    if (valTempoMax>30) valTempoMax=1;
  
    int maxMin=valTempoMax;
    maxMillisCiclo= valTempoMax * 60.0 * 1000.0;
  
    if (oldMaxMin != maxMin){
      dispDurata (maxMin);
      oldMaxMin=maxMin;
    }
}

//--------------------------------------------------------------------------------------------
void LeggiOraCorrente(){
    
    millisCiclo=millis()-millisCicloPrecedente;
    if (millisCiclo > maxMillisCiclo) {
        millisCiclo -= maxMillisCiclo;
        millisCicloPrecedente=millis();
    }
}

//--------------------------------------------------------------------------------------------
void CalcolaOraCorrente(){

    float minuti= float(millisCiclo)/float(maxMillisCiclo)*(24.0*60.0);
    OraCorrente.ora=minuti / 60;
    OraCorrente.minuti=minuti - OraCorrente.ora*60;
    OraCorrente.seriale= (OraCorrente.ora)*60+OraCorrente.minuti;
}

//--------------------------------------------------------------------------------------------------------
//------- Relé On-OFF ------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
void controlloReleGA(){

    //Led Spento - se fuori dall'intervallo
    if ((OraCorrente.seriale < ReleGA_ON.seriale) ||(OraCorrente.seriale > ReleGA_OFF.seriale)){
        digitalWrite(pinReleGA,LOW);
        return;
    }
  //Led Acceso
    digitalWrite(pinReleGA,HIGH);
}

//--------------------------------------------------------------------------------------------
void controlloReleNA(){

    //Led Spento - se fuori dall'intervallo
    if ((OraCorrente.seriale < ReleNA_ON.seriale) && (OraCorrente.seriale > ReleNA_OFF.seriale)){
        digitalWrite(pinReleNA,LOW);
        return;
    }
  //Led Acceso
    digitalWrite(pinReleNA,HIGH);
}

//--------------------------------------------------------------------------------------------------------
//------- Alba PWM Up-Down -------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

void controlloAlba(){

    //Led Spento - se fuori dall'intervallo
    if ((OraCorrente.seriale < AlbaFase1.seriale) || (OraCorrente.seriale > AlbaFase3.seriale)){
        digitalWrite(pinAlba,LOW);
        return;
    }
  
  //PWM Crescente
    if ((OraCorrente.seriale > AlbaFase1.seriale) && (OraCorrente.seriale < AlbaFase2.seriale)){
        int pw = map(OraCorrente.seriale,AlbaFase1.seriale,AlbaFase2.seriale,0,255);
        analogWrite(pinAlba,pw);
        return;
    }
  
    //PWM Decrescente
    if ((OraCorrente.seriale > AlbaFase2.seriale) && (OraCorrente.seriale < AlbaFase3.seriale)){
        int pw = map(OraCorrente.seriale,AlbaFase2.seriale,AlbaFase3.seriale,255,0);
        analogWrite(pinAlba,pw);
        return;
    }
    
}

//--------------------------------------------------------------------------------------------------------
//------- Fuoco a 3 Led ----------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

void controlloFuocoGiorno(){
    
    //Led Spenti - se fuori dall'intervallo  
    if ((OraCorrente.seriale < FuocoGiorno_ON.seriale) || (OraCorrente.seriale > FuocoGiorno_OFF.seriale)){
        digitalWrite(pinFuocoGiorno+0,LOW);
        digitalWrite(pinFuocoGiorno+1,LOW);
        digitalWrite(pinFuocoGiorno+2,LOW);
        return;
    }
    
    //Led PWM con tempi variabili
    analogWrite(pinFuocoGiorno+0, random(160)+(255- 160));
    analogWrite(pinFuocoGiorno+1, random(100)+(255-100));
    analogWrite(pinFuocoGiorno+2, random(250)+(255- 250));
    //Pausa PWM Variabile
    delay(random(100));
}
