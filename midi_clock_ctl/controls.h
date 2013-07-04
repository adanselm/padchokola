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
           const int /* unusedPin */, const int selectorPin);
  ~Controls();
  
  void setup();
  
  const ButtonMode readBtn1();
  const ButtonMode readBtn2();
  const ButtonMode readBtn3();
  const SelectorMode readSelector();

 private:
  const ButtonMode readPinFiltered(const int btnIndex);
  const int readPinDuration(const int btnIndex);
  
 private:
  int mBtnPin[3];
  const int mSelectorPin;
  int mCounter[3];
  unsigned long mLastKeyPressTime[3];
};

#endif
