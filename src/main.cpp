#include "daisy_seed.h"
#include "daisysp.h"

// Use the daisy namespace to prevent having to type
// daisy:: before all libdaisy functions
using namespace daisy;
using namespace daisysp;

// Declare a DaisySeed object called hardware
DaisySeed hardware;

Oscillator osc;

void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size) {
    float osc_out;
    
    for (size_t i = 0; i < size; i += 2) {
        osc_out = osc.Process();

        out[i] = osc_out;
        out[i+1] = osc_out;
    }
}

int main(void)
{
    // Configure and Initialize the Daisy Seed
    // These are separate to allow reconfiguration of any of the internal
    // components before initialization.
    hardware.Configure();
    hardware.Init();
    hardware.SetAudioBlockSize(4);

    float samplerate = hardware.AudioSampleRate();

    osc.Init(samplerate);
    osc.SetAmp(0.1);
    osc.SetFreq(1000);
    osc.SetWaveform(osc.WAVE_SIN);

    hardware.StartAudio(AudioCallback);
    
    for (;;) {}
}
