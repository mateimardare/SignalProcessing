#ifndef FCTS_H
#define FCTS_H

#include <ansi_c.h>

// Global variables
extern int waveInfo[2];
extern double sampleRate;
extern int npoints;
extern double *waveData;
extern double *filteredData;
extern double *envelopeData;

// Filter parameters
extern double alpha;
extern int ma_window;
extern int filter_type;

// Statistics
extern double minval, maxval, medval;
extern double meanval, dispersie;
extern int maxIndex, minIndex, zeros;
extern double skewness, kurtosis;

// Function declarations
double* moving_average(const double* src, int n, int window);
double* filter_order1(const double* src, int n, double alpha_param);
double* compute_envelope(const double* src, int n, int env_window);
double* filtrare_domeniu_timp(double* pts, int npts);
void plot_second(int panel, int graphCtrl, const double* data, int n, double sr, double sec, int color);
int save_graph_to_jpeg(int panel, int graphCtrl, const char* filename);
void compute_statistics();
void update_statistics_ui(int panel);

//
int FIRHighpass(double *input, double *output, double* h, int n_coeff, int n_samples);

#endif
