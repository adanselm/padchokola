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
#ifndef _MIDI_CLOCK_CTL_ENCODER_H_
#define _MIDI_CLOCK_CTL_ENCODER_H_

/////////////////// Rotary encoder stuff
class Encoder
{
  public:
    Encoder(const int encoderPinB, const unsigned int minValue, const unsigned int maxValue, const unsigned int defaultValue);
    ~Encoder();

    void setup();
    const unsigned int readValue() const;

    const unsigned int getMinVal() const;
    const unsigned int getMaxVal() const;

    static const int getStep();
    static void setStep(const int newStep);
    
    static void doEncoder();
    
  private:
    // PinA is fixed to 2 to be able to use interrupt
    void staticInit(const int encoderPinB, const unsigned int minValue, const unsigned int maxValue,
                    const int stepVal, const unsigned int defaultVal);
    static void inc(volatile unsigned int & value, volatile int & stepVal);
    static void dec(volatile unsigned int & value, volatile int & stepVal);

  private:
    static unsigned int mMinVal, mMaxVal;
    static int mEncoderPinA, mEncoderPinB;
    
    // Note:  all variables changed within interrupts are volatile
    static volatile int mStep;
    static volatile unsigned int mEncoderPos;
    
    static volatile int val1, val2;
    static volatile int oldVal1, oldVal2;
    static volatile int pos, oldPos;
    static volatile int turn, oldTurn, turnCount;
};

#endif
