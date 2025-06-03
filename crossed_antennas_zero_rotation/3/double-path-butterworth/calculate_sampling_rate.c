#include <stdio.h>
#include <math.h>
#include "iir_filter.h"

// Function to calculate sampling rate and accuracy
int calculate_sampling_rate(const DataSample *samples, int n_samples, double *sampling_rate, double *accuracy_percent) {
    if (n_samples < 2 || !samples || !sampling_rate || !accuracy_percent) {
        fprintf(stderr, "Insufficient data samples provided.\n");
        return -1;
    }

    double total_diff = 0.0;
    double max_diff = -1e9;
    double min_diff = 1e9;

    // Calculate differences between consecutive timestamps
    for (int i = 1; i < n_samples; i++) {
        double diff = samples[i].time - samples[i - 1].time;
        total_diff += diff;
        if (diff > max_diff) max_diff = diff;
        if (diff < min_diff) min_diff = diff;
    }

    // Average sampling interval
    double avg_interval = total_diff / (n_samples - 1);

    // Derived sampling rate
    *sampling_rate = 1.0 / avg_interval;

    // Calculate accuracy as percent deviation of intervals from average interval
    double max_deviation = fmax(fabs(max_diff - avg_interval), fabs(min_diff - avg_interval));
    *accuracy_percent = (max_deviation / avg_interval) * 100.0;

    // Debug output
    printf("Calculated average sampling interval: %lf seconds\n", avg_interval);
    printf("Sampling rate: %lf Hz\n", *sampling_rate);
    printf("Accuracy: ±%lf%%\n", *accuracy_percent);

    return 0;
}

