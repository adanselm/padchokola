#include "midi_proxy.h"

MidiProxy::MidiProxy()
{
}

MidiProxy::~MidiProxy()
{
}

void MidiProxy::setup(const float iInitialBpm)
{
  //  Set MIDI baud rate:
  Serial.begin(31250);
  setBpm(iInitialBpm);
}

void MidiProxy::doSendMidiClock()
{
  if( mEventTime != 0 && (millis() - mEventTime) >= 1 )
  {
    // Reset timer giving slaves time to prepare for playback
    mEventTime = 0;
  }
  
  if( mNextEvent != InvalidType )
  {
    mEventTime = millis();
    Serial.write(mNextEvent);
    mNextEvent = InvalidType;
  }  
  
  if( mEventTime == 0 )
  {
    Serial.write(Clock);
  }
}

void MidiProxy::sendPlay()
{
  noInterrupts();
  mNextEvent = Start;
  interrupts();
}

void MidiProxy::sendStop()
{
  noInterrupts();
  mNextEvent = Stop;
  interrupts();
}

void MidiProxy::setBpm(const float iBpm)
{
  const double midiClockPerSec = mMidiClockPpqn * iBpm / 60;
  
  int prescaler = 64;
  unsigned char selectBits = (1 << CS11) | (1 << CS10);
  if(iBpm > 610.4f) // First value with cmp_match < 65536 (thus allowing to decrease prescaler for higher precision)
  {
    prescaler = 1;
    selectBits = (1 << CS10);
  }
  else if(iBpm > 76.3f) // First value with cmp_match < 65536
  {
    prescaler = 8;
    selectBits = (1 << CS11);
  }
  const uint16_t cmp_match = 16000000 / (midiClockPerSec * prescaler) - 1 + 0.5f; // (must be < 65536)
  
  noInterrupts();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for given increments
  OCR1A = cmp_match;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 for prescaler 1, CS11 for prescaler 8, and both for prescaler 64
  TCCR1B |= selectBits;
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

ISR(TIMER1_COMPA_vect) //timer1 interrupt
{
  MidiProxy::doSendMidiClock();
}

const int MidiProxy::mMidiClockPpqn = 24;
volatile unsigned long MidiProxy::mEventTime = 0;
volatile MidiProxy::MidiType MidiProxy::mNextEvent = InvalidType;
