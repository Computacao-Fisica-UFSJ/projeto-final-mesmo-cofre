// INCLUSÃO DE BIBLIOTECAS
#include <HX711.h>
#include <Servo.h>

// DEFINIÇÕES DE PINOS
#define pinDT  2
#define pinSCK  3
#define pinMotor1 8
#define pinMotor2 9
#define DEBUG true

int estado = 0;
int pos = 0;
float pesoDesejado = 0.06;

// INSTANCIANDO OBJETOS
HX711 scale;

// DECLARAÇÃO DE VARIÁVEIS
float medida = 0;
Servo agitadorServo;

void setup() {
  Serial.begin(57600);
  Serial1.begin(115200); // Comunicação com ESP8266 (baud rate padrão AT)

  // Reseta o módulo
  sendData("AT+RST\r\n", 2000, DEBUG);
  delay(3000);
  // Conecta ao WiFi
  sendData("AT+CWJAP=\"WIFI_UFSJ\",\"\"\r\n", 2000, DEBUG);
  delay(5000);
  // Configura como Station
  sendData("AT+CWMODE=1\r\n", 1000, DEBUG);
  // Mostra IP atribuído
  sendData("AT+CIFSR\r\n", 1000, DEBUG);
  // Habilita múltiplas conexões
  sendData("AT+CIPMUX=1\r\n", 1000, DEBUG);
  // Inicia servidor na porta 80
  sendData("AT+CIPSERVER=1,80\r\n", 1000, DEBUG);
  
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  scale.begin(pinDT, pinSCK); // CONFIGURANDO OS PINOS DA BALANÇA
  scale.set_scale(434341); // LIMPANDO O VALOR DA ESCALA

  delay(2000);
  scale.tare(); // ZERANDO A BALANÇA PARA DESCONSIDERAR A MASSA DA ESTRUTURA

  Serial.println("Balança Zerada");

  estado = 0;
}

void loop() {
  scale.power_down();
    
  if(Serial1.available()){
    if(Serial1.find("+IPD,")){ // "+IPD" indica que chegou uma requisição HTTP
      delay(300);
      //int connectionId = Serial1.read() - 48; // Pega o ID da conexão (char -> int)
      int connectionId = Serial1.read() - '0';
      
      // Limpa o buffer até encontrar os dados da requisição
      while(Serial1.available() && Serial1.read() != ':') {}

      // Lê a linha da requisição HTTP
      String request = Serial1.readStringUntil('\r');
      Serial.println("Requisicao: " + request);

      if (request.indexOf("GET /start") >= 0) {
          Serial.println("Botão pressionado via WiFi!");
          estado = 1;
      }

      // ----------- SETAR PESO VIA HTML -----------
      if (request.startsWith("GET /setpeso")) {
      
        int idx = request.indexOf("valor=");
        if (idx > 0) {
      
          int endIdx = request.indexOf(' ', idx);
          if (endIdx < 0) endIdx = request.length();
      
          String valorStr = request.substring(idx + 6, endIdx);
      
          pesoDesejado = valorStr.toFloat();
      
          Serial.print("Peso desejado atualizado para: ");
          Serial.println(pesoDesejado, 3);
        }
      }
      
      // ------------------ RESPOSTA HTML ------------------
      String response = "HTTP/1.1 200 OK\r\n";
      response += "Content-Type: text/html\r\n";
      response += "Connection: close\r\n\r\n";
      
      response += "<!DOCTYPE html><html lang='pt-BR'>";
      response += "<head>";
      response += "<meta charset='UTF-8'>";
      response += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
      
      response += "<style>";
      response += "body{"
                  "margin:0;"
                  "font-family: 'Segoe UI', Arial, sans-serif;"
                  "background: linear-gradient(135deg, #f5f7fa, #c3cfe2);"
                  "display:flex;"
                  "justify-content:center;"
                  "align-items:center;"
                  "height:100vh;"
                  "}";
      response += ".card{"
                  "background:#fff;"
                  "padding:30px;"
                  "border-radius:15px;"
                  "box-shadow:0 10px 30px rgba(0,0,0,0.15);"
                  "max-width:400px;"
                  "width:100%;"
                  "text-align:center;"
                  "}";
      response += "h1{"
                  "margin-bottom:10px;"
                  "color:#333;"
                  "}";
      response += "p{"
                  "font-size:18px;"
                  "color:#555;"
                  "}";
      response += "input[type=number]{"
                  "width:100%;"
                  "padding:12px;"
                  "font-size:20px;"
                  "margin-top:10px;"
                  "border-radius:8px;"
                  "border:1px solid #ccc;"
                  "}";
      response += "input[type=submit]{"
                  "width:100%;"
                  "margin-top:15px;"
                  "padding:15px;"
                  "font-size:20px;"
                  "background:#4CAF50;"
                  "color:white;"
                  "border:none;"
                  "border-radius:10px;"
                  "cursor:pointer;"
                  "}";
      response += "input[type=submit]:hover{background:#45a049;}";
      response += "button{"
                  "width:100%;"
                  "margin-top:25px;"
                  "padding:20px;"
                  "font-size:26px;"
                  "background:#2196F3;"
                  "color:white;"
                  "border:none;"
                  "border-radius:12px;"
                  "cursor:pointer;"
                  "}";
      response += "button:hover{background:#1e88e5;}";
      response += "</style>";
      
      response += "</head><body>";
      
      response += "<div class='card'>";
      response += "<h1>🐾 Yummi! Alimentador</h1>";
      
      response += "<p>Peso desejado atual:</p>";
      response += "<p><b>";
      response += String(pesoDesejado, 3);
      response += " kg</b></p>";
      
      response += "<form action='/setpeso' method='GET'>";
      response += "<label>Definir peso (kg)</label>";
      response += "<input type='number' step='0.01' name='valor' required>";
      response += "<input type='submit' value='Salvar Peso'>";
      response += "</form>";
      
      response += "<button onclick=\"location.href='/start'\">Iniciar Alimentação</button>";
      
      response += "</div>";
      
      response += "</body></html>";

      // Envia página ao cliente via ESP8266
      String cipSend = "AT+CIPSEND=" + String(connectionId) + "," + String(response.length()) + "\r\n";
      sendData(cipSend, 1000, DEBUG);
      sendData(response, 1000, DEBUG);

      // Fecha a conexão
      sendData("AT+CIPCLOSE=" + String(connectionId) + "\r\n", 3000, DEBUG);
    }
  }

  if(estado == 1){
     Serial.println("Começou!");
     agitarRacao();
     estado = 2;
  }

  
  if(estado == 2){
    servirRacao();
  }
  
}

