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
 // Initializing this library breaks analogWrite() for digital pins 9 and 10.
 #include <TimerOne.h>
 #include "encoder.h"
 #include "controls.h"
 #include "display_7seg.h"
 
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

// Midi Stuff
const int gMidiClockPpqn = 24;
float gBpm = 60.0;
const int gMinBpm = 20;
const int gMaxBpm = 999;
volatile unsigned long gEventTime = 0;
volatile MidiType gNextEvent = InvalidType;

//
unsigned int gPos = 0, gOldPos = 0;

////////////////////////// Main program
Encoder encoder(3);
Controls controls(5, 6);
Display7Seg ledDisplay(4 /* data */, 9 /* latch */, 10 /* clock */);

void setup()
{
  // Buttons
  encoder.setup();
  controls.setup();
  ledDisplay.setup();
  
  //  Set MIDI baud rate:
  Serial.begin(31250);
//  Serial.begin(9600);
  Timer1.initialize( getClockPeriodInMicrosec(gBpm*10) );
  Timer1.attachInterrupt(doSendMidiClock);
}

void loop()
{
  gPos = encoder.readValue();
  if(gPos != gOldPos)
  {
    gOldPos = gPos;
    
    gBpm = map(gPos, encoder.getMinVal(), encoder.getMaxVal(), gMinBpm*10, gMaxBpm*10) / 10.0;
    Timer1.setPeriod( getClockPeriodInMicrosec(gBpm*10) );
    ledDisplay.setNumber(gBpm);
  }
  
  // Controls
  const int shouldPlay = controls.readPlayBtn();
  const int shouldStop = controls.readRecBtn();
  if( shouldStop == LOW )
  {
    sendMidiStop();
  }
  if( shouldPlay == LOW )
  {
    sendMidiPlay();
  }
  
  // Display
  ledDisplay.display();
}

void doSendMidiClock()
{
  if( gEventTime != 0 && (millis() - gEventTime) >= 1 )
  {
    // Reset timer giving slaves time to prepare for playback
    gEventTime = 0;
  }
  
  if( gNextEvent != InvalidType )
  {
    gEventTime = millis();
    Serial.write(gNextEvent);
    gNextEvent = InvalidType;
  }  
  
  if( gEventTime == 0 )
  {
    Serial.write(Clock);
  }
}

void sendMidiPlay()
{
  noInterrupts();
  gNextEvent = Start;
  interrupts();
}

void sendMidiStop()
{
  noInterrupts();
  gNextEvent = Stop;
  interrupts();
}

long int getClockPeriodInMicrosec(const int iBpmTimesTen)
{
  const long int midiClockPerMinute = gMidiClockPpqn * iBpmTimesTen;
  return (double)(1000000*60*10 / midiClockPerMinute) + 0.5f;
}

