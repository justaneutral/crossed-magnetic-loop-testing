#include "includes.h"

int main(int argc, char **argv) {
    int rv = 0;
    char *rm = "Success\n";

    DataSample data[MAX_SAMPLES];
    int n_samples = 0;

    FIRFilter filter;
    double fs, accuracy_percent;

    close_existing_gnuplot_windows();
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <data_file.csv>\n", argv[0]);
        rv =  EXIT_FAILURE;
        rm = "Arguments\n"; 
    }

    if (!rv && !read_csv(argv[1], data, &n_samples)) {
        fprintf(stderr, "Error reading CSV file.\n");
        rv = EXIT_FAILURE;
        rm = "Wrong input\n";
    }

    if (!rv && n_samples == 0) {
        fprintf(stderr, "No valid samples found.\n");
        rv = EXIT_FAILURE;
        rm = "No samples\n";
    }

    if(!rv) {
      printf("Read %d samples successfully.\n", n_samples);

      remove_dc(data, n_samples);

      if(!calculate_sampling_rate(data, n_samples, &fs, &accuracy_percent)) {
        fs = 64000.0;
        printf("Calculated Sampling Rate = %lf Hz, Accuracy = ±%lf%%\n", fs, accuracy_percent);
      }
      else
      {
        rv = EXIT_FAILURE;
        rm = "Wrong sampling rate\n";
      }
    }

    if(!rv) { 
      if (generate_fir_bandpass(fs, 25000.0, 25400.0, 1, MAX_FIR_TAPS, &filter) != 0) {
        fprintf(stderr, "Filter creation failed.\n");
        rv = EXIT_FAILURE;
        rm = "Bad IIR coeffs\n";
      }
    }

    if(!rv) {
     //test for filter
     //double scale = 0.1*find_scale(data, n_samples);
     //generate_sinusoid(data,n_samples,scale, scale,-12500.0, fs);
     //generate_filtered_noise(data, n_samples, fs, &filter);
     //generate_sinusoid(data,n_samples,scale, scale,12500.0, fs);
     //generate_sinusoid(data,n_samples,scale, scale,25200.0, fs);

      filter_data(data, n_samples, &filter);
      plot_data(data, n_samples);
      plot_fft_db(data, n_samples);
      plot_xy(data, n_samples);
    }

    printf("return value = %d, reason: %s\n", rv, rm);
    return rv;
}

