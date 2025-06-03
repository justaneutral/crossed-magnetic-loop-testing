#include "iir_filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Simplified Butterworth bandpass design using bilinear transform
int generate_iir_bandpass(double fs, double f_low, double f_high,
                          int slope_db_per_hz, double suppression_db,
                          int max_taps, IIRFilter *filter) {

    if (max_taps < 3 || !filter) {
        fprintf(stderr, "Invalid parameters to generate filter.\n");
        return -1;
    }

    printf("[DEBUG] Sampling Frequency: %.2f Hz\n", fs);
    printf("[DEBUG] Filter specs: Low=%.2f Hz, High=%.2f Hz, Slope=%d dB/Hz, Suppression=%.2f dB\n",
            f_low, f_high, slope_db_per_hz, suppression_db);

    int order = slope_db_per_hz / 6; // 6 dB/octave per order for Butterworth

    if (order * 2 + 1 > max_taps) {
        fprintf(stderr, "Insufficient taps for desired slope. Max taps: %d, Needed: %d\n",
                max_taps, order * 2 + 1);
        return -1;
    }

    filter->na = order * 2 + 1;
    filter->nb = filter->na;

    printf("[DEBUG] Calculated Filter Order: %d\n", order);
    printf("[DEBUG] Number of taps used: %d\n", filter->na);

    double W1 = tan(M_PI * f_low / fs);
    double W2 = tan(M_PI * f_high / fs);
    double BW = W2 - W1;
    double W0 = sqrt(W1 * W2);

    printf("[DEBUG] Pre-warped frequencies: W1=%.6f, W2=%.6f\n", W1, W2);

    memset(filter->a, 0, sizeof(filter->a));
    memset(filter->b, 0, sizeof(filter->b));

    filter->b[0] = BW;
    filter->b[1] = 0.0;
    filter->b[2] = -BW;
    filter->a[0] = 1 + BW + W0*W0;
    filter->a[1] = 2*(W0*W0 - 1);
    filter->a[2] = 1 - BW + W0*W0;

    // Normalize
    for (int i = 0; i < 3; i++) {
        filter->b[i] /= filter->a[0];
        filter->a[i] /= filter->a[0];
    }

    printf("[DEBUG] Filter coefficients (b): ");
    for (int i = 0; i < filter->nb; i++)
        printf("%lf ", filter->b[i]);
    printf("\n");

    printf("[DEBUG] Filter coefficients (a): ");
    for (int i = 0; i < filter->na; i++)
        printf("%lf ", filter->a[i]);
    printf("\n");

    return 0;
}

// Forward-backward zero-phase filtering
static void filter_channel(double *data, int n_samples, const IIRFilter *filter) {
    double temp[n_samples];
    memset(temp, 0, sizeof(temp));

    // Forward filtering
    for (int i = 0; i < n_samples; i++) {
        temp[i] = filter->b[0] * data[i];
        for (int j = 1; j < filter->nb; j++)
            if (i-j >= 0) temp[i] += filter->b[j] * data[i-j];
        for (int j = 1; j < filter->na; j++)
            if (i-j >= 0) temp[i] -= filter->a[j] * temp[i-j];
    }

    // Backward filtering
    for (int i = n_samples-1; i >=0; i--) {
        data[i] = filter->b[0] * temp[i];
        for (int j = 1; j < filter->nb; j++)
            if (i+j < n_samples) data[i] += filter->b[j] * temp[i+j];
        for (int j = 1; j < filter->na; j++)
            if (i+j < n_samples) data[i] -= filter->a[j] * data[i+j];
    }
}

void filter_data_zero_phase(DataSample *data, int n_samples, 
                            const IIRFilter *filter) {
    double ch0[n_samples], ch1[n_samples];

    for (int i = 0; i < n_samples; i++) {
        ch0[i] = data[i].ch0;
        ch1[i] = data[i].ch1;
    }

    filter_channel(ch0, n_samples, filter);
    filter_channel(ch1, n_samples, filter);

    for (int i = 0; i < n_samples; i++) {
        data[i].ch0 = ch0[i];
        data[i].ch1 = ch1[i];
    }
}

