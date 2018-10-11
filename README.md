# TeensySynth
An arduino teensy based synthesizer originally from Notes and Volts

## Changes from the original version:
* sdd1306 oled module used for display
  * oled runs on i2c along with audio board saving many pins.
  * moved display updates to a timed code block rather than real time so incoming midi scrolls didn't bog down system
* added rotary click encoder code
  * top row will switch to inverted colors when in setting choice.  rotating encoder will cycle through available setetings.
  * top row will switch to normal non-inverted colors when in adjust mode.  Currently selected setting will be altered.
* CC messages will post MIDI updates to the display
* Playing note name is displayed on the 4th row of the display
  
## To Do:
* all of the arrays are in place to do patch saves/swaps
* once patch saves are working I intend to make them sysex requests for an external device to update
