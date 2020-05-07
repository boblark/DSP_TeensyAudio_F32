/* MathTestAtan.ino numerical test, using audio blocks, of MathAtan2_F32 audio object.
 *
 * This tests the OpenAudio_Arduino / Teensy Audio Library math object as a math
 * calculation.  128 number pairs are loaded through 2 Queues, processed by MathAtan2_F32,
 * and brought out to another Queue to examine.  No real audio/radio function is involved.
 * 
 * Bob Larkin 8 April 2020
 */
#include "Audio.h"
#include <OpenAudio_ArduinoLibrary.h>
#include "mathDSP_F32.h"
#include <math.h>

/* The inputs are the x and y components of a circle divided into 128 segments.
 * The ratio of y/x is the tangent of the angle formed with the x-axis. We are testing
 * the accuracy of our arcTan function that returns the angle given the x and y values.
 * 
 * Audio objects are next.  InputI2S needed for data transfers between blocks, etc.
 */
AudioInputI2S_F32          audioInI2S1;
AudioPlayQueue_F32         queuex;
AudioPlayQueue_F32         queuey;
// Next calculates 128 atan2(y,x) from 2 audio block inputs
MathAtan2_F32              atn2;
AudioRecordQueue_F32       queueAtan;
//Create audio connections
AudioConnection_F32        patchCord1(queuex, 0, atn2, 0);
AudioConnection_F32        patchCord2(queuey, 0, atn2, 1);
AudioConnection_F32        patchCord3(atn2, 0, queueAtan, 0);

float32_t *xData, *yData, *atanData;
float32_t theta[128];  // Angle, around unit circle, in radians

void setup() {
  Serial.begin(300); delay(1000);
  Serial.println ("  ***  Test Accuracy of MathAtan2_F32  ***");
  AudioMemory(5);
  AudioMemory_F32(10);

  // Create x and y data to go once around the unit circle
  xData = queuex.getBuffer();
  yData = queuey.getBuffer();

  // Load the arrays with the angle, x and y, using slow but
  // accurate cosf() and sinf()
  for (int i=0; i<128; i++) {
    theta[i] = -M_PI + 2.0f * M_PI * (float)i / 128.0f;
    xData[i] = cosf(theta[i]);
    yData[i] = sinf(theta[i]);
  }
  // Initialize objects requiring this
  queueAtan.begin();
  queuex.playBuffer();
  queuey.playBuffer();
}

void loop() {
  float32_t *aTan;

  if (queueAtan.available()) {
    Serial.println("Output Available");
    aTan = queueAtan.readBuffer();  
    for (int i = 0; i<128; i++) {
      Serial.print("Computed angle = ");  Serial.print(aTan[i], 7);
      float32_t er = aTan[i] - theta[i];
      // At 0 and any multiple of 2*pi thesre can be ambiguities (not
      // errors) of 2*pi. The following two statements readjusts these:
      if (er < -M_PI) er += 2*M_PI;
      if (er > M_PI)  er -= 2*M_PI;
      Serial.print("   Error in radians = ");  Serial.println(er, 7);
    }
    queueAtan.freeBuffer();
  }
}
