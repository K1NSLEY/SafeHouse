#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "config.h" // Arquivo de configuração e credenciais

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, ADAFRUIT_AIO_USERNAME, ADAFRUIT_AIO_KEY);
Adafruit_MQTT_Publish feed_esp32 = Adafruit_MQTT_Publish(&mqtt, ADAFRUIT_AIO_USERNAME "/feeds/esp32");

// --- INFORMAÇÃO CRIADA: Feed de inscrição para receber comandos MQTT (Criado em 27/05/2026) ---
Adafruit_MQTT_Subscribe sub_esp32 = Adafruit_MQTT_Subscribe(&mqtt, ADAFRUIT_AIO_USERNAME "/feeds/esp32");
// ---------------------------------------------------------------------------------------------

// Constantes e Limiares

// Instância do RFID
MFRC522 mfrc522(PINO_RFID_SS, PINO_RFID_RST);

// Variáveis de Estado
bool alarmeAtivado = false;
unsigned long ultimoTempoRFID = 0;
int estadoAnteriorBotao = HIGH;
int distanciaInicial = 0;
bool distanciaCalibrada = false;
bool sireneTravada = false;

// UID do cartão autorizado (definido em config.h)
byte cartaoAutorizado[4] = {RFID_CARD_UID_0, RFID_CARD_UID_1, RFID_CARD_UID_2, RFID_CARD_UID_3};

// Função para garantir a conexão MQTT
void conectarMQTT() {
  if (mqtt.connected()) return;
  Serial.print("Conectando ao MQTT... ");
  while (mqtt.connect() != 0) {
    Serial.println("Falha. Tentando novamente em 5s...");
    mqtt.disconnect();
    delay(5000);
  }
  Serial.println("MQTT Conectado!");
}

// --- INFORMAÇÃO CRIADA: Função centralizada para alterar o estado do alarme (Criado em 27/05/2026) ---
void definirEstadoAlarme(bool ativar) {
  if (alarmeAtivado == ativar) return; // Ignora se o estado já for o desejado

  alarmeAtivado = ativar;
  distanciaCalibrada = false;
  sireneTravada = false;
  estadoAnteriorBotao = digitalRead(PINO_BOTAO); // Atualiza a janela para não disparar logo ao ligar
  
  Serial.print("Alarme: ");
  Serial.println(alarmeAtivado ? "ATIVADO" : "DESATIVADO");
  
  if(alarmeAtivado) {
     feed_esp32.publish("Status Alarme: ATIVADO");
  } else {
     feed_esp32.publish("Status Alarme: DESATIVADO");
  }
  
  digitalWrite(PINO_BUZZER, HIGH);
  delay(500);
  digitalWrite(PINO_BUZZER, LOW);
}
// ----------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
  
  pinMode(PINO_LDR_LUZ, INPUT);
  pinMode(PINO_LDR_IR, INPUT);
  pinMode(PINO_TRIG, OUTPUT);
  pinMode(PINO_ECHO, INPUT);
  pinMode(PINO_BUZZER, OUTPUT);
  pinMode(PINO_RELE, OUTPUT);
  pinMode(PINO_BOTAO, INPUT_PULLUP); // Botão com resistor interno
  
  digitalWrite(PINO_RELE, LOW);

  // --- INFORMAÇÃO CRIADA: LÓGICA DE CONEXÃO WI-FI ---
  Serial.print("Conectando ao WiFi");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  // --------------------------------------------------

  // --- INFORMAÇÃO CRIADA: Ativação da escuta do feed MQTT (Criado em 27/05/2026) ---
  mqtt.subscribe(&sub_esp32);
  // ---------------------------------------------------------------------------------

  conectarMQTT();
  
  Serial.println("Sistema Iniciado.");
  feed_esp32.publish("Sistema Iniciado"); // Envio MQTT
}

void loop() {
  conectarMQTT(); // Mantém o MQTT conectado

  // --- INFORMAÇÃO CRIADA: Leitura de mensagens MQTT recebidas no loop e controle do relé (Criado e atualizado em 27/05/2026) ---
Adafruit_MQTT_Subscribe *subscription;
  // Timeout de 10ms para não travar a execução dos sensores locais
  while ((subscription = mqtt.readSubscription(10))) {
    if (subscription == &sub_esp32) {
      String comando = (char *)sub_esp32.lastread;
      if (comando == "Ativar Alarme remoto") {
        definirEstadoAlarme(true);
      } else if (comando == "Desativar Alarme remoto") {
        definirEstadoAlarme(false);
      } else if (comando == "ON") {
        digitalWrite(PINO_RELE, HIGH);
        Serial.println("Comando MQTT: Relé LIGADO");

        feed_esp32.publish("Altera\u00e7\u00e3o Luz: Ligada"); // Envio MQTT
      } else if (comando == "OFF") {
        digitalWrite(PINO_RELE, LOW);
        Serial.println("Comando MQTT: Relé DESLIGADO");
        
        feed_esp32.publish("Altera\u00e7\u00e3o Luz: Desligada"); // Envio MQTT
      } else if (comando == "Ping") {
  Serial.println("Comando MQTT: Ping recebido. Enviando status...");

  // Status do fogo
  if (analogRead(PINO_LDR_IR) < LIMIAR_LUZ) {
    feed_esp32.publish("Alerta: Fogo detectado!");
  } else {
    feed_esp32.publish("Status: Fogo não detectado");
  }
  delay(100);

  // Status da luz
  if (digitalRead(PINO_RELE) == HIGH) {
    feed_esp32.publish("Alteração Luz: Ligada");
  } else {
    feed_esp32.publish("Alteração Luz: Desligada");
  }
  delay(100);

  // Status do alarme
  if (alarmeAtivado) {
    feed_esp32.publish("Status Alarme: ATIVADO");
  } else {
    feed_esp32.publish("Status Alarme: DESATIVADO");
  }
}
    }
  }
  // -----------------------------------------------------------------------------------------------------------------------------

  gerenciarLDRLuz();
  gerenciarLDRInfravermelho();
  gerenciarRFID();
  
  // A janela e o movimento só são monitorados (e publicados) se o alarme estiver ativo
  if (alarmeAtivado) {
    monitorarJanelaMovimento();
  }
}

