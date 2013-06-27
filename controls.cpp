#include "controls.h"
#include "Arduino.h"

#define SELECTOR_LOW_LIMIT 340
#define SELECTOR_HIGH_LIMIT 680

#define SHORT_PRESS 350
#define LONG_PRESS 1500

Controls::Controls(const int btn1Pin, const int btn2Pin, const int btn3Pin,
                   const int /* unusedPin */, const int selectorPin)
: mSelectorPin(selectorPin)
{
  mBtnPin[0] = btn1Pin;
  mBtnPin[1] = btn2Pin;
  mBtnPin[2] = btn3Pin;
  mCounter[0] = 0;
  mCounter[1] = 0;
  mCounter[2] = 0;
  mLastKeyPressTime[0] = 0;
  mLastKeyPressTime[1] = 0;
  mLastKeyPressTime[2] = 0;
}

Controls::~Controls() {
}

void Controls::setup()
{
  for( int i = 0; i < 3; ++i )
  {
    pinMode(mBtnPin[i], INPUT);
    digitalWrite(mBtnPin[i], HIGH);  // turn on internal pull-up
  }
}

const Controls::ButtonMode Controls::readBtn1()
{
  return readPinFiltered(0);
}

const Controls::ButtonMode Controls::readBtn2()
{
  return readPinFiltered(1);
}

const Controls::ButtonMode Controls::readBtn3()
{
  return readPinFiltered(2);
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

const Controls::ButtonMode Controls::readPinFiltered(const int btnIndex)
{
  const int duration = readPinDuration(btnIndex);
  const bool isOff = (digitalRead( mBtnPin[btnIndex] ) == HIGH);
  
  if( !isOff && duration > LONG_PRESS )
  {
    // Give us some time before another press is possible:
    mCounter[btnIndex] = - SHORT_PRESS;
    return ButtonLong;
  }
  else if( isOff && duration > SHORT_PRESS )
  {
    // Button is released and it was not a long press => short press
    mCounter[btnIndex] = 0;
    return ButtonShort;
  }
    
  return ButtonOff;
}

const int Controls::readPinDuration(const int btnIndex)
{
  const unsigned long currentTime = millis();
  if( digitalRead( mBtnPin[btnIndex] ) == LOW )
  {
    ++mCounter[btnIndex];
    mLastKeyPressTime[btnIndex] = currentTime;
  }
  else if( (currentTime - mLastKeyPressTime[btnIndex]) > 4 )
    mCounter[btnIndex] = 0;

  return mCounter[btnIndex];
}

