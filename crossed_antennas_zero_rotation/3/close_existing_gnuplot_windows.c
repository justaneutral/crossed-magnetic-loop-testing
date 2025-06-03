#include <stdlib.h>

void close_existing_gnuplot_windows(void) {
    // Kill all previous instances of gnuplot
    system("pkill -f gnuplot");
}

