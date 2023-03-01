#include <pm2008_i2c.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <DHT.h>
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
PM2008_I2C pm2008_i2c;
LiquidCrystal_I2C lcd(0x3F, 16, 2);
int BUTTON1 = 7;
int BUTTON2 = 5;
int count = 0;
int measure = 0;
int buttonState1;
int buttonState2;

float Rtemp;
float temperature;
float reading;

byte u[8] = {
  B00000,
  B10001,
  B10001,
  B10001,
  B11001,
  B10110,
  B10000,
  B00000
};
byte c[8] = {
  B01000,
  B10100,
  B01000,
  B00110,
  B01000,
  B01000,
  B00110,
  B00000
};

void setup() {
  pm2008_i2c.begin();
  pm2008_i2c.command();
  dht.begin();
  pinMode(4, INPUT_PULLUP);
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  int buttonState1 = digitalRead(BUTTON1);
  int buttonState2 = digitalRead(BUTTON2);
  lcdSetup();
  Serial.begin(9600);
}

int lcdSetup()
{
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, u);
  lcd.createChar(1, c);
  lcd.setCursor(4, 0);
  lcd.write(0);
  lcd.print("g/m3");
  lcd.setCursor(10, 0);
  lcd.print("PM");
  lcd.setCursor(0, 1);
  lcd.print("H:");
  lcd.setCursor(9, 1);
  lcd.print("T:");
}
float pm1()
{
  if (measure == 0) {
    lcd.setCursor(0, 0);
    lcd.print(pm2008_i2c.pm1p0_grimm);
    lcd.setCursor(12, 0);
    lcd.print("1.0G");
    Serial.print(pm2008_i2c.pm1p0_grimm);
    Serial.print("ug/m3");
    Serial.print("\t");
    Serial.println("1.0 Grimm");
  }
  else if (measure == 1) {
    lcd.setCursor(0, 0);
    lcd.print(pm2008_i2c.pm1p0_tsi);
    lcd.setCursor(12, 0);
    lcd.print("1.0T");
    Serial.print(pm2008_i2c.pm1p0_tsi);
    Serial.print("ug/m3");
    Serial.print("\t");
    Serial.println("1.0 Tsi");
  }
}
float pm25() {
  if (measure == 0) {
    lcd.setCursor(0, 0);
    lcd.print(pm2008_i2c.pm2p5_grimm);
    lcd.setCursor(12, 0);
    lcd.print("2.5G");
    Serial.print(pm2008_i2c.pm2p5_grimm);
    Serial.print("ug/m3");
    Serial.print("\t");
    Serial.println("2.5 Grimm");
  }
  else if (measure == 1) {
    lcd.setCursor(0, 0);
    lcd.print(pm2008_i2c.pm2p5_tsi);
    lcd.setCursor(12, 0);
    lcd.print("2.5T");
    Serial.print(pm2008_i2c.pm2p5_tsi);
    Serial.print("ug/m3");
    Serial.print("\t");
    Serial.println("2.5 Tsi");
  }
}
float pm10() {
  if (measure == 0) {
    lcd.setCursor(0, 0);
    lcd.print(pm2008_i2c.pm10_grimm);
    lcd.setCursor(12, 0);
    lcd.print("1 0G");
    Serial.print(pm2008_i2c.pm10_grimm);
    Serial.print("ug/m3");
    Serial.print("\t");
    Serial.println("10 Grimm");
  }
  else if (measure == 1) {
    lcd.setCursor(0, 0);
    lcd.print(pm2008_i2c.pm10_tsi);
    lcd.setCursor(12, 0);
    lcd.print("1 0T");
    Serial.print(pm2008_i2c.pm10_tsi);
    Serial.print("ug/m3");
    Serial.print("\t");
    Serial.println("10 Tsi");
  }
}

float temp()
{
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  lcd.setCursor(3, 1);
  lcd.print(humi, 1);
  lcd.print("% ");
  lcd.setCursor(11, 1);
  lcd.print(Rtemp, 1);
  lcd.write(1);
}

float stemp()
{
  float humi = dht.readHumidity();
  float temp = dht.readTemperature();
  Serial.print(humi, 1);
  Serial.print("%\t ");
  Serial.print(temp, 1);
  Serial.print("C");
  Serial.print("\t  ");
}
int lm35()
{
  Rtemp = 0;
  reading = analogRead (A0);
  float Otemp = (5.0*reading*100) / 1024;
  Rtemp += Otemp;
  delay(100);
  float Ntemp = (5.0*reading*100) / 1024;
  (Rtemp += Ntemp)/=2;
  Serial.print("lm35:");
  Serial.print(Rtemp, 2);
  Serial.println("C");

}
int check()
{
  if (buttonState1 == LOW) {
    count += 1;
    delay(20);
  }
  if (buttonState2 == LOW) {
    measure += 1;
    delay(20);
  }
  delay(500);
  if (count == 0) {
    pm25();
    stemp();
  }
  else if (count == 1) {
    pm10();
    stemp();
  }
  else if (count == 2) {
    pm1();
    stemp();
  }
  if (count >= 3) {
    count = 0;
  }
  if (measure >= 2) {
    measure = 0;
  }
}
void loop() {
  buttonState1 = digitalRead(BUTTON1);
  buttonState2 = digitalRead(BUTTON2);
  uint8_t ret = pm2008_i2c.read();
  check();
  temp();
  lm35();
}
