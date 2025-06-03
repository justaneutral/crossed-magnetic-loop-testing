#include "includes.h"

// Remove DC offset
void remove_dc(DataSample *signal, int N) {
    double mean[2] = {0,0};
    for (int i = 0; i < N; i++) {
	mean[0] += signal[i].ch0;
	mean[1] += signal[i].ch1;
    }

    mean[0] /= N;
    mean[1] /= N;

    for (int i = 0; i < N; i++) {
	signal[i].ch0 -= mean[0];
	signal[i].ch1 -= mean[1];
    }
}

