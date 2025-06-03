#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SAMPLES 10000
#define LINE_SIZE 256

typedef struct {
    double time;
    double ch0;
    double ch1;
} DataSample;

int parse_suffix(char *str, double *value) {
    double val;
    char suffix[4] = "";
    int res = sscanf(str, "%lf%3s", &val, suffix);

    if (res < 1) return 0;

    if (strcmp(suffix, "m") == 0)
        val *= 1e-3;
    else if (strcmp(suffix, "u") == 0)
        val *= 1e-6;
    else if (strcmp(suffix, "") != 0)
        return 0;

    *value = val;
    return 1;
}

int read_csv(const char *filename, DataSample *data, int *n_samples) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("fopen");
        return 0;
    }

    char line[LINE_SIZE];
    int idx = 0;

    // Skip header line
    if (!fgets(line, sizeof(line), file)) {
        fprintf(stderr, "Empty file or read error\n");
        fclose(file);
        return 0;
    }

    while (fgets(line, sizeof(line), file) && idx < MAX_SAMPLES) {
        char *token;
        char *rest = line;

        token = strtok_r(rest, ",", &rest);
        if (!token) continue;
        if (!parse_suffix(token, &data[idx].time)) continue;

        token = strtok_r(rest, ",", &rest);
        if (!token) continue;
        if (!parse_suffix(token, &data[idx].ch0)) continue;

        token = strtok_r(rest, ",\n", &rest);
        if (!token) continue;
        if (!parse_suffix(token, &data[idx].ch1)) continue;

        idx++;
    }

    fclose(file);
    *n_samples = idx;
    return 1;
}

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

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <data_file.csv>\n", argv[0]);
        return EXIT_FAILURE;
    }

    DataSample data[MAX_SAMPLES];
    int n_samples = 0;

    if (!read_csv(argv[1], data, &n_samples)) {
        fprintf(stderr, "Error reading CSV file.\n");
        return EXIT_FAILURE;
    }

    if (n_samples == 0) {
        fprintf(stderr, "No valid samples found.\n");
        return EXIT_FAILURE;
    }

    printf("Read %d samples successfully.\n", n_samples);
    plot_data(data, n_samples);

    return EXIT_SUCCESS;
}

