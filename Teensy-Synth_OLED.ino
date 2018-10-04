// Teensy-Synth Part 8
// LFO Test
// By Notes and Volts
// www.notesandvolts.com

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);


// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=382,453
AudioSynthWaveform       waveform1;      //xy=386,394
AudioSynthNoisePink      pink1;          //xy=394,504
AudioMixer4              mixer1;         //xy=577,454
AudioFilterStateVariable filter1;        //xy=725,457
AudioEffectEnvelope      envelope1;      //xy=888,458
AudioSynthSimpleDrum     drum1;          //xy=196,679
AudioOutputI2S           i2s1;           //xy=1049,465
AudioConnection          patchCord8(drum1, 0, mixer1, 3);
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
const char* noteNames[] = {"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B"};
byte globalNote = 0;
byte globalVelocity = 0;
int octave = 0;
const float DIV127 = (1.0 / 127.0);
float detuneFactor = 1;
float bendFactor = 1;
int bendRange = 2;

unsigned int LFOspeed = 2000;
float LFOpitch = 1;
float LFOdepth = 0;
byte LFOmodeSelect = 0;

int FILfreq =  10000;
float FILfactor = 1;

char string[40];
int timecounter = 0;
int changecounter = 0;

ClickEncoder *encoder;
int16_t last, value;
int shift = 0;

byte sysExVal[] = {
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
};

const char* sysExNam[] {
  "osc1 vol",
  "osc2 vol",
  "noiseVol",
  "octave  ",
  "attack  ",
  "delay   ",
  "sustain ",
  "release ",
  "wavefrm1",
  "wavefrm2",
  "detune  ",
  "filter f",
  "filter r",
  "bend rng",
  "lfo sped",
  "lfo dpth",
  "lfo mode"
};

const byte CC[] = { 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116 };




void timerIsr() {
  encoder->service();
}

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

void setup() {





  AudioMemory(20);
  usbMIDI.setHandleControlChange(myControlChange);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandlePitchChange(myPitchBend);
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
  mixer1.gain(3, 1.0);

  envelope1.attack(0);
  envelope1.decay(0);
  envelope1.sustain(1);
  envelope1.release(500);

  //Wire.setSDA(19);
  //Wire.setSCL(18);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
  display.display();
  display.setTextSize(2);
  display.setTextColor(WHITE, BLACK);
  display.setTextWrap(false);
  sprintf(string, "Teensy\n Synth");
  //display.clearDisplay();

  encoder = new ClickEncoder(1, 0, 2);
  encoder->setAccelerationEnabled(0);  // we don't need to accellerate through the menu, someday.
  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);
  last = 0;
}


void loop() {
  static int menu = 0;
  usbMIDI.read();
  LFOupdate(false, LFOmodeSelect, FILfactor, LFOdepth);
  int now = millis();

  value = encoder->getValue();
  ClickEncoder::Button b = encoder->getButton();



  if (b == ClickEncoder::Clicked) {
    shift ? shift = 0 : shift = 1;
  }
  if (shift) {
    if (value > 0) {
      menu++;
    } else if (value < 0) {
      menu--;
    }
    if (menu > 16) {
      menu = 0;
    } else if (menu < 0) {
      menu = 16;
    }


    sprintf(string, "%s\n%d", sysExNam[menu], sysExVal[menu]);

  } else {
    display.setTextColor(WHITE, BLACK);
    if (value != 0) {
      sysExVal[menu] += value;

      //display.setTextColor(WHITE, BLACK);
      //sprintf(string, "%s\nshift:%d", sysExNam[menu], sysExVal[menu]);
      myControlChange(7, CC[menu], sysExVal[menu]);
    }
  }
  if ((now - timecounter) > 60) {
    //display.clearDisplay();
    display.setCursor(0, 0);
    display.print("        \n        ");
    display.setCursor(0, 0);
    if (shift) {
      display.setTextColor(BLACK, WHITE);
    }
    display.print(string);
    display.display();
    timecounter = now;
  }
}

