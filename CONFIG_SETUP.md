# SafeHouse - Sistema de Configuração

## 📁 Estrutura de Configuração

Este projeto usa um sistema similar ao `.env` para gerenciar credenciais e configurações, adaptado para Arduino IDE.

### Arquivos:

- **`config.h`** - Arquivo com suas credenciais (NÃO fazer commit!)
- **`config.h.example`** - Template para criar o `config.h`
- **`SafeHouse.ino`** - Sketch principal (inclui automaticamente `config.h`)

## 🔧 Como Usar

### 1️⃣ Primeira Configuração

```bash
# Copie o arquivo de exemplo
cp config.h.example config.h
```

### 2️⃣ Edite `config.h` com suas credenciais

Abra `config.h` e substitua os valores:

```cpp
#define WLAN_SSID       "SEU_WIFI"
#define WLAN_PASS       "SUA_SENHA"
#define ADAFRUIT_AIO_USERNAME "seu_usuario"
#define ADAFRUIT_AIO_KEY      "sua_chave_api"
```

### 3️⃣ Compile e Faça Upload Normalmente

Na Arduino IDE, abra `SafeHouse.ino` e faça upload como de costume. O arquivo `config.h` será automaticamente incluído.

## 🔐 Segurança

- ⚠️ **NUNCA** faça commit do arquivo `config.h`
- O `.gitignore` já está configurado para ignorar `config.h`
- Compartilhe apenas `config.h.example` com seus colaboradores
- Cada pessoa deve criar seu próprio `config.h` com suas credenciais

## 📝 Adicionar Novas Configurações

Se precisar adicionar uma nova configuração:

1. Adicione a `#define` em `config.h.example`
2. Adicione em `config.h`
3. Use no código normalmente

**Exemplo:**

```cpp
// Em config.h
#define NOVA_CONFIGURACAO 100

// No .ino
int valor = NOVA_CONFIGURACAO;
```

## ✅ Vantagens

- ✨ Separação clara entre código e configuração
- 🔒 Credenciais protegidas (não expostas no Git)
- 📦 Fácil compartilhamento do código sem expor dados sensíveis
- 🔄 Mesma filosofia do `.env` em projetos Node.js/Python
