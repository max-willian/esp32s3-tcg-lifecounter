
#include <Arduino_GFX_Library.h>
#include <Wire.h>
#include <Preferences.h>
#include <math.h>
#include <string.h>
#include <limits.h>
#include "UIIcons.h"
#include "SmoothDigits.h"
#include "SmoothText.h"
#include "SmoothIcons.h"
#include "MenuButton.h"
#include "Hyperspace.h"

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

#define COR_FUNDO       0x0000  // Preto nos menus / fallback
#define COR_BRANCO      0xFFFF
#define COR_VERDE       0x2645
#define COR_DESTAQUE    0xFF43  // Amarelo vivo #FFE81F em RGB565
#define COR_CINZA       0x5AEB
#define COR_TEXTO       COR_DESTAQUE
#define COR_TRILHO      0x39C7  // Cinza discreto
#define COR_CARTAO      0x1082  // Superficies escuras

Arduino_DataBus *bus = new Arduino_ESP32SPI(
  PIN_LCD_DC,
  PIN_LCD_CS,
  PIN_LCD_SCK,
  PIN_LCD_MOSI,
  GFX_NOT_DEFINED
);

Arduino_GFX *lcd = new Arduino_GC9A01(
  bus,
  PIN_LCD_RST,
  0,
  true
);
Arduino_Canvas canvas(240, 240, lcd);
Arduino_GFX *gfx = lcd;
bool canvasDisponivel = false;
bool telaSuja = false;
uint8_t quadroFundo = 0;
unsigned long ultimoQuadro = 0;

// =====================================================
// CONFIGURAÇÕES E VARIÁVEIS
// =====================================================

Preferences preferences;
MenuButton botaoMenu;

int contadores[2] = {0, 0};
int maximosPV[2] = {34, 34};
uint8_t numeroJogadores = 1;
uint8_t jogadorEmEdicao = 0;
uint8_t paginaConfiguracoes = 0;

// 0 = Star Wars Unlimited
// 1 = Magic
uint8_t jogoAtual = 0;

enum Tela {
  TELA_CONTADOR,
  TELA_CONFIGURACOES,
  TELA_MAX_PV,
  TELA_CONFIRMAR_REFRESH,
  TELA_JOGADORES,
  TELA_NAO_IMPLEMENTADO
};

Tela telaAtual = TELA_CONTADOR;

bool toqueAnterior = false;

unsigned long inicioConfirmacao = 0;
const unsigned long TEMPO_CONFIRMAR = 3000;

// =====================================================
// FUNÇÕES AUXILIARES
// =====================================================

// SWU conta dano acumulado; Magic conta vida restante.
int pvInicial(uint8_t jogador) {
  return jogoAtual == 0 ? 0 : maximosPV[jogador];
}

void desenharLegendaMaxPV(uint8_t jogador, int y, bool girar = false) {
  char texto[16];
  snprintf(texto, sizeof(texto), "MAX PV: %d", maximosPV[jogador]);
  SmoothText::draw(*gfx, texto, 120 - SmoothText::width(texto, 10) / 2,
                   y, 10, COR_TEXTO, COR_FUNDO, girar);
}

void limparTela() {
  gfx->fillScreen(COR_FUNDO);
  telaSuja = true;
}

void textoCentralizado(
  const char *texto,
  int y,
  int tamanho,
  uint16_t cor = COR_BRANCO,
  uint16_t fundo = COR_FUNDO
) {
  int altura = tamanho == 1 ? 12 : (tamanho == 2 ? 16 : 22);
  int largura = SmoothText::width(texto, altura);
  int x = (240 - largura) / 2;
  SmoothText::draw(*gfx, texto, x, y, altura, cor, fundo);
}

void desenharBotao(
  int x,
  int y,
  int largura,
  int altura,
  const char *texto,
  uint16_t cor,
  int icone = -1
) {
  gfx->fillRoundRect(x, y, largura, altura, 12, cor);
  gfx->drawRoundRect(x, y, largura, altura, 12, COR_BRANCO);

  const int alturaTexto = 16;
  int larguraTexto = SmoothText::width(texto, alturaTexto);
  int larguraGrupo = larguraTexto + (icone >= 0 ? 28 : 0);
  int textoX = x + (largura - larguraGrupo) / 2;
  if (icone >= 0) {
    UIIcons::draw(*gfx, static_cast<UIIcons::Icon>(icone),
                  textoX, y + (altura - 24) / 2, COR_TEXTO);
    textoX += 28;
  }
  int textoY = y + (altura - alturaTexto) / 2;
  SmoothText::draw(*gfx, texto, textoX, textoY, alturaTexto, COR_BRANCO, cor);
}

