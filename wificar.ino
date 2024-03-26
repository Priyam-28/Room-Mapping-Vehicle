#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define UP_LEFT 5
#define UP_RIGHT 6
#define DOWN_LEFT 7
#define DOWN_RIGHT 8
#define TURN_LEFT 9
#define TURN_RIGHT 10
#define STOP 0

#define FRONT_RIGHT_MOTOR 0
#define BACK_RIGHT_MOTOR 1
#define FRONT_LEFT_MOTOR 2
#define BACK_LEFT_MOTOR 3
#define TRIG_PIN 23
#define ECHO_PIN 22

#define FORWARD 1
#define BACKWARD -1

struct MOTOR_PINS
{
  int pinIN1;
  int pinIN2;    
};
float duration_us, distance_cm;


std::vector<MOTOR_PINS> motorPins = 
{
  {2, 4},  //FRONT_RIGHT_MOTOR
  {18, 19},  //BACK_RIGHT_MOTOR
  {32, 33},  //FRONT_LEFT_MOTOR
  {26, 27},  //BACK_LEFT_MOTOR   
};

const char* ssid     = "UnKnownRider";
const char* password = "12345678";

AsyncWebServer server(80); //launches the server 
AsyncWebSocket ws("/ws"); // makes it forweb socket


const char* htmlHomePage PROGMEM = R"HTMLHOMEPAGE(
<!DOCTYPE html>
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <style>
        *{
            background-image: url();
        }
        .container{
            display: flex;
            align-items: center;
            flex-direction: column;
            
        }
        
        .box1{
            
            display: flex;
            align-items: center;
            justify-content: center;
            height: 20vh;
            width: 20vw;
            color: white;
            background-color: black;
            margin: 5px;

        }
        .box2{
            display: flex;
            align-items: center;
            justify-content: center;
            height: 20vh;
            width: 20vw;
            color: white;
            background-color: black;

        }
        .arrow{
            margin:5px;
            font-size: 18vh;
            display: flex;
            align-items: center;
            justify-content: center;
            height: 20vh;
            width: 20vw;
            color: white;
            background-color: black;
            
            

        }
        
        
    </style>
</head>
<body>

    <div class="container" id="page">
        <h1>Room Mapping Robot</h1><br>
        <h2>Project Kratos</h2>
        <div class="box1">
            <span class="arrow" ontouchstart='onTouchStartAndEnd("1")' ontouchend='onTouchStartAndEnd("0")'>&#8679;</span>
        </div>
        <div class="box1">
            <div class="l1" ontouchstart='onTouchStartAndEnd("3")' ontouchend='onTouchStartAndEnd("0")'><span class="arrow">&#8678;</span></div>
            <div class="l2" ontouchstart='onTouchStartAndEnd("2")' ontouchend='onTouchStartAndEnd("0")' ><span class="arrow">&#8681;</span></div>
            <div class="l3" ontouchstart='onTouchStartAndEnd("4")' ontouchend='onTouchStartAndEnd("0")'><span class="arrow">&#8680;</span></div>
        </div>
    </div>
    <script>
        var webSocketUrl = "ws:\/\/" + window.location.hostname + "/ws";
        var websocket;
        
        function initWebSocket() 
        {
          websocket = new WebSocket(webSocketUrl);
          websocket.onopen    = function(event){};
          websocket.onclose   = function(event){setTimeout(initWebSocket, 2000);};
          websocket.onmessage = function(event){};
        }
  
        function onTouchStartAndEnd(value) 
        {
          websocket.send(value);
        }
            
        window.onload = initWebSocket;
        document.getElementById("page").addEventListener("touchend", function(event){
          event.preventDefault()
        });      
      </script>
</body>
</html>

)HTMLHOMEPAGE";


void rotateMotor(int motorNumber, int motorDirection)
{
  if (motorDirection == FORWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, HIGH);    
  }
  else if (motorDirection == BACKWARD)
  {
    digitalWrite(motorPins[motorNumber].pinIN1,HIGH);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);     
  }
  else
  {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW);       
  }
}
void stopmotors() {
    rotateMotor(FRONT_RIGHT_MOTOR, STOP);
    rotateMotor(BACK_RIGHT_MOTOR, STOP);
    rotateMotor(FRONT_LEFT_MOTOR, STOP);
    rotateMotor(BACK_LEFT_MOTOR, STOP);
}

