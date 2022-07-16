#include <daisy_seed.h>
#include <q/pitch/pitch_detector.hpp>
#include <q/fx/signal_conditioner.hpp>
#include <q/support/notes.hpp>
#include <q/fx/biquad.hpp>
#include <q/synth/sin.hpp>

// namespaces
using namespace daisy;
namespace q = cycfi::q;
using namespace cycfi::q::literals;

// sample rate
uint32_t sample_rate = 48000;

// frequencies
q::frequency detected_f0 = q::frequency(0.0f);
q::frequency lowest = q::notes::C[2];
q::frequency highest = q::notes::C[5];

// preprocessor
q::signal_conditioner::config preprocessor_config;
q::signal_conditioner         preprocessor{preprocessor_config,
                                           lowest,
                                           highest,
                                           sample_rate};
            
// main pitch detector
q::pitch_detector pd{lowest, highest, sample_rate, q::decibel{0}};

// phase objects
q::phase phase_accumulator = q::phase();
q::phase f0_phase          = q::phase(detected_f0, sample_rate);


void PitchDetector(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size) {
    for (size_t i = 0; i < size; i += 2) {
        float sig_l = in[i];
        float sig_r = in[i+1];

        float pd_sig = preprocessor(sig_l);

        if (pd(pd_sig)) {
            detected_f0 = q::frequency{pd.get_frequency()};
            f0_phase    = q::phase(detected_f0, sample_rate);
        }

        out[i] = q::sin(phase_accumulator) * 0.5;
        out[i+1] = sig_r;

        phase_accumulator += f0_phase;
    }
}

void ConfigurePitchDetection() {
    
}