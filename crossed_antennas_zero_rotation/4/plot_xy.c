#include "includes.h"

void plot_xy(DataSample *data, int n_samples) {
    FILE *gp = popen("gnuplot -persistent", "w");
    if (!gp) {
        perror("popen");
        return;
    }

    double scale = find_scale(data, n_samples);

    fprintf(gp, "set title 'Channel 0 vs. Channel 1'\n");
    fprintf(gp, "set xlabel 'Voltage CH 0 (V)'\n");
    fprintf(gp, "set ylabel 'Voltage CH 1 (V)'\n");
    //fprintf(gp, "set size ratio 1'\n"); // this ensures equal scaling
    fprintf(gp, "set xrange [%lf:%lf]\n",-1.0*scale,scale);
    fprintf(gp, "set yrange [%lf:%lf]\n",-1.0*scale,scale);
    fprintf(gp, "set grid\n");
    fprintf(gp, "plot '-' with linespoints title 'CH0 vs CH1'\n");

    for (int i = 0; i < n_samples; i++)
        fprintf(gp, "%lf %lf\n", data[i].ch0, data[i].ch1);
    fprintf(gp, "e\n");

    fflush(gp);
    printf("XY plot displayed in gnuplot window.\n");
    pclose(gp);
}

