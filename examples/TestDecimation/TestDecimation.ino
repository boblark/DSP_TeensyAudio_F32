#include <Arduino.h>
#include "Audio.h"

int32_t n1, n2, gain;
uint32_t lo, hi, val1;

#define FL_ONE  0X3F800000
#define FL_MASK 0X007FFFFF
uint32_t idum = 0;


// uint32_t seed;  // must start at 1
int32_t seed = 1;

void setup(void) {
	Serial.begin(1);  delay(1000);
	// Print 10 int32_t uniform random numbers in hex to compare with book
    //  for(int i=0; i<10; i++)   Serial.println(ranum(), HEX);
    
    // Generate 100 int16 uniform random numbers
    //  for(int i=0; i<100; i++)   Serial.println(ranum16());
    
    // Generate 1000 int32_t random numbers, uniform of (0.0, 1.0)
    //  for(int i=0; i<1000; i++)  Serial.println(ranumf(), 8);

    // Generate 1000 samples of float32_t White Gaussian Noise, mean=0, stdev = 1.0    
    //  for(int i=0; i<1000; i++)  Serial.println(wgnf(), 8);
  
    // Generate 10,000 samples of float32_t WGN based on 50 uniform per sample.
    // This achieves +/-3.8 sigma 
    //  for(int i=0; i<10000; i++)  Serial.println(wgnGenf(0.3f, 2.5f, 50), 8);
    
    // Generate 16 bit GWN
    for(int i=0; i<1000; i++)  Serial.println( wgn16() );
}

void loop(void) { }


// The "Even Quicker" uniform random sample generator from D. E. Knuth and
// H. W. Lewis and described in Chapter 7 of "Numerical Receipes in C",
// 2nd ed, with the comment "this is about as good as any 32-bit linear
// congruential generator, entirely adequate for many uses."  This is white
// noise meaning the the correlation function for any spacing (except 2^32)
// is zero.
uint32_t ranum(void) {
	return (idum = (uint32_t)1664525 * idum + (uint32_t)1013904223);
}

// Here is one way of returning 16-bit random numbers, the high-order 16-bits
// formatted as (-2^15, 2^15-1), i.e., (-32768, 32767).
int16_t ranum16(void) {
	return (q15_t)( (idum = (uint32_t)1664525 * idum + (uint32_t)1013904223) >> 16 );
}

// Extending this simple approach, "Numerical Recipies in C" suggests a fast way to
// turn this into a 32-bit IEEE, quite clever and it works!  This returns samples
// of uniformly distributed random noise in float32_t on (0.0, 1.0)
float32_t ranumf(void) {
	uint32_t it;
	idum = (uint32_t)1664525 * idum + (uint32_t)1013904223;
    it = FL_ONE | (FL_MASK & idum);
    return (*(float *)&it) - 1.0f;
}

// The Central Limit Theorem lets us generate Gaussian noise by adding
// several uniform samples together.  More samples improves the distribution
// for values far from the mean.  Here is the simplest of these, choosing
// 12 samples as it does not need a final scale for the standard deviation
// of 1.  This is float32_t sample of WGN, mean=0, standard deviation = 1.0.
float32_t wgnf(void) {
	uint32_t it;
	float32_t rdev = 0.0f;
    for (int i = 0; i<12; i++){
		idum = (uint32_t)1664525 * idum + (uint32_t)1013904223;
        it = FL_ONE | (FL_MASK & idum);
		rdev += (*(float *)&it) - 1.0f;
	}
    return rdev - 6.0f;
}

/* Taking this last idea back to creating 16-bit integer random samples of GWN,
 * we again use 12 sample sums.  Without shifting, this produces a standard
 *  deviation that is over full scale.  By shifting four more bits, a scale
 * factor of 16, we can make the standard deviation a convenient 2048. Note
 * that since we added 12 numbers, and it takes x16 to overload, 
 * we never exceed 32767.  The max is (12/16) x 32768 or 24576.
 * wgn16() returns an int16_t sample of WGN, mean=0, standard deviation = 2048.
 */
int16_t wgn16(void) {
	int64_t rdev = 0;
    for (int i = 0; i<12; i++){
		idum = (uint32_t)1664525 * idum + (uint32_t)1013904223; //Serial.print("idum = "); Serial.println((q15_t)idum);
 		rdev += (q15_t)idum;
	}
    return (q15_t)(rdev >> 4);      //(q15_t)(rdev >> 20);
}

