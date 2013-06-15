#ifndef _MIDI_CLOCK_CTL_CONTROLS_H_
#define _MIDI_CLOCK_CTL_CONTROLS_H_

/////////////////// Buttons stuff
class Controls
{
 public:
  Controls(const int playStopBtnPin, const int recBtnPin);
  ~Controls();
  
  void setup();
  
  const int readPlayBtn();
  const int readRecBtn();
 
 private:
  const int readPinFiltered(const int pinToRead, const int delayTime);
  
 private:
  const int mPlayStopBtnPin, mRecBtnPin;
  unsigned long mLastKeyPressTime;
};

#endif
