// Define the control inputs
#define MOT_A1_PIN 5
#define MOT_A2_PIN 6
#define MOT_B1_PIN 11
#define MOT_B2_PIN 10

void setup(void)
{
  // Set all the motor control inputs to OUTPUT
  pinMode(MOT_A1_PIN, OUTPUT);
  pinMode(MOT_A2_PIN, OUTPUT);
  pinMode(MOT_B1_PIN, OUTPUT);
  pinMode(MOT_B2_PIN, OUTPUT);

  // Turn off motors - Initial state
  digitalWrite(MOT_A1_PIN, LOW);
  digitalWrite(MOT_A2_PIN, LOW);
  digitalWrite(MOT_B1_PIN, LOW);
  digitalWrite(MOT_B2_PIN, LOW);

  // Initialize the serial UART at 9600 baud
  Serial.begin(9600);
}

void loop(void)
{
  // Motoren van stop naar max snelheid.
  for (int i = 0; i < 256; i++) {
    spin_and_wait(i, i, 25);
  }
  // max snelheid.
  spin_and_wait(255,255,2000);

  // Motoren van max snelheid naar stop.
  for (int i = 0; i < 256 ; i++) {
    spin_and_wait(255 - i, 255 - i, 25);
  }

  // max snelheid reverse.
  spin_and_wait(-255,-255,2000);

  // Stop.
  spin_and_wait(0,0,2000);

  // Max snelheid met motoren in andere richting.
  spin_and_wait(-255, 255, 2000);
  spin_and_wait(0, 0, 1000);
  spin_and_wait(255, -255, 2000);
  spin_and_wait(0, 0, 1000);
}

//Zet de juiste pin low als de richting veranderd.
void set_motor_pwm(int pwm, int IN1_PIN, int IN2_PIN)
{
  if (pwm < 0) {  // reverse speeds
    analogWrite(IN1_PIN, -pwm);
    digitalWrite(IN2_PIN, LOW);

  } else { // stop or forward
    digitalWrite(IN1_PIN, LOW);
    analogWrite(IN2_PIN, pwm);
  }
}

//Geeft weer welke snelheid wordt doorgestuurd.
void set_motor_currents(int pwm_A, int pwm_B)
{
  set_motor_pwm(pwm_A, MOT_A1_PIN, MOT_A2_PIN);
  set_motor_pwm(pwm_B, MOT_B1_PIN, MOT_B2_PIN);

  // Print a status message to the console.
  Serial.print("Set motor A PWM = ");
  Serial.print(pwm_A);
  Serial.print(" motor B PWM = ");
  Serial.println(pwm_B);
}

//De functie waarin we de motorsnelheid van de 2 motoren kunnen doorsturen.
void spin_and_wait(int pwm_A, int pwm_B, int duration)
{
  set_motor_currents(pwm_A, pwm_B);
  delay(duration);
}