#include <BH1750.h>
#include <Wire.h>
#include <Stepper.h>
#include <SPI.h>

#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/*#include <WiFi.h>
  #include <WiFiClient.h>
  #include <BlynkSimpleEsp8266.h>
*/
#define LIGHTS D4
#define FAN D0
#define MIC A0
#define DHT_PIN 1

#define STEPS 75
#define OLED_RESET 0
#define DHTTYPE DHT11
/*
  char auth[] = "BlynkAuthcode";
  char ssid[] = "WiFiSSID";
  char psw[] = "WiFi Wachtwoord";
  BlynkTimer timer;
*/
Adafruit_SSD1306 screen(OLED_RESET);
BH1750 lightMeter;

bool play_on_stage = false;
int lux;
float temp;
int volume;

Stepper stepper(STEPS, D5, D7, D6, D8);
DHT dht(DHT_PIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("Reached Setup");
  Wire.begin();
  //  WiFi.begin(ssid, psw);

  pinMode(DHT_PIN, FUNCTION_3);
  pinMode(LIGHTS, OUTPUT);
  dht.begin();

  pinMode(FAN, OUTPUT);
  screen.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);

  screen.display();
  stepper.setSpeed(200);
  screen.clearDisplay();
  /*  while (WiFi.status() != WL_CONNECTED) {
      // delay(500);
      Serial.print(".");
    }
    Blynk.begin(auth, ssid, psw);*/
  lux = lightMeter.readLightLevel();
  temp = dht.readTemperature();
  volume = analogRead(MIC);
  // timer.setInterval(500L, process_theatre_info);
  // timer.setInterval(500L, update_screen);
}

void update_values() {
  lux = lightMeter.readLightLevel();
  temp = dht.readTemperature();
  volume = analogRead(MIC);
}
/*
  BLYNK_WRITE(V1) {
  if (param.asInt()) {
    turn_on_fan();
  } else {
    turn_on_fan();
  }
  }

  BLYNK_WRITE(V5) {
  if (param.asInt()) {
    turn_on_spots();
  } else {
    turn_off_spots();
  }
  }
*/
void lift_curtain() {
  stepper.step(-STEPS);
}

void lower_curtain() {
  stepper.step(STEPS);
}

void turn_on_fan() {
  digitalWrite(FAN, HIGH);
  //  Blynk.virtualWrite(V0, HIGH);
}

void turn_off_fan() {
  digitalWrite(FAN, LOW);
  //  Blynk.virtualWrite(V0, LOW);
}

void turn_on_spots() {
  digitalWrite(LIGHTS, HIGH);
  //  Blynk.virtualWrite(V5, HIGH);
}

void turn_off_spots() {
  digitalWrite(LIGHTS, LOW);
//  Blynk.virtualWrite(V5, LOW);
}

void update_screen() {
  screen.clearDisplay();
  screen.setTextSize(1);
  screen.setTextColor(WHITE);
  screen.setCursor(0, 0);
  screen.print("Lux= ");
  screen.println(lux);

  screen.print("Temp= ");
  screen.print(temp);
  screen.println(" °C");

  if (volume <= 800) {
    screen.println("People talk");
  } else {
    screen.println("People are quiet");
  }
  screen.display();
/*  Blynk.virtualWrite(V2, lux);
  Blynk.virtualWrite(V3, temp);
  Blynk.virtualWrite(V4, volume);*/
}

void process_theatre_info() {
  if (temp >= 20) {
    turn_on_fan();
  } else {
    turn_off_fan();
  }
  if (!play_on_stage && volume <= 800) {
    play_on_stage = true;
    turn_on_spots();
    lift_curtain();
  } else if (play_on_stage && volume <= 800) {
    turn_off_spots();
    lower_curtain();
    play_on_stage = false;
  } if (lux < 15) {
    turn_on_spots();
  }
}

void loop() {
/*  Blynk.run();
  timer.run();
*/
update_values();
update_screen();
process_theatre_info  ();
delay(500);
}
