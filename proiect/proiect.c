#include <advanlys.h>
#include <utility.h>
#include <formatio.h>
#include <rs232.h>
#include <ansi_c.h>
#include <cvirte.h>
#include <userint.h>
#include "proiect.h"
#include "fcts.h"
#include <math.h>
#include <stdlib.h>

#define SAMPLE_RATE 0
#define NPOINTS     1

// for filtering stuff idk
#define f_pass 1200
#define f_stop 1450
#define n_coeff 55 // this one s for filtering coefficients
#define BETA 7.04 //have no idea how i chose this

static int mainPanel = 0;
static int wavePanel = 0;
static int loading = 0;
static double current_second = 0;
static int total_seconds = 0;

// Var globale pentru al doilea panou
static int fftOffset = 0;
static int fftSamples = 1024;
double fftBufferSize = 0;
static double *fftBuffer = NULL;
static double *freqFilteredData = NULL;
static double *fftSpectrumRaw = NULL;
static double *fftSpectrumRawConv = NULL;
static double *fftSpectrumFilt = NULL;
static double *fftSpectrumFiltConv = NULL;
static char spectrumUnit[80];
	
static double *freqAxis = NULL;
static int freqAxisSize = 0;
static double df = 0.0;


//static double stateinfo[n_coeff] = {0};
// Refresh filtered graph
static void refresh_filtered_graph(int panel) {
    if (!waveData || npoints <= 0) return;

    // Recompute filtered data
    if (filteredData) { free(filteredData); filteredData = NULL; }
    filteredData = filtrare_domeniu_timp(waveData, npoints);

    if ((int)(sampleRate * 10.0) < npoints) {
        plot_second(panel, MAIN_PANEL_FILTERED_DATA, filteredData, npoints, sampleRate, current_second, VAL_RED);
    } else {
        // Case: full signal plotting
        DeleteGraphPlot(panel, MAIN_PANEL_FILTERED_DATA, -1, VAL_IMMEDIATE_DRAW);
        
        // Build time axis for full signal
        double* timeAxis = (double*)calloc(npoints, sizeof(double));
        if (timeAxis == NULL) return;

        for (int i = 0; i < npoints; i++) {
            timeAxis[i] = i / sampleRate;
        }

        // Plot with time on X-axis
		DeleteGraphPlot(panel, MAIN_PANEL_RAW_DATA, -1, VAL_IMMEDIATE_DRAW);
    	PlotXY(panel, MAIN_PANEL_RAW_DATA, timeAxis, waveData, npoints, VAL_DOUBLE, 
			   VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, 
			   VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
        PlotXY(panel, MAIN_PANEL_FILTERED_DATA, timeAxis, filteredData, npoints,
               VAL_DOUBLE, VAL_DOUBLE,
               VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID,
               VAL_CONNECTED_POINTS, VAL_RED);

        free(timeAxis);
    }
	current_second = 0;
	SetCtrlVal(panel, MAIN_PANEL_TIME_TO_PLAY, current_second);
}

//==============================================================================
// Main program
int main(int argc, char* argv[])
{
    InitCVIRTE(0, argv, 0);
    mainPanel = LoadPanel(0, "proiect.uir", MAIN_PANEL);
	wavePanel = LoadPanel (0, "proiect.uir", FREQ_PANEL);
	
	SetCtrlAttribute(mainPanel, MAIN_PANEL_ALPHA_SLIDE, ATTR_DIMMED, 1);
	SetCtrlAttribute(mainPanel, MAIN_PANEL_MED_SLIDE, ATTR_DIMMED, 1);
	
    DisplayPanel(mainPanel);
    RunUserInterface();
	
    if (mainPanel > 0)
        DiscardPanel(mainPanel);

    // Free allocated memory at exit
    if (waveData) free(waveData);
    if (filteredData) free(filteredData);
    if (envelopeData) free(envelopeData);

    return 0;
}

//==============================================================================
// Main panel callback
int CVICALLBACK OnMainPanelCB(int panel, int event, void* callbackData,
                              int eventData1, int eventData2)
{
	
    if (event == EVENT_CLOSE)
        QuitUserInterface(0);
    return 0;
}

//==============================================================================
// Load Button Callback
int CVICALLBACK OnLoadButtonCB(int panel, int control, int event,
                               void* callbackData, int eventData1, int eventData2)
{
    if (event != EVENT_COMMIT || loading) return 0;
    loading = 1;

    LaunchExecutable("D:\\an3\\sem1\\APD\\proect_APD_MateiMardare\\proiect\\venv\\Scripts\\python.exe main.py");

    // Load wave info
    FileToArray("waveInfo.txt", waveInfo, VAL_INTEGER, 2, 1,
                VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);
    sampleRate = waveInfo[SAMPLE_RATE];
	
	npoints = ((double)waveInfo[NPOINTS] / sampleRate > 10.0) ? (int)(sampleRate * 6) : waveInfo[NPOINTS];

    // Free previous allocations
    if (waveData) { free(waveData); waveData = NULL; }
    if (filteredData) { free(filteredData); filteredData = NULL; }
    if (envelopeData) { free(envelopeData); envelopeData = NULL; }

    waveData = (double*)calloc(npoints, sizeof(double));
    filteredData = (double*)calloc(npoints, sizeof(double));

    // 
    FileToArray("waveData.txt", waveData, VAL_DOUBLE, npoints, 1,
                VAL_GROUPS_TOGETHER, VAL_GROUPS_AS_COLUMNS, VAL_ASCII);

    filteredData = filtrare_domeniu_timp(waveData, npoints);

    //
    total_seconds = (int)ceil((double)npoints / sampleRate);
    GetCtrlVal(panel, MAIN_PANEL_TIME_TO_PLAY, &current_second);
	
    // anvelopa semnalului
    int env_win = (int)(0.01 * sampleRate);
    if (env_win < 3) env_win = 3;
    envelopeData = compute_envelope(waveData, npoints, env_win);

	
	// Ne mai invartim putin an jurul cozii
	double* timeAxis = (double*)calloc(npoints, sizeof(double));
	if (timeAxis == NULL) exit(EXIT_FAILURE);  

	for (int i = 0; i < npoints; i++) {
	    timeAxis[i] = i / sampleRate;  
	}

    //Afisare
    DeleteGraphPlot(panel, MAIN_PANEL_RAW_DATA, -1, VAL_IMMEDIATE_DRAW);
    PlotXY(panel, MAIN_PANEL_RAW_DATA, timeAxis, waveData, npoints,
       VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE,
       VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);
	
    DeleteGraphPlot(panel, MAIN_PANEL_FILTERED_DATA, -1, VAL_IMMEDIATE_DRAW);
    PlotXY(panel, MAIN_PANEL_FILTERED_DATA, timeAxis, filteredData, npoints,
       VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE,
       VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);


    // Compute statistics
    compute_statistics();
    update_statistics_ui(panel);

    // Histogram
    double axis[1000];int histo[1000];
    int nBuckets = 40;
    double minv = minval, maxv = maxval;
    if (minv == maxv) maxv = minv + 1.0;
    Histogram(waveData, npoints, minv, maxv, histo, axis, nBuckets);
    PlotXY(panel, MAIN_PANEL_HISTOGRAM, axis, histo, nBuckets,
           VAL_DOUBLE, VAL_INTEGER, VAL_VERTICAL_BAR, VAL_EMPTY_SQUARE, VAL_SOLID, 1, VAL_RED);

    loading = 0;
    return 0;
}

//==============================================================================
// Filter Type Callback
int CVICALLBACK FilterTypeCB(int panel, int control, int event, void* cbdata, int e1, int e2)
{
    if (event != EVENT_COMMIT) return 0;
    int val = 0;
    GetCtrlVal(panel, MAIN_PANEL_FILTER_TYPE, &val);
	
    switch(val){
		case 0:
			SetCtrlAttribute(panel, MAIN_PANEL_ALPHA_SLIDE, ATTR_DIMMED, 1);
			SetCtrlAttribute(panel, MAIN_PANEL_MED_SLIDE, ATTR_DIMMED, 1);
			filter_type = 0;
			break;
		case 1:
			SetCtrlAttribute(panel, MAIN_PANEL_ALPHA_SLIDE, ATTR_DIMMED, 0);
			SetCtrlAttribute(panel, MAIN_PANEL_MED_SLIDE, ATTR_DIMMED, 1);
			filter_type = 1;
			break;
		case 2:
			SetCtrlAttribute(panel, MAIN_PANEL_ALPHA_SLIDE, ATTR_DIMMED, 1);
			SetCtrlAttribute(panel, MAIN_PANEL_MED_SLIDE, ATTR_DIMMED, 0);
			filter_type = 2;
			break;
		default:
			filter_type = 0;
			break;
	}
    refresh_filtered_graph(panel);
    return 0;
}

//==============================================================================
// Navigation & Refresh Callbacks
int CVICALLBACK PrevCB(int panel, int control, int event, void* cbdata, int e1, int e2)
{
    if (event != EVENT_COMMIT || current_second <= 0) return 0;
    current_second-=0.5;
    SetCtrlVal(panel, MAIN_PANEL_TIME_TO_PLAY, current_second);
	plot_second(panel, MAIN_PANEL_FILTERED_DATA, filteredData, npoints, sampleRate, current_second, VAL_RED);
	
	DeleteGraphPlot(panel, MAIN_PANEL_RAW_DATA, -1, VAL_IMMEDIATE_DRAW);
    plot_second(panel, MAIN_PANEL_RAW_DATA, waveData, npoints, sampleRate, current_second, VAL_RED);
	plot_second(panel, MAIN_PANEL_RAW_DATA, envelopeData, npoints, sampleRate, current_second, VAL_GREEN);
    return 0;
}

int CVICALLBACK NextCB(int panel, int control, int event, void* cbdata, int e1, int e2)
{
    if (event != EVENT_COMMIT || current_second >= total_seconds-1) return 0;
    current_second+=0.5;
    SetCtrlVal(panel, MAIN_PANEL_TIME_TO_PLAY, current_second);
	plot_second(panel, MAIN_PANEL_FILTERED_DATA, filteredData, npoints, sampleRate, current_second, VAL_RED);
	
	DeleteGraphPlot(panel, MAIN_PANEL_RAW_DATA, -1, VAL_IMMEDIATE_DRAW);
    plot_second(panel, MAIN_PANEL_RAW_DATA, waveData, npoints, sampleRate, current_second, VAL_RED);
	plot_second(panel, MAIN_PANEL_RAW_DATA, envelopeData, npoints, sampleRate, current_second, VAL_GREEN);
    return 0;
}

int CVICALLBACK RefreshCB(int panel, int control, int event, void* cbdata, int e1, int e2)
{
    if (event != EVENT_COMMIT) return 0;
    refresh_filtered_graph(panel);
    return 0;
}

//==============================================================================
// Slider & Envelope Callbacks
int CVICALLBACK AlphaCB(int panel, int control, int event, void* cbdata, int e1, int e2)
{
    if (event != EVENT_COMMIT) return 0;
    GetCtrlVal(panel, MAIN_PANEL_ALPHA_SLIDE, &alpha);
    if (alpha < 0.0) alpha = 0.0;
    if (alpha > 1.0) alpha = 1.0;
    return 0;
}

int CVICALLBACK filtMedCB (int panel, int control, int event, void *callbackData, int e1, int e2)
{
	int temp = ma_window;
	switch (event)
	{
		case EVENT_COMMIT:
			break;
	}
	GetCtrlVal(panel, MAIN_PANEL_MED_SLIDE, &ma_window); 
	if(temp!=ma_window)	refresh_filtered_graph(panel);
	return 0;
}

int CVICALLBACK TimeToPlayCB(int panel, int control, int event, void* cbdata, int e1, int e2)
{
    if (event != EVENT_COMMIT) return 0;
    int val = 0;
    
    // Get the desired second to display
    GetCtrlVal(panel, MAIN_PANEL_TIME_TO_PLAY, &val);
    if (val < 0) val = 0;
    if (val >= total_seconds) val = total_seconds - 1;

    current_second = val;  // Update the paging index

    // Recompute filters if needed
    if (filter_type == 1) {
        if (filteredData) { free(filteredData); filteredData = NULL; }
        filteredData = filtrare_domeniu_timp(waveData, npoints);
    }

    // Replot current window
	plot_second(panel, MAIN_PANEL_FILTERED_DATA, filteredData, npoints, sampleRate, current_second, VAL_RED);
	
	DeleteGraphPlot(panel, MAIN_PANEL_RAW_DATA, -1, VAL_IMMEDIATE_DRAW);
    plot_second(panel, MAIN_PANEL_RAW_DATA, waveData, npoints, sampleRate, current_second, VAL_RED);
	plot_second(panel, MAIN_PANEL_RAW_DATA, envelopeData, npoints, sampleRate, current_second, VAL_GREEN);
    return 0;
}
int CVICALLBACK EnvelopeCB(int panel, int control, int event,
                            void* cbdata, int e1, int e2)
{
    if (event != EVENT_COMMIT)
        return 0;

    int showEnvelope = 0;
    GetCtrlVal(panel, MAIN_PANEL_ENVELOPE_BTN, &showEnvelope);

    // 
	int start = 0, count = npoints; 
	if (sampleRate * 10.0 < npoints) {
		int samples_per_sec = (int) sampleRate; 
		start = current_second * samples_per_sec; 
		if (start >= npoints) start = npoints - samples_per_sec; 
		if (start < 0) start = 0; 
		count = (start + samples_per_sec > npoints) ? (npoints - start) : samples_per_sec;
 	}
	
    DeleteGraphPlot(panel, MAIN_PANEL_RAW_DATA, -1, VAL_IMMEDIATE_DRAW); 
	
	double* timeAxis = (double*)calloc(npoints, sizeof(double)); 
	if (timeAxis == NULL) exit(EXIT_FAILURE); 
	
	for (int i = 0; i < npoints; i++) { timeAxis[i] = i / sampleRate; } 
	
	PlotXY(panel, MAIN_PANEL_RAW_DATA, timeAxis, waveData, npoints, VAL_DOUBLE, VAL_DOUBLE, VAL_THIN_LINE, VAL_EMPTY_SQUARE, VAL_SOLID, VAL_CONNECTED_POINTS, VAL_RED);

    // plot anvelopă dacă e activată
    if (showEnvelope) {
        if (!envelopeData) {
            envelopeData = (double*)calloc(npoints, sizeof(double));
            if (!envelopeData) {
                free(timeAxis);
                return -1;
            }
            FileToArray("waveEnvelope.txt",
                        envelopeData,
                        VAL_DOUBLE,
                        npoints, 1,
                        VAL_GROUPS_TOGETHER,
                        VAL_GROUPS_AS_COLUMNS,
                        VAL_ASCII);
        }

        PlotXY(panel, MAIN_PANEL_RAW_DATA,
               timeAxis, envelopeData + start, count,
               VAL_DOUBLE, VAL_DOUBLE,
               VAL_THIN_LINE, VAL_EMPTY_SQUARE,
               VAL_SOLID, VAL_CONNECTED_POINTS, VAL_GREEN);
    }

    free(timeAxis);
    return 0;
}

//==============================================================================
// Save Graph Callback
int CVICALLBACK SaveCB(int panel, int control, int event, void* cbdata, int e1, int e2)
{
    if (event != EVENT_COMMIT) return 0;
    char file1[1024], file2[1024];
	int ceva = (int) (current_second*10);
    sprintf(file1, "D:\\an3\\sem1\\APD\\proiect\\grafice\\timp\\grafic_raw_sec_%03d.jpg", ceva);
    sprintf(file2, "D:\\an3\\sem1\\APD\\proiect\\grafice\\timp\\grafic_filtered_sec_%03d.jpg", ceva);
    save_graph_to_jpeg(panel, MAIN_PANEL_RAW_DATA, file1);
    save_graph_to_jpeg(panel, MAIN_PANEL_FILTERED_DATA, file2);
    MessagePopup("Saved", "Graficele au fost salvate.");
    return 0;
}

int CVICALLBACK panelSwitchCB (int panel, int control, int event,
							   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			if(panel == mainPanel)
			{
				SetCtrlVal(wavePanel, FREQ_PANEL_PANEL_SWITCH, 1);
				DisplayPanel(wavePanel);
				HidePanel(panel);
				SetCtrlAttribute(wavePanel, FREQ_PANEL_TIMER, ATTR_ENABLED, 0);
				SetCtrlVal(wavePanel, FREQ_PANEL_START_BTN, 0);
				fftOffset = 0;
				DeleteGraphPlot(wavePanel, FREQ_PANEL_AUDIO, -1, VAL_IMMEDIATE_DRAW);
			    DeleteGraphPlot(wavePanel, FREQ_PANEL_FILTERED_AUDIO, -1, VAL_IMMEDIATE_DRAW);
			    DeleteGraphPlot(wavePanel, FREQ_PANEL_AUDIO_SPECTRUM, -1, VAL_IMMEDIATE_DRAW);
			    DeleteGraphPlot(wavePanel, FREQ_PANEL_FILT_AUDIO_SPECTRUM, -1, VAL_IMMEDIATE_DRAW);
			}
			else
			{
				SetCtrlVal(mainPanel, MAIN_PANEL_PANEL_SWITCH, 0);
				DisplayPanel(mainPanel);
				HidePanel(panel);
				SetCtrlAttribute(wavePanel, FREQ_PANEL_TIMER, ATTR_ENABLED, 1);

			}
			break;
	}
	return 0;
}


