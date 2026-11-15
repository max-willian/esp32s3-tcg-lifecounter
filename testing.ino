
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include <Preferences.h>
#include <math.h>
#include <string.h>

// =====================================================
// CONFIGURAÇÃO DO DISPLAY
// =====================================================

#define PIN_LCD_BL   42
#define PIN_LCD_DC   47
#define PIN_LCD_CS   5
#define PIN_LCD_SCK  4
#define PIN_LCD_MOSI 2
#define PIN_LCD_RST  38

#define PIN_TOUCH_SDA 11
#define PIN_TOUCH_SCL 7
#define PIN_TOUCH_RST 6

#define TOUCH_ADDRESS 0x15

#define COR_FUNDO 0x4208
#define COR_BRANCO 0xFFFF
#define COR_VERDE 0x2645
#define COR_VERMELHO 0xF800
#define COR_CINZA 0x8410

Arduino_DataBus *bus = new Arduino_ESP32SPI(
  PIN_LCD_DC,
  PIN_LCD_CS,
  PIN_LCD_SCK,
  PIN_LCD_MOSI,
  GFX_NOT_DEFINED
);

Arduino_GFX *gfx = new Arduino_GC9A01(
  bus,
  PIN_LCD_RST,
  0,
  true
);

// =====================================================
// CONFIGURAÇÕES E VARIÁVEIS
// =====================================================

Preferences preferences;

int contador = 34;
int maxPV = 34;

// 0 = Star Wars Unlimited
// 1 = Magic
uint8_t jogoAtual = 0;

enum Tela {
  TELA_CONTADOR,
  TELA_CONFIGURACOES,
  TELA_MAX_PV,
  TELA_CONFIRMAR_REFRESH
};

Tela telaAtual = TELA_CONTADOR;

bool toqueAnterior = false;

unsigned long inicioConfirmacao = 0;
const unsigned long TEMPO_CONFIRMAR = 3000;

// =====================================================
// FUNÇÕES AUXILIARES
// =====================================================

void limparTela() {
  gfx->fillScreen(COR_FUNDO);
}

void textoCentralizado(
  const char *texto,
  int y,
  int tamanho,
  uint16_t cor = COR_BRANCO
) {
  int largura = strlen(texto) * 6 * tamanho;
  int x = (240 - largura) / 2;

  gfx->setTextSize(tamanho);
  gfx->setTextColor(cor);
  gfx->setCursor(x, y);
  gfx->print(texto);
}

void desenharBotao(
  int x,
  int y,
  int largura,
  int altura,
  const char *texto,
  uint16_t cor
) {
  gfx->fillRoundRect(
    x, y,
    largura, altura,
    10,
    cor
  );

  gfx->setTextSize(2);
  gfx->setTextColor(COR_BRANCO);

  int larguraTexto = strlen(texto) * 12;
  int textoX = x + (largura - larguraTexto) / 2;
  int textoY = y + (altura - 16) / 2;

  gfx->setCursor(textoX, textoY);
  gfx->print(texto);
}

// =====================================================
// ÍCONE DE CONFIGURAÇÕES
// =====================================================

void desenharEngrenagem() {
  int cx = 120;
  int cy = 30;
  int raio = 10;

  gfx->drawCircle(cx, cy, raio, COR_BRANCO);
  gfx->drawCircle(cx, cy, 4, COR_BRANCO);

  for (int i = 0; i < 8; i++) {
    float angulo = i * PI / 4;

    int x1 = cx + cos(angulo) * 11;
    int y1 = cy + sin(angulo) * 11;

    int x2 = cx + cos(angulo) * 15;
    int y2 = cy + sin(angulo) * 15;

    gfx->drawLine(x1, y1, x2, y2, COR_BRANCO);
  }
}

// =====================================================
// ÍCONE DE REFRESH
// =====================================================

void desenharRefresh() {
  int cx = 120;
  int cy = 190;

  gfx->drawCircle(cx, cy, 15, COR_BRANCO);

  gfx->fillTriangle(
    120, 169,
    112, 180,
    128, 180,
    COR_BRANCO
  );

  gfx->fillTriangle(
    120, 211,
    112, 200,
    128, 200,
    COR_BRANCO
  );
}

// =====================================================
// TELA PRINCIPAL
// =====================================================

void desenharContador() {
  limparTela();

  desenharEngrenagem();

  // Nome do jogo
  if (jogoAtual == 0) {
    textoCentralizado("STAR WARS", 55, 2);
  } else {
    textoCentralizado("MAGIC", 55, 2);
  }

  // Valor do contador
  char textoPV[10];
  sprintf(textoPV, "%d", contador);

  textoCentralizado(textoPV, 95, 5);

  // PV máximo configurado
  char textoMax[20];
  sprintf(textoMax, "MAX: %d", maxPV);

  textoCentralizado(textoMax, 145, 1, COR_BRANCO);

  desenharRefresh();
}

// =====================================================
// TELA DE CONFIGURAÇÕES
// =====================================================

