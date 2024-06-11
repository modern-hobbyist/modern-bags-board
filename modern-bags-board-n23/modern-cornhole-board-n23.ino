/**
 * Author Charlie Steenhagen - Modern Hobbyist
 * https://www.modhobbyist.com
 * 
 * This code is for the Rotating Cornhole board and it operates using a TMC2209 
 * stepper motor driver, a nema 23 stepper motor, and an arduino nano.
 *
 * There are three different game modes including an INIT state
 * 1. Constant Speed - A constant rotation that can be sped up/slowed down with +/- buttons
 *.   and direction can be changed by pressing + & - together
 * 2. Position Hold - At the start of each round, the board moves to a new random location.
 *    Press the + button to advance positions
 * 3. Chaos - Random distance in a random direction at a random speed. 
 */

#include <TMCStepper.h>
#include <AccelStepper.h>
#include <ezButton.h>

#define EN_PIN           2 // Enable
#define MS1 3       // Pin 25 connected to MS1 pin
#define MS2 4       // Pin 26 connected to MS2 pin
#define STEP_PIN         5 // Step
#define DIR_PIN          6 // Direction
#define SW_RX            40 // TMC2208/TMC2224 SoftwareSerial receive pin
#define SW_TX            41 // TMC2208/TMC2224 SoftwareSerial transmit pin
#define SERIAL_PORT Serial // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS 0b00 // TMC2209 Driver address according to MS1 and MS2
#define INC_AMOUNT 100

#define UP_PIN 7
#define MODE_BTN_PIN 8
#define DOWN_PIN 9
#define NEMA_STEPS 200
#define MICRO_STEPS 8
#define STEPS_PER_REVOLUTION NEMA_STEPS * MICRO_STEPS
#define DEFAULT_SPEED 2000
#define MAX_SPEED 8000
#define MAX_ACCEL_SPEED 4000
#define MIN_SPEED 100
#define MIN_RANDOM_SPEED 1000
#define INC_AMOUNT 500
#define STEPS_PER_BEARING_REVOLUTION 16.5 * STEPS_PER_REVOLUTION //99 teeth on bearing / 6 teeth on gear * steps per revolution for stepper

#define R_SENSE 0.11f // Match to your driver
                      // SilentStepStick series use 0.11
                      // UltiMachine Einsy and Archim2 boards use 0.2
                      // Panucatt BSD2660 uses 0.1
                      // Watterott TMC5160 uses 0.075
TMC2209Stepper driver(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS);

AccelStepper stepper = AccelStepper(stepper.DRIVER, STEP_PIN, DIR_PIN);

enum : byte {INIT, CONSTANT, POSITION_HOLD, CHAOS} gameMode = INIT;

ezButton* upButton;
ezButton* downButton;
ezButton* modeButton;
bool upPress = false;
bool downPress = false;
bool modePress = false;
bool upRelease = false;
bool downRelease = false;
bool modeRelease = false;

int speed = DEFAULT_SPEED;
bool direction = true;

void setup() {
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);      // Enable driver in hardware
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);

  upButton = new ezButton(UP_PIN);
  downButton = new ezButton(DOWN_PIN);
  modeButton = new ezButton(MODE_BTN_PIN);
  
  upButton->setDebounceTime(50);
  downButton->setDebounceTime(50);
  modeButton->setDebounceTime(50);
  
  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);

  Serial.begin(115200);
  driver.begin();                 //  SPI: Init CS pins and possible SW SPI pins
                                  // UART: Init SW UART (if selected) with default 115200 baudrate
  driver.toff(5);                 // Enables driver in software
  driver.rms_current(2000);        // Set motor RMS current (adjusted for Nema 23, check your motor specs)
  driver.microsteps(8);          // Set microsteps to 1/8th

  driver.pwm_autoscale(true);     // Needed for stealthChop

  stepper.setMaxSpeed(MAX_SPEED); 
  stepper.setAcceleration(9000); 
  stepper.setEnablePin(EN_PIN);
  stepper.setPinsInverted(false, false, true);
  stepper.enableOutputs();
}

