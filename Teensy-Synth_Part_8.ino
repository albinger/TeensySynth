// Teensy-Synth Part 8
// LFO Test
// By Notes and Volts
// www.notesandvolts.com

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=382,453
AudioSynthWaveform       waveform1;      //xy=386,394
AudioSynthNoisePink      pink1;          //xy=394,504
AudioMixer4              mixer1;         //xy=577,454
AudioFilterStateVariable filter1;        //xy=725,457
AudioEffectEnvelope      envelope1;      //xy=888,458
AudioOutputI2S           i2s1;           //xy=1049,465
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
AudioConnection          patchCord3(pink1, 0, mixer1, 2);
AudioConnection          patchCord4(mixer1, 0, filter1, 0);
AudioConnection          patchCord5(filter1, 0, envelope1, 0);
AudioConnection          patchCord6(envelope1, 0, i2s1, 0);
AudioConnection          patchCord7(envelope1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=565,556
// GUItool: end automatically generated code




// GLOBAL VARIABLES
const byte BUFFER = 8; //Size of keyboard buffer
const float noteFreqs[128] = {8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25, 12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445, 20.602, 21.827, 23.125, 24.5, 25.957, 27.5, 29.135, 30.868, 32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 48.999, 51.913, 55, 58.27, 61.735, 65.406, 69.296, 73.416, 77.782, 82.407, 87.307, 92.499, 97.999, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508, 1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760, 1864.655, 1975.533, 2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963, 3322.438, 3520, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032, 5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040, 7458.62, 7902.133, 8372.018, 8869.844, 9397.273, 9956.063, 10548.08, 11175.3, 11839.82, 12543.85};
byte globalNote = 0;
byte globalVelocity = 0;
int octave = 0;
const float DIV127 = (1.0 / 127.0);
float detuneFactor = 1;
float bendFactor = 1;
int bendRange = 12;

unsigned int LFOspeed = 2000;
float LFOpitch = 1;
float LFOdepth = 0;
byte LFOmodeSelect = 0;

int FILfreq =  10000;
float FILfactor = 1;


byte sysExArray[17] = {
  127, //osc 1 gain
  127, //osc 2 gain
  0,   //noise gain
  2,   //octave
  10,  //attack
  80,  //delay
  80,  //sustain
  20,  //release
  1,   //waveform1
  2,   //waveform2
  6,   //detune
  40,  //fliter freq
  40,  //filter resonance
  2,   //bend range
  40,  //lfo speed
  40,  //lfo depth
  0   //lfo mode
}

void setup() {
  AudioMemory(20);
  usbMIDI.setHandleControlChange(myControlChange);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandlePitchChange(myPitchBend);
  usbMIDI.setHandleSystemExclusive(mySysEx);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.32);
  waveform1.begin(WAVEFORM_SAWTOOTH);
  waveform1.amplitude(0.75);
  waveform1.frequency(82.41);
  waveform1.pulseWidth(0.15);

  waveform2.begin(WAVEFORM_SAWTOOTH);
  waveform2.amplitude(0.75);
  waveform2.frequency(123);
  waveform2.pulseWidth(0.15);

  pink1.amplitude(1.0);

  mixer1.gain(0, 1.0);
  mixer1.gain(1, 1.0);
  mixer1.gain(2, 0.0);

  envelope1.attack(0);
  envelope1.decay(0);
  envelope1.sustain(1);
  envelope1.release(500);

  mySysEx(17, mySysExArray);
}


void loop() {
  usbMIDI.read();
  LFOupdate(false, LFOmodeSelect, FILfactor, LFOdepth);
}


void  mySysEx(byte* array, unsigned array_size)
{

  myControlChange(1, 100, array[0]);
  myControlChange(1, 101, array[1]);
  myControlChange(1, 102, array[2]);
  myControlChange(1, 103, array[3]);
  myControlChange(1, 104, array[4]);
  myControlChange(1, 105, array[5]);
  myControlChange(1, 106, array[6]);
  myControlChange(1, 107, array[7]);
  myControlChange(1, 108, array[8]);
  myControlChange(1, 109, array[9]);
  myControlChange(1, 110, array[10]);
  myControlChange(1, 111, array[11]);
  myControlChange(1, 112, array[12]);
  myControlChange(1, 113, array[13]);
  myControlChange(1, 114, array[14]);
  myControlChange(1, 115, array[15]);
  myControlChange(1, 116, array[16]);

}