// This simple exension allows specifying the parameters  mean = mu,
// standard deviation = sd and the number of uniform samples added together, M.
float32_t wgnGenf(float32_t mu, float32_t sd, uint16_t M) {
	uint32_t it;
	float32_t rdev = 0.0f;
    for (int i = 0; i<M; i++){
		idum = (uint32_t)1664525 * idum + (uint32_t)1013904223;
        it = FL_ONE | (FL_MASK & idum);
		rdev += (*(float *)&it) - 1.0f;
	}
    return mu + sd * 3.4641016f * (rdev - 0.5*(float)M) / sqrtf((float32_t)M);
}

/*
float32_t wgnf(void) {
	float32_t rdev = 0.0f;
    for (int i = 0; i<12; i++)  rdev += ranumf();
    return rdev - 6.0f;
}
* */


#if 0
//Park Miller Carta 31 bit Random Noise
uint32_t rand31_next(void) {
    uint32_t hi, lo;
    lo = 16807*(seed & 0xFFFF);
    hi = (seed >> 16);
    
    lo += (hi & 0x7FFF) << 16;
    lo += hi >> 15;

    lo = (lo & 0x7FFFFFFF) + (lo >> 31);   // Simper change
    //if (lo > 0x7FFFFFFF)  lo -= 0x7FFFFFFF;  // Original
    
    return (seed = (uint32_t)lo);
  }
  #endif

#if 0
uint32_t rn_F32(void) {
lo = seed;
		hi = multiply_16bx16t(16807, lo); // 16807 * (lo >> 16)
		lo = 16807 * (lo & 0xFFFF);
		lo += (hi & 0x7FFF) << 16;
		lo += hi >> 15;
		lo = (lo & 0x7FFFFFFF) + (lo >> 31);
		n1 = signed_multiply_32x16b(gain, lo);
		hi = multiply_16bx16t(16807, lo); // 16807 * (lo >> 16)
		lo = 16807 * (lo & 0xFFFF);
		lo += (hi & 0x7FFF) << 16;
		lo += hi >> 15;
		lo = (lo & 0x7FFFFFFF) + (lo >> 31);
		n2 = signed_multiply_32x16b(gain, lo);
		val1 = pack_16b_16b(n2, n1);
seed=lo;
}
#endif

#if 0
//Park Miller Carta 31 bit Random Noise
unsigned long int rand31_next(void) {
    unsigned long int hi, lo;
    lo = 16807*(seed & 0xFFFF);
    hi = (seed >> 16);
    
    lo += (hi & 0x7FFF) << 16;
    lo += hi >> 15;
    
    if (lo > 0x7FFFFFFF)  lo -= 0x7FFFFFFF;
    return (seed = (long)lo);
  }
#endif

#if 0
// Utility program to create "Half-band" FIR coefficients that
// have every other coefficient = 0.0.
// Bob Larkin, W7PUA  11 June 2020

#include <Arduino.h>
#include "Audio.h"

#ifndef M_PI
#define M_PI   3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2  1.57079632679489661923
#endif

#ifndef M_TWOPI
#define M_TWOPI  (M_PI * 2.0)
#endif

#ifndef MF_PI
#define MF_PI 3.1415926f
#endif

int16_t i,  j;
uint16_t ntaps, n, N, nHalfFIR, nFreq;
float win[100], cf0[100], cf[100], kdb, beta, kbes, scaleXn2, xn2, WindowWt;
float bt, f, piOnNfreq;

