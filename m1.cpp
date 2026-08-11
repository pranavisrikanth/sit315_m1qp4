const int BTN1 = 2;
const int BTN2 = 3;
const int BTN3 = 4;
const int LED1 = 8;

volatile uint8_t lastPortD = 0;
volatile bool btn1Flag = false, btn2Flag = false, btn3Flag = false;
volatile bool timerFlag = false;

void setup() {
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(LED1, OUTPUT);
  Serial.begin(9600);

  PCICR |= (1 << PCIE2);
  PCMSK2 |= (1 << PCINT18);
  PCMSK2 |= (1 << PCINT19);
  PCMSK2 |= (1 << PCINT20);
  lastPortD = PIND;

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 15624;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

ISR(PCINT2_vect) {
  uint8_t currentPortD = PIND;
  uint8_t changed = currentPortD ^ lastPortD;

  if (changed & (1 << PD2)) btn1Flag = true;
  if (changed & (1 << PD3)) btn2Flag = true;
  if (changed & (1 << PD4)) btn3Flag = true;

  lastPortD = currentPortD;
}

ISR(TIMER1_COMPA_vect) {
  timerFlag = true;
}

void loop() {
  if (btn1Flag) {
    btn1Flag = false;
    Serial.println("PCINT: Button 1 changed");
  }
  if (btn2Flag) {
    btn2Flag = false;
    Serial.println("PCINT: Button 2 changed");
  }
  if (btn3Flag) {
    btn3Flag = false;
    Serial.println("PCINT: Button 3 changed");
  }

  bool b1 = digitalRead(BTN1) == LOW;
  bool b2 = digitalRead(BTN2) == LOW;
  bool b3 = digitalRead(BTN3) == LOW;
  digitalWrite(LED1, (b1 || b2 || b3) ? HIGH : LOW);

  if (timerFlag) {
    timerFlag = false;
    static int seconds = 0;
    seconds++;
    Serial.print("Timer: periodic tick # ");
    Serial.println(seconds);
  }
}