void myNoteOn(byte channel, byte note, byte velocity) {
  if ( note > 23 && note < 108) {
    if (channel == 2) {
      display.setTextColor(WHITE, BLACK);
      display.setCursor(4, 48);
      display.print("d:");
      display.print(note);
      display.print(" v:");
      display.print(velocity);
      display.display();
      drum1.frequency(550);
      drum1.length(400);
      drum1.noteOn();
    } else {
      globalNote = note;
      globalVelocity = velocity;
      keyBuff(note, true);
      LFOupdate(true, LFOmodeSelect, FILfactor, LFOdepth);
    }
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
    display.setTextColor(WHITE, BLACK);
    display.setCursor(4, 48);
    display.print("note: ");
    display.print(noteNames[buff[buffSize - 1] % 12]);
    display.print(buff[buffSize - 1] / 12 - 1);
    display.print("   ");
    display.display();
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

          display.setTextColor(WHITE, BLACK);
          display.setCursor(4, 48);
          display.print("note: ");
          display.print(noteNames[buff[buffSize - 1] % 12]);
          display.print(buff[buffSize - 1] / 12 - 1);
          display.print("   ");
          display.display();
          return;
        }
        else {
          oscStop();
          int now = millis();
          if ((now - changecounter) > 5000) {
            display.fillScreen(BLACK);
            display.setTextColor(WHITE, BLACK);
            sprintf(string, "Teensy\n Synth");
            changecounter = millis();
          }
          display.setTextColor(WHITE, BLACK);
          display.setCursor(4, 48);
          display.print("        ");
          display.display();
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
  changecounter = millis();
  switch (control) {

    case 100:
      mixer1.gain(0, (value * DIV127));
      sprintf(string, "OSC1 Gan\n%2.0f%%", ((value * DIV127) * 100));
      break;

    case 101:
      mixer1.gain(1, (value * DIV127));
      sprintf(string, "OSC2 Gan\n%2.0f%%", ((value * DIV127) * 100));
      break;

    case 102:
      mixer1.gain(2, (value * DIV127));
      sprintf(string, "Noise\n%2.0f%%", ((value * DIV127) * 100));
      break;

    case 103:
      switch (value) {
        case 0:
          octave = 24;
          sprintf(string, "OSC2\nOctave +2");
          break;
        case 1:
          octave = 12;
          sprintf(string, "OSC2\nOctave +1");
          break;
        case 2:
          octave = 0;
          sprintf(string, "OSC2\nOctave 0");
          break;
        case 3:
          octave = -12;
          sprintf(string, "OSC2\nOctave -1");
          break;
        case 4:
          octave = -24;
          sprintf(string, "OSC2\nOctave -2");
          break;
      }
      oscSet();
      break;

    case 104:
      envelope1.attack(3000 * (value * DIV127));
      sprintf(string, "Attack\n%.0f", 3000 * (value * DIV127));
      break;

    case 105:
      envelope1.decay(3000 * (value * DIV127));
      sprintf(string, "Decay\n%.0f", 3000 * (value * DIV127));
      break;

    case 106:
      envelope1.sustain(value * DIV127);
      sprintf(string, "Sustain\n%2.0f%%", (value * DIV127) * 100);
      break;

    case 107:
      envelope1.release(3000 * (value * DIV127));
      sprintf(string, "Release\n%.0f", 3000 * (value * DIV127));
      break;

    case 108:
      switch (value) {
        case 0:
          waveform1.begin(WAVEFORM_SINE);
          sprintf(string, "OSC1\nSine");
          break;
        case 1:
          waveform1.begin(WAVEFORM_TRIANGLE);
          sprintf(string, "OSC1\nTriangle");
          break;
        case 2:
          waveform1.begin(WAVEFORM_SAWTOOTH);
          sprintf(string, "OSC1\nSawtooth");
          break;
        case 3:
          waveform1.begin(WAVEFORM_PULSE);
          sprintf(string, "OSC1\nPulse");
          break;
      }
      break;

    case 109:
      switch (value) {
        case 0:
          waveform2.begin(WAVEFORM_SINE);
          sprintf(string, "OSC2\nSine");
          break;
        case 1:
          waveform2.begin(WAVEFORM_TRIANGLE);
          sprintf(string, "OSC2\nTriangle");
          break;
        case 2:
          waveform2.begin(WAVEFORM_SAWTOOTH);
          sprintf(string, "OSC2\nSawtooth");
          break;
        case 3:
          waveform2.begin(WAVEFORM_PULSE);
          sprintf(string, "OSC2\nPulse");
          break;
      }
      break;

    case 110:
      detuneFactor = 1 - (0.05 * (value * DIV127));
      sprintf(string, "Detune\n%f", 1 - (0.05 * (value * DIV127)));
      oscSet();
      break;

    case 111:
      FILfactor = value * DIV127;
      FILfreq = 10000 * (value * DIV127);
      if (LFOmodeSelect < 1 || LFOmodeSelect > 5)filter1.frequency(FILfreq);
      sprintf(string, "Filter\n%d", FILfreq);
      break;

    case 112:
      filter1.resonance((4.3 * (value * DIV127)) + 0.7);
      sprintf(string, "Resonanc\n%2.0f", (value * DIV127) * 100);
      break;

    case 113:
      if (value <= 12 && value > 0) bendRange = value;
      sprintf(string, "Bend Rng\n%d", value);
      break;

    case 114:
      {
        float xSpeed = value * DIV127;
        xSpeed = pow(100, (xSpeed - 1));
        LFOspeed = (70000 * xSpeed);
        sprintf(string, "LFO Sped\n%d", LFOspeed);
        break;
      }

    case 115:
      LFOdepth = value * DIV127;
      sprintf(string, "LFO Dpth\n%2.0f", (value * DIV127) * 100);
      break;

    case 116:
      LFOmodeSelect = value;
      switch (value) {
        case 0:
          sprintf(string, "LFO\nOff");
          break;

        case 1:
          sprintf(string, "LFO\nFree");
          break;

        case 2:
          sprintf(string, "LFO\nDown");
          break;

        case 3:
          sprintf(string, "LFO\nUp");
          break;

        case 4:
          sprintf(string, "LFO\n1-down");
          break;

        case 5:
          sprintf(string, "LFO\n1 - up");
          break;
      } break;
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


