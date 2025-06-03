#include "includes.h"

int read_csv(const char *filename, DataSample *data, int *n_samples) {
    FILE *file = fopen(filename, "r");
    if (!file) { perror("fopen"); return 0; }
    char line[LINE_SIZE];
    int idx = 0;
    fgets(line, sizeof(line), file);  // Skip header
    while (fgets(line, sizeof(line), file) && idx < MAX_SAMPLES) {
        char *token, *rest = line;
        if (!(token = strtok_r(rest, ",", &rest)) || !parse_suffix(token, &data[idx].time)) continue;
        if (!(token = strtok_r(rest, ",", &rest)) || !parse_suffix(token, &data[idx].ch0)) continue;
        if (!(token = strtok_r(rest, ",\n", &rest)) || !parse_suffix(token, &data[idx].ch1)) continue;
        idx++;
    }
    fclose(file);
    *n_samples = idx;
    return 1;
}