void myNoteOn(byte channel, byte note, byte velocity) {
  if ( note > 23 && note < 108 ) {
    globalNote = note;
    globalVelocity = velocity;
    keyBuff(note, true);
    LFOupdate(true, LFOmodeSelect, FILfactor, LFOdepth);
  }
}

void myNoteOff(byte channel, byte note, byte velocity) {
  if ( note > 23 && note < 108 ) {
    keyBuff(note, false);
  }
}

void myPitchBend(byte channel, int bend) {
  float bendF = bend;
  bendF = bendF / 8192;
  bendF = bendF * bendRange;
  bendF = bendF / 12;
  bendFactor = pow(2, bendF);
  oscSet();
}

void keyBuff(byte note, bool playNote) {
  static byte buff[BUFFER];
  static byte buffSize = 0;

  // Add Note
  if (playNote == true && (buffSize < BUFFER) ) {
    oscPlay(note);
    buff[buffSize] = note;
    buffSize++;
    return;
  }

  // Remove Note
  else if (playNote == false && buffSize != 0) {
    for (byte found = 0; found < buffSize; found++) {
      if (buff[found] == note) {
        for (byte gap = found; gap < (buffSize - 1); gap++) {
          buff[gap] = buff[gap + 1];
        }
        buffSize--;
        buff[buffSize] = 255;
        if (buffSize != 0) {
          oscPlay(buff[buffSize - 1]);
          return;
        }
        else {
          oscStop();
          return;
        }
      }
    }
  }
}

void oscPlay(byte note) {
  waveform1.frequency(noteFreqs[note] * bendFactor * LFOpitch);
  waveform2.frequency(noteFreqs[note + octave] * detuneFactor * bendFactor * LFOpitch);
  float velo = (globalVelocity * DIV127);
  waveform1.amplitude(velo);
  waveform2.amplitude(velo);
  pink1.amplitude(velo);

  envelope1.noteOn();
}

void oscStop() {
  envelope1.noteOff();
}

void oscSet() {
  waveform1.frequency(noteFreqs[globalNote] * bendFactor * LFOpitch);
  waveform2.frequency(noteFreqs[globalNote + octave] * detuneFactor * bendFactor * LFOpitch);
}

void myControlChange(byte channel, byte control, byte value) {
  switch (control) {
    case 100:
      mixer1.gain(0, (value * DIV127));
      break;

    case 101:
      mixer1.gain(1, (value * DIV127));
      break;

    case 102:
      mixer1.gain(2, (value * DIV127));
      break;

    case 103:
      switch (value) {
        case 0:
          octave = 24;
          break;
        case 1:
          octave = 12;
          break;
        case 2:
          octave = 0;
          break;
        case 3:
          octave = -12;
          break;
        case 4:
          octave = -24;
          break;
      }
      oscSet();
      break;

    case 104:
      envelope1.attack(3000 * (value * DIV127));
      break;

    case 105:
      envelope1.decay(3000 * (value * DIV127));
      break;

    case 106:
      envelope1.sustain(value * DIV127);
      break;

    case 107:
      envelope1.release(3000 * (value * DIV127));
      break;

    case 108:
      switch (value) {
        case 0:
          waveform1.begin(WAVEFORM_SINE);
          break;
        case 1:
          waveform1.begin(WAVEFORM_TRIANGLE);
          break;
        case 2:
          waveform1.begin(WAVEFORM_SAWTOOTH);
          break;
        case 3:
          waveform1.begin(WAVEFORM_PULSE);
          break;
      }
      break;

    case 109:
      switch (value) {
        case 0:
          waveform2.begin(WAVEFORM_SINE);
          break;
        case 1:
          waveform2.begin(WAVEFORM_TRIANGLE);
          break;
        case 2:
          waveform2.begin(WAVEFORM_SAWTOOTH);
          break;
        case 3:
          waveform2.begin(WAVEFORM_PULSE);
          break;
      }
      break;

    case 110:
      detuneFactor = 1 - (0.05 * (value * DIV127));
      oscSet();
      break;

    case 111:
      FILfactor = value * DIV127;
      FILfreq = 10000 * (value * DIV127);
      if (LFOmodeSelect < 1 || LFOmodeSelect > 5)filter1.frequency(FILfreq);
      break;

    case 112:
      filter1.resonance((4.3 * (value * DIV127)) + 0.7);
      break;

    case 113:
      if (value <= 12 && value > 0) bendRange = value;
      break;

    case 114:
      {
        float xSpeed = value * DIV127;
        xSpeed = pow(100, (xSpeed - 1));
        LFOspeed = (70000 * xSpeed);
        break;
      }

    case 115:
      LFOdepth = value * DIV127;
      break;

    case 116:
      LFOmodeSelect = value;
      break;
  }
}