void processCarMovement(String inputValue)
{
  Serial.printf("Got value as %s %d\n", inputValue.c_str(), inputValue.toInt());
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(ECHO_PIN, HIGH);

  // calculate the distance
  distance_cm = 0.017 * duration_us;

  // print the value to Serial Monitor
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");
  
  switch(inputValue.toInt())
  {

    case UP:
    if(distance_cm>15){
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);                  

    }
    else{
      stopmotors();
    }
      break;
  
    case DOWN:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);   
      break;
  
    case LEFT:
      rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
      rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
      rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
      rotateMotor(BACK_LEFT_MOTOR, BACKWARD);   
      break;
  
    case RIGHT:
      rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
      rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
      rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
      rotateMotor(BACK_LEFT_MOTOR, FORWARD);  
      break;
  
    // case UP_LEFT:
    //   rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
    //   rotateMotor(BACK_RIGHT_MOTOR, STOP);
    //   rotateMotor(FRONT_LEFT_MOTOR, STOP);
    //   rotateMotor(BACK_LEFT_MOTOR, FORWARD);  
    //   break;
  
    // case UP_RIGHT:
    //   rotateMotor(FRONT_RIGHT_MOTOR, STOP);
    //   rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
    //   rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
    //   rotateMotor(BACK_LEFT_MOTOR, STOP);  
    //   break;
  
    // case DOWN_LEFT:
    //   rotateMotor(FRONT_RIGHT_MOTOR, STOP);
    //   rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
    //   rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
    //   rotateMotor(BACK_LEFT_MOTOR, STOP);   
    //   break;
  
    // case DOWN_RIGHT:
    //   rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
    //   rotateMotor(BACK_RIGHT_MOTOR, STOP);
    //   rotateMotor(FRONT_LEFT_MOTOR, STOP);
    //   rotateMotor(BACK_LEFT_MOTOR, BACKWARD);   
    //   break;
  
    // case TURN_LEFT:
    //   rotateMotor(FRONT_RIGHT_MOTOR, FORWARD);
    //   rotateMotor(BACK_RIGHT_MOTOR, FORWARD);
    //   rotateMotor(FRONT_LEFT_MOTOR, BACKWARD);
    //   rotateMotor(BACK_LEFT_MOTOR, BACKWARD);  
    //   break;
  
    // case TURN_RIGHT:
    //   rotateMotor(FRONT_RIGHT_MOTOR, BACKWARD);
    //   rotateMotor(BACK_RIGHT_MOTOR, BACKWARD);
    //   rotateMotor(FRONT_LEFT_MOTOR, FORWARD);
    //   rotateMotor(BACK_LEFT_MOTOR, FORWARD);   
    //   break;
  
    case STOP:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
      break;
  
    default:
      rotateMotor(FRONT_RIGHT_MOTOR, STOP);
      rotateMotor(BACK_RIGHT_MOTOR, STOP);
      rotateMotor(FRONT_LEFT_MOTOR, STOP);
      rotateMotor(BACK_LEFT_MOTOR, STOP);    
      break;
  }
}

void handleRoot(AsyncWebServerRequest *request) 
{
  request->send_P(200, "text/html", htmlHomePage);
}

void handleNotFound(AsyncWebServerRequest *request) 
{
    request->send(404, "text/plain", "File Not Found");
}


void onWebSocketEvent(AsyncWebSocket *server, 
                      AsyncWebSocketClient *client, 
                      AwsEventType type,
                      void *arg, 
                      uint8_t *data, 
                      size_t len) 
{                      
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      //client->text(getRelayPinsStatusJson(ALL_RELAY_PINS_INDEX));
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      processCarMovement("0");
      break;
    case WS_EVT_DATA:
      AwsFrameInfo *info;
      info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
      {
        std::string myData = "";
        myData.assign((char *)data, len);
        processCarMovement(myData.c_str());       
      }
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
    default:
      break;  
  }
}

void setUpPinModes()
{
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  for (int i = 0; i < motorPins.size(); i++)
  {
    pinMode(motorPins[i].pinIN1, OUTPUT);
    pinMode(motorPins[i].pinIN2, OUTPUT);  
    rotateMotor(i, STOP);  
  }
}


void setup(void) 
{
  setUpPinModes();
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
  
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  
  server.begin();
  Serial.println("HTTP server started");
  
}

void loop() {
    static unsigned long previousMillis = 0;
    const unsigned long interval = 500; // Interval in milliseconds

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        // Time to measure distance
        previousMillis = currentMillis;

        digitalWrite(TRIG_PIN, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG_PIN, LOW);

        duration_us = pulseIn(ECHO_PIN, HIGH);
        distance_cm = 0.017 * duration_us;

        Serial.print("distance: ");
        Serial.print(distance_cm);
        Serial.println(" cm");
    }

    // Other non-blocking tasks can go here
    ws.cleanupClients();
}
