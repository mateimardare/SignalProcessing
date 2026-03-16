/**************************************************************************/
/* LabWindows/CVI User Interface Resource (UIR) Include File              */
/*                                                                        */
/* WARNING: Do not add to, delete from, or otherwise modify the contents  */
/*          of this include file.                                         */
/**************************************************************************/

#include <userint.h>

#ifdef __cplusplus
    extern "C" {
#endif

     /* Panels and Controls: */

#define  FREQ_PANEL                       1       /* callback function: onFreqPanelCB */
#define  FREQ_PANEL_PANEL_SWITCH          2       /* control type: binary, callback function: panelSwitchCB */
#define  FREQ_PANEL_AUDIO                 3       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_FILTERED_AUDIO        4       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_FILT_AUDIO_SPECTRUM   5       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_AUDIO_SPECTRUM        6       /* control type: graph, callback function: (none) */
#define  FREQ_PANEL_SAMPLES_FFT           7       /* control type: slide, callback function: SamplesNumCB */
#define  FREQ_PANEL_RING                  8       /* control type: ring, callback function: (none) */
#define  FREQ_PANEL_FREQ_FILTER           9       /* control type: ring, callback function: (none) */
#define  FREQ_PANEL_START_BTN             10      /* control type: toggle, callback function: OnStartTimer_Freq */
#define  FREQ_PANEL_FREQ_PEAK             11      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_POWER_PEAK            12      /* control type: numeric, callback function: (none) */
#define  FREQ_PANEL_POZA_SPECTRU          13      /* control type: command, callback function: snap1CB */
#define  FREQ_PANEL_TIMER                 14      /* control type: timer, callback function: OnTimer_FreqCB */
#define  FREQ_PANEL_DECORATION            15      /* control type: deco, callback function: (none) */

#define  MAIN_PANEL                       2       /* callback function: OnMainPanelCB */
#define  MAIN_PANEL_LOAD_BUTTON           2       /* control type: command, callback function: OnLoadButtonCB */
#define  MAIN_PANEL_ALPHA_SLIDE           3       /* control type: scale, callback function: AlphaCB */
#define  MAIN_PANEL_RAW_DATA              4       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_FILTERED_DATA         5       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_HISTOGRAM             6       /* control type: graph, callback function: (none) */
#define  MAIN_PANEL_SAVE_BTN              7       /* control type: command, callback function: SaveCB */
#define  MAIN_PANEL_FILTER_TYPE           8       /* control type: ring, callback function: FilterTypeCB */
#define  MAIN_PANEL_REFRESH_BTN           9       /* control type: command, callback function: RefreshCB */
#define  MAIN_PANEL_ENVELOPE_BTN          10      /* control type: radioButton, callback function: EnvelopeCB */
#define  MAIN_PANEL_TIME_TO_PLAY          11      /* control type: numeric, callback function: TimeToPlayCB */
#define  MAIN_PANEL_PREV_BTN              12      /* control type: command, callback function: PrevCB */
#define  MAIN_PANEL_NEXT_BTN              13      /* control type: command, callback function: NextCB */
#define  MAIN_PANEL_MEAN_VAL              14      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MIN_IDX               15      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_SKEWENESS             16      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MED_VAL               17      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_DISP_VAL              18      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MAX_VAL               19      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MIN_VAL               20      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_ZEROS                 21      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_MAX_IDX               22      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_KURTOSIS              23      /* control type: numeric, callback function: (none) */
#define  MAIN_PANEL_DECORATION            24      /* control type: deco, callback function: (none) */
#define  MAIN_PANEL_DECORATION_2          25      /* control type: deco, callback function: (none) */
#define  MAIN_PANEL_PANEL_SWITCH          26      /* control type: binary, callback function: panelSwitchCB */
#define  MAIN_PANEL_MED_SLIDE             27      /* control type: slide, callback function: filtMedCB */


     /* Control Arrays: */

          /* (no control arrays in the resource file) */


     /* Menu Bars, Menus, and Menu Items: */

          /* (no menu bars in the resource file) */


     /* Callback Prototypes: */

int  CVICALLBACK AlphaCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK EnvelopeCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK FilterTypeCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK filtMedCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK NextCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK onFreqPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnLoadButtonCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnMainPanelCB(int panel, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnStartTimer_Freq(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK OnTimer_FreqCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK panelSwitchCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK PrevCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK RefreshCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SamplesNumCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK SaveCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK snap1CB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);
int  CVICALLBACK TimeToPlayCB(int panel, int control, int event, void *callbackData, int eventData1, int eventData2);


#ifdef __cplusplus
    }
#endif