void agitarRacao(){
  // Stir servo section    
   agitadorServo.attach(4);    
   for(pos = 0; pos <= 180; pos += 1) {                                  
    agitadorServo.write(pos);              
    delay(5);   
   } 
   for(pos = 180; pos>=0; pos-=1)  {                                
    agitadorServo.write(pos);          
    delay(10);   
   } 
   delay(200);
   for(pos = 0; pos <= 180; pos += 1) {                                  
    agitadorServo.write(pos);              
    delay(10);
   } 
   for(pos = 180; pos>=0; pos-=1)  {                                
    agitadorServo.write(pos);          
    delay(5);
   }  
   agitadorServo.detach();  
}

void servirRacao(){
  Serial.println("Entrei!");
  scale.power_up();
  medida = scale.get_units(5); // SALVANDO NA VARIAVEL O VALOR DA MÉDIA DE 5 MEDIDAS
  Serial.println(medida, 3); // ENVIANDO PARA MONITOR SERIAL A MEDIDA COM 3 CASAS DECIMAIS

  digitalWrite(pinMotor1, HIGH);
  digitalWrite(pinMotor2, LOW);
  
  scale.power_down(); // DESLIGANDO O SENSOR
  delay(1000); // AGUARDA 5 SEGUNDOS
  scale.power_up(); // LIGANDO O SENSOR  

  if(medida >= pesoDesejado){
    digitalWrite(pinMotor1, LOW);
    digitalWrite(pinMotor2, LOW);
    estado = 0;
  }
}

// Função que envia comandos AT ao ESP8266 e retorna resposta
String sendData(String command, const int timeout, boolean debug){
  String response = "";
  Serial1.print(command);
  long int time = millis();
  while ((time + timeout) > millis()){
    while (Serial1.available()){
      char c = Serial1.read();
      response += c;
    }
  }
  if(debug){
    Serial.println(response); // Mostra resposta do ESP8266
  }
  return response;
}
