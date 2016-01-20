#include <avr/interrupt.h>
#include <avr/sleep.h>

#define PWMPINPOS 11
#define PWMPINNEG 3
#define BOOSTPIN 10
#define SENSORPIN A4
#define LEDPIN 19
#define COUNT 10

void setup() {
  pinMode(PWMPINPOS, OUTPUT); 
  pinMode(PWMPINNEG, OUTPUT);
  digitalWrite(PWMPINNEG, LOW);
  digitalWrite(PWMPINPOS, LOW);
  pinMode(LEDPIN, OUTPUT);

  // Set H-bridge PWM prescale to 1
  TCCR2B&=~0b111;
  TCCR2B|=3;
  // Set Boost PWM prescale to 1
  TCCR1B&=~0b111;
  TCCR1B|=3;
  // Add 1ms compare interrupt to timer 1
  OCR0A = 0xAF; // Leet AF
  TIMSK0 |= _BV(OCIE0A);
  
  Serial.begin(9600);
  digitalWrite(LEDPIN, HIGH);
  analogWrite(BOOSTPIN, 0);
  //analogWrite(PWMPINNEG, 127);
}


SIGNAL(TIMER0_COMPA_vect) {
  static unsigned int idx = 0;
  static unsigned int pin = 0;
  //floor(255.*sin(linspace(0,pi,25)))'
  const uint8_t sine256[COUNT]  = {78, 149, 206, 242, 255, 242, 206, 149, 78, 0};
  // iterate through PWM "amplitudes"
  if(idx==0) pin=!pin;
  analogWrite((pin ? PWMPINPOS : PWMPINNEG), sine256[idx%COUNT]);
  //analogWrite(PWMPINPOS, sine256[idx]);
  
  idx = (idx+1)%10;
}

void boost() {
  static uint8_t i = 0;
  // read the value from the sensor:
  float sensorValue = analogRead(SENSORPIN);
  float voltage = sensorValue*50.0/1024.0;
  if(voltage>15 && i>1) {
    i--;
  } else if(voltage<15 && i<200) {
    i++;
  }
  Serial.println(i);
  Serial.println(voltage);
  analogWrite(BOOSTPIN, i);
}

void loop() {
  Serial.println("Hallo wreld");
  delay(100);
  boost();
    /*set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_cpu();
    sleep_disable();*/
}

