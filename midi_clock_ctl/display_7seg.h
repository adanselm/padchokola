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
#ifndef _MIDI_CLOCK_CTL_DISPLAY_7SEG_H_
#define _MIDI_CLOCK_CTL_DISPLAY_7SEG_H_

#include <Arduino.h>

#define NUM_DIGITS 4

class Display7Seg
{
 public:
  Display7Seg(const int dataPin, const int latchPin, const int clockPin);
  ~Display7Seg();
  
  // To be called on main program setup
  void setup();
  
  // To be called in main loop function
  void display();
  
  void setNumber(const float numberToDisplay);
  void setNumber(const unsigned int numberToDisplay);
  
 private:
  void setNumber(const byte digit1, const byte digit2, const byte digit3, const byte digit4);
  
 private:
  const int mDataPin;
  const int mLatchPin; /* ST_CP of 74HC595 */
  const int mClockPin; /* SH_CP of 74HC595 */
  byte mCurrentDigit;
  byte mDigits[NUM_DIGITS];
  
  static const int mNumbersCodes[10];
  static const int mDigitsCodes[NUM_DIGITS];
  static const int mSeparatorCode;
};

#endif
