#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define TRIG 8
#define ECHO 9

void setup() {
  Serial.begin(9600);
  Serial.println("Initializing DHT11 + HC-SR04 Test...");
  dht.begin();
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  delay(2000);
}

void loop() {
  // Read Temperature and Humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check DHT validity
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT11 Sensor error! Check wiring.");
  } else {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print(" °C  |  Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
  }

  // Ultrasonic distance measurement
  long duration;
  float distance;

  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  duration = pulseIn(ECHO, HIGH);
  distance = (duration * 0.0343) / 2;  // Convert to cm

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  Serial.println("-----------------------------");
  delay(2000);
}