// =======================================================================================
// functii pentru al doilea pano
// =======================================================================================

int CVICALLBACK onFreqPanelCB (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			QuitUserInterface(0);
			break;
	}
	return 0;
}

int CVICALLBACK OnStartTimer_Freq (int panel, int control, int event,
								   void *callbackData, int eventData1, int eventData2)
{
	int state;
	fftOffset = 0;
	switch (event)
	{
		case EVENT_COMMIT:
			GetCtrlVal(panel,FREQ_PANEL_START_BTN,&state);
			SetCtrlAttribute(panel,FREQ_PANEL_TIMER,ATTR_ENABLED,state);
			SetCtrlAttribute(panel,FREQ_PANEL_SAMPLES_FFT,ATTR_DIMMED,state);
			//Delete Graph?

			break;
	}
	return 0;
}
int CVICALLBACK OnTimer_FreqCB(int panel, int control, int event,
                               void *callbackData, int eventData1, int eventData2)
{
    if (event != EVENT_TIMER_TICK)
        return 0;
    if (!waveData || npoints < 256)
        return 0;

    /* ================= FFT SIZE ================= */
    int idx;
    GetCtrlVal(panel, FREQ_PANEL_SAMPLES_FFT, &idx);
    int fftSamples = (idx == 0) ? 1024 : (idx == 1) ? 2048 : 4096;
    if (fftOffset + fftSamples > npoints)
        fftOffset = 0;

    int downFactor = ((sampleRate == 44100 || sampleRate == 22050) && fftSamples > 1024) ? 4 : 1;
    int nFFT = fftSamples / downFactor;

    /* ================= ALLOC BUFFERS ================= */
    if (!fftBuffer || fftBufferSize != nFFT) {
        free(fftBuffer); free(freqFilteredData);
        free(fftSpectrumRaw); free(fftSpectrumRawConv);
        free(fftSpectrumFilt); free(fftSpectrumFiltConv);

        fftBuffer           = malloc(nFFT * sizeof(double));
        freqFilteredData    = malloc(nFFT * sizeof(double));
        fftSpectrumRaw      = malloc(nFFT / 2 * sizeof(double));
        fftSpectrumRawConv  = malloc(nFFT / 2 * sizeof(double));
        fftSpectrumFilt     = malloc(nFFT / 2 * sizeof(double));
        fftSpectrumFiltConv = malloc(nFFT / 2 * sizeof(double));

        fftBufferSize = nFFT;
    }

    /* ================= COPY DATA ================= */
    for (int i = 0; i < nFFT; i++) {
        fftBuffer[i] = waveData[fftOffset + i * downFactor];           
        freqFilteredData[i] = fftBuffer[i]; 
    }

    /* ================= WINDOW ================= */
    int winType;
    GetCtrlVal(panel, FREQ_PANEL_RING, &winType);
    WindowConst winConst;
    ScaledWindowEx(fftBuffer, nFFT,
                   winType == 1 ? HAMMING : BLKMAN,
                   0, &winConst);

    double dt = 1.0 / (sampleRate / downFactor);

    /* ================= FFT RAW ================= */
    AutoPowerSpectrum(fftBuffer, nFFT, dt, fftSpectrumRaw, &df);
    memset(spectrumUnit, 0, sizeof(spectrumUnit));
    SpectrumUnitConversion(
        fftSpectrumRaw,
        nFFT / 2,
        SPECTRUM_POWER,
        1,
        0,
        df,
        winConst,
        fftSpectrumRawConv,
        spectrumUnit
    );

    /* ================= FREQ AXIS ================= */
    if (freqAxisSize != nFFT / 2) {
        free(freqAxis);
        freqAxis = malloc((nFFT / 2) * sizeof(double));
        freqAxisSize = nFFT / 2;
    }
    for (int i = 0; i < freqAxisSize; i++)
        freqAxis[i] = i * df;

    /* ================= POWER & PEAK ================= */
    double freqPeak = 0.0, powerPeak = 0.0;
    PowerFrequencyEstimate(fftSpectrumRaw, nFFT / 2, -1, winConst, df, 7, &freqPeak, &powerPeak);
    SetCtrlVal(panel, FREQ_PANEL_POWER_PEAK, powerPeak);
    SetCtrlVal(panel, FREQ_PANEL_FREQ_PEAK, freqPeak);

    /* ================= FILTER ================= */
    int filterType;
    GetCtrlVal(panel, FREQ_PANEL_FREQ_FILTER, &filterType);

    if (filterType == 0) { // FIR Kaiser HPF
        double *coeff = calloc(n_coeff, sizeof(double));
        Ksr_HPF(sampleRate, f_stop, n_coeff, coeff, BETA);
        FIRHighpass(freqFilteredData, freqFilteredData, coeff, n_coeff, nFFT);
		//FIRFiltering(freqFilteredData, nFFT, coeff, stateinfo, n_coeff, freqFilteredData);
        free(coeff);
    } else if (filterType == 1) { // Chebyshev LPF
        Ch_LPF(freqFilteredData, nFFT, sampleRate, f_pass, 0.5, 5, freqFilteredData);
    }

    /* ================= FFT SPECTRUM FILTERED ================= */
    memcpy(fftBuffer, freqFilteredData, nFFT * sizeof(double));
    ScaledWindowEx(fftBuffer, nFFT,
                   winType == 1 ? HAMMING : BLKMAN,
                   0, &winConst);

    double dfFilt;
    AutoPowerSpectrum(fftBuffer, nFFT, dt, fftSpectrumFilt, &dfFilt);
    memset(spectrumUnit, 0, sizeof(spectrumUnit));
    SpectrumUnitConversion(
        fftSpectrumFilt,
        nFFT / 2,
        SPECTRUM_POWER,
        1,
        0,
        dfFilt,
        winConst,
        fftSpectrumFiltConv,
        spectrumUnit
    );

    /* ================= PLOT ================= */
    DeleteGraphPlot(panel, FREQ_PANEL_AUDIO, -1, VAL_IMMEDIATE_DRAW);
    DeleteGraphPlot(panel, FREQ_PANEL_FILTERED_AUDIO, -1, VAL_IMMEDIATE_DRAW);
    DeleteGraphPlot(panel, FREQ_PANEL_AUDIO_SPECTRUM, -1, VAL_IMMEDIATE_DRAW);
    DeleteGraphPlot(panel, FREQ_PANEL_FILT_AUDIO_SPECTRUM, -1, VAL_IMMEDIATE_DRAW);

	/*SetAxisScalingMode(panel, FREQ_PANEL_AUDIO, VAL_LEFT_YAXIS, VAL_MANUAL, minval, maxval);
	SetAxisScalingMode(panel, FREQ_PANEL_FILTERED_AUDIO, VAL_LEFT_YAXIS, VAL_MANUAL, minval/100, maxval/100);*/

    PlotWaveform(panel, FREQ_PANEL_AUDIO,
                 waveData + fftOffset, nFFT,
                 VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0 / sampleRate,
                 VAL_THIN_LINE, VAL_EMPTY_SQUARE,
                 VAL_SOLID, 1, VAL_RED);

    PlotWaveform(panel, FREQ_PANEL_FILTERED_AUDIO,
                 freqFilteredData, nFFT,
                 VAL_DOUBLE, 1.0, 0.0, 0.0, 1.0 / sampleRate,
                 VAL_THIN_LINE, VAL_EMPTY_SQUARE,
                 VAL_SOLID, 1, VAL_BLUE);

    PlotWaveform(panel, FREQ_PANEL_AUDIO_SPECTRUM,
                 fftSpectrumRawConv, nFFT / 2,
                 VAL_DOUBLE, 1.0, 0.0, 0.0, df,
                 VAL_THIN_LINE, VAL_EMPTY_SQUARE,
                 VAL_SOLID, 1, VAL_GREEN);

    PlotWaveform(panel, FREQ_PANEL_FILT_AUDIO_SPECTRUM,
                 fftSpectrumFiltConv, nFFT / 2,
                 VAL_DOUBLE, 1.0, 0.0, 0.0, dfFilt,
                 VAL_THIN_LINE, VAL_EMPTY_SQUARE,
                 VAL_SOLID, 1, VAL_BLUE);

    SetCtrlAttribute(panel, FREQ_PANEL_AUDIO_SPECTRUM, ATTR_XNAME, "Frequency [Hz]");
    SetCtrlAttribute(panel, FREQ_PANEL_AUDIO_SPECTRUM, ATTR_YNAME, spectrumUnit);

    SetCtrlAttribute(panel, FREQ_PANEL_FILT_AUDIO_SPECTRUM, ATTR_XNAME, "Frequency [Hz]");
    SetCtrlAttribute(panel, FREQ_PANEL_FILT_AUDIO_SPECTRUM, ATTR_YNAME, spectrumUnit);

    /* ================= UPDATE OFFSET ================= */
    fftOffset += fftSamples;
    if (fftOffset + fftSamples > npoints)
        fftOffset = 0;

    return 0;
}



