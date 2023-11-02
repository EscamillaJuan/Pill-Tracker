#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <vector>
#include <TimeAlarms.h>

const char* ssid = "TuPuntoDeAcceso";
const char* password = "TuClaveDeAcceso";
const int ledPin = 5;
AsyncWebServer server(80);

AlarmId id1;
AlarmId id2;
AlarmId id3;
AlarmId id4;
AlarmId id5;
AlarmId id6;
AlarmId id7;
AlarmId id8;

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
              font-size: 1rem;
            }
            header {
              background-color: #213555;
              color: white;
              text-align: center;
              padding: 10px;
              max-width: 100%;
            }
            .container {
              display: flex;
              flex-direction: column;
              width: 100%;
              height: 100vh;
              margin: 0;
              padding: 20px;
              border: 1px solid #ccc;
              border-radius: 5px;
              background-color: #f9f9f9;
              align-items: center;
            }
          .medicamento {
            margin-bottom: 10px;
            width: 30%;
          }
          .medicamento label {
            display: block;
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
            width: 30%;
          }
          .dosis label,
          .dosis input[type="number"] {
            flex: 2;
          }
          .dosis input[type="number"] {
            margin-left: 10px;
            width: 50%;
            border: 1px solid #ccc;
            border-radius: 3px;
          }
          input[type="submit"] {
            width: 30%;
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

             <div class="medicamento">
              <label for="name3">Medicamento 3:</label>
              <input
                type="text"
                id="name3"
                name="name3"
                placeholder="Nombre del medicamento"
              />
            </div>
            <div class="dosis">
              <label for="interval3">Intervalo de dosis:</label>
              <input
                type="number"
                id="interval3"
                name="interval3"
                placeholder="Horas"
              />
            </div>

            <div class="medicamento">
              <label for="name4">Medicamento 4:</label>
              <input
                type="text"
                id="name4"
                name="name4"
                placeholder="Nombre del medicamento"
              />
            </div>
            <div class="dosis">
              <label for="interval4">Intervalo de dosis:</label>
              <input
                type="number"
                id="interval4"
                name="interval4"
                placeholder="Horas"
              />
            </div>

            <div class="medicamento">
              <label for="name5">Medicamento 5:</label>
              <input
                type="text"
                id="name5"
                name="name5"
                placeholder="Nombre del medicamento"
              />
            </div>
            <div class="dosis">
              <label for="interval5">Intervalo de dosis:</label>
              <input
                type="number"
                id="interval5"
                name="interval5"
                placeholder="Horas"
              />
            </div>

            <div class="medicamento">
              <label for="name6">Medicamento 6:</label>
              <input
                type="text"
                id="name6"
                name="name6"
                placeholder="Nombre del medicamento"
              />
            </div>
            <div class="dosis">
              <label for="interval6">Intervalo de dosis:</label>
              <input
                type="number"
                id="interval6"
                name="interval6"
                placeholder="Horas"
              />
            </div>

             <div class="medicamento">
              <label for="name7">Medicamento 7:</label>
              <input
                type="text"
                id="name7"
                name="name7"
                placeholder="Nombre del medicamento"
              />
            </div>
            <div class="dosis">
              <label for="interval7">Intervalo de dosis:</label>
              <input
                type="number"
                id="interval7"
                name="interval7"
                placeholder="Horas"
              />
            </div>

            <div class="medicamento">
              <label for="name8">Medicamento 8:</label>
              <input
                type="text"
                id="name8"
                name="name8"
                placeholder="Nombre del medicamento"
              />
            </div>
            <div class="dosis">
              <label for="interval4">Intervalo de dosis:</label>
              <input
                type="number"
                id="interval8"
                name="interval8"
                placeholder="Horas"
              />
            </div>
            
            <input type="submit" value="Configurar Alarma" onclick='configureAlarm()'>
          </div>
          <script>
            function configureAlarm() {
              const interval1 = document.getElementById('interval1').value;
              const interval2 = document.getElementById('interval2').value;
              const interval3 = document.getElementById('interval3').value;
              const interval4 = document.getElementById('interval4').value;
              const interval5 = document.getElementById('interval5').value;
              const interval6 = document.getElementById('interval6').value;
              const interval7 = document.getElementById('interval7').value;
              const interval8 = document.getElementById('interval8').value;
                
              fetch('/setAlarm', {
                method: 'POST',
                headers: {
                  'Content-Type': 'application/x-www-form-urlencoded',
                },
                body: `interval1=${interval1}&interval2=${interval2}&interval3=${interval3}&interval4=${interval4}&interval5=${interval5}&interval6=${interval6}&interval7=${interval7}&interval8=${interval8}`,
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
    unsigned long interval[8];
    if (request->hasParam("interval1", true) && request->hasParam("interval2", true)) {
      interval[0] = request->getParam("interval1", true)->value().toInt();
      interval[1] = request->getParam("interval2", true)->value().toInt();
      interval[2] = request->getParam("interval3", true)->value().toInt();
      interval[3] = request->getParam("interval4", true)->value().toInt();
      interval[4] = request->getParam("interval5", true)->value().toInt();
      interval[5] = request->getParam("interval6", true)->value().toInt();
      interval[6] = request->getParam("interval7", true)->value().toInt();
      interval[7] = request->getParam("interval8", true)->value().toInt();
      Serial.println(interval[0]);
      Serial.println(interval[1]);
      Serial.println(interval[2]);
      Serial.println(interval[3]);
      Serial.println(interval[4]);
      Serial.println(interval[5]);
      Serial.println(interval[6]);
      Serial.println(interval[7]);


      Alarm.free(id1);
      Alarm.free(id2);
      Alarm.free(id3);
      Alarm.free(id4);
      Alarm.free(id5);
      Alarm.free(id6);
      Alarm.free(id7);
      Alarm.free(id8);
      id1 = Alarm.timerRepeat(interval[0], buzzer);
      id2 = Alarm.timerRepeat(interval[1], buzzer);
      id3 = Alarm.timerRepeat(interval[2], buzzer);
      id4 = Alarm.timerRepeat(interval[3], buzzer);
      id5 = Alarm.timerRepeat(interval[4], buzzer);
      id6 = Alarm.timerRepeat(interval[5], buzzer);
      id7 = Alarm.timerRepeat(interval[6], buzzer);
      id8 = Alarm.timerRepeat(interval[7], buzzer);

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
