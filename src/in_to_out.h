#include <daisy_seed.h>

using namespace daisy;

void InToOut(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size) {
    for (size_t i = 0; i < size; i += 2) {
        float in_L = in[i];
        float in_R = in[i+1];

        // do stuff to signal here i guess

        out[i] = in_L;
        out[i+1] = in_R;
    }
}