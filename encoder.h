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
