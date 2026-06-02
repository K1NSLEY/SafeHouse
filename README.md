# 🏠 SafeHouse - Sistema de Segurança Inteligente

> **Projeto acadêmico para a disciplina de Sistemas Embarcados**  
> UniFECAF - Centro Universitário FECAF

---

## 📚 Contexto do Projeto

Este projeto foi desenvolvido como trabalho prático para a faculdade **UniFECAF**, com objetivo de aplicar conceitos de:

- **Sistemas Embarcados** com IoT
- **Comunicação MQTT** e integração com nuvem
- **Integração de Sensores** (ultrassom, luz infravermelha, LDR)
- **Identificação por RFID**
- **Conectividade WiFi** com ESP32

---

## 🎯 O Que é SafeHouse?

SafeHouse é um **sistema de segurança residencial inteligente** que monitora e protege ambientes através de sensores integrados e controle remoto via WiFi.

### Funcionalidades Principais:

✅ **Ativação/Desativação de Alarme** via cartão RFID  
✅ **Monitoramento de Movimento** com sensor ultrassônico  
✅ **Detecção de Incêndio** com sensor LDR infravermelha  
✅ **Controle de Iluminação** automático e remoto  
✅ **Alertas em Tempo Real** via MQTT para Adafruit IO  
✅ **Controle Remoto** via WiFi (Adafruit IO Dashboard)  
✅ **Botão de Pânico** para ativação de alarme  

---

## 🔧 Componentes Utilizados

| Componente | Especificação | Função |
|-----------|--------------|--------|
| **Microcontrolador** | ESP32 | Processamento e conectividade |
| **Leitor RFID** | MFRC522 | Autenticação com cartão |
| **Sensor de Distância** | HC-SR04 (Ultrassônico) | Detecção de movimento |
| **Sensor LDR** | 2x LDR + Resistor | Luz ambiente + Detecção de fogo |
| **Buzzer** | Ativo/Passivo | Sirene de alarme |
| **Relé** | 5V/10A | Controle de iluminação |
| **Botão** | Switch momentâneo | Ativador de alarme |
| **Protocolo** | MQTT | Comunicação com nuvem |
| **Plataforma IoT** | Adafruit IO | Dashboard remoto |

---

## 📊 Arquitetura

```
┌─────────────┐
│    ESP32    │ ◄─────────► WiFi ◄─────────► Adafruit IO
│ (Firmware)  │                               (Dashboard)
└─────────────┘
      ▲
      │
  Sensores Conectados:
  ├─ MFRC522 (RFID)
  ├─ HC-SR04 (Ultrassônico)
  ├─ LDR (Luz)
  ├─ Buzzer
  ├─ Relé
  └─ Botão
```

---

## 🚀 Como Começar

### Pré-requisitos

- **Arduino IDE** versão 1.8.x ou superior
- **Bibliotecas necessárias:**
  - `MFRC522` (para RFID)
  - `WiFi` (integrada no ESP32)
  - `Adafruit MQTT Client`
  - `SPI` (integrada)

### 1️⃣ Instalação

```bash
# Clone ou baixe o repositório
git clone <seu_repositorio>
cd SafeHouse
```

### 2️⃣ Configuração das Credenciais

```bash
# Copie o template de configuração
cp SafeHouse/config.h.example SafeHouse/config.h
```

Edite `SafeHouse/config.h` com suas informações:

```cpp
#define WLAN_SSID       "Seu_WiFi"
#define WLAN_PASS       "Sua_Senha"
#define ADAFRUIT_AIO_USERNAME "seu_usuario"
#define ADAFRUIT_AIO_KEY      "sua_chave_api"
```

### 3️⃣ Upload no ESP32

1. Abra `SafeHouse/SafeHouse.ino` na Arduino IDE
2. Selecione a placa: **ESP32 Dev Module**
3. Configure a porta COM
4. Clique em **Upload**

### 4️⃣ Acesso ao Dashboard

Acesse o Adafruit IO em https://io.adafruit.com com suas credenciais para visualizar e controlar o sistema remotamente.

---

## 📋 Estrutura do Projeto

```
SafeHouse/
├── SafeHouse.ino              # Firmware principal
├── config.h                   # Configurações (Git ignorado)
├── config.h.example           # Template de configuração
├── README.md                  # Este arquivo
├── CONFIG_SETUP.md            # Guia de configuração
└── .gitignore                 # Protege credenciais
```

---

## 🔐 Segurança

- ⚠️ **Credenciais protegidas**: O arquivo `config.h` não é versionado (`.gitignore`)
- 🔒 **RFID**: Apenas cartões autorizados ativam o sistema
- 📡 **Comunicação criptografada**: MQTT com TLS opcional
- 🔑 **Chaves de API**: Armazenadas localmente, não expostas

---

## 📱 Controle Remoto

### Via Adafruit IO Dashboard

- **Ativar/Desativar Alarme** remotamente
- **Ligar/Desligar Luz**
- **Receber Alertas** de movimento e incêndio
- **Status em Tempo Real** do sistema

### Via Cartão RFID

- Aproxime o cartão do leitor para ativar/desativar o alarme

### Via Botão Físico

- Pressione o botão para ativação rápida

---

## 🛠️ Troubleshooting

| Problema | Solução |
|---------|---------|
| WiFi não conecta | Verifique SSID/senha em `config.h` |
| MQTT desconecta | Confirme credenciais Adafruit IO |
| Sensor não funciona | Verifique pinagem em `config.h` |
| RFID não lê cartão | Calibre a distância do leitor |

---

## 👥 Equipe

| Papel | Responsável |
|------|------------|
| Desenvolvimento | [Seus Nomes] | [Seus Nomes] |
| Hardware | [Seus Nomes] |
| Documentação | [Seus Nomes] |
| Apresentação | UniFECAF - [Data] |

---

## 📝 Licença

Este projeto é fornecido como trabalho acadêmico para fins educacionais.

---

## 📞 Suporte

Para dúvidas ou problemas:
- Consulte [CONFIG_SETUP.md](CONFIG_SETUP.md)
- Verifique a seção Troubleshooting
- Revisite a documentação das bibliotecas usadas

---

**Desenvolvido com ❤️ para UniFECAF**  
*Última atualização: Junho de 2026*
