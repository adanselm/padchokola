#ifndef _MIDI_CLOCK_CTL_ENCODER_H_
#define _MIDI_CLOCK_CTL_ENCODER_H_

/////////////////// Rotary encoder stuff
class Encoder
{
  public:
    Encoder(const int encoderPinB);
    ~Encoder();

    void setup();
    const unsigned int readValue() const;

    const unsigned int getMinVal() const;
    const unsigned int getMaxVal() const;

    static void doEncoder();

  private:
    // PinA is fixed to 2 to be able to use interrupt
    void staticInit(const int encoderPinB, const int stepVal);

  private:
    const unsigned int mMinVal, mMaxVal;
    static int mEncoderPinA, mEncoderPinB;
    static int mStep;
    static volatile unsigned int mEncoderPos; // variables changed within interrupts are volatile
};

#endif
