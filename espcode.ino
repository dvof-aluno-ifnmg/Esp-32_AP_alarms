#include <WiFi.h>
#include <WebServer.h>

// Configurações do AP
const char* ap_ssid = "ESP32_MedManager";
const char* ap_password = "12345678"; // Mínimo 8 caracteres

WebServer server(80);

void handleForm() {
  if (server.method() == HTTP_POST) {
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
      "<a href='/'>Voltar</a></body></html>");
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
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", 
      "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
      "<title>Gerenciador de Medicamentos</title>"
      "<style>"
      "body {font-family: Arial; margin: 40px;}"
      "form {max-width: 400px; margin: 0 auto;}"
      "input, button {padding: 8px; margin: 5px 0; width: 100%;}"
      "</style></head>"
      "<body>"
      "<h2>Cadastrar Medicamento</h2>"
      "<form action='/cadastro' method='post'>"
      "<label for='mdcm'>Nome do medicamento:</label><br>"
      "<input type='text' id='mdcm' name='mdcm' required><br>"
      "<label for='hora'>Horário:</label><br>"
      "<input type='time' id='hora' name='hora' required><br>"
      "<label for='itvl'>Intervalo (horas):</label><br>"
      "<input type='number' id='itvl' name='itvl' required><br><br>"
      "<button type='submit'>Enviar</button>"
      "</form></body></html>");
  });
  
  server.on("/cadastro", HTTP_POST, handleForm);
  
  server.begin();
  Serial.println("Servidor HTTP iniciado");
}

void loop() {
  server.handleClient();
}