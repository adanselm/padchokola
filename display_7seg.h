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
