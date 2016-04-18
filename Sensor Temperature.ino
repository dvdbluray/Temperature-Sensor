#include <LiquidCrystal.h>
#define DEBUG 0 // No Debug
#define PIEZO 9
#define LCD_DB7 2
#define LCD_DB6 3
#define LCD_DB5 4 
#define LCD_DB4 5
#define LCD_BACKLIGHT 10
#define LCD_E 11
#define LCD_RS 12
#define PIN_SENSOR A0
#define STANDARD_TEMP 23
#define CRITIC_TEMP 24
#define MAX_BRIGHTNESS 128
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);
bool flag = 0;
//Initially lastTemperature set to 0.
float lastTemperature = 0;

float* getVoltageAndTemperature(int sensor) {
    float values[2];
    values[0] = (sensor / 1024.0) * 5.0;
    values[1] = (values[0] - .5) * 100;
    return values;
}

void changePin(int pin_a, int pin_b, int pin_c) {
    digitalWrite(pin_a, HIGH);
    digitalWrite(pin_b, LOW);
    digitalWrite(pin_c, LOW);
}

void setup() {
    Serial.begin(9600);
    for(int pin_number = 6; pin_number < 9; pin_number++) {
        pinMode(pin_number, OUTPUT);
        digitalWrite(pin_number, HIGH);
    }
    lcd.begin(16,2);
    pinMode(LCD_BACKLIGHT, OUTPUT);
    analogWrite(LCD_BACKLIGHT, MAX_BRIGHTNESS);
    lcd.print("Setup in corso.");
    lcd.setCursor(0,1);
    lcd.print("Attendere... ");
    delay(1000);
}

void loop() {
    int sensorValue = analogRead(PIN_SENSOR);
    uint32_t sec = 1000;
    int notes[] = {262, 294, 330};
    float* value = getVoltageAndTemperature(sensorValue);
    float voltage = *value;
    float temperature = *(value+1);
    
    if (DEBUG) {
        Serial.print("Sensor Value: ");
        Serial.println(sensorValue);
        Serial.print("Volts: ");
        Serial.println(voltage);
        Serial.print("Celsius: ");
        Serial.println(temperature);
        delay(1000);
    }
    
    if(temperature <= STANDARD_TEMP) {
        changePin(6,7,8);
        if (lastTemperature > STANDARD_TEMP) {
            flag = 0;
        }
        if (!flag) {
            tone(PIEZO, notes[0], sec);
            flag = 1;
        }
    }
    else if((temperature > STANDARD_TEMP) && (temperature < CRITIC_TEMP)) {
        changePin(7,8,6);
        if ((lastTemperature < STANDARD_TEMP) || (lastTemperature > CRITIC_TEMP)) {
            flag = 0;
        }
        if (!flag) {
            tone(PIEZO, notes[1], sec);
            flag = 1;
        }
    }
    else {
        changePin(8,7,6);
        if (lastTemperature < CRITIC_TEMP) {
            flag = 0;
        }
        if (!flag) {
            tone(PIEZO, notes[2], sec);
            flag = 1;
        }
    }
    lastTemperature = temperature;
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Gradi: "); lcd.print(temperature, 2);
    lcd.setCursor(0,1);
    lcd.print("Volt: "); lcd.print(voltage, 2);
    delay(4000);
  }
