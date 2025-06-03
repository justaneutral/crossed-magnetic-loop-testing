#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <complex.h>

#define MAX_SAMPLES 10000
#define LINE_SIZE 256
#define MAX_TAPS 20
#define DB_FLOOR -100.0  // Minimum magnitude displayed (floor) in dB
#define PI 3.14159265358979323846

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    double time;
    double ch0;
    double ch1;
} DataSample;


typedef struct {
    double a[MAX_TAPS]; // feedback taps
    double b[MAX_TAPS]; // feedforward taps
    int na;             // number of a taps
    int nb;             // number of b taps
} IIRFilter;

// Function prototypes
void close_existing_gnuplot_windows(void);
void plot_fft_db(DataSample *data, int n_samples);
void generate_sinusoid(DataSample *data, int n_samples,
                       double amplitude_ch0, double amplitude_ch1,
                       double frequency_hz, double sampling_rate_hz);
void plot_fft(DataSample *data, int n_samples);
void filter_channel(double *data, int n_samples, const IIRFilter *filter);
void generate_filtered_noise(DataSample *data, int n_samples, double fs, const IIRFilter *filter);
double find_scale(const DataSample *data, int n_samples);
int calculate_sampling_rate(const DataSample *samples, int n_samples, double *sampling_rate, double *accuracy_percent);
int generate_iir_bandpass(double fs, double f_low, double f_high,int slope_db_per_hz, double suppression_db,int max_taps, IIRFilter *filter);
void filter_data_zero_phase(DataSample *data, int n_samples,const IIRFilter *filter);
// Parsing suffixes: 'm' (milli), 'u' (micro)
int parse_suffix(char *str, double *value);
int read_csv(const char *filename, DataSample *data, int *n_samples);
void plot_data(DataSample *data, int n_samples);
void plot_xy(DataSample *data, int n_samples);
void remove_dc(DataSample *signal, int N);


#endif // __INCLUDES_H__
