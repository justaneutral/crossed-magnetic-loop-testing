#include "includes.h"

/*
void generate_sinusoid(DataSample *data, int n_samples,
                       double amplitude_ch0, double amplitude_ch1,
                       double frequency_hz, double sampling_rate_hz)
{
    if (!data || n_samples <= 0 || sampling_rate_hz <= 0) {
        fprintf(stderr, "Invalid arguments provided.\n");
        return;
    }

    for (int i = 0; i < n_samples; i++) {
        data[i].time = (double)i / sampling_rate_hz;
        double angle = 2 * M_PI * frequency_hz * data[i].time;
        data[i].ch0 = amplitude_ch0 * sin(angle);
        data[i].ch1 = amplitude_ch1 * sin(angle);
    }
}
*/

void generate_sinusoid(DataSample *data, int n_samples,
                       double amplitude_ch0, double amplitude_ch1,
                       double frequency_hz, double sampling_rate_hz)
{
    if (!data || n_samples <= 0 || sampling_rate_hz <= 0) {
        fprintf(stderr, "Invalid arguments provided.\n");
        return;
    }

    if (frequency_hz < 0) {
        printf("[DEBUG] Negative frequency provided (%.2f Hz). Clearing data array.\n", frequency_hz);
        memset(data, 0, sizeof(DataSample) * n_samples);
    } else {
        printf("[DEBUG] Adding sinusoid: frequency = %.2f Hz, amplitudes CH0 = %.2f, CH1 = %.2f\n",
               frequency_hz, amplitude_ch0, amplitude_ch1);

        for (int i = 0; i < n_samples; i++) {
            data[i].time = (double)i / sampling_rate_hz;
            double angle = 2 * M_PI * frequency_hz * data[i].time;
            data[i].ch0 += amplitude_ch0 * sin(angle);
            data[i].ch1 += amplitude_ch1 * sin(angle);
        }
    }
}
