#include "includes.h"

double find_scale(const DataSample *data, int n_samples) {
    double max_abs = 0.0;

    for (int i = 0; i < n_samples; i++) {
        double abs_ch0 = fabs(data[i].ch0);
        double abs_ch1 = fabs(data[i].ch1);

        if (abs_ch0 > max_abs)
            max_abs = abs_ch0;

        if (abs_ch1 > max_abs)
            max_abs = abs_ch1;
    }

    return max_abs;
}
