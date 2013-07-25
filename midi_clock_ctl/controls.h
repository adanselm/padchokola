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
#ifndef _MIDI_CLOCK_CTL_CONTROLS_H_
#define _MIDI_CLOCK_CTL_CONTROLS_H_

#define CONTROLS_BTN_COUNT 7

/////////////////// Buttons stuff
class Controls
{
 public:
   enum SelectorMode
  {
    SelectorNone = 0,
    SelectorFirst,
    SelectorSecond
  };
  
  enum ButtonMode
  {
    ButtonOff = 0,
    ButtonShort,
    ButtonLong
  };

  Controls(const int btn1Pin, const int btn2Pin, const int btn3Pin,
           const int btn4Pin, const int btn5Pin, const int btn6Pin,
           const int btn7Pin, const int selectorPin);
  ~Controls();
  
  void setup();
  
  /* Read button btnNumber (starting from 1) */
  const ButtonMode readBtn(const int btnNumber);
  const SelectorMode readSelector();

 private:
  const ButtonMode readPinFiltered(const int btnIndex);
  const int readPinDuration(const int btnIndex);
  
 private:
  int mBtnPin[CONTROLS_BTN_COUNT];
  const int mSelectorPin;
  int mCounter[CONTROLS_BTN_COUNT];
  unsigned long mLastKeyPressTime[CONTROLS_BTN_COUNT];
};

#endif
