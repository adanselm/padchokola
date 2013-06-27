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

//
float gBpm = 120.0f;
float gOldBpm = 0.0f;
const int gMinBpm = 20;
const int gMaxBpm = 900;
unsigned long gLastUpdate = 0;
Controls::SelectorMode gLastSelectorMode = Controls::SelectorNone;
//

////////////////////////// Main program
Encoder encoder(3, gMinBpm*10, gMaxBpm*10, gBpm*10);
Controls controls(5, 6, 7, 8, A0);
Display7Seg ledDisplay(4 /* data */, 10 /* latch */, 9 /* clock */);
MidiProxy midi;

Controls::SelectorMode checkSelector();
void checkButtons(const Controls::SelectorMode currentMode);

void setup()
{
  // Buttons
  encoder.setup();
  controls.setup();
  ledDisplay.setup();
  midi.setup();
  pinMode(8, OUTPUT);
  checkSelector();

  gLastUpdate = millis();
}

void loop()
{
  const Controls::SelectorMode currentMode = checkSelector();
  
  setBpmFromEncoder();  
  checkButtons(currentMode);
  
  // Display
  ledDisplay.display();
}

/// Read selector and apply matching sync mode
Controls::SelectorMode checkSelector()
{
  const Controls::SelectorMode currentMode = controls.readSelector();
  
  if( currentMode == gLastSelectorMode )
    return gLastSelectorMode;
    
  gLastSelectorMode = currentMode;
  switch( currentMode )
  {
    case Controls::SelectorNone:
    {
      midi.setMode(MidiProxy::SynchroNone);
      return Controls::SelectorNone;
    }
    case Controls::SelectorFirst:
    {
      midi.setMode(MidiProxy::SynchroClock);
      midi.setBpm(gBpm);
      return Controls::SelectorFirst;
    }
    case Controls::SelectorSecond:
    {
      midi.setMode(MidiProxy::SynchroMTC);
      return Controls::SelectorSecond;
    }
  }
  return Controls::SelectorNone;
}

void checkButtons(const Controls::SelectorMode currentMode)
{
  // Controls
  const Controls::ButtonMode btn1 = Controls::ButtonOff;//controls.readBtn1();
  const Controls::ButtonMode btn2 = Controls::ButtonOff;//controls.readBtn2();
  const Controls::ButtonMode btn3 = controls.readBtn3();
  
  digitalWrite(8, LOW);

  if( btn1 == Controls::ButtonShort )
  {
    digitalWrite(8, HIGH);
    if( currentMode == Controls::SelectorNone )
      midi.sendDefaultControlChangeOn(BTN1_SHORT_CC);
    else
      midi.sendPlay();
  }
  else if( btn2 == Controls::ButtonShort )
  {
    digitalWrite(8, HIGH);
    midi.sendDefaultControlChangeOn(BTN2_SHORT_CC);
  }
  else if( btn3 == Controls::ButtonShort )
  {
    digitalWrite(8, HIGH);
    midi.sendDefaultControlChangeOn(BTN3_SHORT_CC);
  }
}

void setBpmFromEncoder()
{
  const unsigned long currentTime = millis();
  const int timeDiff = currentTime - gLastUpdate;
  
  gBpm = encoder.readValue() / 10.0;
  
  if(gBpm != gOldBpm)
  {
    gOldBpm = gBpm;
    
    midi.setBpm(gBpm);
    ledDisplay.setNumber(gBpm);
    
    // Short update: accelerate encoder
    if(timeDiff <= ACCEL_TIME_DELTA)
    {
      // increase rate 0.5 bpm after the other, with a limit of 10
      encoder.setStep( min(encoder.getStep() + 5, 100) );
    }
    gLastUpdate = currentTime;
  }
  
  // No updates in a while: decelerate
  if(timeDiff > ACCEL_TIME_DELTA)
  {
    encoder.setStep(1);
  }
}


