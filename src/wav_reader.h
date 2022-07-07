#include <daisy_seed.h>
#include <daisysp.h>
#include <stdio.h>

using namespace daisy;
using namespace daisysp;

SdmmcHandler   sdcard;
FatFSInterface fsi;
WavPlayer      sampler, sampler2;

Oscillator osc;

void ReadAudioFile(AudioHandle::InterleavingInputBuffer  in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t                                size)
{
    float sampler_out, sampler2_out, osc_out;

    for(size_t i = 0; i < size; i += 2)
    {
        sampler_out = s162f(sampler.Stream());

        osc_out = osc.Process();
        
        out[i] = sampler_out * 0.1f;
        out[i + 1] = sampler_out * 0.1f;
    }
}


void setup_sampler(DaisySeed &hw) {
    SdmmcHandler::Config sd_cfg;
    sd_cfg.Defaults();
    sd_cfg.speed = SdmmcHandler::Speed::VERY_FAST;
    sd_cfg.width = SdmmcHandler::BusWidth::BITS_1;
    
    sdcard.Init(sd_cfg);
    fsi.Init(FatFSInterface::Config::MEDIA_SD);
    f_mount(&fsi.GetSDFileSystem(), "/", 1);

    sampler.Init(fsi.GetSDPath());
    //sampler2.Init(fsi.GetSDPath());

    sampler.SetLooping(true);
    //sampler2.SetLooping(true);

    osc.Init(48000);
    osc.SetFreq(900);
    osc.SetAmp(0.1);
    osc.SetWaveform(Oscillator::WAVE_SIN);

    hw.SetAudioBlockSize(48);
    hw.StartAudio(ReadAudioFile);

    for(;;) {
        sampler.Prepare();
        //sampler2.Prepare();
    }
}