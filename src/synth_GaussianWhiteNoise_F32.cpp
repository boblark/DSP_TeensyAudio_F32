/*
 *  synth_GaussianWhiteNoise_F32.cpp
 *  by Bob Larkin  W7PUA 15 June 2020
 * 
	Created: Chip Audette, OpenAudio, Feb 2017
	

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

#include "synth_whitenoise_f32.h"
#include "input_i2s_f32.h" //for the audio_convert_i16_to_f32 routine

// Park-Miller-Carta Pseudo-Random Number Generator
// http://www.firstpr.com.au/dsp/rand31/

void AudioSynthNoiseWhite_F32::update(void)
{
	audio_block_f32_t *blockOut;
	uint32_t it;
	float32_t rdev = 0.0f;
    float32_t* pd;

	if (sd < 0.01f) {
		return;  // Not enabled
	}

#if TEST_TIME_GWN
  if (iitt++ >1000000) iitt = -10;
  uint32_t t1, t2;
  t1 = tElapse;
#endif
	blockOut = AudioStream_F32::allocate_f32();
	if (!blockOut) {
	    if(errorPrint)  Serial.println("GWN-ERR: No output memory");
	        return;
	}

    pd = &blockOut->data[0];  // Pointer to write data
    /* The "Even Quicker" uniform random sample generator from D. E. Knuth and
     * H. W. Lewis and described in Chapter 7 of "Numerical Receipes in C",
     * 2nd ed, with the comment "this is about as good as any 32-bit linear
     * congruential generator, entirely adequate for many uses."
     */
    for(i0; i<blockSize; i++)  {
	    rdev = 0.0f;
        for (int i = 0; i<12; i++){   // Add 12, using Central Limit to get Gaussian
	       	idum = (uint32_t)1664525 * idum + (uint32_t)1013904223;
            it = FL_ONE | (FL_MASK & idum);
		    rdev += (*(float *)&it) - 1.0f;
	    }
	    // Next, to get general form
        //  return mu + sd * 3.4641016f * (rdev - 0.5*(float)M) / sqrtf((float32_t)M);
        *pd++ = sd*(rdev - 6.0f);   // Specific for mu=0.0, M=12
    }

	AudioStream_F32::transmit(blockOut);
	AudioStream_F32::release(blockOut);

#if TEST_TIME_GWN
  t2 = tElapse;
  if(iitt++ < 0) {
	  Serial.print("At Gaussian Noise end, microseconds = ");
	  Serial.println (t2 - t1);
   }
   t1 = tElapse;
#endif
}
