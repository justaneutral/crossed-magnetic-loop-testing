#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MAX_SAMPLES 8192
#define MAX_ORDER 8
#define PI 3.14159265358979323846

typedef struct {
    int order;
    double a[MAX_ORDER + 1];
    double b[MAX_ORDER + 1];
} IIRFilter;

// Generate linear frequency sweep sinusoid
void generate_sweep(double *signal, int n_samples, double fs) {
    double f0 = 0.0;
    double f1 = fs / 2.0;

    for (int n = 0; n < n_samples; n++) {
        double t = (double)n / fs;
        double instantaneous_freq = f0 + (f1 - f0) * ((double)n / n_samples);
        signal[n] = sin(2 * PI * instantaneous_freq * t);
    }
}

// Generate simplified bandpass IIR coefficients (Butterworth)
int generate_bandpass_iir(double fs, double f_low, double f_high, 
                          double transition_bw, double attenuation_db, 
                          IIRFilter *filter) {
    if (f_low >= f_high || f_high >= fs / 2) {
        fprintf(stderr, "Invalid cutoff frequencies.\n");
        return -1;
    }

    filter->order = 2;
    
    // Simplified second-order bandpass Butterworth design
    double w0 = 2 * PI * sqrt(f_low * f_high) / fs;
    double bw = 2 * PI * (f_high - f_low) / fs;

    double alpha = sin(w0) * sinh(log(2) / 2.0 * bw / sin(w0));

    filter->b[0] = alpha;
    filter->b[1] = 0.0;
    filter->b[2] = -alpha;

    filter->a[0] = 1.0 + alpha;
    filter->a[1] = -2.0 * cos(w0);
    filter->a[2] = 1.0 - alpha;

    // Normalize coefficients
    for (int i = 0; i <= filter->order; i++) {
        filter->b[i] /= filter->a[0];
        filter->a[i] /= filter->a[0];
    }

    return 0;
}

// Zero-phase (forward-backward) filtering
void filter_signal(double *signal, int n_samples, const IIRFilter *filter) {
    double temp[n_samples];
    memset(temp, 0, sizeof(temp));

    // Forward filtering
    for (int n = 0; n < n_samples; n++) {
        temp[n] = filter->b[0] * signal[n];
        for (int k = 1; k <= filter->order; k++) {
            if (n >= k)
                temp[n] += filter->b[k] * signal[n - k] - filter->a[k] * temp[n - k];
        }
    }

    // Backward filtering
    double output[n_samples];
    memset(output, 0, sizeof(output));

    for (int n = n_samples - 1; n >= 0; n--) {
        output[n] = filter->b[0] * temp[n];
        for (int k = 1; k <= filter->order; k++) {
            if (n + k < n_samples)
                output[n] += filter->b[k] * temp[n + k] - filter->a[k] * output[n + k];
        }
    }

    // Copy result back to original signal
    memcpy(signal, output, sizeof(output));
}

int main(void) {
    const double fs = 1000.0;             // Sampling frequency (Hz)
    const int n_samples = 1024;           // Number of samples
    const double f_low = 100.0;           // Lower cutoff frequency
    const double f_high = 200.0;          // Upper cutoff frequency
    const double transition_bw = 20.0;    // Transition bandwidth (Hz)
    const double attenuation_db = 40.0;   // Out-of-band attenuation (dB)

    double input_signal[MAX_SAMPLES];
    double filtered_signal[MAX_SAMPLES];

    // Generate sweep
    generate_sweep(input_signal, n_samples, fs);
    memcpy(filtered_signal, input_signal, sizeof(double) * n_samples);

    // Generate filter coefficients
    IIRFilter filter;
    if (generate_bandpass_iir(fs, f_low, f_high, transition_bw, attenuation_db, &filter) != 0) {
        fprintf(stderr, "Failed to generate filter coefficients.\n");
        return EXIT_FAILURE;
    }

    // Filter the signal
    filter_signal(filtered_signal, n_samples, &filter);

    // Print first 20 samples of input and filtered signals
    printf("Sample\tInput\tFiltered\n");
    for (int i = 0; i < 20; i++) {
        printf("%d\t%.6f\t%.6f\n", i, input_signal[i], filtered_signal[i]);
    }

    return EXIT_SUCCESS;
}