// =====================================================
// ÍCONE DE CONFIGURAÇÕES
// =====================================================

void desenharEngrenagem() {
  SmoothIcons::button(*gfx, SmoothIcons::settings, 120, 33,
                       COR_TEXTO, COR_CARTAO, COR_FUNDO);
}

// =====================================================
// ÍCONE DE REFRESH
// =====================================================

void desenharRefresh() {
  SmoothIcons::button(*gfx, SmoothIcons::refresh, 120, 195,
                       COR_TEXTO, COR_CARTAO, COR_FUNDO);
}

// =====================================================
// TELA PRINCIPAL
// =====================================================

void desenharContador() {
  limparTela();
  if (canvasDisponivel) Hyperspace::render(canvas.getFramebuffer(), quadroFundo);

  if (numeroJogadores == 2) {
    // P1 fica de frente para o jogador do lado oposto da mesa.
    gfx->drawFastHLine(16, 120, 208, COR_TRILHO);
    SmoothText::draw(*gfx, "P1", 120 - SmoothText::width("P1", 12) / 2,
                     17, 12, COR_TEXTO, COR_FUNDO, true);
    if (jogoAtual == 1) {
      SmoothText::draw(*gfx, "MAGIC", 120 - SmoothText::width("MAGIC", 12) / 2,
                       86, 12, COR_TEXTO, COR_FUNDO, true);
    }
    textoCentralizado("P2", 219, 1, COR_TEXTO);
    for (int jogador = 0; jogador < 2; ++jogador) {
      const int cy = jogador == 0 ? 60 : 180;
      desenharLegendaMaxPV(jogador, jogador == 0 ? 29 : 203, jogador == 0);
      SmoothDigits::draw(*gfx, contadores[jogador], 120, cy, 120, 40,
                         COR_DESTAQUE, COR_FUNDO, jogador == 0);
      UIIcons::draw(*gfx, UIIcons::Minus, 28, cy - 12, COR_TEXTO);
      UIIcons::draw(*gfx, UIIcons::Plus, 188, cy - 12, COR_TEXTO);
    }
    // Acoes globais junto a divisoria, sem ocupar os quadrantes de PV.
    SmoothIcons::button(*gfx, SmoothIcons::settings, 96, 120,
                         COR_TEXTO, COR_CARTAO, COR_FUNDO);
    SmoothIcons::button(*gfx, SmoothIcons::refresh, 144, 120,
                         COR_TEXTO, COR_CARTAO, COR_FUNDO);
    return;
  }

  // Controles continuam nas mesmas regioes de toque.
  desenharEngrenagem();

  // Identificacao do jogo.
  if (jogoAtual == 1) {
    textoCentralizado("MAGIC", 57, 2, COR_TEXTO);
  }

  // Digitos com cobertura de 4 bits, sem ampliar a fonte bitmap basica.
  SmoothDigits::draw(*gfx, contadores[0], 120, 110, 144, 56, COR_DESTAQUE, COR_FUNDO);
  UIIcons::draw(*gfx, UIIcons::Minus, 18, 101, COR_TEXTO);
  UIIcons::draw(*gfx, UIIcons::Plus, 198, 101, COR_TEXTO);

  desenharLegendaMaxPV(0, 146);
  desenharRefresh();
}

// =====================================================
// TELA DE CONFIGURAÇÕES
// =====================================================

