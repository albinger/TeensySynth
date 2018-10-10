# Andrew's mode to TeensySynth


## Changes from the original version:
* I installed an OLED display
  * oled runs on i2c along with audio board saving many pins.
  * i2c uses pins 18 and 19
  * CC messages on values coming from a midi controller were buffering badly across usb MIDI when I was doing display updates directly in myChangeControl(). I changed to updating 2 global variables instead of screen updates and then handle the screeen updates based on a millis() read in the main loop().
  * every so often I reset the global variables to say "teensy synth" on the display.  If you are turning knobs this gets overwritten with the updates otherwise most of the time you are playing you get to see the name of the synth.

  * added rotary click encoder code
  * top row will switch to inverted colors when in setting choice.  rotating encoder will cycle through available setetings.
  * top row will switch to normal non-inverted colors when in adjust mode.  Currently selected setting will be altered.

* Playing note name is displayed on the 4th row of the display.  
  * This is handled directly in the functions to play the notes.
  * I added an array with the note names so that I can display notes instead of MIDI note numbers.
  