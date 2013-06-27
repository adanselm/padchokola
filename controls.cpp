#include "controls.h"
#include "Arduino.h"

#define SELECTOR_LOW_LIMIT 340
#define SELECTOR_HIGH_LIMIT 680

#define SHORT_PRESS 350
#define LONG_PRESS 1500

Controls::Controls(const int btn1Pin, const int btn2Pin, const int btn3Pin,
                   const int /* unusedPin */, const int selectorPin)
: 
mBtn1Pin(btn1Pin), mBtn2Pin(btn2Pin), mBtn3Pin(btn3Pin),
mSelectorPin(selectorPin), mCounter(0),
mLastKeyPressTime(0)
{
}

Controls::~Controls() {
}

void Controls::setup()
{
  pinMode(mBtn1Pin, INPUT);
  pinMode(mBtn2Pin, INPUT);
  pinMode(mBtn3Pin, INPUT);
  digitalWrite(mBtn1Pin, HIGH);  // turn on internal pull-up
  digitalWrite(mBtn2Pin, HIGH);  // turn on internal pull-up
  digitalWrite(mBtn3Pin, HIGH);  // turn on internal pull-up
}

const Controls::ButtonMode Controls::readBtn1()
{
  return readPinFiltered(mBtn1Pin);
}

const Controls::ButtonMode Controls::readBtn2()
{
  return readPinFiltered(mBtn2Pin);
}

const Controls::ButtonMode Controls::readBtn3()
{
  return readPinFiltered(mBtn3Pin);
}

const Controls::SelectorMode Controls::readSelector()
{
  const int pinValue = analogRead(mSelectorPin);
  
  if( pinValue < SELECTOR_LOW_LIMIT )
    return SelectorNone;
    
  if( pinValue > SELECTOR_HIGH_LIMIT )
    return SelectorSecond;
    
  return SelectorFirst;
}

const Controls::ButtonMode Controls::readPinFiltered(const int pinToRead)
{
  const int duration = readPinDuration(pinToRead);
  const bool isOff = (digitalRead(pinToRead) == HIGH);
  
  if( !isOff && duration > LONG_PRESS )
  {
    // Give us some time before another press is possible:
    mCounter = - SHORT_PRESS;
    return ButtonLong;
  }
  else if( isOff && duration > SHORT_PRESS )
  {
    // Button is released and it was not a long press => short press
    mCounter = 0;
    return ButtonShort;
  }
    
  return ButtonOff;
}

const int Controls::readPinDuration(const int pinToRead)
{
  const unsigned long currentTime = millis();
  if( digitalRead(pinToRead) == LOW )
  {
    ++mCounter;
    mLastKeyPressTime = currentTime;
  }
  else if( (currentTime - mLastKeyPressTime) > 4 )
    mCounter = 0;

  return mCounter;
}

