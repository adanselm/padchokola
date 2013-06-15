#include "controls.h"
#include "Arduino.h"

Controls::Controls(const int playStopBtnPin, const int recBtnPin)
: 
mPlayStopBtnPin(playStopBtnPin), mRecBtnPin(recBtnPin), mLastKeyPressTime(0)
{
}

Controls::~Controls() {
}

void Controls::setup()
{
  pinMode(mPlayStopBtnPin, INPUT);
  pinMode(mRecBtnPin, INPUT);
  digitalWrite(mPlayStopBtnPin, HIGH);  // turn on internal pull-up
  digitalWrite(mRecBtnPin, HIGH);  // turn on internal pull-up
}

const int Controls::readPlayBtn()
{
  return readPinFiltered(mPlayStopBtnPin, 100);
}

const int Controls::readRecBtn()
{
  return readPinFiltered(mRecBtnPin, 100);
}

const int Controls::readPinFiltered(const int pinToRead, const int delayTime)
{
  const unsigned long currentTime = millis();
  if( (currentTime - mLastKeyPressTime) >= delayTime )
  {
    mLastKeyPressTime = currentTime;
    return digitalRead(pinToRead);
  }
  return HIGH;
}

