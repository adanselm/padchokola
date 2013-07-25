/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      /*
 MidiClockCtl : MIDI controller (Time Clock) with rotary encoder, 4 digit 7seg display and footswitches.
 Copyright (C) 2013 Adrien Anselme

 This file is part of MidiClockCtl.

 MidiClockCtl is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 2 of the License, or
 (at your option) any later version.

 MidiClockCtl is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with MidiClockCtl.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "controls.h"
#include "Arduino.h"

#define SELECTOR_LOW_LIMIT 340
#define SELECTOR_HIGH_LIMIT 680

#define SHORT_PRESS 350
#define LONG_PRESS 1500

Controls::Controls(const int btn1Pin, const int btn2Pin, const int btn3Pin,
                   const int btn4Pin, const int btn5Pin, const int btn6Pin,
                   const int btn7Pin, const int selectorPin)
: mSelectorPin(selectorPin)
{
  mBtnPin[0] = btn1Pin;
  mBtnPin[1] = btn2Pin;
  mBtnPin[2] = btn3Pin;
  
  for( int i = 0; i < CONTROLS_BTN_COUNT; ++i )
  {
    mCounter[i] = 0;
    mLastKeyPressTime[i] = 0;
  }
}

Controls::~Controls() {
}

void Controls::setup()
{
  for( int i = 0; i < CONTROLS_BTN_COUNT; ++i )
  {
    pinMode(mBtnPin[i], INPUT);
    digitalWrite(mBtnPin[i], HIGH);  // turn on internal pull-up
  }
}

const Controls::ButtonMode Controls::readBtn(const int btnNumber)
{
  return readPinFiltered(btnNumber - 1);
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

