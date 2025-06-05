#include <LiquidCrystal_I2C.h>



LiquidCrystal_I2C lcd(0x27, 16, 2);  // Endereço I2C (pode ser 0x27 ou 0x3F)

const int pinBotao1 = 6;
const int pinBotao2 = 7;
const int pinBotao3 = 8;
const int pinBotao4 = 9;
const int pinBuzzer = 10;
const int pinLedVermelho = 2;
const int pinLedAzul = 3;
const int pinLedAmarelo = 4;
const int pinLedVerde = 5;
float fatorDificuldade = 1.0;
int pinsRodada[50];
int freqsRodada[50];
int rodada = 0;
int highScore = 0;
bool reset = false;

bool ledAtivo = false;
unsigned long tempoAtivado = 0;
int pins[4] = {pinLedVermelho, pinLedAzul, pinLedAmarelo, pinLedVerde};
int frequencias[4] = {263, 349, 466, 622}; // Os leds são associados com as frequêencias

unsigned long ultimoTempoLeitura = 0;
const int tempoDebounce = 100;

void setup()
{
  pinMode(pinLedVerde, OUTPUT);
  pinMode(pinLedAzul, OUTPUT);
  pinMode(pinLedVermelho, OUTPUT);
  pinMode(pinLedAmarelo, OUTPUT);
  pinMode(pinBuzzer, OUTPUT);
  
  pinMode(pinBotao1, INPUT_PULLUP);
  pinMode(pinBotao2, INPUT_PULLUP);
  pinMode(pinBotao3, INPUT_PULLUP);
  pinMode(pinBotao4, INPUT_PULLUP);
  
  randomSeed(analogRead(A0));  
  
    lcd.init();               // Inicializa o LCD
  lcd.backlight();          // Liga a luz de fundo

}

void loop() {
  delay(10);
  if (rodada == 0){
    esperarIniciar();
  }
  iniciar();
}

void iniciar(){
  iniciarRodada();
  bool passouRodada = false;
  if (!reset){
    passouRodada = receberInputs();
  }

  if (passouRodada){

    Serial.println("passou");
    rodada++;
    delay(500);
  } else { 
    lcd.clear();
    if (reset){
    	lcd.print("Reiniciando...");
        reset = false;
    } else {
    	lcd.print("Perdeu!");
    }
  	
    if (rodada > highScore){
     	highScore = rodada;
      	lcd.setCursor(0,1);
      	lcd.print("Novo recorde: " + String(highScore));
    } else {
     	lcd.setCursor(0,1);
      	lcd.print("Nao bateu Rec!");
    }
    rodada = 0;
    piscarTodos();
    delay(1000);
  }
}

void setFatorDificuldade(){ 
  fatorDificuldade = 1 - (0.5 * rodada)/(rodada + 1); 
  // d(x) = 1 - (0.5 * r)/(r + 1)
}

void esperarIniciar(){
  bool esperar = true;
  lcd.setCursor(0, 0);      // Coloca o cursor na primeira linha
  lcd.print("Aperte qualquer");
  lcd.setCursor(0, 1);
  lcd.print("botao p/ comecar");
  lcd.setCursor(0, 0);
  while(esperar) {
    delay(10);
    if (algumBotaoPressionado()) {
     	esperar = false;
      	lcd.clear();
      	lcd.print("Iniciando...");
    	delay(1000);
    }
  	
  }
  lcd.clear();
  
}

bool algumBotaoPressionado(){
  return digitalRead(pinBotao1) == LOW || digitalRead(pinBotao2) == LOW || digitalRead(pinBotao3) == LOW || digitalRead(pinBotao4) == LOW;
}

void iniciarRodada(){
  lcd.clear();
  lcd.print("Score: " + String(rodada));
  lcd.setCursor(0, 1);
  lcd.print("Recorde: " + String(highScore));
  setFatorDificuldade();
  Serial.println("Fator dificuldade:");
  Serial.println(fatorDificuldade);
  int indiceAleatorio = random(0,4);
  int frequenciaAleatoria = frequencias[indiceAleatorio];
  int ledAleatorio = pins[indiceAleatorio];
  pinsRodada[rodada] = ledAleatorio;
  freqsRodada[rodada] = frequenciaAleatoria;
  
  for(int i = 0; i <= rodada; i++){
    delay(10);
    if (algumBotaoPressionado()) {
      reset = true;
      return;
    }
    piscarComSom(pinsRodada[i], freqsRodada[i]);
    delay(150 * fatorDificuldade);
  }
}

void piscarTodos(){
  digitalWrite(pinLedVermelho, HIGH); 
  
  digitalWrite(pinLedAmarelo, HIGH);

  digitalWrite(pinLedVerde, HIGH);

  digitalWrite(pinLedAzul, HIGH);
  
  tone(pinBuzzer, 200);
  
  delay(300);
  
  digitalWrite(pinLedVermelho, LOW); 
  
  digitalWrite(pinLedAmarelo, LOW);

  digitalWrite(pinLedVerde, LOW);

  digitalWrite(pinLedAzul, LOW);
    
  noTone(pinBuzzer)  ;
    
}
                      
void piscarComSom(int pinLed, int frequencia){
  
  digitalWrite(pinLed, HIGH);
  tone(pinBuzzer, frequencia); 

  delay(450 * fatorDificuldade);

  digitalWrite(pinLed, LOW);
  noTone(pinBuzzer);

}

bool receberInputs(){
  for(int i = 0; i <= rodada; i++){
    delay(10);
    int inputValido = validarInput(pinsRodada[i], freqsRodada[i]);
    
    if (!inputValido) {return false;}
  }
  
  return true;
}

bool validarInput(int pinLedEmitido, int frequencia) {
  unsigned long tempoEspera = millis();
  
  while(millis() - tempoEspera < (4000 * fatorDificuldade)) { // Timeout de 3 segundos
    delay(10);
    if (millis() - ultimoTempoLeitura > tempoDebounce) {
      
      if (digitalRead(pinBotao1) == LOW) {
        ultimoTempoLeitura = millis();
        Serial.println("Botão 1 pressionado");
        if (pinLedVermelho != pinLedEmitido) return false;
        piscarComSom(pinLedVermelho, frequencia);
        return true;
      }
      else if (digitalRead(pinBotao2) == LOW) {
        ultimoTempoLeitura = millis();
        Serial.println("Botão 2 pressionado");
        if (pinLedAzul != pinLedEmitido) return false;
        piscarComSom(pinLedAzul, frequencia);
        return true;
      }
      else if (digitalRead(pinBotao3) == LOW) {
        ultimoTempoLeitura = millis();
        Serial.println("Botão 3 pressionado");
        if (pinLedAmarelo != pinLedEmitido) return false;
        piscarComSom(pinLedAmarelo, frequencia);
        return true;
      }
      else if (digitalRead(pinBotao4) == LOW) {
        ultimoTempoLeitura = millis();
        Serial.println("Botão 4 pressionado");
        if (pinLedVerde != pinLedEmitido) return false;
        piscarComSom(pinLedVerde, frequencia);
        return true;
      }
    }
  }
  return false; // Timeout ou input errado
}