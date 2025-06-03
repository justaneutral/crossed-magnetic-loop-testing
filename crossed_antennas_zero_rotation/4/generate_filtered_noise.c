#include "includes.h"


// White noise generator [-1..1]
double white_noise() {
    return 2.0 * ((double)rand() / RAND_MAX - 0.5);
}


void generate_filtered_noise(DataSample *data, int n_samples, double fs, const FIRFilter *filter) {
    srand(time(NULL));

    double ch0[n_samples], ch1[n_samples];

    for (int i = 0; i < n_samples; i++) {
        data[i].time = (double)i / fs;
        ch0[i] = white_noise();
        ch1[i] = white_noise();
    }

    filter_fir(ch0, n_samples, filter);
    filter_fir(ch1, n_samples, filter);

    for (int i = 0; i < n_samples; i++) {
        data[i].ch0 = ch0[i];
        data[i].ch1 = ch1[i];
    }
}
