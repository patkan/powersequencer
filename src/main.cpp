#include <Arduino.h>

#define SLED_PIN (13u)
#define TIMEOUT_BUTTON_PRESS (200u)
#define TIME_SWITCH_ON_AUX_CHANNELS (1000u)

/** struct of switch channels */
typedef struct SWITCH_CHANNEL {
  uint8_t gpioPin;   /*!< gpio pin */
  uint8_t buttonPin; /*!< button pin */
  bool switchState;  /*!< whether the switch is enabled */
} SWITCH_CHANNEL_s;

/** maximum number of channels */
#define NUMBER_OF_CHANNELS (4u)

static SWITCH_CHANNEL_s switchChannels[NUMBER_OF_CHANNELS] = {
    {.gpioPin = 12u, .buttonPin = 0u, .switchState = false},
    {.gpioPin = 5u, .buttonPin = 9u, .switchState = false},
    {.gpioPin = 4u, .buttonPin = 10u, .switchState = false},
    {.gpioPin = 15u, .buttonPin = 14u, .switchState = false}};

/** tracks when button0 has been pressed last time */
static unsigned long button0Pressed = 0u;

/** tracks when button1 has been pressed last time */
static unsigned long button1Pressed = 0u;

/** tracks when button2 has been pressed last time */
static unsigned long button2Pressed = 0u;

/** tracks when button3 has been pressed last time */
static unsigned long button3Pressed = 0u;

/** configures the switch channels */
static void configureSwitchChannels(void);

/** sets a switch channel to a certain value */
static void setSwitchChannel(uint8_t channelNumber, bool channelState);

/** flip state of a switch */
static void flipSwitchChannel(uint8_t channelNumber);

/** ISR: handles button 0 */
void IRAM_ATTR ISR_buttonHandler0(void);

/** ISR: handles button 1 */
void IRAM_ATTR ISR_buttonHandler1(void);

/** ISR: handles button 2 */
void IRAM_ATTR ISR_buttonHandler2(void);

/** ISR: handles button 3 */
void IRAM_ATTR ISR_buttonHandler3(void);

/** configure button channels (set as input and attach to ISR) */
static void configureButtons(void);

/** flag indicating that switch channels are initialized */
static bool switchChannelsInitialized = false;

static void configureSwitchChannels(void) {
  for (uint8_t c = 0u; c < NUMBER_OF_CHANNELS; c++) {
    pinMode(switchChannels[c].gpioPin, OUTPUT);
  }
  switchChannelsInitialized = true;
}

static void setSwitchChannel(uint8_t channelNumber, bool channelState) {
  assert(channelNumber < NUMBER_OF_CHANNELS);
  assert(switchChannelsInitialized == true);
  if (channelState == true) {
    digitalWrite(switchChannels[channelNumber].gpioPin, HIGH);
  } else {
    digitalWrite(switchChannels[channelNumber].gpioPin, LOW);
  }
  switchChannels[channelNumber].switchState = channelState;
}

static void flipSwitchChannel(uint8_t channelNumber) {
  assert(channelNumber < NUMBER_OF_CHANNELS);
  bool newSwitchState = !(switchChannels[channelNumber].switchState);
  setSwitchChannel(channelNumber, newSwitchState);
}

void IRAM_ATTR ISR_buttonHandler0(void) {
  unsigned long currentTime = millis();
  if (currentTime - button0Pressed > TIMEOUT_BUTTON_PRESS) {
    flipSwitchChannel(0u);
    button0Pressed = currentTime;
  }
}
void IRAM_ATTR ISR_buttonHandler1(void) {
  unsigned long currentTime = millis();
  if (currentTime - button1Pressed > TIMEOUT_BUTTON_PRESS) {
    flipSwitchChannel(1u);
    button1Pressed = currentTime;
  }
}
void IRAM_ATTR ISR_buttonHandler2(void) {
  unsigned long currentTime = millis();
  if (currentTime - button2Pressed > TIMEOUT_BUTTON_PRESS) {
    flipSwitchChannel(2u);
    button2Pressed = currentTime;
  }
}
void IRAM_ATTR ISR_buttonHandler3(void) {
  unsigned long currentTime = millis();
  if (currentTime - button3Pressed > TIMEOUT_BUTTON_PRESS) {
    flipSwitchChannel(3u);
    button3Pressed = currentTime;
  }
}

static void configureButtons(void) {
  /* switch channels should be initialized as now button presses
     could trigger the ISR */
  assert(switchChannelsInitialized == true);
  pinMode(switchChannels[0u].buttonPin, INPUT_PULLUP);
  attachInterrupt(switchChannels[0u].buttonPin, ISR_buttonHandler0, FALLING);
  pinMode(switchChannels[1u].buttonPin, INPUT_PULLUP);
  attachInterrupt(switchChannels[1u].buttonPin, ISR_buttonHandler1, FALLING);
  pinMode(switchChannels[2u].buttonPin, INPUT_PULLUP);
  attachInterrupt(switchChannels[2u].buttonPin, ISR_buttonHandler2, FALLING);
  pinMode(switchChannels[3u].buttonPin, INPUT_PULLUP);
  attachInterrupt(switchChannels[3u].buttonPin, ISR_buttonHandler3, FALLING);
}

void setup() {
  // put your setup code here, to run once:
  /* set and light up status LED */
  pinMode(SLED_PIN, OUTPUT);
  digitalWrite(SLED_PIN, LOW);
  /* initialize switch channels */
  configureSwitchChannels();
  /* enable switch channel 0 */
  setSwitchChannel(0u, true);
  /* wait TIME_SWITCH_ON_AUX_CHANNELS second */
  delay(TIME_SWITCH_ON_AUX_CHANNELS);
  /* sett all three other channels */
  setSwitchChannel(1u, true);
  setSwitchChannel(2u, true);
  setSwitchChannel(3u, true);
  /* configure interrupts on switch channels */
  configureButtons();
}

void loop() {
  // put your main code here, to run repeatedly:
}