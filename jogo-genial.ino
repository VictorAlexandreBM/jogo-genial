#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);  // Endereço I2C (pode ser 0x27 ou 0x3F)

// Variáveis de pins
const int pinBotao1 = 6;
const int pinBotao2 = 7;
const int pinBotao3 = 8;
const int pinBotao4 = 9;
const int pinBuzzer = 10;
const int pinLedVermelho = 2;
const int pinLedAzul = 3;
const int pinLedAmarelo = 4;
const int pinLedVerde = 5;

// O fator de dificuldade determina a velocidade durante a sequência de leds
// Por isso, quanto menor esse fator, menor os delays, e, portanto, maior a dificuldade.
float fatorDificuldade = 1.0;

// Isso salva a sequência criada.
// É necessário salvar tanto as cores quanto as frequêcias emitidas para imitar a sequência.

int pinsRodada[50];
int freqsRodada[50];
int rodada = 0;
int highScore = 0;

bool reset = false;

// Os pins e frequências são inseridos em arrays para seleção aleatória.
int pins[4] = {pinLedVermelho, pinLedAzul, pinLedAmarelo, pinLedVerde};
int frequencias[4] = {263, 349, 466, 622}; // Os leds são mapeados às frequêencias

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
  
  lcd.init();               
  lcd.backlight();          

}

void loop() {
  delay(10);
  if (rodada == 0){
    esperarIniciar();
  }
  iniciar();
}

void iniciar(){
  bool passouRodada = false;

  iniciarRodada();
  
  // reset é uma variável booleana que é settada para true apena quando o 
  // usuário deseja resetar, não quando ele perde.
  // receberInputs retorna um booleano para a derrota/vitória do usuário
  if (!reset){
    passouRodada = receberInputs();
  }

  if (passouRodada){

    Serial.println("passou");
    rodada++;
    delay(500);

  } else { 
    // não sei se precisa, só pra garantir que será limpado
    lcd.clear();

    if (reset){
    	lcd.print("Reiniciando...");
      // redefine o reset para o valor inicial dele
      reset = false;
    } else {
      // Isso ocorre quando o usuário perdeu e não foi reset
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
  //Fórmula do fator de dificuldade, veja a função abaixo
  // d(r) = 1 - (0.5 * r)/(r + 1)
  // O fator de dificuldade começa em 1, diminui bastante no início, e se estabiliza próximo 
  // a 0,65 para as próximas rodaas
  fatorDificuldade = 1 - (0.5 * rodada)/(rodada + 1); 
  
}

void esperarIniciar(){
  bool esperar = true;
  
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Genial Game");
  lcd.setCursor(7, 1);
  lcd.print("<3");
  
  delay(2000);
  lcd.clear();

  lcd.setCursor(0, 0);
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
    
    //Quando se inicia a rodada, atualiza o fator de dificuldade global
    setFatorDificuldade();
    
    //Veja que o pin e o led estão mapeados no mesmo índice aleatório
    int indiceAleatorio = random(0,4);
    int frequenciaAleatoria = frequencias[indiceAleatorio];
    int ledAleatorio = pins[indiceAleatorio];

    //Armazena a frequência e o led tocado nas arrays, para depois replicar a sequência
    pinsRodada[rodada] = ledAleatorio;
    freqsRodada[rodada] = frequenciaAleatoria;
    
    // replica a sequência com base nas arrays
    for(int i = 0; i <= rodada; i++){
      delay(10);
      //Enquanto o jogo estiver tocando, o usuário pode apertar um botão para reiniciar rapidamente
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
  
  while(millis() - tempoEspera < (4000 * fatorDificuldade)) { // Para armazenar um timeout 
  
    delay(10);
    //evita debounce nos botões
    if (millis() - ultimoTempoLeitura > tempoDebounce) {
      
      // Verifica qual foi pressionado pelo usuário retorna verdadeiro se foi o ledemitido
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
