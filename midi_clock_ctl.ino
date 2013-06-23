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

//
float gBpm = 120.0f;
float gOldBpm = 0.0f;
const int gMinBpm = 20;
const int gMaxBpm = 900;
unsigned long gLastUpdate = 0;
//

////////////////////////// Main program
Encoder encoder(3, gMinBpm*10, gMaxBpm*10, gBpm*10);
Controls controls(5, 6);
Display7Seg ledDisplay(4 /* data */, 9 /* latch */, 10 /* clock */);
MidiProxy midi;

void setup()
{
  // Buttons
  encoder.setup();
  controls.setup();
  ledDisplay.setup();
  midi.setup(gBpm); // a Value of 0 bpm makes MidiProxy use MTC instead of Midi Clock.

  gLastUpdate = millis();
}

void loop()
{
  setBpmFromEncoder();
  
  // Controls
  const int shouldPlay = controls.readPlayBtn();
  const int shouldStop = controls.readRecBtn();
  if( shouldStop == LOW )
  {
    midi.sendStop();
  }
  if( shouldPlay == LOW )
  {
    midi.sendPlay();
  }
  
  // Display
  ledDisplay.display();
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