void setup(void) {
   Serial.begin(1); delay(1000);
   Serial.println("Design Half-Band FIR Filter ");
   
   // INPUT DATA, kdb AND ntaps
   kdb = 35.0;  
   // ntaps must be odd, but ntaps+1 should be divisable by 4 to avoid
   // a zero coeficient at the ends.
   ntaps = 79;	
   while(4*((ntaps+1)/4) != (ntaps+1))  ntaps--;  // Lower if necessary
   
   nHalfFIR = (ntaps - 1)/2;
   N= ntaps-1;
   for(i=0; i<100; i++)  cf0[i]=8.8888f;
   for(i=0; i<100; i++)  cf[i] =9.9999f; 
     
   cf0[0] = 0.5f;
   for (i=1; i<=nHalfFIR; i++)  {
       cf0[i] = sinf(i*M_PI_2)/(i*M_PI);     
   }

   if (kdb < 20.0f)
        beta = 0.0;
   else
        beta = -2.17+0.17153*kdb-0.0002841*kdb*kdb; // Within a dB or so
        
   // Note: i0f is the fp zero'th order modified Bessel function (see mathDSP_F32.h)
   kbes = 1.0f / i0f(beta);      // An additional derived parameter used in loop
   scaleXn2 = 4.0f / ( ((float32_t)ntaps - 1.0f)*((float32_t)ntaps - 1.0f) ); // Needed for even & odd

   for (i=0; i<=nHalfFIR; i++) {  // For 39 Taps, this is 0 to 19, including center
        xn2 = (int16_t)(0.5f+(float32_t)i);
        xn2 = scaleXn2*xn2*xn2;
        WindowWt=kbes*i0f(beta*sqrt(1.0-xn2));
        win[i] = WindowWt;
        cf0[i] *= WindowWt;
    }

    for (i=nHalfFIR; i>=0; i--) {
        cf[ntaps - i - 1] = cf0[nHalfFIR - i];  // Move lower half to upper, retaining order
	 }
    for (i=0; i<nHalfFIR; i++) 
        cf[i] = cf[ntaps - i - 1];  // Create lower half 
        
	// *****  Write coefficient data  *********
   Serial.print("For Sidelobe Max = "); Serial.print(kdb);
   Serial.print(", Number of Coeffs = "); Serial.println(ntaps);
   Serial.println("nCoeff  Window  Coefficient");
   for (i=0; i<ntaps; i++) {
	   Serial.print(i);  Serial.print(",    ");
	   if(i<=nHalfFIR) Serial.print(win[nHalfFIR-i],8);
	   else	          Serial.print(win[i-nHalfFIR],8);
	   Serial.print(", "); Serial.println(cf[i],8);
	}
	
	// *****  Write frequency response  *********
	Serial.println();  Serial.println("Frequency Response in dB");
	Serial.println("Rel Freq  Response");
    nFreq=500;
    piOnNfreq = MF_PI / (float32_t)nFreq;
    for (i=0; i<nFreq; i++) {
        bt = cf[nHalfFIR];  // Center coefficient
        for (j=0; j<nHalfFIR; j++) {  // Add in the others twice, as they are symmetric
            bt += 2.0f*cf[j]*cosf(piOnNfreq*(float32_t)((nHalfFIR-j)*i));
         }
         Serial.print(0.5*((float)i) / ((float)nFreq), 6); Serial.print(", ");
         Serial.println(20.0f*log10f(fabsf(bt)),3);     // Convert to dB
	}

	// *****  Write C code  *********	
    // y[n] = b[0] * x[n] + b[1] * x[n-1] + b[2] * x[n-2] + ...+ b[numTaps-1] * x[n-numTaps+1]  
 // THIS IS NOT CORRECT, BUT CAPTURES THE SPIRIT...
    Serial.println("C code with full unwrapping");
    Serial.println("for(uint16_t cfi=0; cfi<cfiMax; cfi++) {");
    for(i=0; i<nHalfFIR; i++) {
		if(2*(i/2)==i) {
		    Serial.print("    y[");  Serial.print(i); Serial.print("] += (");
		    Serial.print(cf[i],8); Serial.print("f*x[cfiMax - ");
		    Serial.print(i); Serial.println("]);");
		    Serial.print("    y[");  Serial.print(ntaps-i-1); Serial.print("] += (");
		    Serial.print(cf[i],8); Serial.print("f*x[cfiMax - ");
		    Serial.print(ntaps-i-1); Serial.println("]);");
		}
	}
	Serial.print("    y[");  Serial.print(nHalfFIR); Serial.print("] += "); Serial.print(cf[nHalfFIR],8); Serial.println("f*x[cfi];");
	Serial.println("}");
}

void loop(void) {
}

// Modified zeroth order Bessel function
float i0f(float x) {
    float af, bf, cf;
    if( (af=fabsf(x)) < 3.75f ) {
        cf = x/3.75f;
        cf = cf*cf;
        bf=1.0f+cf*(3.515623f+cf*(3.089943f+cf*(1.20675f+cf*(0.265973f+
             cf*(0.0360768f+cf*0.0045813f)))));
    }
    else {
        cf = 3.75f/af;
        bf=(expf(af)/sqrtf(af))*(0.3989423f+cf*(0.0132859f+cf*(0.0022532f+
             cf*(-0.0015756f+cf*(0.0091628f+cf*(-0.0205771f+cf*(0.0263554f+
             cf*(-0.0164763f+cf*0.0039238f))))))));
    }
    return bf;
}
#endif