bool left = false;

void loop() {
  readDownButton();
  readUpButton();
  readModeButton();

  if(modeRelease) {
    speed = DEFAULT_SPEED;
    switchGameMode();
  }

  if(gameMode == INIT) {
    stepper.setSpeed(0);
    stepper.setMaxSpeed(MAX_SPEED);
    stepper.disableOutputs(); // Turn off the motor
    direction = true;
  } else if(gameMode == CONSTANT) {
    if(upPress && downPress) {
      // TODO decide if I like this direction change
      direction = !direction;

      // Clearing these cause we've read them for our purposes.
      upPress = false;
      downPress = false;
    } else if(upRelease) {
      speed += INC_AMOUNT;
      if(speed > MAX_SPEED){
        speed = MAX_SPEED;
      }
    } else if (downRelease) {
      speed -= INC_AMOUNT;
      if(speed < MIN_SPEED){
        speed = MIN_SPEED;
      }
    }
  } else if ( gameMode == POSITION_HOLD) {
    // Game mode where the board moves to a static position each throw
    if(upRelease || modeRelease) {      
      int randomSteps = random(STEPS_PER_BEARING_REVOLUTION / 4, STEPS_PER_BEARING_REVOLUTION);
      stepper.setMaxSpeed(MAX_SPEED);
      stepper.moveTo(randomSteps);
    }
  } else if (gameMode == CHAOS) {
    // Game mode where the board moves a random distance in a random direction at a random speed, repeatedly.
    if(stepper.distanceToGo() == 0){
      int randomSpeed = random(MIN_RANDOM_SPEED,MAX_ACCEL_SPEED);
      int randomSteps = random(STEPS_PER_BEARING_REVOLUTION / 5, STEPS_PER_BEARING_REVOLUTION);
      int newDirection = random(2);

      if(newDirection == 0) {
        randomSteps *= -1;
        randomSpeed *= -1;
      }

      speed = randomSpeed;

      stepper.setMaxSpeed(speed);
      stepper.setSpeed(speed);
      stepper.move(randomSteps);
    }
  }

  // Where the actual movements are executed
  if(gameMode == CONSTANT) {
    if(!direction){
      stepper.setSpeed(-1 * speed);
    }else{
      stepper.setSpeed(speed);
    }

    // Using this for the constant mode so we can have higher speeds.
    stepper.runSpeed();
  } else if(stepper.distanceToGo() != 0) {
    // Using this for 2nd two game modes so we can utilize acceleration.
    stepper.run();
  } 
  
}

void readDownButton(){
  downButton->loop();
  downRelease = false;
  
  if(downButton->isPressed()){
    downPress = true;
  }

  if(downButton->isReleased()){
    //Do quick press action 
    downRelease = true;
    downPress = false;
  }
}

void readUpButton(){
  upButton->loop();
  upRelease = false;
  
  if(upButton->isPressed()){
    upPress = true;
  }

  if(upButton->isReleased()){
    //Do quick press action 
    upRelease = true;
    upPress = false;
  }
}

void readModeButton(){
  modeButton->loop();
  modeRelease = false;
  
  if(modeButton->isPressed()){
    modePress = true;
  }

  if(modeButton->isReleased()){
    //Do quick press action 
    modeRelease = true;
    modePress = false;
  }
}

void switchGameMode() {
  stepper.setCurrentPosition(0);
  stepper.stop();

  // Toggle between game modes
  switch (gameMode) {
    case INIT:
      // Switching to CONSTANT
      gameMode = CONSTANT;
      stepper.enableOutputs();
      speed = DEFAULT_SPEED;
      break;
    case CONSTANT:
      // Switching to POSITION_HOLD 
      gameMode = POSITION_HOLD;
      speed = MAX_SPEED;
      break;
    case POSITION_HOLD:
      // Switching to RANDOM
      gameMode = CHAOS;
      break;
    case CHAOS:
      // Switching to INIT
      gameMode = INIT;
      break;
  }
}
