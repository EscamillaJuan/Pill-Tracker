#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <vector>

const char* ssid = "TuPuntoDeAcceso";
const char* password = "TuClaveDeAcceso";
const int ledPin = 5;
AsyncWebServer server(80);

struct Alarm {
  String name;
  unsigned long time;
  unsigned int quantity;
  unsigned long startTime;
};

std::vector<Alarm> alarms;
unsigned long alarmStartTime = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

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
              <label for="quantity1">Cantidad de dosis:</label>
              <input
                type="number"
                id="quantity1"
                name="quantity1"
                placeholder="Cantidad"
              />
            </div>
            <input type="submit" value="Configurar Alarma" onclick='configureAlarm()'>
          </div>
          <script>
            function configureAlarm() {
              const name = document.getElementById("name1").value;
              const interval = document.getElementById("interval1").value;
              const quantity = document.getElementById("quantity1").value;

              fetch('/setAlarm', {
                method: 'POST',
                headers: {
                  'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: `name=${name}&interval=${interval}&quantity=${quantity}`,
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
    unsigned int quantity;

    if (request->hasParam("name", true) && request->hasParam("interval", true) && request->hasParam("quantity", true)) {
      name = request->getParam("name", true)->value();
      interval = request->getParam("interval", true)->value().toInt();
      quantity = request->getParam("quantity", true)->value().toInt();

      Alarm newAlarm;
      newAlarm.name = name;
      newAlarm.time = interval;
      newAlarm.quantity = quantity;
      newAlarm.startTime = millis();
      alarms.push_back(newAlarm);

      request->send(200, "text/plain", "Alarma configurada con éxito");
    } else {
      request->send(400, "text/plain", "Error en la configuración de la alarma");
    }
  });

  server.begin();
}

void loop() {
  if (alarms.empty()) { }
  else{
    unsigned long currentTime = millis();
    for(int i=0; i < alarms.size(); ++i){
      if((currentTime - alarms[i].startTime) >= (alarms[i].time * 1000)){
        digitalWrite(ledPin, HIGH);
        delay(5000);
        digitalWrite(ledPin, LOW);
        alarms.erase(alarms.begin() + i);
      }
      // Serial.println(alarms[i].name);
      // Serial.println(alarms[i].quantity);
      // Serial.println(alarms[i].time * 1000);
      // Serial.println(currentTime);
    }
  }
}
