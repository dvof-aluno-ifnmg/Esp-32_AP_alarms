#include <WiFi.h>       // Inclusão da biblioteca WiFi para gerenciar a conexão Wi-Fi
#include <WebServer.h>  // Inclusão da biblioteca WebServer para criar um servidor web
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <WiFiManager.h>
#include <ESPmDNS.h>

MPU6050 mpu(Wire);
WebServer server(80);  // Criação de um objeto WebServer na porta 80
float tempo;
String htmltext(float var1, float var2, float var3, float var4, float var5) {  //Variável que armazenará o script HTML
  String html = "<!DOCTYPE html>\n";
  html += "<html lang=\"pt-br\">\n";

  html += "<head>\n";
  html += "<meta charset=\"UTF-8\">\n";
  html += "<title>Dados dos sensores</title>\n";
  html += "<style>\n";
  html += "html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  html += "body{margin-top: 50px;} \n";
  html += "h1 {color: #444444; margin: 50px auto 30px;}\n";
  html += "p {font-size: 24px; color: #444444; margin-bottom: 10px;}\n";
  html += "</style>\n";
  html += "</head>\n";

  html += "<body>\n";
  html += "<h1>Dados dos sensores</h1>\n";
  html += "<p>Eixo Y: ";
  html += (float)var1;
  html += "m/s</p>\n";
  html += "<p>Eixo X: ";
  html += (float)var2;
  html += "m/s</p>\n";
  html += "<p>Eixo Z:";
  html += (float)var3;   
  html += "m/s</p>";
  html += "<p>Temperatura: ";
  html += (float)var4; 
  html += "ºC</p>";
  html += "<p><h6> Tempo de funcionamento do sistema: ";
  html += (float)var5;
  html += "</h6></p>";
  html += "<p><a href=\"/pin2/on\">Ligar pin2</a></p>";
  html += "<p><a href=\"/pin2/off\">Desligar pin2</a></p>";
  html += "<p><a href=\"/refresh\">recarregar</a></p>";
  html += "</body>\n";
  html += "</html>\n";
  return html;  //Retorna o script
}
void conectado() { 
  tempo = millis();
  mpu.update();
  float tempr = mpu.getTemp();
  float Xa = mpu.getAccX();
  float Ya = mpu.getAccY();
  float Za = mpu.getAccZ();
  server.send(200, "text/html", htmltext(Ya,Xa,Za,tempr,tempo));  // Responde à requisição com uma página HTML simples
}

void vON(){
digitalWrite(2,HIGH);
server.sendHeader("location","/");
server.send(303);
}

void vOFF(){
digitalWrite(2,LOW);
server.sendHeader("location","/");
server.send(303);
}

void vrefresh(){
 server.sendHeader("location","/");
 server.send(303);
}






void setup() {  // Função setup() do Arduino, chamada uma vez ao inicializar
 
  pinMode(2,OUTPUT);
  delay(2000);
  Serial.begin(250000); 
  delay(2000);         

  //WIFIManager setup
    WiFi.mode(WIFI_STA);
    WiFiManager wm;
    wm.resetSettings();
    bool res;
    res = wm.autoConnect("ESP-32","suasenha"); // password protected ap
        if(!res) {
        Serial.println("Falha na conexão! Reiniciando sistema...");
        ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("Conectado!");
    }    
  
  //webserver setup
  Serial.println("Conectando-se a ");
  delay(1000);
  while (WiFi.status() != WL_CONNECTED) {  // Enquanto não estiver conectado ao Wi-Fi
    delay(1000);                           // Aguarda 1 segundo
    Serial.print(".");                  // Imprime um ponto para indicar tentativas de conexão
  }
  Serial.println("");                 // Imprime uma linha em branco
  Serial.println("WiFi conectado.");  // Imprime mensagem indicando que o Wi-Fi está conectado
  Serial.print("Endereço de IP: ");
  Serial.println(WiFi.localIP());        // Imprime o endereço IP atribuído ao ESP32

  //ESPmDNS setup
    if (!MDNS.begin("esp32")) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");

  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);

//WIFI 
  server.on("/", conectado);
  server.on("/pin2/on", vON);
  server.on("/pin2/off", vOFF);
  server.on("/refresh", vrefresh);
  server.begin();                        // Inicia o servidor


  Serial.println("Servidor iniciado.");  // Imprime mensagem indicando que o servidor está iniciado


  Wire.begin();
  mpu.begin();
  mpu.calcGyroOffsets(true);
  mpu.update();
}

void loop(){
  server.handleClient();
}
