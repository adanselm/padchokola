#include "encoder.h"
#include "Arduino.h"

Encoder::Encoder(const int encoderPinB, const unsigned int minValue, const unsigned int maxValue, const unsigned int defaultValue)
{
  staticInit(encoderPinB, minValue, maxValue, 1, defaultValue);
}

Encoder::~Encoder() {}

void Encoder::setup()
{
  pinMode(mEncoderPinA, INPUT);
  pinMode(mEncoderPinB, INPUT);
  digitalWrite(mEncoderPinA, HIGH);
  digitalWrite(mEncoderPinB, HIGH);
  attachInterrupt(0, Encoder::doEncoder, CHANGE); // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(1, Encoder::doEncoder, CHANGE); // encoder pin on interrupt 1 (pin 3)
  
  val1 = val2 = oldVal1 = oldVal2 = pos = oldPos = turn = oldTurn = turnCount = 0;
}

const unsigned int Encoder::readValue() const
{
  unsigned int temp = 0;

  noInterrupts(); // Ensures interrupt doesn't happen while reading the value
  temp = mEncoderPos;
  interrupts();

  return temp;
}

const unsigned int Encoder::getMinVal() const
{
  return mMinVal;
}

const unsigned int Encoder::getMaxVal() const
{
  return mMaxVal;
}

const int Encoder::getStep()
{
  return mStep;
}

void Encoder::setStep(const int newStep)
{
  mStep = newStep;
}

void Encoder::doEncoder() 
{
  // Adapted from https://gist.github.com/medecau/154809
  val1 = digitalRead(mEncoderPinA);
  val2 = digitalRead(mEncoderPinB);
  
    // Detect changes
  if ( val1 != oldVal1 || val2 != oldVal2) {
    
      //for each pair there's a position out of four
    if      ( val1 == 1 && val2 == 1 ) // stationary position
      pos = 0;
    else if ( val1 == 0 && val2 == 1 )
      pos = 1;
    else if ( val1 == 0 && val2 == 0 )
      pos = 2;
    else if ( val1 == 1 && val2 == 0 )
      pos = 3;
    
    turn = pos-oldPos;
    
    if (abs(turn) != 2) // impossible to understand where it's turning otherwise.
      if (turn == -1 || turn == 3)
        turnCount++;
      else if (turn == 1 || turn == -3)
        turnCount--;
    
    if (pos == 0)
    {
      // only assume a complete step on stationary position
      if(turnCount > 0)
        dec(mEncoderPos, mStep); // CCW
      else if(turnCount < 0)
        inc(mEncoderPos, mStep); // CW
      turnCount = 0;
    }
    
    oldVal1 = val1;
    oldVal2 = val2;
    oldPos  = pos;
    oldTurn = turn;
  }
}

// PinA is fixed to 2 to be able to use interrupt
void Encoder::staticInit(const int encoderPinB, const unsigned int minValue, const unsigned int maxValue,
                         const int stepVal, const unsigned int defaultVal)
{
  mEncoderPinA = 2;
  mEncoderPinB = encoderPinB;
  mMinVal = minValue;
  mMaxVal = maxValue;
  mStep = stepVal;
  mEncoderPos = defaultVal;
}

void Encoder::inc(volatile unsigned int & value, volatile int & stepVal)
{
  value = min(mMaxVal, value + stepVal);
//  if( mMaxVal - value >= stepVal )
//    value += stepVal;
//  else
//    value = mMinVal + stepVal - (mMaxVal - value) - 1;
}

void Encoder::dec(volatile unsigned int & value, volatile int & stepVal)
{
  value = max(mMinVal, value - stepVal);
//  if( value - mMinVal >= stepVal )
//    value -= stepVal;
//  else
//    value = mMaxVal - stepVal + value - mMinVal + 1;
}

unsigned int Encoder::mMinVal = 0;
unsigned int Encoder::mMaxVal = 0;
int Encoder::mEncoderPinA = 2;
int Encoder::mEncoderPinB = 0;
volatile int Encoder::mStep = 0;
volatile unsigned int Encoder::mEncoderPos = 0;

volatile int Encoder::val1 = 0;
volatile int Encoder::val2 = 0;
volatile int Encoder::oldVal1 = 0;
volatile int Encoder::oldVal2 = 0;
volatile int Encoder::pos = 0;
volatile int Encoder::oldPos = 0;
volatile int Encoder::turn = 0;
volatile int Encoder::oldTurn = 0;
volatile int Encoder::turnCount = 0;

