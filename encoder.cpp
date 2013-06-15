#include "encoder.h"
#include "Arduino.h"

#define DEFAULT_BPM 60
#define MIN_BPM 20
#define MAX_BPM 999

Encoder::Encoder(const int encoderPinB)
: mMinVal(0), mMaxVal(mMinVal - 1)
{
  staticInit(encoderPinB, 1);
}

Encoder::~Encoder() {}

void Encoder::setup()
{
  pinMode(mEncoderPinA, INPUT);
  pinMode(mEncoderPinB, INPUT);
  digitalWrite(mEncoderPinA, HIGH);
  digitalWrite(mEncoderPinB, HIGH);
  mEncoderPos = map(DEFAULT_BPM*10, MIN_BPM*10, MAX_BPM*10, mMinVal, mMaxVal);
  attachInterrupt(0, Encoder::doEncoder, CHANGE); // encoder pin on interrupt 0 (pin 2)
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

void Encoder::doEncoder()
{
  const int valA = digitalRead(mEncoderPinA);
  const int valB = digitalRead(mEncoderPinB);
  if( valA == HIGH )
  {
    if( valB == LOW )
      mEncoderPos -= mStep;    // CCW
    else
      mEncoderPos += mStep;    // CW
  }
  else
  {
    if( valB == LOW )
      mEncoderPos += mStep;    // CW
    else
      mEncoderPos -= mStep;    // CCW
  }
}

// PinA is fixed to 2 to be able to use interrupt
void Encoder::staticInit(const int encoderPinB, const int stepVal)
{
  mEncoderPinA = 2;
  mEncoderPinB = encoderPinB;
  mStep = stepVal;
  mEncoderPos = 0;
}

int Encoder::mEncoderPinA = 2;
int Encoder::mEncoderPinB = 0;
int Encoder::mStep = 0;
volatile unsigned int Encoder::mEncoderPos = 0;