void desenharConfiguracoes() {
  limparTela();

  textoCentralizado("CONFIGURACOES", 25, 2);

  gfx->drawRoundRect(
    20, 65,
    200, 45,
    8,
    COR_BRANCO
  );

  textoCentralizado("MAXIMO PV", 80, 2);

  gfx->drawRoundRect(
    20, 120,
    200, 45,
    8,
    COR_BRANCO
  );

  if (jogoAtual == 0) {
    textoCentralizado("JOGO: SWU", 135, 2);
  } else {
    textoCentralizado("JOGO: MAGIC", 135, 2);
  }

  desenharBotao(
    65, 185,
    110, 35,
    "VOLTAR",
    COR_CINZA
  );
}

// =====================================================
// TELA DE ALTERAÇÃO DO PV MÁXIMO
// =====================================================

void desenharMaxPV() {
  limparTela();

  textoCentralizado("MAXIMO PV", 35, 2);

  char texto[10];
  sprintf(texto, "%d", maxPV);

  textoCentralizado(texto, 105, 5);

  // Botão diminuir
  gfx->fillRoundRect(
    20, 90,
    65, 65,
    10,
    COR_VERMELHO
  );

  textoCentralizado("-", 112, 4);

  // Botão aumentar
  gfx->fillRoundRect(
    155, 90,
    65, 65,
    10,
    COR_VERDE
  );

  gfx->setTextSize(4);
  gfx->setTextColor(COR_BRANCO);
  gfx->setCursor(176, 112);
  gfx->print("+");

  desenharBotao(
    65, 180,
    110, 40,
    "SALVAR",
    COR_VERDE
  );
}

// =====================================================
// TELA DE CONFIRMAÇÃO DO REFRESH
// =====================================================

void desenharConfirmacaoRefresh() {
  limparTela();

  textoCentralizado("CERTEZA?", 65, 3);

  desenharBotao(
    20, 125,
    90, 45,
    "SIM",
    COR_VERDE
  );

  desenharBotao(
    130, 125,
    90, 45,
    "NAO",
    COR_VERMELHO
  );

  textoCentralizado("3 SEGUNDOS", 190, 1);
}

// =====================================================
// NAVEGAÇÃO
// =====================================================

void voltarContador() {
  telaAtual = TELA_CONTADOR;
  desenharContador();
}

void abrirConfirmacaoRefresh() {
  telaAtual = TELA_CONFIRMAR_REFRESH;

  inicioConfirmacao = millis();

  desenharConfirmacaoRefresh();

  Serial.println("[REFRESH] Aguardando confirmacao");
}

void cancelarRefresh() {
  Serial.println("[REFRESH] Cancelado");

  voltarContador();
}

void confirmarRefresh() {
  contador = maxPV;

  Serial.println("[REFRESH] PV restaurado!");

  voltarContador();
}

// =====================================================
// CONFIGURAÇÕES PERSISTENTES
// =====================================================

void carregarConfiguracoes() {
  preferences.begin("contador", false);

  maxPV = preferences.getUChar("maxPV", 34);
  jogoAtual = preferences.getUChar("jogo", 0);

  // Proteção contra valores inválidos
  if (maxPV < 20 || maxPV > 40) {
    maxPV = 34;
  }

  if (jogoAtual > 1) {
    jogoAtual = 0;
  }

  contador = maxPV;

  Serial.println("[CONFIG] Configuracoes carregadas");
  Serial.print("[CONFIG] Maximo PV: ");
  Serial.println(maxPV);

  Serial.print("[CONFIG] Jogo: ");
  Serial.println(jogoAtual);
}

void salvarMaxPV() {
  preferences.putUChar("maxPV", maxPV);

  Serial.print("[CONFIG] Novo PV maximo salvo: ");
  Serial.println(maxPV);
}

void salvarJogo() {
  preferences.putUChar("jogo", jogoAtual);

  Serial.print("[CONFIG] Jogo salvo: ");
  Serial.println(jogoAtual);
}

// =====================================================
// LEITURA DO TOUCH CST816S
// =====================================================

bool lerTouch(int &x, int &y) {
  Wire.beginTransmission(TOUCH_ADDRESS);
  Wire.write(0x02);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom(
    (uint8_t)TOUCH_ADDRESS,
    (uint8_t)5
  ) != 5) {
    return false;
  }

  uint8_t dados[5];

  for (int i = 0; i < 5; i++) {
    dados[i] = Wire.read();
  }

  if (dados[0] == 0) {
    return false;
  }

  x = ((dados[1] & 0x0F) << 8) | dados[2];
  y = ((dados[3] & 0x0F) << 8) | dados[4];

  return true;
}

// =====================================================
// PROCESSAMENTO DOS TOQUES
// =====================================================

