#include "includes.h"

void plot_fft(DataSample *data, int n_samples) {
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

    fprintf(gp, "set title 'FFT Magnitude Spectrum'\n");
    fprintf(gp, "set xlabel 'Frequency (Hz)'\n");
    fprintf(gp, "set ylabel 'Magnitude'\n");
    fprintf(gp, "plot '-' with lines title 'CH 0', '-' with lines title 'CH 1'\n");

    // Frequency axis
    for (int k = 0; k < n_samples / 2; k++) {
        double freq = (fs * k) / n_samples;
        double magnitude = cabs(fft_ch0[k]) * 2 / n_samples;
        fprintf(gp, "%lf %lf\n", freq, magnitude);
    }
    fprintf(gp, "e\n");

    for (int k = 0; k < n_samples / 2; k++) {
        double freq = (fs * k) / n_samples;
        double magnitude = cabs(fft_ch1[k]) * 2 / n_samples;
        fprintf(gp, "%lf %lf\n", freq, magnitude);
    }
    fprintf(gp, "e\n");

    fflush(gp);
    printf("FFT plotted in gnuplot window.\n");
    pclose(gp);
}