// DE RECTIFICAT 
int CVICALLBACK SamplesNumCB(int panel, int control, int event,
                             void *callbackData, int e1, int e2)
{
    if (event != EVENT_COMMIT)
        return 0;

	DeleteGraphPlot(panel, FREQ_PANEL_AUDIO, -1, VAL_IMMEDIATE_DRAW);
	DeleteGraphPlot(panel, FREQ_PANEL_FILTERED_AUDIO, -1, VAL_IMMEDIATE_DRAW);
	DeleteGraphPlot(panel, FREQ_PANEL_AUDIO_SPECTRUM, -1, VAL_IMMEDIATE_DRAW);
	DeleteGraphPlot(panel, FREQ_PANEL_FILT_AUDIO_SPECTRUM, -1, VAL_IMMEDIATE_DRAW);
	
	fftOffset = 0;
	
    GetCtrlVal(panel, FREQ_PANEL_SAMPLES_FFT, &fftSamples);
	
	free(fftBuffer);      fftBuffer = NULL;
	free(fftSpectrumFilt);   fftSpectrumFilt = NULL;
	free(freqFilteredData); freqFilteredData = NULL;

    return 0;
}

int CVICALLBACK onFilterRing (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
				fftOffset = 0;
				DeleteGraphPlot(panel,FREQ_PANEL_AUDIO_SPECTRUM,-1,VAL_DELAYED_DRAW);
				DeleteGraphPlot(panel,FREQ_PANEL_FILT_AUDIO_SPECTRUM,-1,VAL_DELAYED_DRAW);
			break;
	}
	return 0;
}

