/*     synth_sin_cos_f32.h
 *   AudioSynthSineCosine_F32
 * 
 * Status: Checked for function and accuracy.  19 April 2020
 *
 * Created: Bob Larkin 15 April 2020
 *
 * Based on Chip Audette's OpenAudio sine(), that was
 * Modeled on: AudioSynthWaveformSine from Teensy Audio Library
 *
 * Purpose: Create sine and cosine wave of given amplitude, frequency
 * and phase.  Outputs are audio_block_f32_t blocks of float32_t.
 * Routines are from the arm CMSIS library and use a 512 point lookup
 * table with linear interpolation to achieve float accuracy limits.
 *
 * This provides for setting the phase of the sine, setting the difference
 * in phase between the sine and cosine and setting the
 * (-amplitude, amplitude) range.  If these are at the default values,
 * called doSimple, the caluclation is faster.
 * For doSimple either true or false, the frequency can be changed
 * at will using the frequency() method. 
 * 
 * Defaults:
 * Frequency:  1000.0 Hz
 * Phase of Sine:  0.0 radians (0.0 deg)
 * Phase of Cosine:  pi/2 radians (90.0 deg) ahead of Sine
 * Amplitude:  -1.0 to 1.0
 * 
 * License: MIT License. Use at your own risk.
 *
 */

#ifndef synth_sin_cos_f32_h_
#define synth_sin_cos_f32_h_

// #include "Arduino.h"
#include "AudioStream_F32.h"
#include "arm_math.h"

#ifndef M_PI
#define M_PI   3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2  1.57079632679489661923
#endif

#ifndef M_TWOPI
#define M_TWOPI  (M_PI * 2.0)
#endif

#define MF2_PI 6.2831853f

#define TEST_TIME_SC 0

class AudioSynthSineCosine_F32 : public AudioStream_F32  {
//GUI: inputs:0, outputs:2 //this line used for automatic generation of GUI node
//GUI: shortName:SineCosine  //this line used for automatic generation of GUI node
public:
    AudioSynthSineCosine_F32(void) : AudioStream_F32(0, NULL) { } //uses default AUDIO_SAMPLE_RATE from AudioStream.h

    AudioSynthSineCosine_F32(const AudioSettings_F32 &settings) : AudioStream_F32(0, NULL) {  
	    setSampleRate_Hz(settings.sample_rate_Hz);
	    setBlockLength(settings.audio_block_samples);
    }
    
    void frequency(float32_t fr) {    // Frequency in Hz
		freq = fr;
        if (freq < 0.0f) freq = 0.0f;
        else if (freq > sample_rate_Hz/2.0f) freq = sample_rate_Hz/2.0f;
        phaseIncrement = 512.0f * freq / sample_rate_Hz;
    }

    /* Externally, phase comes in the range (0,2*M_PI) keeping with C math functions
     * Internally,  the full circle is represented as (0.0, 512.0).  This is
     * convenient for finding the entry to the sine table.
     */
    void phase_r(float32_t a) {
        while (a < 0.0f) a += MF2_PI;
        while (a > MF2_PI) a -= MF2_PI;
        phaseS = 512.0f * a / MF2_PI;
        doSimple = false;
        return;
    }
    
    // phaseS_C_r  is the number of radians that the cosine output leads the
    // sine output.  The default is M_PI_2 = pi/2 = 1.57079633 radians,
    // corresponding to 90.00 degrees cosine leading sine.
    void phaseS_C_r(float32_t a) {
        while (a < 0.0f) a += MF2_PI;
        while (a > MF2_PI) a -= MF2_PI;
        // Internally a full circle is 512.00 of phase
        phaseS_C = 512.0f * a / MF2_PI;
        doSimple = false;
        return;
    }

    // The amplitude, a, is the peak, as in zero-to-peak.  This produces outputs
    // ranging from -a to +a.  Both outputs are the same amplitude.
    void amplitude(float32_t a) {
        amplitude_pk = a;
        doSimple = false;
        return;
    }

     // Speed up calculations by setting phaseS_C=90deg, amplitude=1
     void simple(bool s) {
        doSimple = s;
        if(doSimple) {
			phaseS_C = 128.0f;
			amplitude_pk = 1.0f;
	    }
        return;
    }   

    void setSampleRate_Hz(float32_t fs_Hz) {
        // Check freq range
        if (freq > sample_rate_Hz/2.0f) freq = sample_rate_Hz/2.f;
        // update phase increment for new frequency
        phaseIncrement = 512.0f * freq / fs_Hz;
    }

    void setBlockLength(uint16_t bl) {
      if(bl > 128)  bl = 128;
      block_length = bl;
      }
    
    virtual void update(void);

private:
#if TEST_TIME_SC
//  *Temporary* - TEST_TIME_SC allows measuring time in microseconds for each part of the update()
elapsedMicros tElapse;
int32_t iitt = 999000;     // count up to a million during startup
#endif

    float32_t freq = 1000.0f;
    float32_t phaseS = 0.0f;
    float32_t phaseS_C = 128.00;
    float32_t amplitude_pk = 1.0f;
    float32_t sample_rate_Hz = AUDIO_SAMPLE_RATE;
    float32_t phaseIncrement = 512.0f * freq /sample_rate_Hz;
    uint16_t  block_length = 128;
    // if only freq() is used, the complexities of phase, phaseS_C,
    // and amplitude are not used, speeding up the sin and cos:
    bool      doSimple = true;
};
#endif
