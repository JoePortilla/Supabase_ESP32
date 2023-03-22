#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "secrets.hpp"

// VARIABLES Y CONSTANTES
// GPIO al que se conecta el LED
const uint8_t ledPin = 2;
// Tabla en donde se va hacer la solicitud.
const String TABLE = "Control_DEMO";
// Columna que se va a leer
const String COLUMN = "status";
// Payload a recibir
String payload;

void setup()
{
  // Inicializar la comunicación serial a 115200 baudios.
  Serial.begin(115200);

  // Definir el GPIO del LED como salida.
  pinMode(ledPin, OUTPUT);
  // Inicializar el LED apagado.
  digitalWrite(ledPin, 0);

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
    http.begin(URL + "/rest/v1/" + TABLE + "?select=" + COLUMN);
    // Se agrega el APIKEY del servicio
    http.addHeader("apikey", SUPABASE_KEY);
    http.addHeader("Authorization", "Bearer " + SUPABASE_KEY);
    // Se especifica el tipo de contenido, para que el servidor pueda interpretarlo.
    http.addHeader("Content-Type", "application/json");
    // Preferir una respuesta que contenga solo la info. mínima necesaria para completar la solicitud.
    http.addHeader("Prefer", "return=minimal");

    // Recibir información mediante el metodo GET
    uint16_t httpCode = http.GET();

    // Se comprueba el codigo que retorna el servidor a la solicitud enviada.
    if (httpCode > 0)
    {
      // Si el codigo devuelto por el servidor es > 0, la solicitud fue enviada.
      Serial.printf("Codigo HTTP: %d.\n", httpCode);
      // Se guarda el mensaje recibido
      payload = http.getString();
      // Formateo de payload. (llega en formato JSON).
      payload.replace(" ", "");
      payload.replace("\n", "");
      payload.trim();
      payload.remove(0, 1);
      payload.remove(payload.length() - 1, 1);
      // Se muestra el mensaje recibido.
      Serial.printf("Mensaje recibido: %s.\n", payload);
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

  // Logica a seguir segun el contenido del Payload.
  if (payload == "{\"status\":1}")
  {
    digitalWrite(ledPin, 1);
  }

  if (payload == "{\"status\":0}")
  {
    digitalWrite(ledPin, 0);
  }

  delay(2000);
}