void desenharConfiguracoes() {
  limparTela();
  textoCentralizado("CONFIGURACOES", 32, 2, COR_TEXTO);
  if (paginaConfiguracoes == 1 && numeroJogadores == 2) {
    textoCentralizado("2/2", 50, 1, COR_TEXTO);
    desenharBotao(25, 85, 190, 45, "JOGADORES: 2", COR_CARTAO);
    desenharBotao(48, 185, 38, 32, "<", COR_CARTAO);
    desenharBotao(94, 185, 100, 32, "VOLTAR", COR_CARTAO);
    return;
  }
  paginaConfiguracoes = 0;
  desenharBotao(25, 55, 190, 35, "PV JOGADOR 1", COR_CARTAO);
  if (numeroJogadores == 2) {
    desenharBotao(25, 95, 190, 35, "PV JOGADOR 2", COR_CARTAO);
    desenharBotao(25, 135, 190, 35, jogoAtual == 0 ? "JOGO: SWU" : "JOGO: MAGIC", COR_CARTAO);
  } else {
    desenharBotao(25, 95, 190, 35, jogoAtual == 0 ? "JOGO: SWU" : "JOGO: MAGIC", COR_CARTAO);
    desenharBotao(25, 135, 190, 35, "JOGADORES: 1", COR_CARTAO);
  }
  if (numeroJogadores == 2) {
    desenharBotao(46, 185, 100, 32, "VOLTAR", COR_CARTAO);
    desenharBotao(154, 185, 38, 32, ">", COR_CARTAO);
  } else {
    desenharBotao(60, 185, 120, 35, "VOLTAR", COR_CARTAO, UIIcons::Back);
  }
}

void desenharJogadores() {
  limparTela();
  textoCentralizado("JOGADORES", 30, 2, COR_TEXTO);
  const char *rotulos[] = {"1", "2", "3", "4"};
  for (int i = 0; i < 4; ++i) {
    int x = (i % 2) == 0 ? 46 : 130;
    int y = i < 2 ? 65 : 125;
    desenharBotao(x, y, 64, 44, rotulos[i],
                   numeroJogadores == i + 1 ? COR_TRILHO : COR_CARTAO);
  }
  desenharBotao(60, 185, 120, 35, "VOLTAR", COR_CARTAO, UIIcons::Back);
}

void desenharNaoImplementado() {
  limparTela();
  textoCentralizado("nao implementado", 95, 2, COR_TEXTO);
  desenharBotao(60, 175, 120, 40, "VOLTAR", COR_CARTAO, UIIcons::Back);
}

// =====================================================
// TELA DE ALTERAÇÃO DO PV MÁXIMO
// =====================================================

void desenharMaxPV() {
  limparTela();

  textoCentralizado(jogadorEmEdicao == 0 ? "PV JOGADOR 1" : "PV JOGADOR 2", 35, 2, COR_TEXTO);

  // Valor em destaque.
  SmoothDigits::draw(*gfx, maximosPV[jogadorEmEdicao], 120, 122, 64, 28, COR_DESTAQUE, COR_FUNDO);

  // Diminuir.
  gfx->fillRoundRect(20, 90, 65, 65, 14, COR_CARTAO);
  gfx->drawRoundRect(20, 90, 65, 65, 14, COR_DESTAQUE);
  UIIcons::draw(*gfx, UIIcons::Minus, 40, 110, COR_TEXTO);

  // Aumentar.
  gfx->fillRoundRect(155, 90, 65, 65, 14, COR_CARTAO);
  gfx->drawRoundRect(155, 90, 65, 65, 14, COR_DESTAQUE);
  UIIcons::draw(*gfx, UIIcons::Plus, 176, 110, COR_TEXTO);

  desenharBotao(60, 180, 120, 40, "SALVAR", COR_CARTAO, UIIcons::Check);
}

// =====================================================
// TELA DE CONFIRMAÇÃO DO REFRESH
// =====================================================

void desenharConfirmacaoRefresh() {
  limparTela();

  textoCentralizado("CERTEZA?", 65, 3, COR_TEXTO);
  textoCentralizado(numeroJogadores == 2 ? "RESTAURAR AMBOS?" : "RESTAURAR PV?", 103, 1, COR_BRANCO);

  desenharBotao(20, 125, 90, 45, "SIM", COR_CARTAO, UIIcons::Check);
  desenharBotao(130, 125, 90, 45, "NAO", COR_CARTAO, UIIcons::Close);

  textoCentralizado("3 SEGUNDOS", 190, 1, COR_TEXTO);
}

// =====================================================
// NAVEGAÇÃO
// =====================================================

void abrirConfiguracoes() {
  paginaConfiguracoes = 0;
  telaAtual = TELA_CONFIGURACOES;
  desenharConfiguracoes();
  Serial.println("[TELA] Configuracoes");
}

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
  for (int i = 0; i < numeroJogadores; ++i) contadores[i] = pvInicial(i);

  Serial.println("[REFRESH] PV restaurado!");

  voltarContador();
}

