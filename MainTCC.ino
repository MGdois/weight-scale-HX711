// INCLUSÃO DE BIBLIOTECAS
#include <HX711.h>
#include <PushButton.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "index.h"  //Web page header file

// DEFINIÇÕES DE PINOS
#define pinDT 4
#define pinSCK 15
#define pinBotao 16
#define pinGas 34  // PINO ANALÓGICO

// DEFINIÇÕES DE LIMITES
#define pesoMin 0.010
#define pesoMax 1.00
#define gasMax 300  // MÁXIMO DE GÁS PERMITIDO

//DEFINIÇÕES DO LCD
#define endereco 0x27
#define colunas 16
#define linhas 2
#define escala 372580

//===============================================================
//COMUNICAÇÃO WEB

//Definindo webserver
WebServer server(80);

//Enter your SSID and PASSWORD
const char* ssid     = "Redmi Note 8";
const char* password = "mg123456";

//===============================================================


// INSTANCIANDO OBJETOS
HX711 scale;
PushButton botao(pinBotao);
LiquidCrystal_I2C lcd(endereco, colunas, linhas);

// DECLARAÇÃO DE VARIÁVEIS
float medida = 0;
int aceso = 0;  //Variável global de controle de estado
//bool vazamentoDetectado = false;
String mensagemCliente = "Sistema Inicializado"; // Mensagem padrão para o cliente
String novaMensagem = "Verificando"; // Mensagem padrão para o cliente


void handleRoot() {
 String s = MAIN_page; //Lê HTML contents
 server.send(200, "text/html", s); //Manda para pagina na web 
}


void handleNovaMensagem() {
    server.send(200, "text/plain", novaMensagem); // Envia o valor da nova variável
}

void handleADC() {
    server.send(200, "text/plain", mensagemCliente); // Envia a mensagem ao cliente
}

void exibirMensagem(String linha1, String linha2) {
  lcd.clear();          // Limpa o display
  lcd.setCursor(0, 0);  // Posiciona o cursor na primeira linha
  lcd.print(linha1);    // Exibe a primeira linha
  lcd.setCursor(0, 1);  // Posiciona o cursor na segunda linha
  lcd.print(linha2);    // Exibe a segunda linha

  // Envia a mensagem para a Serial
  Serial.println(linha1);
  Serial.println(linha2);
}

void exibirPorcentagem(String linha1, String linha2) {
  lcd.clear();          // Limpa o display
  lcd.setCursor(2, 0);  // Posiciona o cursor na primeira linha
  lcd.print(linha1);    // Exibe a primeira linha
  lcd.setCursor(4, 1);  // Posiciona o cursor na segunda linha
  lcd.print(linha2);    // Exibe a segunda linha
}

void setup() {
  Serial.begin(115200);
  Serial.println("Carregando Sketch...");

//Conectando WIFI 
  WiFi.mode(WIFI_STA); //Connectto your wifi
  WiFi.begin(ssid, password);

  Serial.println("Connecting to ");
  Serial.print(ssid);
  
  while(WiFi.waitForConnectResult() != WL_CONNECTED){ //Wait for WiFi to connect 
      Serial.print(".");
      delay(500);
    }
    
//Se conectado aparece o endereço de IP no monitor serial
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  mensagemCliente;
  server.on("/", handleRoot);      //This is display page
  server.on("/readADC", handleADC);//To get update of ADC Value only
  server.on("/readNovaMensagem", handleNovaMensagem); // Nova rota
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");

//Iniciando DISPLAY LCD
  lcd.init();       // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight();  // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear();      // LIMPA O DISPLAY

  pinMode(pinGas, INPUT);
  scale.begin(pinDT, pinSCK);  // CONFIGURANDO OS PINOS DA BALANÇA
  scale.set_scale(escala);     // ENVIANDO O VALOR DA ESCALA CALIBRADO
  delay(2000);
  scale.tare();  // ZERANDO A BALANÇA PARA DESCONSIDERAR A MASSA DA ESTRUTURA

  exibirMensagem("Setup", "Finalizado");
  delay(2000);
  lcd.clear();  // LIMPA O DISPLAY
}

void loop() {
  server.handleClient();

  int ValorG = analogRead(pinGas);

  if (ValorG >= gasMax) {  // Caso o valor de gás esteja acima do permitido
            aceso = 1;
            exibirMensagem(" - VAZAMENTO -", " - DETECTADO - ");
            novaMensagem = "VAZAMENTO DETECTADO";
            delay(500);
            Serial.println("VAZAMENTO DETECTADO: " + String(ValorG));
        
        delay(1500);
    } else if (ValorG < gasMax && aceso == 1) {
        aceso = 0;
        exibirMensagem("VAZAMENTO", "CONTROLADO");
        novaMensagem = "VAZAMENTO CONTROLADO";
        delay(2000);
        novaMensagem = "...";
        Serial.println("Sistema Normal: " + String(ValorG));
        delay(1000);
        lcd.clear();
    } else {

      scale.power_up();// Certifique-se de que o sensor está ligado
      delay(500);          
      medida = scale.get_units(7);  // Atualiza a medida constantemente
    
      int porcentagem = (medida - pesoMin / pesoMax) * 100;  // Convertendo a medida em porcentagem

      if (medida <= pesoMin) {  // CONFERE SE A MASSA ESTÁ NA FAIXA VÁLIDA
        scale.tare();           // ZERA A BALANÇA CASO A MASSA SEJA MENOR QUE O VALOR MIN
        porcentagem = 0;
        exibirPorcentagem("Porcentagem:", String(porcentagem) + " % ");
        mensagemCliente = "Porcentagem: 0% ";
        Serial.println("Porcentagem 0% ");
        Serial.println(String(medida, 3) + " kg");

      } else if (medida >= pesoMax) {
        porcentagem = 100;
        exibirPorcentagem("Porcentagem:", String(porcentagem) + " % " );
        mensagemCliente = "Porcentagem: 100% " + String(porcentagem) + " % " ;
        Serial.println("Porcentagem 0% ");
        Serial.println(String(medida, 3) + " kg");

      } else {
        exibirPorcentagem("Porcentagem:", String(porcentagem) + " % ");
        mensagemCliente = "Porcentagem: " + String(porcentagem) + " % ";
        Serial.println("Porcentagem: " + String(porcentagem) + " % ");  // Exibe a porcentagem
        Serial.println(String(medida - pesoMin, 3) + " kg");
      }

      delay(1000);  // Atualiza a medida a cada segundo (pode ajustar conforme necessário)
    }
  }
