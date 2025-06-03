#include "includes.h"

// Generate FIR bandpass filter using a Hamming windowed sinc filter
int generate_fir_bandpass(double fs, double f_low, double f_high,
    double suppression_db,
    int max_taps, FIRFilter *filter) {
if (!filter || f_low >= f_high || fs <= 0.0 || f_high >= fs/2.0) {
fprintf(stderr, "Invalid filter parameters.\n");
return -1;
}

// Transition bandwidth estimation from suppression_db
double trans_bw = (fs / 2.0) * (suppression_db / 120.0);
if (trans_bw <= 0.0) trans_bw = (f_high - f_low) * 0.1;

// Estimate number of taps using empirical formula for Hamming window
int N = (int)(3.3 * fs / trans_bw);
if (N % 2 == 0) N += 1; // ensure odd number of taps for symmetry
if (N > max_taps) {
fprintf(stderr, "Required taps (%d) exceed max taps (%d).\n", N, max_taps);
return -1;
}

filter->num_taps = N;

int M = (N - 1) / 2;
double f1 = f_low / fs;
double f2 = f_high / fs;

// Generate filter taps
for (int n = 0; n < N; n++) {
int k = n - M;
if (k == 0) {
filter->taps[n] = 2.0 * (f2 - f1);
} else {
filter->taps[n] = (sin(2 * M_PI * f2 * k) - sin(2 * M_PI * f1 * k)) / (M_PI * k);
}
// Apply Hamming window
filter->taps[n] *= (0.54 - 0.46 * cos(2 * M_PI * n / (N - 1)));
}

// Normalize filter taps
double sum = 0.0;
for (int n = 0; n < N; n++) {
sum += filter->taps[n];
}
for (int n = 0; n < N; n++) {
filter->taps[n] /= sum;
}

return 0;
}



// FIR filtering (convolution)
void filter_fir(double *data, int n_samples, const FIRFilter *filter) {
    if (!data || !filter || n_samples <= 0) {
        fprintf(stderr, "Invalid arguments for filter_fir.\n");
        return;
    }

    double *temp = (double*)calloc(n_samples, sizeof(double));
    if (!temp) {
        fprintf(stderr, "Memory allocation failed.\n");
        return;
    }

    int half_len = filter->num_taps / 2;

    for (int i = 0; i < n_samples; i++) {
        double sum = 0.0;
        for (int j = 0; j < filter->num_taps; j++) {
            int idx = i - half_len + j;
            if (idx >= 0 && idx < n_samples)
                sum += data[idx] * filter->taps[j];
        }
        temp[i] = sum;
    }

    memcpy(data, temp, n_samples * sizeof(double));
    free(temp);
}


void filter_data(DataSample *data, int n_samples, const FIRFilter *filter) {
    double x[n_samples];
    double y[n_samples];

    for (int i = 0; i < n_samples; i++) {
        x[i] = data[i].ch0;
        y[i] = data[i].ch1;
    }

    //filtering
    filter_fir(x, n_samples, filter);
    filter_fir(y, n_samples, filter);
 
    for (int i = 0; i < n_samples; i++) {
        data[i].ch0 = x[i];
        data[i].ch1 = y[i];
    }
}
