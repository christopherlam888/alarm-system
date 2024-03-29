#include <SoftwareSerial.h>
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>

// pin variables
const int LED_RED =3;
const int LED_GREEN = 4;
const int LED_BLUE = 5;
const int BUZZER = 6;
const int ALARM = 7; 
const int MOTION_SENSOR = 2;
const int FINGERPRINT_IN = 8;
const int FINGERPRINT_OUT = 9;
const int BUTTON = 10;
const int LCD_RS = A5, LCD_EN = A4, LCD_D4 = A3, LCD_D5 = A2, LCD_D6 = A1, LCD_D7 = A0;

bool armed = false; // armed state
int motion_state = LOW; // get motion
bool triggered = false; // triggered state
bool message_displayed = false; // message on lcd screen

// define the software serial pins
SoftwareSerial mySerial(FINGERPRINT_IN, FINGERPRINT_OUT);

// initialize the fingerprint sensor
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// initialize the LCD
LiquidCrystal LCD(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

void setup() {

  // initialize serial
  Serial.begin(9600);
  
  // initialize pin inputs and outputs
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(ALARM, OUTPUT);
  pinMode(MOTION_SENSOR, INPUT);  
  pinMode(BUTTON, INPUT);

  // start the software serial communication
  mySerial.begin(57600);
  // initialize the fingerprint sensor
  finger.begin(57600);
  // get fingerprints from memory
  finger.getTemplateCount();
  
  // lcd setup
  LCD.begin(16, 2);

}

void loop() {

  // add fingerprint
  if (finger.templateCount == 0 || (getFingerprintID() == true && digitalRead(BUTTON))) {

    // set outputs
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("SET NEW");
    LCD.setCursor(0, 1);
    LCD.print("FINGERPRINT");
    LCD.setCursor(0, 0);

    // get fingerprint
    getFingerprintEnroll();
    finger.getTemplateCount();

    message_displayed = false;
    LCD.clear();

  }

  // get fingerprint
  if (getFingerprintID() == true){ 
    // toggle armed state
    if (armed) {
      armed = false;
      message_displayed = false;
      LCD.clear();
    }
    else {
      armed = true;
      message_displayed = false;
      LCD.clear();
    }
  }

  // armed functionality
  if (armed) {

    // set outputs
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
    noTone(ALARM);
    noTone(BUZZER);
    if (!message_displayed) {
      LCD.print("ARMED");
      message_displayed = true;
    }

    // read motion
    motion_state = digitalRead(MOTION_SENSOR);

    // if motion detected
    if (motion_state == HIGH) {

      triggered = true; // triggered state
      message_displayed = false;
      LCD.clear();

      int timer = 0; // initialize timer

      // time 30 seconds or until disarmed
      while (timer < 30 && triggered == true){
        
        // set outputs
        digitalWrite(LED_RED, HIGH);
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_BLUE, LOW);
        tone(BUZZER, 540);
        if (!message_displayed) {
          LCD.print("TRIGGERED!");
          message_displayed = true;          
        }

        // get fingerprint to disarm
        if (getFingerprintID() == true){ 
          triggered = false; // not triggered state
          armed = false; // not armed state
          message_displayed = false;
          LCD.clear();
        }

        delay(1000); // wait one second
        timer++; // increment the timer

      }
      // still not disarmed after 30 seconds
      if (timer == 30){        
        // until disarmed
        while (triggered == true){
          
          // set outputs
          noTone(BUZZER);
          tone(ALARM, 540);
          delay(750);
          tone(ALARM, 660);
          delay(750);
          
          // get fingerprint to disarm
          if (getFingerprintID() == true){ 
            triggered = false; // not triggered state
            armed = false; // triggered state
            message_displayed = false;
            LCD.clear();
          }

        }        
      }
    }
  }
  // not armed functionality
  else {
    // set outputs    
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
    noTone(ALARM);
    noTone(BUZZER);
    if (!message_displayed) {
      LCD.print("READY");
      message_displayed = true;
    }
  }
}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;Waiting for valid finger to enroll as #
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(1);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(1);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(1);
  p = finger.storeModel(1);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
   
  return true;
}