void processarToque(int x, int y) {

  Serial.print("[TOUCH] X: ");
  Serial.print(x);
  Serial.print(" | Y: ");
  Serial.println(y);

  // ---------------------------------------------------
  // TELA PRINCIPAL
  // ---------------------------------------------------

  if (telaAtual == TELA_CONTADOR) {

    // Engrenagem / configurações
    if (
      y < 48 &&
      x >= 90 &&
      x <= 150
    ) {
      telaAtual = TELA_CONFIGURACOES;

      desenharConfiguracoes();

      Serial.println("[TELA] Configuracoes");

      return;
    }

    // Botão refresh
    if (
      y >= 165 &&
      x >= 75 &&
      x <= 165
    ) {
      abrirConfirmacaoRefresh();

      return;
    }

    // Incrementar ou decrementar
    if (x >= 120) {

      // STAR WARS: respeita PV máximo
      if (jogoAtual == 0) {

        if (contador < maxPV) {
          contador++;

          Serial.println("[ACAO] +1 SWU");
        } else {
          Serial.println("[LIMITE] Maximo PV atingido");
        }

      }

      // MAGIC: não respeita PV máximo
      else {

        contador++;

        Serial.println("[MAGIC] +1");
      }

    } else {

      // Ambos os jogos podem diminuir até zero
      if (contador > 0) {
        contador--;

        Serial.println("[ACAO] -1");
      } else {
        Serial.println("[LIMITE] PV minimo atingido");
      }
    }

    desenharContador();

    return;
  }

  // ---------------------------------------------------
  // TELA DE CONFIGURAÇÕES
  // ---------------------------------------------------

  if (telaAtual == TELA_CONFIGURACOES) {

    // Configurar PV máximo
    if (y >= 55 && y <= 110) {

      telaAtual = TELA_MAX_PV;

      desenharMaxPV();

      Serial.println("[TELA] Alterar maximo PV");

      return;
    }

    // Alternar jogo
    if (y >= 112 && y <= 165) {

      if (jogoAtual == 0) {
        jogoAtual = 1;
      } else {
        jogoAtual = 0;
      }

      salvarJogo();

      desenharConfiguracoes();

      Serial.println("[CONFIG] Jogo alterado");

      return;
    }

    // Voltar
    if (y >= 180) {

      voltarContador();

      return;
    }
  }

  // ---------------------------------------------------
  // TELA DE ALTERAÇÃO DO PV MÁXIMO
  // ---------------------------------------------------

  if (telaAtual == TELA_MAX_PV) {

    // Diminuir
    if (
      y >= 80 &&
      y <= 155 &&
      x < 90
    ) {

      if (maxPV > 20) {
        maxPV--;

        Serial.println("[CONFIG] PV maximo -1");
      }

      desenharMaxPV();

      return;
    }

    // Aumentar
    if (
      y >= 80 &&
      y <= 155 &&
      x > 150
    ) {

      if (maxPV < 40) {
        maxPV++;

        Serial.println("[CONFIG] PV maximo +1");
      }

      desenharMaxPV();

      return;
    }

    // Salvar
    if (y >= 175 && y <= 230) {

      salvarMaxPV();

      // Ao salvar, o contador assume o novo máximo
      contador = maxPV;

      voltarContador();

      Serial.println("[CONFIG] PV atualizado");

      return;
    }
  }

  // ---------------------------------------------------
  // TELA DE CONFIRMAÇÃO DO REFRESH
  // ---------------------------------------------------

  if (telaAtual == TELA_CONFIRMAR_REFRESH) {

    if (y >= 115 && y <= 180) {

      // SIM
      if (x < 120) {
        confirmarRefresh();

        return;
      }

      // NÃO
      else {
        cancelarRefresh();

        return;
      }
    }
  }
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(500);

  Serial.println();
  Serial.println("================================");
  Serial.println(" INICIANDO CONTADOR DE PV");
  Serial.println("================================");

  // Backlight
  pinMode(PIN_LCD_BL, OUTPUT);
  digitalWrite(PIN_LCD_BL, LOW);

  // Display
  Serial.println("[LCD] Inicializando display...");

  if (!gfx->begin()) {
    Serial.println("[ERRO] Falha ao inicializar display");
  } else {
    Serial.println("[LCD] Display inicializado");
  }

  // Touch reset
  Serial.println("[TOUCH] Inicializando CST816S...");

  pinMode(PIN_TOUCH_RST, OUTPUT);

  digitalWrite(PIN_TOUCH_RST, LOW);
  delay(50);

  digitalWrite(PIN_TOUCH_RST, HIGH);
  delay(200);

  Wire.begin(
    PIN_TOUCH_SDA,
    PIN_TOUCH_SCL
  );

  Wire.setClock(100000);

  Serial.println("[TOUCH] I2C inicializado");

  // Configurações salvas
  carregarConfiguracoes();

  // Primeira tela
  desenharContador();

  Serial.println("[SISTEMA] Pronto!");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // Verifica o timeout da confirmação
  if (telaAtual == TELA_CONFIRMAR_REFRESH) {

    if (
      millis() - inicioConfirmacao >=
      TEMPO_CONFIRMAR
    ) {

      Serial.println("[REFRESH] Tempo esgotado");

      // Volta sem modificar o contador
      voltarContador();
    }
  }

  int x = 0;
  int y = 0;

  bool tocando = lerTouch(x, y);

  // Detecta apenas o início de um toque
  if (tocando && !toqueAnterior) {
    processarToque(x, y);
  }

  toqueAnterior = tocando;

  delay(25);
}