int CVICALLBACK snap1CB (int panel, int control, int event,
                         void *callbackData, int eventData1, int eventData2)
{
    switch (event)
    {
        case EVENT_COMMIT:
        {
            char fileAudio[1024], fileAudioFilt[1024];
            char fileSpecRaw[1024], fileSpecFilt[1024];

            // grafice domeniu timp
            sprintf(fileAudio, "D:\\an3\\sem1\\APD\\proiect\\grafice\\fourrier\\grafic_audio_%03d.jpg", fftOffset/fftSamples);
            sprintf(fileAudioFilt, "D:\\an3\\sem1\\APD\\proiect\\grafice\\fourrier\\grafic_audio_filt_%03d.jpg", fftOffset/fftSamples);

            // grafice domeniu frecventa
            sprintf(fileSpecRaw, "D:\\an3\\sem1\\APD\\proiect\\grafice\\fourrier\\grafic_spectru_raw_%03d.jpg", fftOffset/fftSamples);
            sprintf(fileSpecFilt, "D:\\an3\\sem1\\APD\\proiect\\grafice\\fourrier\\grafic_spectru_filt_%03d.jpg", fftOffset/fftSamples);

            // salvare grafice
            save_graph_to_jpeg(panel, FREQ_PANEL_AUDIO, fileAudio);
            save_graph_to_jpeg(panel, FREQ_PANEL_FILTERED_AUDIO, fileAudioFilt);
            save_graph_to_jpeg(panel, FREQ_PANEL_AUDIO_SPECTRUM, fileSpecRaw);
            save_graph_to_jpeg(panel, FREQ_PANEL_FILT_AUDIO_SPECTRUM, fileSpecFilt);

            MessagePopup("Saved", "Toate cele 4 grafice au fost salvate.");
        }
        break;
    }
    return 0;
}

