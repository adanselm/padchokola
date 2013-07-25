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
#include "display_7seg.h"

#define MSG_DURATION 1000

Display7Seg::Display7Seg(const int dataPin, const int latchPin, const int clockPin)
: 
mDataPin(dataPin), mLatchPin(latchPin), mClockPin(clockPin),
mCurrentDigit(0), mCurrentMsgDuration(0)
{
}

Display7Seg::~Display7Seg()
{
}

void Display7Seg::setup()
{
  pinMode(mDataPin, OUTPUT);
  pinMode(mLatchPin, OUTPUT);
  pinMode(mClockPin, OUTPUT);

  mCurrentDigit = 0;
  mCurrentMsgDuration = 0;

  for( int i = 0; i < NUM_DIGITS; ++i )
  {
    mLedData[i] = 0;
    mMsgData[i] = -1;
  }
}

void Display7Seg::display()
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(mLatchPin, LOW);

  shiftOut(mDataPin, mClockPin, LSBFIRST, mDigitsCodes[mCurrentDigit]);
  
  if( mMsgData[mCurrentDigit] >= 0 )
  {
    // There is a msg to display instead of number
    const int number = mMsgData[mCurrentDigit];
    shiftOut(mDataPin, mClockPin, LSBFIRST, number);
  }
  else
  {
    // put a comma after 3 digits:
    const int number = mCurrentDigit == 2 ? mLedData[mCurrentDigit] | mSeparatorCode
                                          : mLedData[mCurrentDigit];
    shiftOut(mDataPin, mClockPin, LSBFIRST, number);
  }
  
  //take the latch pin high so the LEDs will light up:
  digitalWrite(mLatchPin, HIGH);

  if( mCurrentDigit == (NUM_DIGITS - 1) )
    mCurrentDigit = 0;
  else
    ++mCurrentDigit;
    
  if( mCurrentMsgDuration < MSG_DURATION )
    ++mCurrentMsgDuration;
  else
    resetMsg();
}

void Display7Seg::setNumber(const float numberToDisplay)
{
  const unsigned int number = numberToDisplay * 10;
  setNumber(number);
}

void Display7Seg::setNumber(const unsigned int number)
{
  const byte digit1 = number / 1000;
  const byte digit2 = (number - digit1 * 1000) / 100 ;
  const byte digit3 = (number - digit1 * 1000 - digit2 * 100) / 10;
  const byte digit4 = number - digit1 * 1000 - digit2 * 100 - digit3 * 10;
  setNumber(digit1, digit2, digit3, digit4);
}

void Display7Seg::setNumber(const byte digit1, const byte digit2, const byte digit3, const byte digit4)
{
  mLedData[0] = mNumbersCodes[ constrain(digit1, 0, 9) ];
  mLedData[1] = mNumbersCodes[ constrain(digit2, 0, 9) ];
  mLedData[2] = mNumbersCodes[ constrain(digit3, 0, 9) ];
  mLedData[3] = mNumbersCodes[ constrain(digit4, 0, 9) ];
}

void Display7Seg::setStatusMsg(const char* msg)
{
  mCurrentMsgDuration = 0;
  for( int i = 0; i < NUM_DIGITS; ++i )
  {
    mMsgData[i] = mLettersCodes[ constrain(msg[i] - 'a', 0, 25) ];
  }
}

void Display7Seg::resetMsg()
{
  for( int i = 0; i < NUM_DIGITS; ++i )
  {
    mMsgData[i] = -1;
  }
}

const int Display7Seg::mDigitsCodes[NUM_DIGITS] = { 
  B10000000,
  B01000000,
  B00100000,
  B00010000
};

const int Display7Seg::mNumbersCodes[10] = { 
  B11111100, // 0
  B01100000, // 1
  B11011010, // 2
  B11110010, // 3
  B01100110, // 4
  B10110110, // 5
  B10111110, // 6
  B11100000, // 7
  B11111110, // 8
  B11110110  // 9
};

const int Display7Seg::mLettersCodes[26] = { 
  B11101110, // A
  B00111110, // b
  B10011100, // C
  B01111010, // d
  B10011110, // E
  B10001110, // F
  B10111110, // G
  B01101110, // H
  B01100000, // I
  B01110000, // J
  B00000000, // K - not possible
  B00011100, // L
  B11101100, // M - approximative :/
  B00101010, // n - approximative :/
  B11111100, // O
  B11001110, // P
  B11100110, // q
  B10001100, // r
  B10110110, // S
  B00011110, // t
  B01111100, // U
  B00000000, // V - not possible
  B00000000, // W - not possible
  B00000000, // X - not possible
  B01110000, // y - approximative :/
  B01100110, // Z - not possible
};

const int Display7Seg::mSeparatorCode = B00000001; // comma to be bitwise OR'd with any number


