#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <vector>
#include <TimeAlarms.h>

const char* ssid = "TuPuntoDeAcceso";
const char* password = "TuClaveDeAcceso";
const int ledPin = 5;
AsyncWebServer server(80);

AlarmId id;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  setTime(0, 0, 0, 1, 1, 2023);
  WiFi.softAP(ssid, password);
  Serial.println("Punto de acceso WiFi creado");
  Serial.println(WiFi.softAPIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = R"(
      <!DOCTYPE html>
      <html>
        <head>
          <style>
            body {
              font-family: Arial, sans-serif;
            }
            header {
              background-color: #213555;
              color: white;
              text-align: center;
              padding: 10px;
            }
            .container {
              max-width: 100%;
              height: 100%;
              margin: 0 auto;
              padding: 20px;
              border: 1px solid #ccc;
              border-radius: 5px;
              background-color: #f9f9f9;
            }
            .medicamento {
              margin-bottom: 10px;
            }
            .medicamento label {
              display: block;
              font-weight: bold;
            }
            .medicamento input[type="text"] {
              width: 100%;
              padding: 5px;
              margin-top: 3px;
              margin-bottom: 10px;
              border: 1px solid #ccc;
              border-radius: 3px;
            }
            .dosis {
              margin-bottom: 10px;
              display: flex;
              justify-content: space-between;
            }
            .dosis label,
            .dosis input[type="number"] {
              flex: 2;
            }
            .dosis input[type="number"] {
              margin-left: 10px;
              width: 40%;
            }
          </style>
        </head>
        <body>
          <header>
            <h1>Auto-Medico</h1>
          </header>
          <div class="container">
            <div class="medicamento">
              <label for="name1">Medicamento 1:</label>
              <input
                type="text"
                id="name1"
                name="name1"
                placeholder="Nombre del medicamento"
              />
            </div>
            <div class="dosis">
              <label for="interval1">Intervalo de dosis:</label>
              <input
                type="number"
                id="interval1"
                name="interval1"
                placeholder="Horas"
              />
            </div>

            <div class="medicamento">
              <label for="name2">Medicamento 2:</label>
              <input
                type="text"
                id="name2"
                name="name2"
                placeholder="Nombre del medicamento"
              />
            </div>
            <div class="dosis">
              <label for="interval2">Intervalo de dosis:</label>
              <input
                type="number"
                id="interval2"
                name="interval2"
                placeholder="Horas"
              />
            </div>
            
            <input type="submit" value="Configurar Alarma" onclick='configureAlarm()'>
          </div>
          <script>
            function configureAlarm() {
              const name = document.getElementById(`name1`).value;
              const interval = document.getElementById(`interval1`).value;
                
              fetch('/setAlarm', {
                method: 'POST',
                headers: {
                  'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: `name=${name}&interval=${interval}`,
              })
              .then(response => {
                if (response.ok) {
                  console.log('Alarma configurada con éxito');
                  location.reload();
                } else {
                  console.error('Error al configurar la alarma');
                }
              });
            }
          </script>
        </body>
      </html>
    )";

    request->send(200, "text/html", html);
  });

  server.on("/setAlarm", HTTP_POST, [](AsyncWebServerRequest *request){
    String name;
    unsigned long interval;
    if (request->hasParam("name", true) && request->hasParam("interval", true)) {
      name = request->getParam("name", true)->value();
      interval = request->getParam("interval", true)->value().toInt();
      Serial.println(name);
      Alarm.free(id);
      id = Alarm.timerRepeat(interval, buzzer);
      request->send(200, "text/plain", "Alarma configurada con éxito");
    } else {
      request->send(400, "text/plain", "Error en la configuración de la alarma");
    }
  });

  server.begin();
}

void buzzer(){
  digitalWrite(ledPin, HIGH);
  delay(2000);
  digitalWrite(ledPin, LOW);
}

void loop() {
  Alarm.delay(1000);
}
