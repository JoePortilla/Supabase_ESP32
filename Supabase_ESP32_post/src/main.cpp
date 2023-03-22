/**
 * \file Supabase_ESP32_post main.cpp
 * \brief Publicar un contador en Supabase mediante la biblioteca HTTPClient
 * \author Joseph Santiago Portilla. Ing. Electrónico - @JoePortilla
 */

// BIBLIOTECAS
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.hpp"

// VARIABLES Y CONSTANTES
// Inicialización de contador
uint8_t cont = 0;
// Tabla en donde se va hacer la solicitud.
const String TABLE = "Contador_DEMO";
// Columna en donde se va a escribir.
const String COLUMN = "contador";
// Payload a enviar.
String payload;

void setup()
{
  // Inicializar la comunicación serial a 115200 baudios.
  Serial.begin(115200);

  // Iniciar conexión WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("\nConectando a WiFi.");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.printf("\nWiFi conectado. IP: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  // Comprobación del estado de conexión WiFi.
  if ((WiFi.status() == WL_CONNECTED))
  {
    // Creación de un objeto de la clase HTTPClient
    HTTPClient http;

    // Se inicializa la solicitud y se define la URL del servidor al que se desea contactar.
    http.begin(URL + "/rest/v1/" + TABLE);
    // Se agrega el APIKEY del servicio
    http.addHeader("apikey", SUPABASE_KEY);
    http.addHeader("Authorization", "Bearer " + SUPABASE_KEY);
    // Se especifica el tipo de contenido, para que el servidor pueda interpretarlo.
    http.addHeader("Content-Type", "application/json");
    // Preferir una respuesta que contenga solo la info. mínima necesaria para completar la solicitud.
    http.addHeader("Prefer", "return=minimal");

    // Incrementar contador.
    cont++;
    Serial.printf("Contador: %d.\n", cont);

    // Enviar la información mediante el metodo POST
    payload = "{\"" + COLUMN + "\":" + String(cont) + "}";
    uint16_t httpCode = http.POST(payload);

    // Se comprueba el codigo que retorna el servidor a la solicitud enviada.
    if (httpCode > 0)
    {
      // Si el codigo devuelto por el servidor es > 0, la solicitud fue enviada.
      Serial.printf("Codigo HTTP: %d.\n", httpCode);
    }
    else
    {
      // Si el codigo devuelto por el servidor es < 0, el ESP32 tuvo un error en la solicitud.
      Serial.printf("Error en solicitud HTTP\n");
    }

    // Finalizar la comunicación del cliente HTTP para librerar recursos del microcontrolador
    // y evitar problemas de comunicación.
    http.end();
  }
  else
  {
    Serial.printf("Error en Conexión WiFi.\n");
  }

  delay(2000);
}
