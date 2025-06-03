#ifndef IIR_FILTER_H
#define IIR_FILTER_H

#define _USE_MATH_DEFINES
#include <math.h>

#define MAX_TAPS 20

typedef struct {
    double a[MAX_TAPS]; // feedback taps
    double b[MAX_TAPS]; // feedforward taps
    int na;             // number of a taps
    int nb;             // number of b taps
} IIRFilter;

typedef struct {
    double time;
    double ch0;
    double ch1;
} DataSample;

// Function prototypes
int calculate_sampling_rate(const DataSample *samples, int n_samples, double *sampling_rate, double *accuracy_percent);
int generate_iir_bandpass(double fs, double f_low, double f_high,
                          int slope_db_per_hz, double suppression_db,
                          int max_taps, IIRFilter *filter);

void filter_data_zero_phase(DataSample *data, int n_samples, 
                            const IIRFilter *filter);

#endif