void gerenciarLDRLuz() {
  int nivelLuz = analogRead(PINO_LDR_LUZ);
  static int estadoAnteriorLDRLuz = -1;
  int estadoAtual = (nivelLuz < LIMIAR_LUZ) ? 1 : 0;

  if (estadoAtual != estadoAnteriorLDRLuz) { // Verifica se o estado mudou
    if (estadoAtual == 1) { // Recebendo luz
      digitalWrite(PINO_RELE, HIGH);
      Serial.println("Luz desligada");
      feed_esp32.publish("Alteração Luz: Desligada"); // Envio MQTT
    } else { // Não recebendo luz
      digitalWrite(PINO_RELE, LOW);
      Serial.println("Luz ligada");
      feed_esp32.publish("Alteração Luz: Ligada"); // Envio MQTT
    }
    estadoAnteriorLDRLuz = estadoAtual; // Atualiza a memória do estado
  }
}

// 2. LDR Infravermelho (Buzzer Intermitente)
void gerenciarLDRInfravermelho() {
  int nivelIR = analogRead(PINO_LDR_IR);
  static int estadoAnteriorIR = -1;
  int estadoAtual = (nivelIR < LIMIAR_LUZ) ? 1 : 0;

  // Verifica se o estado mudou para imprimir e enviar
  if (estadoAtual != estadoAnteriorIR) { 
    if (estadoAtual == 1) { // Recebendo IR / Fogo detectado
      Serial.println("Fogo detectado");
      feed_esp32.publish("Alerta: Fogo detectado!"); // Envio MQTT
    } else { // Não recebendo IR
      Serial.println("Não há fogo detectado");
      feed_esp32.publish("Status: Fogo não detectado"); // Envio MQTT
    }
    estadoAnteriorIR = estadoAtual; // Atualiza a memória do estado
  }

  // Mantém o buzzer intermitente enquanto estiver recebendo IR
  if (estadoAtual == 1) { 
    digitalWrite(PINO_BUZZER, HIGH);
    delay(100);
    digitalWrite(PINO_BUZZER, LOW);
    delay(100);
  }
}

// 3. RFID (Ativa Alarme com delay/filtro)
void gerenciarRFID() {
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Filtro de tempo (3 segundos entre leituras)
  if (millis() - ultimoTempoRFID < 3000) {
    return; 
  }
  ultimoTempoRFID = millis();

  // Verifica se o cartão é o autorizado
  bool cartaoValido = true;
  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != cartaoAutorizado[i]) {
      cartaoValido = false;
      break;
    }
  }

  if (cartaoValido) {
    // --- INFORMAÇÃO ALTERADA: Substituição da lógica original pela função centralizada (Criado em 27/05/2026) ---
    definirEstadoAlarme(!alarmeAtivado);
    // ------------------------------------------------------------------------------------------------------------
  }
}

// 4. Janela e Movimento (Se alarme ativado)
void monitorarJanelaMovimento() {
  // Se houve violação, trava o código disparando a sirene continuamente
  if (sireneTravada) {
    dispararSirene("Alarme em disparo contínuo!");
    return;
  }

  // Verificação da Janela (Fim de curso)
  int estadoAtualBotao = digitalRead(PINO_BOTAO);
  if (estadoAtualBotao != estadoAnteriorBotao) { // Pressionado ou Aliviado
    sireneTravada = true;
    estadoAnteriorBotao = estadoAtualBotao;
    feed_esp32.publish("Alerta: Alteração na Janela!"); // Envio MQTT
    dispararSirene("Alteração na Janela!");
    return;
  }
  estadoAnteriorBotao = estadoAtualBotao;

  // Verificação de Movimento (Ultrassom)
  digitalWrite(PINO_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PINO_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PINO_TRIG, LOW);
  
  long duracao = pulseIn(PINO_ECHO, HIGH);
  int distancia = duracao * 0.034 / 2;

  if (distancia <= 0 || distancia > 400) return; // Ignora leituras de erro do sensor

  // Calibração: pega a primeira leitura após ativar o alarme
  if (!distanciaCalibrada) {
    distanciaInicial = distancia;
    distanciaCalibrada = true;
    Serial.print("Ambiente calibrado. Distância inicial: ");
    Serial.print(distanciaInicial);
    Serial.println(" cm");
    return;
  }

  // Verifica se a distância atual variou além da tolerância permitida
  if (abs(distancia - distanciaInicial) > TOLERANCIA_DISTANCIA) {
    sireneTravada = true;
    feed_esp32.publish("Alerta: Movimento Detectado!"); // Envio MQTT
    dispararSirene("Movimento Detectado (Alteração no ambiente)!");
  }
}

void dispararSirene(const char* motivo) {
  Serial.println(motivo);
  for (int i = 0; i < 5; i++) { // Toca alarme 5 vezes rápido
    digitalWrite(PINO_BUZZER, HIGH);
    delay(200);
    digitalWrite(PINO_BUZZER, LOW);
    delay(200);
  }
}