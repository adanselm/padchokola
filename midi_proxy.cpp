#include "midi_proxy.h"

MidiProxy::MidiProxy()
{
}

MidiProxy::~MidiProxy()
{
}

void MidiProxy::setup(const float iInitialBpm)
{
  // Set MIDI baud rate:
  Serial.begin(31250);
  
  if( iInitialBpm > 0.0f )
    setBpm(iInitialBpm);
  else
    setMode(true);
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

void MidiProxy::doSendMTC()
{  
  if( mNextEvent != Stop )
  {
    if( mNextEvent == Start)
      resetPlayhead();
      
    sendMTCQuarterFrame(mCurrentQFrame);
    mCurrentQFrame = (mCurrentQFrame + 1) % 8;
    
    if(mCurrentQFrame == 0)
      updatePlayhead();
  }
}

bool MidiProxy::setMode(bool useMTC)
{
  mUseMTC = useMTC;
  
  if(mUseMTC)
  {
    setTimer(24 * 4);
  }
}

bool MidiProxy::isModeMTC()
{
  return mUseMTC;
}

void MidiProxy::sendMTCQuarterFrame(int index)
{
  Serial.write(TimeCodeQuarterFrame);
  
  byte MTCData = 0;
  switch(mMTCQuarterFrameTypes[index])
  {
    case FramesLow:
      MTCData = mPlayhead.frames & 0x0f;
      break;
    case FramesHigh:
      MTCData = (mPlayhead.frames & 0xf0) >> 4;
      break;
    case SecondsLow:
      MTCData = mPlayhead.seconds & 0x0f;
      break;
    case SecondsHigh:
      MTCData = (mPlayhead.seconds & 0xf0) >> 4;
      break;
    case MinutesLow:
      MTCData = mPlayhead.minutes & 0x0f;
      break;
    case MinutesHigh:
      MTCData = (mPlayhead.minutes & 0xf0) >> 4;
      break;
    case HoursLow:
      MTCData = mPlayhead.hours & 0x0f;
      break;
    case HoursHighAndSmpte:
      MTCData = (mPlayhead.hours & 0xf0) >> 4 | mCurrentSmpteType;
      break;
  }
  Serial.write( mMTCQuarterFrameTypes[index] | MTCData );
}

// To be called every two frames (so once a complete cycle of quarter frame messages have passed)
void MidiProxy::updatePlayhead()
{
  // Compute counter progress
  // update occurring every 2 frames 
  mPlayhead.frames += 2;
  mPlayhead.seconds += mPlayhead.frames / 24;
  mPlayhead.frames = mPlayhead.frames % 24;
  mPlayhead.minutes += mPlayhead.seconds / 60;
  mPlayhead.seconds = mPlayhead.seconds % 60;
  mPlayhead.hours += mPlayhead.minutes / 60;
}

void MidiProxy::resetPlayhead()
{
  mPlayhead.frames = 0;
  mPlayhead.seconds = 0;
  mPlayhead.minutes = 0;
  mPlayhead.hours = 0;
}

void MidiProxy::setBpm(const float iBpm)
{
  if( ! isModeMTC() )
  {
    const double midiClockPerSec = mMidiClockPpqn * iBpm / 60;
    setTimer(midiClockPerSec);
  }
}

void MidiProxy::setTimer(const double frequency)
{
  if(frequency > 244.16f) //610.4f) // First value with cmp_match < 65536 (thus allowing to decrease prescaler for higher precision)
  {
    mPrescaler = 1;
    mSelectBits = (1 << CS10);
  }
  else if(frequency > 30.52f) //76.3f) // First value with cmp_match < 65536
  {
    mPrescaler = 8;
    mSelectBits = (1 << CS11);
  }
  else
  {
    mPrescaler = 64;
    mSelectBits = (1 << CS11) | (1 << CS10);
  }
  const uint16_t cmp_match = 16000000 / (frequency * mPrescaler) - 1 + 0.5f; // (must be < 65536)
  
  noInterrupts();
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for given increments
  OCR1A = cmp_match;
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS10 for prescaler 1, CS11 for prescaler 8, and both for prescaler 64
  TCCR1B |= mSelectBits;
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
}

ISR(TIMER1_COMPA_vect) //timer1 interrupt
{
  if( MidiProxy::isModeMTC() )
    MidiProxy::doSendMTC();
  else
    MidiProxy::doSendMidiClock();
}

int MidiProxy::mPrescaler = 0;
unsigned char MidiProxy::mSelectBits = 0;

const int MidiProxy::mMidiClockPpqn = 24;
volatile unsigned long MidiProxy::mEventTime = 0;
volatile MidiProxy::MidiType MidiProxy::mNextEvent = InvalidType;

const MidiProxy::SmpteMask MidiProxy::mCurrentSmpteType = Frames24;
volatile MidiProxy::Playhead MidiProxy::mPlayhead = MidiProxy::Playhead();
volatile int MidiProxy::mCurrentQFrame = 0;
const MidiProxy::MTCQuarterFrameType MidiProxy::mMTCQuarterFrameTypes[8] = { FramesLow, FramesHigh, SecondsLow, SecondsHigh,
                                                                             MinutesLow, MinutesHigh, HoursLow, HoursHighAndSmpte };
bool MidiProxy::mUseMTC = false;
