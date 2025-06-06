Projeto de arquitetura e organização de computadores II

Código desenvolvido por Victor Alexandre Borges Milhomem
# O que é o Jogo Genial?

Projeto inspirado no jogo de memória Genius da Estrela. O jogo possui 4 leds: azul, vermelho, amarelo e verde. Cada led é associado a um botão com a mesma cor.

O fluxo do jogo é o seguinte, em específico:

- O usuário aperta um botão para iniciar;
- Uma luz aleatória então é emitida, junto com uma nota;
- O usuário deve então apertar o botão correspondente à luz;
- Então, outra rodada começa, e as mesmas leds e notas da rodada anterior são acesas na mesma sequência;
- O usuário deve então replicar a sequência, e avançar para a próxima rodada;
- A velocidade do ritmo das luzes e notas emitidas aumenta a cada rodada.
- O jogo acaba quando:
    - O usuário ficar um tempo ocioso quando for imitar a sequência (por exemplo, 3 segundos);
    - O usuário erre qualquer cor da sequência.

O jogo genial reproduz esse mesmo fluxo!
# Materiais

- 1 protoboard 
- 15 Jumpers macho-macho, sendo:
	- 1 para conexã GND
	- 8 para os botões, sendo:
		- 4 para conectar os pinos
		- 4 para conectar ao GND
	- 4 para a conexão aos ânodos dos leds
	- 2 para o buzzer passivo
- 4 Resistores 220 Ohms para os leds
- 4 Jumpers macho-fêmea, para o módulo I2C do display Lcd 16x2
- 1 Placa Arduino mini R3
- 1 Display Lcd 16x2 com módulo I2C
- 1 Buzzer Passivo
- 4 Push buttons
- 4 Leds de cores diferentes
- Papelão para a case
# Imagem do protótipo
A imagem da disposição física do dispositivo no simulador **TinkerCad** nos ajuda a entender como cada material foi utilizado, assim como a lógica do código conversa com o hardware.

![image](https://github.com/user-attachments/assets/6c34d47e-aa76-4032-8416-1ff10dbe8c11)

# Lógica do código

O principal aspecto do jogo é a sequência aleatória criada. Portanto, não é de se surpreender que muitas implementações são baseadas em **arrays** que armazenam sequências e geração de índices aleatórios.

As arrays a seguir são globais. A primeira armazena o pin de qual led foi piscado, a segunda, as frequências de som dos leds, que deve ser armazenada para emissão do som.

``` c++
int pinsRodada[50];

int freqsRodada[50];

int rodada = 0;
```

A seguir, são as variáveis globais, indicam o controle dos leds, dos botões e buzzer.
``` c++
const int pinBotao1 = 6;
const int pinBotao2 = 7;
const int pinBotao3 = 8;
const int pinBotao4 = 9;
const int pinBuzzer = 10;
const int pinLedVermelho = 2;
const int pinLedAzul = 3;
const int pinLedAmarelo = 4;
const int pinLedVerde = 5;

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
```

Para geração aleatória de sequência, os pins dos leds e suas frequências são armazenados em arrays de 4 posições. Dessa forma, é bem fácil gerar um índice aleatório de 0 a 3, e acessar uma posição na array:

``` c++
int pins[4] = {pinLedVermelho, pinLedAzul, pinLedAmarelo, pinLedVerde};

int frequencias[4] = {263, 349, 466, 622}; // Os leds são mapeados às frequências
```

Os leds são mapeados às frequências, o que gera um questionamento: por que não se utilizou matrizes?

Porque, inicialmente, a intenção do programa era ter várias frequências, de forma aleatória para os leds. Somente no fim do projeto que foi decidido que seriam apenas 4 frequências mapeadas aos leds.  

O programa espera um input do usuário para começar, essa lógica de espera de input é feita na função `esperarIniciar()`;

``` c++
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
```

A função: 
- Imprime no display o necessário
- Aguarda input do usuário a partir da função `algumBotaoPressionado()`  e simula carragamento 
	- Esta função só é um booleano com vários "ous" dos 4 botões:

``` c++
bool algumBotaoPressionado(){

	return digitalRead(pinBotao1) == LOW || digitalRead(pinBotao2) == LOW || digitalRead(pinBotao3) == LOW || digitalRead(pinBotao4) == LOW;
	
}
```

`iniciar()` deve ser uma função bem intuitiva:

``` c++
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
```

A função de `iniciarRodada()` é a parte que o jogo **"conversa"** com o jogador, que faz a sequência 

``` c++
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
		if (algumBotaoPressionado()){
			reset = true;
			return;
		}
		piscarComSom(pinsRodada[i], freqsRodada[i]);
		delay(150 * fatorDificuldade);
	}

}
```

O `fatorDificuldade` começa em 1.0. Ele multiplica o intervalo de `delay()` entre os botões (portanto um valor menor deixa mais rápido e mais difícil de acompanhar a sequência) e vai diminuindo conforme o usuário avança no programa. É calculado por uma função racional, que **vai de 1 a 0.7 muito rapidamente nas primeiras rodadas, e se estabiliza em 0.6**. 

``` c++
void setFatorDificuldade(){
	//Fórmula do fator de dificuldade, veja a função abaixo
	// d(r) = 1 - (0.5 * r)/(r + 1)
	// O fator de dificuldade começa em 1, diminui bastante no início, e se estabiliza próximo
	// a 0,65 para as próximas rodaas

	fatorDificuldade = 1 - (0.5 * rodada)/(rodada + 1);

}
```

O usuário **"responde"** a sequência oferecida pelo programa a partir da função `receberInputs()`, que retorna verdadeiro caso acerte toda a sequência.
Errar apenas uma única ordem da sequência retorna falso. 

``` c++
bool receberInputs(){
  for(int i = 0; i <= rodada; i++){
    delay(10);
    int inputValido = validarInput(pinsRodada[i], freqsRodada[i]);
    
    if (!inputValido) {return false;}
  }
  
  return true;
}
```

A função `validarInput()` só verifica qual botão foi pressionado e valida se faz parte da frequência no momento, essa função permite que **cada botão seja mapeado a um led.**

```c++

bool validarInput(int pinLedEmitido, int frequencia) {
  unsigned long tempoEspera = millis();
  
  while(millis() - tempoEspera < (4000 * fatorDificuldade)) { // Para armazenar um timeout 
  
    delay(10);
    //evita debounce nos botões
    if (millis() - ultimoTempoLeitura > tempoDebounce) {
      
      // Verifica qual foi pressionado pelo usuário retorna verdadeiro se foi o ledemitido
      // Cada botão é mapeado a um led 
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
```

Essas são as principais funções que compõem a lógica da aplicação.

# Planos futuros

- Um dos grandes planos do projeto é elaborar um modo música, no qual o jogador selecionar uma música, ela toca na frequência que o buzzer disponibilizar, e o usuário tem que tocar os mesmos leds emitidos pela música, seguindo a sequência.
- Uma grande limitação foi a falta de espaço para inserir botões, além de com disposição dos jumpers, leds e botões. Dessa forma, a disponibilidade de protoboards maiores, além de cabos macho fêmea e botões mais dinâmicos traria um grande avanço para o projeto.
- Outra limitação foi a lógica: O programa roda num paradgima estrutural, ou seja, só temos um arquivo "faz-tudo". Caso quisessemos expandir o projeto, o ideal seria modularizar.
