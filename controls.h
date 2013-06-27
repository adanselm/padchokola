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
