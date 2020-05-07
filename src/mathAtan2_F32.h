/*
 * mathAtan2_F32.h
 *
 * 3 April 2020  Bob Larkin
 * Staus - Accuracy and time tested.  Still BETA
 *         block_size fixed at 128
 *
 * This building block is in support of, and using structure of the library:
 * Chip Audette, OpenAudio, Apr 2017
 * In turn, this is based on the Teensy Audio Library of PJRC
 *     -------------------
 * There are two inputs,  0 (x) and 1 (y)
 * There is one output, ArcTan2(y/x) expressed in
 * radians (180 degrees is Pi radians).
 * 
 * Measurements show that it takes 37 microseconds to
 * process a 128 point block on a T3.6
 *
 * MIT License,  Use at your own risk.
*/

#ifndef _math_atan2_f32_h
#define _math_atan2_f32_h
// Test the number of microseconds to execute update(), if set to 1
#define TEST_TIME_ATAN 0

#include "AudioStream_F32.h"
#include "mathDSP_F32.h"

class MathAtan2_F32 : public AudioStream_F32 {
//GUI: inputs:2, outputs:1  //this line used for automatic generation of GUI node
//GUI: shortName: MathAtan2
public:
    // Option of AudioSettings_F32 change to block size or sample rate:
    MathAtan2_F32(void) :  AudioStream_F32(2, inputQueueArray_f32)
        {  }
    MathAtan2_F32(const AudioSettings_F32 &settings) : AudioStream_F32(2, inputQueueArray_f32)
        {block_size = settings.audio_block_samples; }  // Sample rate not used

    // Do atan2(y,x) on a full audio block
    void update(void) {
        audio_block_f32_t *block_0, *block_1;
        mathDSP_F32 mathDSP1;    // Math support functions
#if TEST_TIME_ATAN
        uint32_t t1, t2;
        t1 = tElapse;
#endif
        // Get first block
        block_0 = AudioStream_F32::receiveWritable_f32(0);
        if (!block_0)  return;
        // Get second input
        block_1 = AudioStream_F32::receiveReadOnly_f32(1);
        if (!block_1){
            AudioStream_F32::release(block_0);
            return;
        }
        for (uint i = 0; i<block_size; i++) {
            // block_0 is x, block_1 is y, find atan2(y, x)
            block_0->data[i] = mathDSP1.fastAtan2(block_1->data[i], block_0->data[i]);
        }
        AudioStream_F32::release(block_1);   // Done with this one
        AudioStream_F32::transmit(block_0, 0);  // The answers
        AudioStream_F32::release(block_0);

#if TEST_TIME_ATAN
        // This is temporary during initial testing
        t2 = tElapse;
        Serial.print("Microseconds: ");
        Serial.println (t2 - t1);
#endif
    }     // End update()
 
private:
    uint16_t block_size = AUDIO_BLOCK_SAMPLES;
    // Two input data pointers
    audio_block_f32_t *inputQueueArray_f32[2];
    
    //  *Temporary* - TEST_TIME_ATAN allows measuring time in microseconds
#if TEST_TIME_ATAN
    elapsedMicros tElapse;
#endif

};
#endif

