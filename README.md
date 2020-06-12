# DSP_TeensyAudio_F32
Collected blocks for use with OpenAudio/Tympan libraries with floating point.

This is a collection of dsp blocks, i.e. Teensy Arduino objects to work with the Chip Audette OpenAudio_ArduinoLibrary.  This library provides 32-bit floating-point extensions to the PJRC Teensy Audio Library, which exchanges data in 16-bit data blocks.  The target processor currently has been both the PJRC Teensy 3.6 and Teensy 4.0.

My use of these blocks is for radio receivers and transmitter.  The goal is to be able to put together full-featured radios with DSP functions for low-frequency i-f and audio fnctions performed by existing libraries augmented by the blocks here.  Until a radio, or two, is functioning this is quite experimental.  Every attempt will be made to not change existing member function definitions, but that may happen, when really needed.  New member functions may be added without concern of breaking exsisting code, so that will happen.

This library can be installed in parallel with Chip's https://github.com/chipaudette/OpenAudio_ArduinoLibrary if that library has the following:
* Remove input_i2s_f32.h and .cpp (use I16 versions with AudioConvert_I16toF32)
* Remove output_i2s_f32.h and .cpp  (and AudioConvert_F32toI16)

and because of need for simple fixes
* Remove synth_pinknoise_f32.h and .cpp
* Remove synth_whitenoise_f32.h and .cpp

and replace AudioMixer_F32.h and .cpp with the same files from the Tympan library https://github.com/Tympan/Tympan_Library

INO's should use

#include "Audio.h"

#include "OpenAudio_ArduinoLibrary.h"

#include "DSP_TeensyAudio_F32.h"

Bob Larkin,  W7PUA
