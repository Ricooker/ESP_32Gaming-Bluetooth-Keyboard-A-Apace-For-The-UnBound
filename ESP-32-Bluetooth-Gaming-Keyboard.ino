//masukkan library
#include <BleKeyboard.h>

//tentukan esp bakal terbaca oleh device
BleKeyboard bleKeyboard("ESP32_ASFTU", "MyKeyboard", 100) ;

//definisikan joystick
#define VRX 34
#define VRY 35
#define JS_SW 25

//definisikan 7 custom tombol
#define BTNQ 19
#define BTNE 18 
#define BTNTAB 14 
#define BTNC 13 
#define BTNLSHIFT 5                               
#define BTNX 4 
#define BTNZ 2

//definisikan potensio untuk volume
#define VOL_PIN 32

//untuk joystick
int thresholdLow = 1500;
int thresholdHigh = 2500;
const int numReadings = 10;
int readings[numReadings];
int readIndex = 0;
int total = 0;
//untuk volume
int lastVolumePercent = -1;

void setup() { 
    Serial.begin(115200);

    //input joystick
    pinMode(VRX, INPUT);
    pinMode(VRY, INPUT);
    pinMode(JS_SW, INPUT_PULLUP);

    //input potensio
    pinMode(VOL_PIN, INPUT);

    //input tombol
    pinMode(BTNQ, INPUT_PULLUP);
    pinMode(BTNE, INPUT_PULLUP);
    pinMode(BTNTAB, INPUT_PULLUP);
    pinMode(BTNC, INPUT_PULLUP);
    pinMode(BTNLSHIFT, INPUT_PULLUP);
    pinMode(BTNX, INPUT_PULLUP);
    pinMode(BTNZ, INPUT_PULLUP);

    //untuk mengendalikan volume
    for (int i = 0; i < numReadings; i++) {
        readings[i] = 0;
    }

    bleKeyboard.begin();
}

void loop() {
  if (bleKeyboard.isConnected()) {
  
  //baca analog joystick
  int xVal = analogRead(VRX);
  int yVal = analogRead(VRY);

  //baca analog potensio
  int volumeVal = analogRead(VOL_PIN); // Baca nilai analog

  //melepaskan semua tombol
  bleKeyboard.release(KEY_UP_ARROW);
  bleKeyboard.release(KEY_DOWN_ARROW);
  bleKeyboard.release(KEY_LEFT_ARROW);
  bleKeyboard.release(KEY_RIGHT_ARROW);

  //hitung jarak simpangan
  int xDeviation = abs (xVal - 2048);
  int yDeviation = abs(yVal - 2048);

  //menentukan mana yang ditekan
  if (xDeviation > thresholdLow || yDeviation > thresholdLow) {
    if (xDeviation > yDeviation) {
      if (xVal < thresholdLow) {
        bleKeyboard.press(KEY_UP_ARROW);
        Serial.println("Atas");
      } else if (xVal > thresholdHigh) {
        bleKeyboard.press(KEY_DOWN_ARROW);
        Serial.println("Bawah");
      }
    } else {
      if (yVal > thresholdHigh) {
        bleKeyboard.press(KEY_LEFT_ARROW);
        Serial.println("Kiri");
        } else if (yVal < thresholdLow) {
          bleKeyboard.press(KEY_RIGHT_ARROW);
          Serial.println("Kanan");
        }
      }
    }
  
  //baca analog tekan jadi spacebar
  if (digitalRead(JS_SW) == LOW) {
  bleKeyboard.press(' ');
  Serial.println("tekan");
  } else {
    bleKeyboard.release(' ');
    }

  //baca tekan pada masing masing tombol
  if (digitalRead(BTNQ) == LOW) {
    bleKeyboard.press('q');
  } else {
    bleKeyboard.release('q');
  }

  if (digitalRead(BTNE) == LOW) {
    bleKeyboard.press('e');
  } else {
    bleKeyboard.release('e');
  }

  if (digitalRead(BTNTAB) == LOW) {
    bleKeyboard.press(KEY_TAB);
  } else {
    bleKeyboard.release(KEY_TAB);
  }

  if (digitalRead(BTNC) == LOW) {
    bleKeyboard.press('c');
  } else {
    bleKeyboard.release('c');
  }

  if (digitalRead(BTNLSHIFT) == LOW) {
    bleKeyboard.press(KEY_LEFT_SHIFT);
  } else {
    bleKeyboard.release(KEY_LEFT_SHIFT);
  }

  if (digitalRead(BTNX) == LOW) {
    bleKeyboard.press('x');
  } else {
    bleKeyboard.release('x');
  }

  if (digitalRead(BTNZ) == LOW) {
    bleKeyboard.press('z');
  } else {
    bleKeyboard.release('z');
  }

  //untuk mengendalikan volume
  total -= readings[readIndex];
        readings[readIndex] = analogRead(VOL_PIN);
        total += readings[readIndex];
        readIndex = (readIndex + 1) % numReadings;
        
        int averageVolume = total / numReadings;
        int volumePercent = map(averageVolume, 100, 4000, 0, 100); // Kalibrasi dengan margin
        volumePercent = constrain(volumePercent, 0, 100);
        
        Serial.print("Raw: ");
        Serial.print(averageVolume);
        Serial.print(" -> ");
        Serial.print(volumePercent);
        Serial.println("%");

  if (abs(volumePercent - lastVolumePercent) >= 3) {
      int steps = abs(volumePercent - lastVolumePercent);

      for(int i= 0; i< steps; i++){ 
      if (volumePercent > lastVolumePercent) {
        bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
        Serial.println("Volume UP");
      } else {
        bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
        Serial.println("Volume DOWN");
      }
      delay(30);
  }
      lastVolumePercent = volumePercent; // Simpan nilai terakhir
    }
  } 
  delay(50);
  }