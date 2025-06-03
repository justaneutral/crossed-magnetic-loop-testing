#include "includes.h"

// Parsing suffixes: 'm' (milli), 'u' (micro)
int parse_suffix(char *str, double *value) {
    double val;
    char suffix[4] = "";
    int res = sscanf(str, "%lf%3s", &val, suffix);
    if (res < 1) return 0;
    if (strcmp(suffix, "m") == 0) val *= 1e-3;
    else if (strcmp(suffix, "u") == 0) val *= 1e-6;
    else if (strcmp(suffix, "") != 0) return 0;
    *value = val;
    return 1;
}