void LFOupdate(bool retrig, byte mode, float FILtop, float FILbottom) {
  static float LFO = 0;
  static unsigned long LFOtime = 0;
  static bool LFOdirection = false;
  unsigned long currentMicros = micros();
  static bool LFOstop = false;
  static float LFOrange = 0;
  static byte oldMode = 0;
  static bool retriggered = false;

  if (retrig == true) retriggered = true;


  if (currentMicros - LFOtime >= LFOspeed) {
    LFOtime = currentMicros;

    if (mode != oldMode) {
      if (mode == 0 || mode == 8) {
        LFOpitch = 1;
        oscSet();
        filter1.frequency(FILfreq);
      }
      else if (mode >= 1 || mode <= 7) {
        LFOpitch = 1;
        oscSet();
      }
      else if (mode >= 9 || mode <= 13) {
        filter1.frequency(FILfreq);
      }
      oldMode = mode;
    }

    LFOrange = FILtop - FILbottom;
    if (LFOrange < 0) LFOrange = 0;

    // LFO Modes
    switch (mode) {

      case 0: //Filter OFF
        return;
        break;
      case 1: //Filter FREE
        filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 2: //Filter DOWN
        if (retriggered == true) {
          LFOdirection = true;
          LFO = 1.0;
        }
        filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 3: //Filter UP
        if (retriggered == true) {
          LFOdirection = false;
          LFO = 0;
        }
        filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 4: //Filter 1-DN
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = true;
          LFO = 1.0;
        }
        if (LFOstop == false) filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 5: //Filter 1-UP
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = false;
          LFO = 0;
        }
        if (LFOstop == false) filter1.frequency(10000 * ((LFOrange * LFO) + LFOdepth));
        break;
      case 8: //Pitch OFF
        return;
        break;
      case 9: //Pitch FREE
        LFOpitch = (LFO * LFOdepth) + 1;
        oscSet();
        break;
      case 10: //Pitch DOWN
        if (retriggered == true) {
          LFOdirection = true;
          LFO = 1.0;
        }
        LFOpitch = (LFO * LFOdepth) + 1;
        oscSet();
        break;
      case 11: //Pitch UP
        if (retriggered == true) {
          LFOdirection = false;
          LFO = 0;
        }
        LFOpitch = (LFO * LFOdepth) + 1;
        oscSet();
        break;
      case 12: //Pitch 1-DN
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = true;
          LFO = 1.0;
        }
        if (LFOstop == false) {
          LFOpitch = (LFO * LFOdepth) + 1;
          oscSet();
        }
        break;
      case 13: //Pitch 1-UP
        if (retriggered == true) {
          LFOstop = false;
          LFOdirection = false;
          LFO = 0;
        }
        if (LFOstop == false) {
          LFOpitch = (LFO * LFOdepth) + 1;
          oscSet();
        }
        break;
    }

    retriggered = false;

    // Update LFO
    if (LFOdirection == false) { //UP
      LFO = (LFO + 0.01);
      if (LFO >= 1) {
        LFOdirection = true;
        LFOstop = true;
      }
    }

    if (LFOdirection == true) { //Down
      LFO = (LFO - 0.01);
      if (LFO <= 0) {
        LFOdirection = false;
        LFOstop = true;
      }
    }
  }
}
