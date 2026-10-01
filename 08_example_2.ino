// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10

#define _DIST_MIN 100.0
#define _DIST_MID 200.0
#define _DIST_MAX 300.0

#define TIMEOUT ((INTERVAL * 1000UL) / 2)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time = 0;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);

  // LED OFF
  analogWrite(PIN_LED, 255);

  Serial.begin(57600);
}

void loop()
{
  float distance;
  int led_value;

  // 25ms sampling
  if ((millis() - last_sampling_time) < INTERVAL)
    return;

  last_sampling_time += INTERVAL;

  // distance measurement
  distance = USS_measure(PIN_TRIG, PIN_ECHO);

  // -----------------------------
  // LED brightness control
  // -----------------------------

  if ((distance == 0.0) ||
      (distance <= _DIST_MIN) ||
      (distance >= _DIST_MAX))
  {
    // 100mm 이하 또는 300mm 이상
    led_value = 255;       // LED OFF
  }
  else if (distance <= _DIST_MID)
  {
    // 100mm -> 200mm
    // OFF -> 최대 밝기

    led_value = (int)(
      255.0 -
      ((distance - _DIST_MIN) /
       (_DIST_MID - _DIST_MIN) * 255.0)
    );
  }
  else
  {
    // 200mm -> 300mm
    // 최대 밝기 -> OFF

    led_value = (int)(
      ((distance - _DIST_MID) /
       (_DIST_MAX - _DIST_MID)) * 255.0
    );
  }

  analogWrite(PIN_LED, led_value);


  // -----------------------------
  // Serial Plotter
  // -----------------------------
  Serial.print("MIN:");
  Serial.print(_DIST_MIN);

  Serial.print("\tDIST:");
  Serial.print(distance);

  Serial.print("\tMID:");
  Serial.print(_DIST_MID);

  Serial.print("\tMAX:");
  Serial.println(_DIST_MAX);
}


// get distance in millimeter
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
