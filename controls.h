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
  const ButtonMode readPinFiltered(const int pinToRead);
  const int readPinDuration(const int pinToRead);
  
 private:
  const int mBtn1Pin, mBtn2Pin, mBtn3Pin, mSelectorPin;
  int mCounter;
  unsigned long mLastKeyPressTime;
};

#endif
