// pin variables
const int LED_GREEN = 2;
const int LED_YELLOW = 3;
const int LED_RED = 4;
const int BUZZER = 5;
const int ALARM = 6; 
const int MOTION_SENSOR = 7;
const int BUTTON_ARM = 8;
const int BUTTON_TRIGGER = 9;

bool armed = false; // armed state
int motion_state = LOW; // get motion
bool triggered = false; // triggered state

void setup() {
  // initialize serial
  Serial.begin(9600);
  // initialize pin inputs and outputs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(ALARM, OUTPUT);
  pinMode(MOTION_SENSOR, INPUT);
  pinMode(BUTTON_ARM, INPUT);
  pinMode(BUTTON_TRIGGER, INPUT);
}

void loop() {

  // get button press
  if (digitalRead(BUTTON_ARM) == HIGH){ 
    // toggle armed state
    if (armed) {
      armed = false;
    }
    else {
      armed = true;
    }
  }

  // armed functionality
  if (armed) {

    // set outputs
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_YELLOW, HIGH);
    digitalWrite(LED_RED, LOW);
    noTone(ALARM);
    noTone(BUZZER);

    // read motion
    motion_state = digitalRead(MOTION_SENSOR);
    // if motion detected
    if (motion_state == HIGH) {
      triggered = true; // triggered state
      int timer = 0; // initialize timer
      // time 30 seconds or until disarmed
      while (timer < 30 && triggered == true){
        // set outputs
        digitalWrite(LED_RED, HIGH);
        tone(BUZZER, 540);
        // get button press to disarm
        if (digitalRead(BUTTON_TRIGGER) == HIGH){
          triggered = false; // not triggered state
          armed = false; // not armed state
        }
        delay(1000); // wait one second
        timer++; // increment the timer
      }
      // still not disarmed after 30 seconds
      if (timer == 30){
        // until disarmed
        while (triggered == true){
          // set outputs
          digitalWrite(LED_RED, HIGH);
          noTone(BUZZER);
          tone(ALARM, 540);
          delay(750);
          tone(ALARM, 660);
          delay(750);
          // get button press to disarm
          if (digitalRead(BUTTON_TRIGGER) == HIGH){
            triggered = false; // not triggered state
            armed = false; // triggered state
          }
        }        
      }
    }
  }
  // not armed functionality
  else {

    // set outputs    
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_YELLOW, LOW);
    digitalWrite(LED_RED, LOW);
    noTone(ALARM);
    noTone(BUZZER);

  }

}
