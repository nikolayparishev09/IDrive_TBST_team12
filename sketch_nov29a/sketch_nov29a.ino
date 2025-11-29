#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define LED_RED   D1
#define LED_GREEN D2
#define POT_PIN   A0

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_SDA 14  // D5
#define I2C_SCL 12  // D6
#define DISP_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

char signal = 'R'; // сигнал от Python, 'G' за зелен, 'R' за червен

void setup() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, DISP_ADDR)) {
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
}

void loop() {
  // Четене на потенциометъра
  int raw = analogRead(POT_PIN);
  float pot_value = raw / 1023.0; // 0.0 - 1.0

  // Четене на сигнал от Python
  if (Serial.available() > 0) {
    signal = Serial.read();
  }

  // LED логика
  if (pot_value > 0.5) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
  } else {
    if (signal == 'G') {
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_RED, LOW);
    } else {
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
    }
  }

  // OLED дисплей
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print("Potentiometer: ");
  display.println(pot_value, 2);

  display.print("Gesture: ");
  display.println(signal == 'G' ? "Detected" : "None");

  display.display();

  delay(50);
}
