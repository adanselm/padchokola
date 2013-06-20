#ifndef _MIDI_CLOCK_CTL_MIDI_PROXY_H_
#define _MIDI_CLOCK_CTL_MIDI_PROXY_H_

#include <Arduino.h>

class MidiProxy
{
public:
  MidiProxy();
  ~MidiProxy();

  // To be called on main program setup
  // A value of 0 initializes MTC mode instead of default Midi Clock
  void setup(const float iInitialBpm);

  // Only active in Midi Clock mode
  void setBpm(const float iBpm);

  static bool setMode(bool useMTC);
  static bool isModeMTC();
  
  void sendPlay();
  void sendStop();
  
  static void doSendMidiClock();
  static void doSendMTC();
    
private:
  enum MidiType 
  {
    InvalidType           = 0x00,    ///< For notifying errors
    NoteOff               = 0x80,    ///< Note Off
    NoteOn                = 0x90,    ///< Note On
    AfterTouchPoly        = 0xA0,    ///< Polyphonic AfterTouch
    ControlChange         = 0xB0,    ///< Control Change / Channel Mode
    ProgramChange         = 0xC0,    ///< Program Change
    AfterTouchChannel     = 0xD0,    ///< Channel (monophonic) AfterTouch
    PitchBend             = 0xE0,    ///< Pitch Bend
    SystemExclusive       = 0xF0,    ///< System Exclusive
    TimeCodeQuarterFrame  = 0xF1,    ///< System Common - MIDI Time Code Quarter Frame
    SongPosition          = 0xF2,    ///< System Common - Song Position Pointer
    SongSelect            = 0xF3,    ///< System Common - Song Select
    TuneRequest           = 0xF6,    ///< System Common - Tune Request
    Clock                 = 0xF8,    ///< System Real Time - Timing Clock
    Start                 = 0xFA,    ///< System Real Time - Start
    Continue              = 0xFB,    ///< System Real Time - Continue
    Stop                  = 0xFC,    ///< System Real Time - Stop
    ActiveSensing         = 0xFE,    ///< System Real Time - Active Sensing
    SystemReset           = 0xFF,    ///< System Real Time - System Reset
  };
  
  enum MTCQuarterFrameType
  {
    FramesLow             = 0x00,
    FramesHigh            = 0x10,
    SecondsLow            = 0x20,
    SecondsHigh           = 0x30,
    MinutesLow            = 0x40,
    MinutesHigh           = 0x50,
    HoursLow              = 0x60,
    HoursHighAndSmpte     = 0x70,
  };
  
  enum SmpteMask
  {
    Frames24              = B0000,
    Frames25              = B0010,
    Frames30drop          = B0100,
    Frames30              = B0110,
  };
  
  struct Playhead
  {
    byte frames;
    byte seconds;
    byte minutes;
    byte hours;
  };
  
private:
  static void sendMTCQuarterFrame(int index);
  static void updatePlayhead();
  static void resetPlayhead();
  static void setTimer(const double frequency);
  
private:
  // Midi Clock Stuff
  static const int mMidiClockPpqn;
  static volatile unsigned long mEventTime;
  static volatile MidiType mNextEvent;
  static int mPrescaler;
  static unsigned char mSelectBits;
  
  // MTC stuff
  static bool mUseMTC;
  static const SmpteMask mCurrentSmpteType;
  static volatile Playhead mPlayhead;
  static volatile int mCurrentQFrame;
  static const MTCQuarterFrameType mMTCQuarterFrameTypes[8];
};

#endif

