#ifndef OUTPUT_H
#define OUTPUT_H

#define WAVEFORM_RESOLUTION 120 //

#define AMPLITUDE_MAX 1.0
#define AMPLITUDE_MIN 0.0
#define AMPLITUDE_RESOLUTION 0.01

#define PLATEAU_MAX 1.0
#define PLATEAU_MIN 0.0
#define PLATEAU_RESOLUTION 0.01

#define FREQUENCY_MIN 5.0
#define FREQUENCY_MAX 300.0
#define FREQUENCY_RESOLUTION 1.0

// the #include statment and code go here...

int getOutputSignalValueL(int phaseIndex, float plateauProportion, float vcAmplitude, bool symmetricVibration, bool inverseVibration, bool torqueL, bool torqueR);
int getOutputSignalValueR(int phaseIndex, float plateauProportion, float vcAmplitude, bool symmetricVibration, bool inverseVibration, bool torqueL, bool torqueR);



#endif
