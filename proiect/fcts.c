#include <userint.h>
#include <advanlys.h>
#include "fcts.h"
#include "proiect.h"
#include <stdlib.h>
#include <string.h>
#include <toolbox.h>
#include <math.h>

// Global variables — **defined only here**
int waveInfo[2];
double sampleRate = 0.0;
int npoints = 0;
double *waveData = NULL;
double *filteredData = NULL;
double *envelopeData = NULL;

double alpha = 0.1;
int ma_window = 16;
int filter_type = 0;

double minval = 0.0;
double maxval = 0.0;
double medval = 0.0;
double meanval = 0.0;
double dispersie = 0.0;
double skewness = 0.0;
double kurtosis = 0.0;
int maxIndex = 0, minIndex = 0, zeros = 0;

//==============================================================================
// Functii

/*
double* moving_average(const double* src, int n, int window)
{
    if (!src || n <= 0) return NULL;
  
    if (window <= 1) {
        double* out = (double*) calloc(n, sizeof(double));
        if (!out) return NULL;
        memcpy(out, src, n * sizeof(double));
        return out;
    }

    double* out = (double*) calloc(n, sizeof(double));
    if (!out) return NULL;

    int half = window / 2;
    for (int i = 0; i < n; i++) {
        int start = i - half; if (start < 0) start = 0;
        int end = start + window - 1; if (end >= n) end = n - 1;
        double sum = 0.0;
        int cnt = 0;
        for (int j = start; j <= end; j++) { sum += src[j]; cnt++; }
        out[i] = sum / (double)cnt;
    }
    return out;
}
*/

double* moving_average(const double* src, int n, int window)
{
    if (!src || n <= 0) return NULL;

    double* out = (double*) calloc(n, sizeof(double));
    if (!out) return NULL;

    for (int i = 0; i < n; i++) {
        int start = i;
        int end = start + window - 1; if (end >= n) end = n - 1;
        double sum = 0.0;
        int cnt = 0;
        for (int j = start; j <= end; j++) { sum += src[j]; cnt++; }
        out[i] = sum / (double)cnt;
    }
    return out;
}

double* filter_order1(const double* src, int n, double alpha_param)
{
    if (!src || n <= 0) return NULL;

    double* out = (double*) calloc(n, sizeof(double));
    if (!out) return NULL;

    out[0] = src[0];
    for (int i = 1; i < n; i++)
        out[i] = (1.0 - alpha_param) * out[i - 1] + alpha_param * src[i];

    return out;
}

double* compute_envelope(const double* src, int n, int peak_window)
{
    if (!src || n <= 0) return NULL;
    if (peak_window < 1) peak_window = 1;

    ssize_t npeaks = 0;
    double *peakLocs = NULL;
    double *peakAmps = NULL;
    double *peakDerivs = NULL;

    // Threshold
    double threshold = 0.5 * maxval;

    // 
    double *dataCopy = (double*)calloc(n, sizeof(double));
    if (!dataCopy) return NULL;
    for (int i = 0; i < n; ++i) {
        dataCopy[i] = fabs(src[i]);
        if (dataCopy[i] > maxval) maxval = dataCopy[i];
    }

    // 
    AnalysisLibErrType err = PeakDetector(dataCopy, n, threshold, peak_window,0, 1, 1, &npeaks, &peakLocs, &peakAmps, &peakDerivs);

    free(dataCopy);

    if (err != 0 || npeaks <= 0) {
        return NULL;
    }

    // Allocate envelope array
    double* env = (double*)calloc(n, sizeof(double));
    if (!env) return NULL;

    // interpolare intre varfuri
    ssize_t firstPeak = (ssize_t)peakLocs[0];
    for (ssize_t i = 0; i <= firstPeak && i < n; ++i)
        env[i] = peakAmps[0];
    for (ssize_t p = 0; p < npeaks - 1; ++p) {
        ssize_t x0 = (ssize_t)peakLocs[p];
        ssize_t x1 = (ssize_t)peakLocs[p + 1];
        double y0 = peakAmps[p];
        double y1 = peakAmps[p + 1];
        if (x1 == x0) { 
            env[x0] = (env[x0] > y0) ? env[x0] : y0;
            continue;
        }
        for (ssize_t i = x0; i <= x1 && i < n; ++i) {
            double t = (double)(i - x0) / (double)(x1 - x0);
            env[i] = y0 + t * (y1 - y0);
        }
    }

    // final
    ssize_t lastPeak = (ssize_t)peakLocs[npeaks - 1];
    for (ssize_t i = lastPeak; i < n; ++i)
        env[i] = peakAmps[npeaks - 1];
	
    return env;
}

double* filtrare_domeniu_timp(double* pts, int npts)
{
	//aici ar merge un switch
    if (!pts || npts <= 0) return NULL;

    if (filter_type == 1)
        return moving_average(pts, npts, ma_window);
    else if (filter_type == 2)
        return filter_order1(pts, npts, alpha);
    else {
        double* out = (double*) calloc(npts, sizeof(double));
        if (!out) return NULL;
        memcpy(out, pts, npts * sizeof(double));
        return out;
    }
}

