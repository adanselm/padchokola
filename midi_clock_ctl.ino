/*                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      /*
 MIDI controller (Time Clock) with rotary encoder, 4 digit 7seg display and footswitches.
 
 The circuit:
 * digital in 1 connected to MIDI jack pin 5
 * MIDI jack pin 2 connected to ground
 * MIDI jack pin 4 connected to +5V through 220-ohm resistor
 * digital pin 2 connected to rotary encoder pin 1 (required for interruption)
 * encoder pin 2 connected to ground
 * encoder pin 3 connected to any digital pin (e.g. digital pin 3)
 
 created 3 Jun 2013
 by Adrien Anselme
 
 */
#include "encoder.h"
#include "controls.h"
#include "display_7seg.h"
#include "midi_proxy.h"

#define ACCEL_TIME_DELTA 200

#define BTN1_SHORT_CC 24
#define BTN1_LONG_CC 23
#define BTN2_SHORT_CC 25
#define BTN2_LONG_CC 26
#define BTN3_SHORT_CC 27
#define BTN3_LONG_CC 28

class Application
{
public:
  Application(const float defaultBpm, const float minBpm, const float maxBpm,
  const int encoderPin, const int btn1Pin, const int btn2Pin,
  const int btn3Pin, const int btn4Pin, const int selectorPin,
  const int ledDataPin, const int ledLatchPin, const int ledClockPin)
: 
    mBpm(defaultBpm), mOldBpm(0.0f), mMinBpm(minBpm), mMaxBpm(maxBpm),
    mLastUpdate(0), mLastSelectorMode(Controls::SelectorNone),
    mIsPlaying(false), mShouldReset(true),
    mEncoder(encoderPin, mMinBpm*10, mMaxBpm*10, mBpm*10),
    mControls(btn1Pin, btn2Pin, btn3Pin, btn4Pin, selectorPin),
    mLedDisplay(ledDataPin, ledLatchPin, ledClockPin)
    {
    }

  ~Application()
  {
  }

  void setup()
  {
    // Buttons
    mEncoder.setup();
    mControls.setup();
    mLedDisplay.setup();
    mMidi.setup();

    checkSelector();

    mLastUpdate = millis();
  }

  void loop()
  {
    const Controls::SelectorMode currentMode = checkSelector();

    setBpmFromEncoder();  
    checkButtons(currentMode);

    // Display
    mLedDisplay.display();
  }

private:
  float mBpm;
  float mOldBpm;
  const int mMinBpm;
  const int mMaxBpm;
  unsigned long mLastUpdate;
  Controls::SelectorMode mLastSelectorMode;
  bool mIsPlaying;
  bool mShouldReset;
  //
  Encoder mEncoder;
  Controls mControls;
  Display7Seg mLedDisplay;
  MidiProxy mMidi;

private:
  /// Read selector and apply matching sync mode
  Controls::SelectorMode checkSelector()
  {
    const Controls::SelectorMode currentMode = mControls.readSelector();

    if( currentMode == mLastSelectorMode )
      return mLastSelectorMode;

    mLastSelectorMode = currentMode;
    switch( currentMode )
    {
      case Controls::SelectorNone:
      {
        mMidi.setMode(MidiProxy::SynchroNone);
        return Controls::SelectorNone;
      }
      case Controls::SelectorFirst:
      {
        mMidi.setMode(MidiProxy::SynchroClock);
        mMidi.setBpm(mBpm);
        return Controls::SelectorFirst;
      }
      case Controls::SelectorSecond:
      {
        mMidi.setMode(MidiProxy::SynchroMTC);
        return Controls::SelectorSecond;
      }
    }
    return Controls::SelectorNone;
  }

  void doButton1Short(const Controls::SelectorMode currentMode)
  {
    if( currentMode == Controls::SelectorNone )
    {
      mMidi.sendDefaultControlChangeOn(BTN1_SHORT_CC);
    }
    else if(mIsPlaying)
    {
      mMidi.sendStop();
      mIsPlaying = false;
    }
    else
    {
      if( mShouldReset )
      {
        mMidi.sendPlay();
        mShouldReset = false;
      }
      else
        mMidi.sendContinue();

      mIsPlaying = true;
    }
  }

  void doButton1Long(const Controls::SelectorMode currentMode)
  {
    if( currentMode == Controls::SelectorNone )
      mMidi.sendDefaultControlChangeOn(BTN1_LONG_CC);
    else
    {
      mMidi.sendStop();
      mShouldReset = true;
    }
  }

  void checkButtons(const Controls::SelectorMode currentMode)
  {
    // Controls
    const Controls::ButtonMode btn1 = mControls.readBtn1();
    const Controls::ButtonMode btn2 = mControls.readBtn2();
    const Controls::ButtonMode btn3 = mControls.readBtn3();

    if( btn1 == Controls::ButtonShort )
    {
      doButton1Short(currentMode);
    }
    else if( btn1 == Controls::ButtonLong )
    {
      doButton1Long(currentMode);
    }
    else if( btn2 == Controls::ButtonShort )
    {
      mMidi.sendDefaultControlChangeOn(BTN2_SHORT_CC);
    }
    else if( btn2 == Controls::ButtonLong )
    {
      mMidi.sendDefaultControlChangeOn(BTN2_LONG_CC);
    }
    else if( btn3 == Controls::ButtonShort )
    {
      mMidi.sendDefaultControlChangeOn(BTN3_SHORT_CC);
    }
    else if( btn3 == Controls::ButtonLong )
    {
      mMidi.sendDefaultControlChangeOn(BTN3_LONG_CC);
    }
  }

  void setBpmFromEncoder()
  {
    const unsigned long currentTime = millis();
    const int timeDiff = currentTime - mLastUpdate;

    mBpm = mEncoder.readValue() / 10.0;

    if(mBpm != mOldBpm)
    {
      mOldBpm = mBpm;

      mMidi.setBpm(mBpm);
      mLedDisplay.setNumber(mBpm);

      // Short update: accelerate encoder
      if(timeDiff <= ACCEL_TIME_DELTA)
      {
        // increase rate 0.5 bpm after the other, with a limit of 10
        mEncoder.setStep( min(mEncoder.getStep() + 5, 100) );
      }
      mLastUpdate = currentTime;
    }

    // No updates in a while: decelerate
    if(timeDiff > ACCEL_TIME_DELTA)
    {
      mEncoder.setStep(1);
    }
  }

}; // end of class Application

////////////////////////// Main program
Application gApp(120.0f /* defaultbpm */, 20 /* minbpm */, 900 /* maxbpm */,
3 /* encoderpin */, 5 /* btn1 */, 6 /* btn2 */, 7 /* btn3 */,
8 /* btn4 */, A0 /* selectorpin */, 
4 /* leddata */, 10 /* ledlatch */, 9 /* ledclock */);
void setup()
{
  gApp.setup();
}

void loop()
{
  gApp.loop();
}





