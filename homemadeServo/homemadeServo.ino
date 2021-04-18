// Moteur
#define POS_MIN 14
#define POS_MAX 1010

// Asserv
#define DEAD_ZONE 20

// Input
#define PULSEIN_MIN 1000
#define PULSEIN_MAX 2000

volatile int goalPos = 512;
volatile long rise_date;
volatile long fall_date;

void setup() {
  pinMode(2, INPUT_PULLUP);
  
  EICRA = (1 << ISC00);    // set INT0 to trigger on ANY logic change
  EIMSK = (1 << INT0);     // Turns on INT0
  sei();                   // turn on interrupts
  
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  Serial.begin(115200);
}

ISR (INT0_vect)
{
  if(PIND & (1 << PIND2)) {
    //High
    rise_date = micros();
  } else {
    //Low
    fall_date = micros();
    if(fall_date-rise_date >= PULSEIN_MIN && fall_date-rise_date <= PULSEIN_MAX) {
      goalPos = (fall_date - rise_date - PULSEIN_MIN) * 1024 / (PULSEIN_MAX - PULSEIN_MIN);

      //limit goal pos
      goalPos = goalPos > POS_MAX ? POS_MAX : goalPos;
      goalPos = goalPos < POS_MIN ? POS_MIN : goalPos;
    }
  }
}

void loop() {
  int pos = analogRead(1);
  
  digitalWrite(3, (pos > (goalPos + DEAD_ZONE)) ? HIGH : LOW);
  digitalWrite(4, (pos < (goalPos - DEAD_ZONE)) ? HIGH : LOW);
}
