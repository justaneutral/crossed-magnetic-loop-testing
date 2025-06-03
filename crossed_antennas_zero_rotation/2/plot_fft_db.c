#include "includes.h"

void plot_fft_db(DataSample *data, int n_samples) {
    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    double fs = 1.0 / (data[1].time - data[0].time);

    double complex fft_ch0[n_samples];
    double complex fft_ch1[n_samples];

    // Perform DFT calculation
    for (int k = 0; k < n_samples; k++) {
        fft_ch0[k] = 0;
        fft_ch1[k] = 0;
        for (int n = 0; n < n_samples; n++) {
            double theta = -2.0 * M_PI * k * n / n_samples;
            fft_ch0[k] += data[n].ch0 * cexp(I * theta);
            fft_ch1[k] += data[n].ch1 * cexp(I * theta);
        }
    }

    fprintf(gp, "set title 'FFT Magnitude Spectrum (dB) with Floor %g dB'\n", DB_FLOOR);
    fprintf(gp, "set xlabel 'Frequency (Hz)'\n");
    fprintf(gp, "set ylabel 'Magnitude (dB)'\n");
    fprintf(gp, "plot '-' with lines title 'CH 0', '-' with lines title 'CH 1'\n");

    // Channel 0 Frequency axis
    for (int k = 0; k < n_samples / 2; k++) {
        double freq = (fs * k) / n_samples;
        double magnitude = cabs(fft_ch0[k]) * 2 / n_samples;
        double magnitude_db = 20 * log10(magnitude);
        if (magnitude_db < DB_FLOOR)
            magnitude_db = DB_FLOOR;
        fprintf(gp, "%lf %lf\n", freq, magnitude_db);
    }
    fprintf(gp, "e\n");

    // Channel 1 Frequency axis
    for (int k = 0; k < n_samples / 2; k++) {
        double freq = (fs * k) / n_samples;
        double magnitude = cabs(fft_ch1[k]) * 2 / n_samples;
        double magnitude_db = 20 * log10(magnitude);
        if (magnitude_db < DB_FLOOR)
            magnitude_db = DB_FLOOR;
        fprintf(gp, "%lf %lf\n", freq, magnitude_db);
    }
    fprintf(gp, "e\n");

    fflush(gp);
    printf("FFT plotted in gnuplot window (dB scale with floor at %g dB).\n", DB_FLOOR);
    pclose(gp);
}