void plot_second(int panel, int graphCtrl, const double* data,
                 int n, double sr, double sec, int color)
{
    if (!data || sr <= 0.0) return;

    int samples_per_sec = (int)sr;
    int start = (int)(sec * samples_per_sec);
    if (start >= n) return;

    int count = samples_per_sec;
    if (start + count > n)
        count = n - start;

    double x[count];
    for (int i=0; i<count; i++)
        x[i] = (start + i) / sr;  // timp în secunde

    if(graphCtrl == MAIN_PANEL_FILTERED_DATA)
        DeleteGraphPlot(panel, graphCtrl, -1, VAL_IMMEDIATE_DRAW);

    PlotXY(panel, graphCtrl,
           x, data + start, count,
           VAL_DOUBLE,
		   VAL_DOUBLE,
           VAL_THIN_LINE,
           VAL_EMPTY_SQUARE,
           VAL_SOLID,
           VAL_CONNECTED_POINTS,
           color);
}

int save_graph_to_jpeg(int panel, int graphCtrl, const char* filename)
{
    int bitmapID;
    if (GetCtrlDisplayBitmap(panel, graphCtrl, 1, &bitmapID) < 0) return -1;
    if (SaveBitmapToJPEGFile(bitmapID, filename, JPEG_PROGRESSIVE, 100) < 0) return -1;
    return 0;
}

void compute_statistics()
{
    if (!waveData || npoints <= 0) return;

    MaxMin1D(waveData, npoints, &maxval, &maxIndex, &minval, &minIndex);
    Mean(waveData, npoints, &meanval);
    StdDev(waveData, npoints, &meanval, &dispersie);
    Median(waveData, npoints, &medval);

    zeros = 0;
    for (int i = 0; i < npoints - 1; i++)
        if (waveData[i] * waveData[i + 1] < 0.0 || waveData[i] == 0 ) zeros++;

    // Skewness & kurtosis using central moments
	int i = 0;
	//sint confuz, dar nu are sens altfel???
	double threshold = 0.01*maxval;
    while(fabs(waveData[i])<threshold && i<npoints)
		i++;
	int j = 0;
	double* trunc = (double*)calloc(sampleRate,sizeof(double));
	while(j<sampleRate){
		trunc[j] = waveData[i+j];
		j++;
	}
    Moment(trunc, sampleRate, 3, &skewness);
    Moment(trunc, sampleRate, 4, &kurtosis);
}

void update_statistics_ui(int panel)
{
    SetCtrlVal(panel, MAIN_PANEL_MIN_VAL, minval);
    SetCtrlVal(panel, MAIN_PANEL_MAX_VAL, maxval);
    SetCtrlVal(panel, MAIN_PANEL_MIN_IDX, minIndex);
    SetCtrlVal(panel, MAIN_PANEL_MAX_IDX, maxIndex);
    SetCtrlVal(panel, MAIN_PANEL_DISP_VAL, dispersie);
    SetCtrlVal(panel, MAIN_PANEL_MEAN_VAL, meanval);
    SetCtrlVal(panel, MAIN_PANEL_MED_VAL, medval);
    SetCtrlVal(panel, MAIN_PANEL_ZEROS, zeros);
    SetCtrlVal(panel, MAIN_PANEL_SKEWENESS, skewness);
    SetCtrlVal(panel, MAIN_PANEL_KURTOSIS, kurtosis);
	
	
	FILE *fp = fopen("stats.txt", "w");
    if (!fp) {
        MessagePopup("Error", "Cannot open file for writing!");
        return;
    }

    fprintf(fp, "MIN_VAL %.6lf\n", minval);
    fprintf(fp, "MAX_VAL %.6lf\n", maxval);
    fprintf(fp, "MIN_IDX %d\n", minIndex);
    fprintf(fp, "MAX_IDX %d\n", maxIndex);
    fprintf(fp, "DISPERSION %.6lf\n", dispersie);
    fprintf(fp, "MEAN %.6lf\n", meanval);
    fprintf(fp, "MEDIAN %.6lf\n", medval);
    fprintf(fp, "ZEROS %d\n", zeros);
    fprintf(fp, "SKEWNESS %.6lf\n", skewness);
    fprintf(fp, "KURTOSIS %.6lf\n", kurtosis);

    fclose(fp);
}


// pentru filtrare si de lene sa folosim filtre deja facute de convolutie
int FIRHighpass(double *input, double *output, double* h, int n_coeff, int n_samples)
{

    for (int i = 0; i < n_samples; i++)
    {
        double y = 0.0;
        for (int k = 0; k < n_coeff; k++)
        {
            if ((i - k) >= 0)
                y += h[k] * input[i - k];
        }
        output[i] = y;
    }
    return 0;
}
