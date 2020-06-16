/*
 *  synth_GaussianWhiteNoise_F32.h
 *  by Bob Larkin  W7PUA 15 June 2020
 * 	
	Extended by: Chip Audette, OpenAudio, Feb 2017
 *	
	License: MIT License.  Use at your own risk.
*/

/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef synth_GaussianWhiteNoise_f32_h_
#define synth_GaussianWhiteNoise_f32_h_
#include "Arduino.h"
//#include "AudioStream.h"
#include "AudioStream_F32.h"

// Temporary timing test
#define TEST_TIME_GWN 0

#define FL_ONE  0X3F800000
#define FL_MASK 0X007FFFFF

class AudioSynthGaussianWhiteNoise_F32 : public AudioStream_F32
{
//GUI: inputs:0, outputs:1  //this line used for automatic generation of GUI node
//GUI: shortName:Gaussianwhitenoise  //this line used for automatic generation of GUI node
public:
	AudioSynthGaussianWhiteNoise_F32() : AudioStream_F32(0, NULL) { }
	// Allow for changing block size?
	AudioSynthGaussianWhiteNoise_F32(const AudioSettings_F32 &settings) : AudioStream_F32(0, NULL) { }

    // Gaussian amplitude is specified by the 1-sigma (standard deviation) value.
    // sd=0.0 is un-enabled.
	void amplitude(float _sd) {
		sd = _sd;  // Enduring copy 
		if (sd<0.0)  sd=0.0;
	}
	
	virtual void update(void);
	
private:
    blockSize = 128;
    uint32_t idum = 12345;
    float32_t sd = 0.0;  // Default to off
    
    //  *Temporary* - TEST_TIME allows measuring time in microseconds for each part of the update()
#if TEST_TIME_GWN
    elapsedMicros tElapse;
    int32_t iitt = 999000;     // count up to a million during startup
#endif
};

#endif
