#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <indexHtml.h>
#include <stylesCss.h>
#include <removePageHtml.h>
#include <addPageHtml.h>
#include<listamed.h>

// Configurações do AP
const char* ap_ssid = "ESP32_MedManager";
const char* ap_password = "12345678"; // Mínimo 8 caracteres

WebServer server(80);


void rotas(){
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", indexHtml);
  });

  server.on("/styles.css", HTTP_GET, []() {
    server.send(200, "text/css", stylesCss);
  });

    server.on("/removePage.html", HTTP_GET, []() {
    server.send(200, "text/html", removePageHtml);
  });
  
    server.on("/listamed.html", HTTP_GET, []() {
    server.send(200, "text/html", getListaMedHtml());
  });

      server.on("/addPage.html", HTTP_GET, []() {
    server.send(200, "text/html", addPageHtml);
  });
  

  server.on("/add", HTTP_GET, handleAdd); 
  server.on("/remov", HTTP_GET, handleRemov);
}

void handleRemov() {
  if (server.method() == HTTP_GET) {
    String medicamento = server.arg("medTOrem");

    Serial.println("\nDados recebidos via formulário:");
    Serial.println("Medicamento: " + medicamento);
    
    // Envia resposta ao cliente
    server.send(200, "text/html",      
   "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
   "<title>Confirmação</title></head><body>"
   "<h2>Medicamento removido pela ESP32:</h2>"
   "<p>Medicamento: " + medicamento + "</p>"
   "<a href='/'>Voltar</a></body></html>"
      );
  }
}


void handleAdd() {
  if (server.method() == HTTP_GET) {
    String medicamento = server.arg("mdcm");
    String horario = server.arg("hora");
    String intervalo = server.arg("itvl");
    
    Serial.println("\nDados recebidos via formulário:");
    Serial.println("Medicamento: " + medicamento);
    Serial.println("Horário: " + horario);
    Serial.println("Intervalo: " + intervalo + " horas");
    
    // Envia resposta ao cliente
    server.send(200, "text/html",      
    "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
      "<title>Confirmação</title></head><body>"
      "<h2>Dados recebidos pela ESP32:</h2>"
      "<p>Medicamento: " + medicamento + "</p>"
      "<p>Horário: " + horario + "</p>"
      "<p>Intervalo: " + intervalo + " horas</p>"
      "<a href='/'>Voltar</a></body></html>"
      );
  }
}

void setup() {
  Serial.begin(115200);
  
  // Configura a ESP32 como Access Point
  WiFi.softAP(ap_ssid, ap_password);
  
  Serial.println("");
  Serial.println("Access Point criado");
  Serial.print("SSID: ");
  Serial.println(ap_ssid);
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  // Configura as rotas
  rotas();
  
  //configura o DNS
  char* dns = "medmanager";
      if (!MDNS.begin(dns)) {
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.printf("DNS iniciado acesse: %s.local\n", dns);

  //inicia o servidor
  server.begin();
  Serial.println("Servidor HTTP iniciado");
  MDNS.addService("http", "tcp", 80);
}

void loop() {
  server.handleClient();
}