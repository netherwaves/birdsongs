#pragma once

#ifndef H_CHANT
#define H_CHANT

#include <math.h>


struct Chant {
    const char* filename;
    const char* name;

    float pitch_stdev;
    float pitch_mean;
    float amp_stdev;
    float amp_mean;
    float length_mean;
};


float CalculateDistance(Chant* c1, Chant* c2) {
    float dist;
    float sum = 0;

    // pitch_stdev
    dist = (c2->pitch_stdev - c1->pitch_stdev);
    sum += dist * dist;

    // pitch_avg
    dist = (c2->pitch_mean - c1->pitch_mean);
    sum += dist * dist;

    // amp_stdev
    dist = (c2->amp_stdev - c1->amp_stdev);
    sum += dist * dist;

    // amp_avg
    dist = (c2->amp_mean - c1->amp_mean);
    sum += dist * dist;

    // length_avg
    dist = (c2->length_mean - c1->length_mean);
    sum += dist * dist;

    // return
    return sum > 0.0 ? sqrt(sum) : 0.0;
}


#endif