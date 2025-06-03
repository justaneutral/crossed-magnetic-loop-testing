#include "includes.h"

void plot_data(DataSample *data, int n_samples) {
    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    fprintf(gp, "set title 'Voltage Channels vs. Time'\n");
    fprintf(gp, "set xlabel 'Time (s)'\n");
    fprintf(gp, "set ylabel 'Voltage (V)'\n");
    fprintf(gp, "plot '-' with lines title 'CH 0', '-' with lines title 'CH 1'\n");

    for (int i = 0; i < n_samples; i++)
        fprintf(gp, "%lf %lf\n", data[i].time, data[i].ch0);
    fprintf(gp, "e\n");

    for (int i = 0; i < n_samples; i++)
        fprintf(gp, "%lf %lf\n", data[i].time, data[i].ch1);
    fprintf(gp, "e\n");

    fflush(gp);
    printf("Plot displayed in gnuplot window.\n");
    pclose(gp);
}

