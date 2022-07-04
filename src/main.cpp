#include <daisy_seed.h>

// stuff
#include "pitch_detector.h"
#include "in_to_out.h"

// namespaces
using namespace daisy;
DaisySeed hardware;


int main(void) {
    hardware.Configure();
    hardware.Init();

    hardware.StartAudio(InToOut);
}