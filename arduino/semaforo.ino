
#include <stdio.h>

// ================================
// PINOS DOS LEDs
// ================================

const int carroVermelho = 6;
const int carroAmarelo = 7;
const int carroVerde = 8;

const int pedestreVermelho = 4;
const int pedestreVerde = 5;

// ================================
// VARIAVEIS DO SISTEMA
// ================================

int quantidadeCarros = 1;
int quantidadePessoas = 10;

int carrosCiclo;
int pessoasCiclo;

bool carroPrimeiro = true;
bool ultimoPrimeiroFoiCarro = false;

// Estados do semaforo
enum Estado {
  CARRO_VERDE,
  CARRO_AMARELO,
  PEDESTRE_VERDE,
  VERMELHO_INTERMEDIARIO,
  VERMELHO_FINAL
};

Estado estadoAtual;

unsigned long inicioEstado = 0;
unsigned long tempoEstado = 0;

// Comunicacao serial
char entrada[40];
byte posicao = 0;

unsigned long ultimoCaractereRecebido = 0;

// ================================
// CALCULO DO TEMPO
// ================================

unsigned long calcularTempo(int quantidade) {

  unsigned long tempo = 5000 + (quantidade * 1000);

  if (tempo > 15000) {
    tempo = 15000;
  }

  return tempo;
}

// ================================
// DESLIGAR TODOS OS LEDs
// ================================

void desligarTodos() {

  digitalWrite(carroVermelho, LOW);
  digitalWrite(carroAmarelo, LOW);
  digitalWrite(carroVerde, LOW);

  digitalWrite(pedestreVermelho, LOW);
  digitalWrite(pedestreVerde, LOW);
}

// ================================
// ENTRAR EM UM ESTADO
// ================================

void entrarEstado(int novoEstado) {

  estadoAtual = (Estado)novoEstado;

  inicioEstado = millis();

  desligarTodos();

  switch (estadoAtual) {

    case CARRO_VERDE:

      digitalWrite(carroVerde, HIGH);
      digitalWrite(pedestreVermelho, HIGH);

      tempoEstado = calcularTempo(carrosCiclo);

      break;

    case CARRO_AMARELO:

      digitalWrite(carroAmarelo, HIGH);
      digitalWrite(pedestreVermelho, HIGH);

      tempoEstado = 2000;

      break;

    case PEDESTRE_VERDE:

      digitalWrite(carroVermelho, HIGH);
      digitalWrite(pedestreVerde, HIGH);

      tempoEstado = calcularTempo(pessoasCiclo);

      break;

    case VERMELHO_INTERMEDIARIO:

    case VERMELHO_FINAL:

      digitalWrite(carroVermelho, HIGH);
      digitalWrite(pedestreVermelho, HIGH);

      tempoEstado = 1000;

      break;
  }
}

// ================================
// INICIAR UM NOVO CICLO
// ================================

void iniciarCiclo() {

  carrosCiclo = quantidadeCarros;
  pessoasCiclo = quantidadePessoas;

  // Definir quem tera prioridade

  if (carrosCiclo > pessoasCiclo) {

    carroPrimeiro = true;

  } else if (pessoasCiclo > carrosCiclo) {

    carroPrimeiro = false;

  } else {

    // Em caso de empate, alternar prioridade

    carroPrimeiro = !ultimoPrimeiroFoiCarro;
  }

  ultimoPrimeiroFoiCarro = carroPrimeiro;

  // Mostrar informacoes no Monitor Serial

  Serial.println("====================");
  Serial.println("NOVO CICLO");

  Serial.print("Carros: ");
  Serial.println(carrosCiclo);

  Serial.print("Pessoas: ");
  Serial.println(pessoasCiclo);

  if (carroPrimeiro) {

    Serial.println("PRIORIDADE: CARROS");

    entrarEstado(CARRO_VERDE);

  } else {

    Serial.println("PRIORIDADE: PEDESTRES");

    entrarEstado(PEDESTRE_VERDE);
  }
}

// ================================
// PROCESSAR DADOS RECEBIDOS
// ================================

void processarEntrada() {

  entrada[posicao] = '\0';

  int carros;
  int pessoas;

  if (sscanf(entrada, "C=%d,P=%d", &carros, &pessoas) == 2) {

    if (carros >= 0 && carros <= 20 &&
        pessoas >= 0 && pessoas <= 20) {

      quantidadeCarros = carros;
      quantidadePessoas = pessoas;

      Serial.println("DADOS ATUALIZADOS!");

      Serial.print("Carros recebidos: ");
      Serial.println(quantidadeCarros);

      Serial.print("Pessoas recebidas: ");
      Serial.println(quantidadePessoas);

    } else {

      Serial.println("ERRO: valores fora do limite.");
    }

  } else {

    Serial.println("ERRO: formato invalido.");
  }

  posicao = 0;
}

// ================================
// RECEBER DADOS PELA SERIAL
// ================================

void receberDados() {

  while (Serial.available() > 0) {

    char caractere = Serial.read();

    if (caractere == '\n' || caractere == '\r') {

      if (posicao > 0) {
        processarEntrada();
      }

    } else {

      if (posicao < sizeof(entrada) - 1) {

        entrada[posicao++] = caractere;

        ultimoCaractereRecebido = millis();

      } else {

        posicao = 0;

        Serial.println("ERRO: mensagem muito longa.");
      }
    }
  }

  // Processar mesmo sem quebra de linha

  if (posicao > 0 &&
      millis() - ultimoCaractereRecebido >= 100) {

    processarEntrada();
  }
}

// ================================
// CONTROLAR O SEMAFORO
// ================================

void controlarSemaforo() {

  if (millis() - inicioEstado >= tempoEstado) {

    switch (estadoAtual) {

      case CARRO_VERDE:

        entrarEstado(CARRO_AMARELO);

        break;

      case CARRO_AMARELO:

        if (carroPrimeiro) {

          entrarEstado(VERMELHO_INTERMEDIARIO);

        } else {

          entrarEstado(VERMELHO_FINAL);
        }

        break;

      case PEDESTRE_VERDE:

        if (carroPrimeiro) {

          entrarEstado(VERMELHO_FINAL);

        } else {

          entrarEstado(VERMELHO_INTERMEDIARIO);
        }

        break;

      case VERMELHO_INTERMEDIARIO:

        if (carroPrimeiro) {

          entrarEstado(PEDESTRE_VERDE);

        } else {

          entrarEstado(CARRO_VERDE);
        }

        break;

      case VERMELHO_FINAL:

        iniciarCiclo();

        break;
    }
  }
}

// ================================
// CONFIGURACAO INICIAL
// ================================

void setup() {

  Serial.begin(9600);

  pinMode(carroVermelho, OUTPUT);
  pinMode(carroAmarelo, OUTPUT);
  pinMode(carroVerde, OUTPUT);

  pinMode(pedestreVermelho, OUTPUT);
  pinMode(pedestreVerde, OUTPUT);

  Serial.println("SEMAFORO INTELIGENTE");
  Serial.println("Inicializando sistema...");

  iniciarCiclo();
}

// ================================
// LOOP PRINCIPAL
// ================================

void loop() {

  receberDados();

  controlarSemaforo();
}
