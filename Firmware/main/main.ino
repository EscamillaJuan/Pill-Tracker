#include <WiFi.h>
#include <ESPAsyncWebSrv.h>
#include <vector>
#include <TimeAlarms.h>
#include <queue>
#include <ESP_FlexyStepper.h>
#include <ESP32Servo.h>
#include <WS2812FX.h>

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

int melody[] = {
    NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

const char* ssid = "Auto-Dosis";
const char* password = "mispastillasatodahora";
const int MOTOR_STEP_PIN = 22;
const int MOTOR_DIRECTION_PIN = 23;
const int MOTOR_ENABLE = 21;
const int SERVO_PIN = 5;
const int PIN_NEO = 19;
const int NUM_LEDS = 50;

AsyncWebServer server(80);

AlarmId id1;
AlarmId id2;
AlarmId id3;
AlarmId id4;
AlarmId id5;
AlarmId id6;
AlarmId id7;
AlarmId id8;

std::queue<int> QueueSteps;
int motorDirection = 1;
const int SPEED_IN_STEPS_PER_SECOND = 100;
const int ACCELERATION_IN_STEPS_PER_SECOND_PER_SECOND = 100;
const int DECELERATION_IN_STEPS_PER_SECOND_PER_SECOND = 200;
int AlarmSteps[8] = {0,25,50,75,100,125,150,175};

ESP_FlexyStepper stepper; // Motor a pasos
Servo servo;              //Servomotor
WS2812FX Neo_pixel = WS2812FX(NUM_LEDS, PIN_NEO, NEO_RGB + NEO_KHZ800); //Neopixel

void processStepsTask(void *pvParameters){
  (void)pvParameters;
  for (;;) {
    Neo_pixel.service();
    if(stepper.getDistanceToTargetSigned() == 0 && !QueueSteps.empty()){

      process_movement();                           //Se activan los leds en Amarillo
      digitalWrite(MOTOR_ENABLE, LOW);
      int steps = QueueSteps.front();
      long relativeSteps = steps * motorDirection;
      stepper.moveRelativeInSteps(relativeSteps);
      delay(1000);
      servo.write(180);
      delay(1000);        
      servo.write(90);
      delay(1000);
      
      motorDirection *= -1;
      relativeSteps = steps * motorDirection;
      stepper.moveRelativeInSteps(relativeSteps);
      QueueSteps.pop();
      motorDirection *= -1;
      digitalWrite(MOTOR_ENABLE, HIGH);
      delay(1000);
      Neo_pixel.stop();                            //Se apagan los leds

      if(QueueSteps.size()<=1) {
        process_notify();
      }
    }
    vTaskDelay(10);
  }
}

void setup() {
  Serial.begin(115200);
  setTime(0, 0, 0, 1, 1, 2023);

  WiFi.softAP(ssid, password);
  Serial.println("Punto de acceso WiFi creado");
  Serial.print("IP Adress:");
  Serial.println(WiFi.softAPIP());

  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(MOTOR_ENABLE, HIGH);

  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2400);
  servo.write(90);

  stepper.connectToPins(MOTOR_STEP_PIN, MOTOR_DIRECTION_PIN);
  stepper.setSpeedInStepsPerSecond(SPEED_IN_STEPS_PER_SECOND);
  stepper.setAccelerationInStepsPerSecondPerSecond(ACCELERATION_IN_STEPS_PER_SECOND_PER_SECOND);
  stepper.setDecelerationInStepsPerSecondPerSecond(DECELERATION_IN_STEPS_PER_SECOND_PER_SECOND);


  xTaskCreate(
    processStepsTask,
    "processStepsTask",
    2048,
    NULL,
    1,
    NULL
  );

  Neo_setup();
  if(Neo_pixel.isRunning()){
    delay(3000);
    Neo_pixel.stop();
    //Serial.println("Neo Ready!");  
  }

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
            <h1>Auto-Dosis</h1>
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
    if (request->hasParam("interval1", true) && request->hasParam("interval2", true) && 
        request->hasParam("interval3", true) && request->hasParam("interval4", true) && 
        request->hasParam("interval5", true) && request->hasParam("interval6", true) &&
        request->hasParam("interval7", true) && request->hasParam("interval8", true)) {
      interval[0] = request->getParam("interval1", true)->value().toInt();
      interval[1] = request->getParam("interval2", true)->value().toInt();
      interval[2] = request->getParam("interval3", true)->value().toInt();
      interval[3] = request->getParam("interval4", true)->value().toInt();
      interval[4] = request->getParam("interval5", true)->value().toInt();
      interval[5] = request->getParam("interval6", true)->value().toInt();
      interval[6] = request->getParam("interval7", true)->value().toInt();
      interval[7] = request->getParam("interval8", true)->value().toInt();
      Alarm.free(id1);
      Alarm.free(id2);
      Alarm.free(id3);
      Alarm.free(id4);
      Alarm.free(id5);
      Alarm.free(id6);
      Alarm.free(id7);
      Alarm.free(id8);
      id1 = Alarm.timerRepeat(interval[0]*60, process_alarm);
      id2 = Alarm.timerRepeat(interval[1]*60, process_alarm);
      id3 = Alarm.timerRepeat(interval[2]*60, process_alarm);
      id4 = Alarm.timerRepeat(interval[3]*60, process_alarm);
      id5 = Alarm.timerRepeat(interval[4]*60, process_alarm);
      id6 = Alarm.timerRepeat(interval[5]*60, process_alarm);
      id7 = Alarm.timerRepeat(interval[6]*60, process_alarm);
      id8 = Alarm.timerRepeat(interval[7]*60, process_alarm);

      request->send(200, "text/plain", "Alarma configurada con éxito");
      Serial.println("Alarma configurada con éxito");
      Neo_pixel.setMode(FX_MODE_CHASE_RAINBOW);
      Neo_pixel.start();
      delay(2000);
      Neo_pixel.stop();
    } else {
      request->send(400, "text/plain", "Error en la configuración de la alarma");
    }
  });

  server.begin();
}
void process_movement(){  
  Neo_pixel.setColor(RED);
  Neo_pixel.setMode(FX_MODE_BLINK);
  Neo_pixel.start();
}
void Neo_setup(){
  Neo_pixel.init();
  Neo_pixel.setBrightness(255);
  Neo_pixel.setSpeed(100);
  Neo_pixel.setMode(FX_MODE_RAINBOW_LARSON);
  Neo_pixel.start();  
}
void process_notify() {    
  //LEDs  
  Neo_pixel.setColor(GREEN);
  Neo_pixel.setMode(FX_MODE_BLINK);
  Neo_pixel.start();
  //Sonido
  // digitalWrite(4, HIGH);
  // delay(1000);
  // digitalWrite(4, LOW);
  // delay(1000);
  // digitalWrite(4, HIGH);
  // delay(1000);
  // digitalWrite(4, LOW);
  // delay(1000);  

  for (int i=0; i<8; ++i){
    int noteDuration = 1000/noteDurations[i];
    tone(4, melody[i], noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(4);
  }
  delay(6000);
  Neo_pixel.stop();
}

void process_alarm(){
  AlarmID_t id = Alarm.getTriggeredAlarmId();
  QueueSteps.push(AlarmSteps[id]);
}

void loop() {
  Alarm.delay(1000);
  Neo_pixel.service();
}