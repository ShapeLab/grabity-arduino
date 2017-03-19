#include "output.h"

int getOutputSignalValueL(int phaseIndex, float plateauProportion, float vcAmplitude, bool symmetricVibration, bool inverseVibration, bool torqueL, bool torqueR) {
  int resultL;
  // if it's a plateau, return so.
  if (symmetricVibration) {
    if (phaseIndex < WAVEFORM_RESOLUTION / 2) {
      resultL = (int)(vcAmplitude * 0x800) + 0x7ff;
    } else {
      resultL = (int)(0x7ff - vcAmplitude * 0x7ff);
    }
  }

  else if (inverseVibration) {
    if (plateauProportion * WAVEFORM_RESOLUTION > phaseIndex) {
      resultL = 0xfff - ((int)(vcAmplitude * 0x800) + 0x7ff);
    }
    // if it's not, use the pp, amp, and ph/wavres to solve it.
    else {
      float phasor = phaseIndex / (float)(WAVEFORM_RESOLUTION);
      float top = (vcAmplitude * .5) + .5;
      float drop = (phasor - plateauProportion) * (vcAmplitude / (1 - plateauProportion));
      resultL = 0xfff - (int)(0xfff * (top - drop));
    }
  }

  else if (torqueL) {
    if (plateauProportion * WAVEFORM_RESOLUTION > phaseIndex) {
      resultL = 0xfff - ((int)(vcAmplitude * 0x800) + 0x7ff);
    }
    // if it's not, use the pp, amp, and ph/wavres to solve it.
    else {
      float phasor = phaseIndex / (float)(WAVEFORM_RESOLUTION);
      float top = (vcAmplitude * .5) + .5;
      float drop = (phasor - plateauProportion) * (vcAmplitude / (1 - plateauProportion));
      resultL = 0xfff - (int)(0xfff * (top - drop));
    }
  }

  else {
    if (plateauProportion * WAVEFORM_RESOLUTION > phaseIndex) {
      resultL = (int)(vcAmplitude * 0x800) + 0x7ff;
    }
    // if it's not, use the pp, amp, and ph/wavres to solve it.
    else {
      float phasor = phaseIndex / (float)(WAVEFORM_RESOLUTION);
      float top = (vcAmplitude * .5) + .5;
      float drop = (phasor - plateauProportion) * (vcAmplitude / (1 - plateauProportion));
      resultL = (int)(0xfff * (top - drop));
    }
  }
  return resultL;
}

int getOutputSignalValueR(int phaseIndex, float plateauProportion, float vcAmplitude, bool symmetricVibration, bool inverseVibration, bool torqueL, bool torqueR) {
  int resultR;
  // if it's a plateau, return so.
  if (symmetricVibration) {
    if (phaseIndex < WAVEFORM_RESOLUTION / 2) {
      resultR = (int)(vcAmplitude * 0x800) + 0x7ff;
    } else {
      resultR = (int)(0x7ff - vcAmplitude * 0x7ff);
    }
  }

  else if (inverseVibration) {
    if (plateauProportion * WAVEFORM_RESOLUTION > phaseIndex) {
      resultR = 0xfff - ((int)(vcAmplitude * 0x800) + 0x7ff);
    }
    // if it's not, use the pp, amp, and ph/wavres to solve it.
    else {
      float phasor = phaseIndex / (float)(WAVEFORM_RESOLUTION);
      float top = (vcAmplitude * .5) + .5;
      float drop = (phasor - plateauProportion) * (vcAmplitude / (1 - plateauProportion));
      resultR = 0xfff - (int)(0xfff * (top - drop));
    }
  }

  else if (torqueR) {
    if (plateauProportion * WAVEFORM_RESOLUTION > phaseIndex) {
      resultR = 0xfff - ((int)(vcAmplitude * 0x800) + 0x7ff);
    }
    // if it's not, use the pp, amp, and ph/wavres to solve it.
    else {
      float phasor = phaseIndex / (float)(WAVEFORM_RESOLUTION);
      float top = (vcAmplitude * .5) + .5;
      float drop = (phasor - plateauProportion) * (vcAmplitude / (1 - plateauProportion));
      resultR = 0xfff - (int)(0xfff * (top - drop));
    }
  }

  else {
    if (plateauProportion * WAVEFORM_RESOLUTION > phaseIndex) {
      resultR = (int)(vcAmplitude * 0x800) + 0x7ff;
    }
    // if it's not, use the pp, amp, and ph/wavres to solve it.
    else {
      float phasor = phaseIndex / (float)(WAVEFORM_RESOLUTION);
      float top = (vcAmplitude * .5) + .5;
      float drop = (phasor - plateauProportion) * (vcAmplitude / (1 - plateauProportion));
      resultR = (int)(0xfff * (top - drop));
    }
  }
  return resultR;
}
