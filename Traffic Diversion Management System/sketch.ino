#define BLYNK_TEMPLATE_ID "TMPL3DoTa8aLk"
#define BLYNK_TEMPLATE_NAME "Iot Project"
#define BLYNK_AUTH_TOKEN "7jtEfGGXTRqZMzUuaXZt--IcOc_dacS_"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Wokwi-GUEST";
char pass[] = "";

const int sensorPin1 = 34;
const int sensorPin2 = 35;
const int sensorPin3 = 32;
const int sensorPin4 = 33;
const int redLED1 = 16;
const int greenLED1 = 17;
const int redLED2 = 18;
const int greenLED2 = 19;
const int redLED3 = 21;
const int greenLED3 = 22;
const int redLED4 = 23;
const int greenLED4 = 25;

const int threshold = 1500;

const unsigned long greenDuration = 10000; 
const unsigned long switchDelay = 5000;  
unsigned long switchStartMillis = 0;
int currentLane = 0;

int traffic1 = 0;
int traffic2 = 0;
int traffic3 = 0;
int traffic4 = 0;

int maxTraffic = 0;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass); 

  Blynk.begin(auth, ssid, pass); 

  pinMode(redLED1, OUTPUT);
  pinMode(greenLED1, OUTPUT);
  pinMode(redLED2, OUTPUT);
  pinMode(greenLED2, OUTPUT);
  pinMode(redLED3, OUTPUT);
  pinMode(greenLED3, OUTPUT);
  pinMode(redLED4, OUTPUT);
  pinMode(greenLED4, OUTPUT);

  setAllRed(); 
}

void loop() {
  Blynk.run(); 

  traffic1 = analogRead(sensorPin1); 
  traffic2 = analogRead(sensorPin2); 
  traffic3 = analogRead(sensorPin3); 
  traffic4 = analogRead(sensorPin4); 

  Blynk.virtualWrite(V1, traffic1);
  Blynk.virtualWrite(V2, traffic2);
  Blynk.virtualWrite(V3, traffic3);
  Blynk.virtualWrite(V4, traffic4);

  unsigned long currentMillis = millis();

  maxTraffic = max(max(traffic1, traffic2), max(traffic3, traffic4)); // Update maxTraffic here
  if (maxTraffic > threshold) {
    if (traffic1 == maxTraffic) {
      manageTraffic(1, traffic1);
    } else if (traffic2 == maxTraffic) {
      manageTraffic(2, traffic2);
    } else if (traffic3 == maxTraffic) {
      manageTraffic(3, traffic3);
    } else if (traffic4 == maxTraffic) {
      manageTraffic(4, traffic4);
    }
    switchStartMillis = currentMillis; 
  } else {
    if (currentMillis - switchStartMillis >= switchDelay) {
      switchStartMillis = currentMillis; 
      switch (currentLane) {
        case 1:
          setLane(1, false);
          setLane(2, true);
          currentLane = 2;
          break;
        case 2:
          setLane(2, false);
          setLane(3, true);
          currentLane = 3;
          break;
        case 3:
          setLane(3, false);
          setLane(4, true);
          currentLane = 4;
          break;
        case 4:
          setLane(4, false);
          setLane(1, true);
          currentLane = 1;
          break;
        default:
          setLane(1, true);
          currentLane = 1;
          break;
      }
    }
  }
}

void manageTraffic(int lane, int traffic) {
  if (currentLane != 0 && currentLane != lane) {
    setLane(currentLane, false);
  }

  currentLane = lane;
  unsigned long startMillis = millis();
  setLane(lane, true);

  bool switchToNewLane = false;
  while (millis() - startMillis < greenDuration) {
    delay(1000); 
    switch (lane) {
      case 1:
        traffic = analogRead(sensorPin1);
        if (traffic2 > threshold || traffic3 > threshold || traffic4 > threshold) {
          switchToNewLane = true;
        }
        break;
      case 2:
        traffic = analogRead(sensorPin2);
        if (traffic1 > threshold || traffic3 > threshold || traffic4 > threshold) {
          switchToNewLane = true;
        }
        break;
      case 3:
        traffic = analogRead(sensorPin3);
        if (traffic1 > threshold || traffic2 > threshold || traffic4 > threshold) {
          switchToNewLane = true;
        }
        break;
      case 4:
        traffic = analogRead(sensorPin4);
        if (traffic1 > threshold || traffic2 > threshold || traffic3 > threshold) {
          switchToNewLane = true;
        }
        break;
    }

    if (switchToNewLane) {
      int newMaxTraffic = max(max(traffic1, traffic2), max(traffic3, traffic4));
      if (newMaxTraffic > threshold && newMaxTraffic != maxTraffic) {
       
        if (traffic1 == newMaxTraffic) {
          manageTraffic(1, traffic1);
        } else if (traffic2 == newMaxTraffic) {
          manageTraffic(2, traffic2);
        } else if (traffic3 == newMaxTraffic) {
          manageTraffic(3, traffic3);
        } else if (traffic4 == newMaxTraffic) {
          manageTraffic(4, traffic4);
        }
        break; 
      }
    }

    if (traffic <= threshold) {
      break;
    }
  }
}

void setLane(int lane, bool isGreen) {
  int redLED, greenLED;

  switch (lane) {
    case 1:
      redLED = redLED1;
      greenLED = greenLED1;
      break;
    case 2:
      redLED = redLED2;
      greenLED = greenLED2;
      break;
    case 3:
      redLED = redLED3;
      greenLED = greenLED3;
      break;
    case 4:
      redLED = redLED4;
      greenLED = greenLED4;
      break;
    default:
      return;
  }

  if (isGreen) {
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED, HIGH);
  } else {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
  }
}

void setAllRed() {
  digitalWrite(redLED1, HIGH);
  digitalWrite(greenLED1, LOW);
  digitalWrite(redLED2, HIGH);
  digitalWrite(greenLED2, LOW);
  digitalWrite(redLED3, HIGH);
  digitalWrite(greenLED3, LOW);
  digitalWrite(redLED4, HIGH);
  digitalWrite(greenLED4, LOW);
}
