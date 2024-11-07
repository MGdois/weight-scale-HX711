// INCLUSÃO DE BIBLIOTECAS
#include <HX711.h>
#include <PushButton.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// DEFINIÇÕES DE PINOS
#define pinDT 15
#define pinSCK 4
#define pinBotao 16

// DEFINIÇÕES
#define pesoMin 0.010
#define pesoMax 1.0
#define endereco  0x27 
#define colunas   16
#define linhas    2
#define escala 391975.16f

// INSTANCIANDO OBJETOS
HX711 scale;
PushButton botao(pinBotao);
LiquidCrystal_I2C lcd(endereco, colunas, linhas);

// DECLARAÇÃO DE VARIÁVEIS
float medida = 0;

void setup() {
  Serial.begin(115200);

  lcd.init(); // INICIA A COMUNICAÇÃO COM O DISPLAY
  lcd.backlight(); // LIGA A ILUMINAÇÃO DO DISPLAY
  lcd.clear(); // LIMPA O DISPLAY
  
  scale.begin(pinDT, pinSCK);  // CONFIGURANDO OS PINOS DA BALANÇA
  scale.set_scale(escala);     // ENVIANDO O VALOR DA ESCALA CALIBRADO

  delay(2000);
  scale.tare();  // ZERANDO A BALANÇA PARA DESCONSIDERAR A MASSA DA ESTRUTURA
  Serial.println("Setup Finalizado!");
  lcd.print("Setup");
  lcd.setCursor(0,1);
  lcd.print("Finalizado");
  delay(2000);
  lcd.clear(); // LIMPA O DISPLAY
}

void loop() {
    
  botao.button_loop();  // LEITURA DO BOTAO
  
  if (botao.pressed()) {  // QUANDO PRESSIONAR O BOTÃO

    lcd.clear();
    scale.power_up();  // LIGANDO O SENSOR
    medida = scale.get_units(5);  // SALVANDO NA VARIAVEL O VALOR DA MÉDIA DE 5 MEDIDAS

    if (medida <= pesoMin) {  // CONFERE SE A MASSA ESTÁ NA FAIXA VÁLIDA
      scale.tare();           // ZERA A BALANÇA CASO A MASSA SEJA MENOR QUE O VALOR MIN
      medida = 0;
      Serial.println("Tara Configurada!");
      Serial.println(String(medida, 3) + " kg");
      lcd.print("Tara ");
      lcd.setCursor(0,1);
      lcd.print("Configurada ");
      delay(1000);
      lcd.clear();  
      
    } else{
      scale.tare();  // ZERA A BALANÇA 
      medida = 0;
      Serial.println("Tara Configurada!");
      Serial.println(String(medida, 3) + " kg");
      lcd.print("Tara ");
      lcd.setCursor(0,1);
      lcd.print("Configurada ");
      delay(1000);
      lcd.clear(); 

    }
    
  } else{

    scale.power_up();  // Certifique-se de que o sensor está ligado
    medida = scale.get_units(2);  // Atualiza a medida constantemente
    
    int porcentagem = (medida / pesoMax) * 100;  // Convertendo a medida em porcentagem
    
    if (medida <= pesoMin) {  // CONFERE SE A MASSA ESTÁ NA FAIXA VÁLIDA
      scale.tare();           // ZERA A BALANÇA CASO A MASSA SEJA MENOR QUE O VALOR MIN
      porcentagem = 0;
        lcd.setCursor(2, 0);
        lcd.print("Porcentagem: ");
        lcd.setCursor(5, 1);
        lcd.print(String(porcentagem) + " % ");  // Exibe a porcentagem
       
        Serial.println("Porcentagem 0% ");
        
        //lcd.print(String(medida, 3) + " kg");
        Serial.println(String(medida, 3) + " kg");
    }else if (medida >= pesoMax) { 
      porcentagem = 100;
      lcd.setCursor(2, 0);
      lcd.print("Porcentagem: ");
      lcd.setCursor(5, 1);
      lcd.print(String(porcentagem) + " % ");  // Exibe a porcentagem
       
        Serial.println("Porcentagem 0% ");
      Serial.println(String(medida, 3) + " kg");
    } else{
      lcd.setCursor(2, 0);
      lcd.print("Porcentagem: ");  // Exibe a porcentagem
      lcd.setCursor(5, 1);
      lcd.print(String(porcentagem) + " % ");  // Exibe a porcentagem
      Serial.println("Porcentagem: " + String(porcentagem) + " % ");  // Exibe a porcentagem
      //lcd.print(String(medida, 3) + " kg");
      Serial.println(String(medida, 3) + " kg");
    }
  
    delay(1000); // Atualiza a medida a cada segundo (pode ajustar conforme necessário) 
  }
    
    
}

// IMPLEMENTO DE FUNÇÕES
