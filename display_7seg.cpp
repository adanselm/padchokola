#include "display_7seg.h"

Display7Seg::Display7Seg(const int dataPin, const int latchPin, const int clockPin)
: 
mDataPin(dataPin), mLatchPin(latchPin), mClockPin(clockPin),
mCurrentDigit(0)
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

  for( int i = 0; i < NUM_DIGITS; ++i )
  {
    mDigits[i] = 0;
  }
}

void Display7Seg::display()
{
  // take the latchPin low so 
  // the LEDs don't change while you're sending in bits:
  digitalWrite(mLatchPin, LOW);

  shiftOut(mDataPin, mClockPin, LSBFIRST, mDigitsCodes[mCurrentDigit]);
  
  const int number = mCurrentDigit == 2 ? mNumbersCodes[ mDigits[mCurrentDigit] ] | mSeparatorCode
                                        : mNumbersCodes[ mDigits[mCurrentDigit] ];
  shiftOut(mDataPin, mClockPin, LSBFIRST, number);

  //take the latch pin high so the LEDs will light up:
  digitalWrite(mLatchPin, HIGH);

  if( mCurrentDigit == (NUM_DIGITS - 1) )
    mCurrentDigit = 0;
  else
    ++mCurrentDigit;
}

void Display7Seg::setNumber(const float numberToDisplay)
{
  const int number = numberToDisplay * 10;
  const byte digit1 = number / 1000;
  const byte digit2 = (number - digit1 * 1000) / 100 ;
  const byte digit3 = (number - digit1 * 1000 - digit2 * 100) / 10;
  const byte digit4 = number - digit1 * 1000 - digit2 * 100 - digit3 * 10;
  setNumber(digit1, digit2, digit3, digit4);
}

void Display7Seg::setNumber(const byte digit1, const byte digit2, const byte digit3, const byte digit4)
{
  mDigits[0] = constrain(digit1, 0, 9);
  mDigits[1] = constrain(digit2, 0, 9);
  mDigits[2] = constrain(digit3, 0, 9);
  mDigits[3] = constrain(digit4, 0, 9);
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

const int Display7Seg::mSeparatorCode = B00000001; // comma to be bitwise OR'd with any number