// =====================================================
// CONFIGURAÇÕES PERSISTENTES
// =====================================================

void carregarConfiguracoes() {
  preferences.begin("contador", false);

  maximosPV[0] = preferences.getUChar("maxPV", 34);
  maximosPV[1] = preferences.getUChar("maxPV2", 34);
  numeroJogadores = preferences.getUChar("jogadores", 1);
  jogoAtual = preferences.getUChar("jogo", 0);

  if (jogoAtual > 1) jogoAtual = 0;

  // Proteção contra valores inválidos
  if (numeroJogadores < 1 || numeroJogadores > 2) numeroJogadores = 1;
  for (int i = 0; i < 2; ++i) {
    if (maximosPV[i] < 20 || maximosPV[i] > 40) maximosPV[i] = 34;
    contadores[i] = pvInicial(i);
  }


  Serial.println("[CONFIG] Configuracoes carregadas");
  Serial.print("[CONFIG] Maximo PV: ");
  Serial.println(maximosPV[0]);

  Serial.print("[CONFIG] Jogo: ");
  Serial.println(jogoAtual);
}

void salvarMaxPV() {
  preferences.putUChar(jogadorEmEdicao == 0 ? "maxPV" : "maxPV2", maximosPV[jogadorEmEdicao]);

  Serial.print("[CONFIG] Novo PV maximo salvo: ");
  Serial.println(maximosPV[jogadorEmEdicao]);
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

bool dentro(int x, int y, int esquerda, int topo, int largura, int altura) {
  return x >= esquerda && x < esquerda + largura && y >= topo && y < topo + altura;
}

void alterarPV(uint8_t jogador, bool aumentar) {
  int &pv = contadores[jogador];
  if (aumentar) {
    if ((jogoAtual == 1 || pv < maximosPV[jogador]) && pv < INT_MAX) ++pv;
  } else if (pv > 0) {
    --pv;
  }
}

void processarToque(int x, int y) {
  if (x < 0 || x >= 240 || y < 0 || y >= 240) return;

  if (telaAtual == TELA_CONTADOR) {
    if (numeroJogadores == 2) {
      // As acoes globais tem prioridade sobre os quatro quadrantes de PV.
      if (dentro(x, y, 76, 100, 40, 40)) {
        abrirConfiguracoes();
        return;
      }
      if (dentro(x, y, 124, 100, 40, 40)) {
        abrirConfirmacaoRefresh();
        return;
      }
      alterarPV(y < 120 ? 0 : 1, x >= 120);
    } else {
      // Preserva as regioes originais no modo de um jogador.
      if (y < 48 && x >= 90 && x <= 150) {
        abrirConfiguracoes();
        return;
      }
      if (y >= 165 && x >= 75 && x <= 165) {
        abrirConfirmacaoRefresh();
        return;
      }
      alterarPV(0, x >= 120);
    }
    desenharContador();
    return;
  }

  if (telaAtual == TELA_CONFIGURACOES) {
    if (paginaConfiguracoes == 1) {
      if (dentro(x, y, 25, 85, 190, 45) && numeroJogadores == 2) {
        telaAtual = TELA_JOGADORES;
        desenharJogadores();
      } else if (dentro(x, y, 48, 185, 38, 32)) {
        paginaConfiguracoes = 0;
        desenharConfiguracoes();
      } else if (dentro(x, y, 94, 185, 100, 32)) {
        voltarContador();
      }
      return;
    }
    if (dentro(x, y, 25, 55, 190, 35)) {
      jogadorEmEdicao = 0;
      telaAtual = TELA_MAX_PV;
      desenharMaxPV();
    } else if (numeroJogadores == 2 && dentro(x, y, 25, 95, 190, 35)) {
      jogadorEmEdicao = 1;
      telaAtual = TELA_MAX_PV;
      desenharMaxPV();
    } else if (dentro(x, y, 25, numeroJogadores == 2 ? 135 : 95, 190, 35)) {
      jogoAtual = jogoAtual == 0 ? 1 : 0;
      for (int i = 0; i < 2; ++i) contadores[i] = pvInicial(i);
      salvarJogo();
      desenharConfiguracoes();
    } else if (numeroJogadores == 1 && dentro(x, y, 25, 135, 190, 35)) {
      telaAtual = TELA_JOGADORES;
      desenharJogadores();
    } else if (numeroJogadores == 2 && dentro(x, y, 154, 185, 38, 32)) {
      paginaConfiguracoes = 1;
      desenharConfiguracoes();
    } else if (numeroJogadores == 2 ? dentro(x, y, 46, 185, 100, 32)
                                    : dentro(x, y, 60, 185, 120, 35)) {
      voltarContador();
    }
    return;
  }

  if (telaAtual == TELA_JOGADORES) {
    for (int i = 0; i < 4; ++i) {
      const int bx = (i % 2) == 0 ? 46 : 130;
      const int by = i < 2 ? 65 : 125;
      if (!dentro(x, y, bx, by, 64, 44)) continue;
      if (i >= 2) {
        telaAtual = TELA_NAO_IMPLEMENTADO;
        desenharNaoImplementado();
      } else {
        numeroJogadores = i + 1;
        preferences.putUChar("jogadores", numeroJogadores);
        // Alternar o layout preserva os PV dos dois jogadores nesta sessao.
        abrirConfiguracoes();
      }
      return;
    }
    if (dentro(x, y, 60, 185, 120, 35)) abrirConfiguracoes();
    return;
  }

  if (telaAtual == TELA_NAO_IMPLEMENTADO) {
    if (dentro(x, y, 60, 175, 120, 40)) {
      telaAtual = TELA_JOGADORES;
      desenharJogadores();
    }
    return;
  }

  if (telaAtual == TELA_MAX_PV) {
    int &maximo = maximosPV[jogadorEmEdicao];
    if (y >= 80 && y <= 155 && x < 90) {
      if (maximo > 20) --maximo;
      desenharMaxPV();
    } else if (y >= 80 && y <= 155 && x > 150) {
      if (maximo < 40) ++maximo;
      desenharMaxPV();
    } else if (y >= 175 && y <= 230) {
      salvarMaxPV();
      // O salvamento restaura apenas o jogador editado.
      contadores[jogadorEmEdicao] = pvInicial(jogadorEmEdicao);
      voltarContador();
    }
    return;
  }

  if (telaAtual == TELA_CONFIRMAR_REFRESH && y >= 115 && y <= 180) {
    if (x < 120) confirmarRefresh();
    else cancelarRefresh();
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

  // LCD initialization is unchanged. Canvas only composes completed frames.
  canvasDisponivel = canvas.begin(GFX_SKIP_OUTPUT_BEGIN);
  if (canvasDisponivel) {
    gfx = &canvas;
    SmoothDigits::targetBuffer() = canvas.getFramebuffer();
  } else {
    Serial.println("[UI] Canvas indisponivel: fundo solido, sem animacao");
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

  botaoMenu.begin();

  // Primeira tela
  desenharContador();
  if (canvasDisponivel) canvas.flush();
  telaSuja = false;
  ultimoQuadro = millis();

  Serial.println("[SISTEMA] Pronto!");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // Uma abertura por pressionamento; nao interrompe edicao nem confirmacao.
  const bool abrirMenu = botaoMenu.pressed() && telaAtual == TELA_CONTADOR;
  if (abrirMenu) abrirConfiguracoes();

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
  if (tocando && !toqueAnterior && !abrirMenu) {
    processarToque(x, y);
  }

  toqueAnterior = tocando;

  // Advance by source timing without blocking touch or confirmation timeout.
  const unsigned long agora = millis();
  if (canvasDisponivel && telaAtual == TELA_CONTADOR) {
    bool avancou = false;
    uint8_t passos = 0;
    while (agora - ultimoQuadro >= Hyperspace::duration(quadroFundo) &&
           passos < Hyperspace::FRAME_COUNT) {
      ultimoQuadro += Hyperspace::duration(quadroFundo);
      quadroFundo = (quadroFundo + 1) % Hyperspace::FRAME_COUNT;
      avancou = true;
      ++passos;
    }
    if (passos == Hyperspace::FRAME_COUNT) ultimoQuadro = agora;
    if (avancou) desenharContador();
  } else {
    // Pause while menus/confirmation are open; no catch-up burst on return.
    ultimoQuadro = agora;
  }
  if (telaSuja) {
    if (canvasDisponivel) canvas.flush();
    telaSuja = false;
  }

  delay(25);
}
