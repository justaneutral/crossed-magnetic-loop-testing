#include <stdio.h>
#include <stdlib.h>
#include "iir_filter.h"

#define N_SAMPLES 1024

int main() {
    DataSample data[N_SAMPLES];
    double fs_init = 1000.0; // Sampling frequency in Hz

    // Simulate some data
    for (int i = 0; i < N_SAMPLES; i++) {
        data[i].time = i/fs_init;
        data[i].ch0 = sin(2*M_PI*50*data[i].time) + 0.5*sin(2*M_PI*200*data[i].time);
        data[i].ch1 = sin(2*M_PI*80*data[i].time) + 0.3*sin(2*M_PI*300*data[i].time);
    }

    IIRFilter filter;
    double fs, accuracy_percent;

    if(calculate_sampling_rate(data, N_SAMPLES, &fs, &accuracy_percent)) {
      printf("Sampling Rate = %lf Hz, Accuracy = ±%lf%%\n", fs, accuracy_percent);
    }
    else
      fs = fs_init;

    if (generate_iir_bandpass(fs, 40.0, 100.0, 12, 60.0, MAX_TAPS, &filter) != 0) {
        fprintf(stderr, "Filter creation failed.\n");
        return -1;
    }

    filter_data_zero_phase(data, N_SAMPLES, &filter);

    printf("[Filtered Data]\n");
    for (int i = 0; i < 10; i++)
        printf("t=%.3fs: ch0=%.6f, ch1=%.6f\n", data[i].time, data[i].ch0, data[i].ch1);

    return 0;
}

