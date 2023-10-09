/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Main wxWidgets Program
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.         *
 **************************************************************************/
#include "config.h"

#ifdef __MINGW32__
#undef IPV6STRICT  // mingw FTBS fix:  missing struct ip_mreq
#include <windows.h>
#endif

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif  // precompiled headers

#ifdef __WXMSW__
//#include "c:\\Program Files\\visual leak detector\\include\\vld.h"
#endif


#ifdef __WXMSW__
#include <math.h>
#include <psapi.h>
#include <stdlib.h>
#include <time.h>
#endif

#ifdef OCPN_HAVE_X11
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif

#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/dynlib.h>
#include <wx/jsonval.h>
#include <wx/jsonreader.h>
#include <wx/timer.h>

#include "config_vars.h"
#include "CanvasConfig.h"
#include "chartbase.h"
#include "chartdb.h"
#include "chart_ctx_factory.h"
#include "chcanv.h"
#include "color_handler.h"
#include "cutil.h"
#include "dychart.h"
#include "FontMgr.h"
#include "georef.h"
#include "glChartCanvas.h"
#include "gui_lib.h"
#include "idents.h"
#include "Layer.h"
#include "load_errors_dlg.h"
#include "MarkInfo.h"
#include "multiplexer.h"
#include "N2KParser.h"
#include "navutil_base.h"
#include "navutil.h"
#include "NMEALogWindow.h"
#include "OCPN_AUIManager.h"
#include "ocpn_frame.h"
#include "OCPNPlatform.h"
#include "OCPN_Sound.h"
#include "options.h"
#include "styles.h"
#include "s52plib.h"
#include "s57chart.h"

#include "SystemCmdSound.h"
#include "tcmgr.h"
#include "timers.h"

static void UpdatePositionCalculatedSogCog();

//------------------------------------------------------------------------------
//      Fwd Declarations
//------------------------------------------------------------------------------
 WX_DEFINE_ARRAY_PTR(ChartCanvas *, arrayofCanvasPtr);

//------------------------------------------------------------------------------
//      Static variable definition
//------------------------------------------------------------------------------

extern OCPN_AUIManager *g_pauimgr;
extern MyConfig *pConfig;
extern arrayofCanvasPtr g_canvasArray;
extern MyFrame *gFrame;

extern double vLat, vLon;
extern double initial_scale_ppm, initial_rotation;
extern wxString g_locale;
extern ColorScheme global_color_scheme;
extern options *g_pOptions;
extern options *g_options;

#ifdef ocpnUSE_GL
GLenum g_texture_rectangle_format;
#endif

#if wxUSE_XLOCALE || !wxCHECK_VERSION(3, 0, 0)
extern wxLocale *plocale_def_lang;
#endif

extern OCPNPlatform *g_Platform;
extern BasePlatform
    *g_BasePlatform;  // points to g_platform, handles brain-dead MS linker.

extern s52plib *ps52plib;

extern bool g_bTrackActive;
extern ocpnStyle::StyleManager *g_StyleManager;
extern bool g_bmasterToolbarFull;
extern bool g_bInlandEcdis;
extern int g_nAutoHideToolbar;
extern bool g_bAutoHideToolbar;
extern bool g_bshowToolbar;
extern wxString g_toolbarConfig;
extern wxString g_toolbarConfigSecondary;
extern float g_toolbar_scalefactor;
extern float g_compass_scalefactor;
extern bool g_bShowMenuBar;
extern bool g_bShowCompassWin;

extern bool g_benable_rotate;
extern int g_GUIScaleFactor;
extern int g_ChartScaleFactor;
extern float g_ChartScaleFactorExp;
extern int g_last_ChartScaleFactor;
extern int g_ShipScaleFactor;
extern float g_ShipScaleFactorExp;
extern int g_ENCSoundingScaleFactor;
extern int g_ENCTextScaleFactor;

extern bool g_bShowTide;
extern bool g_bShowCurrent;
extern bool g_bUIexpert;
extern wxString g_default_wp_icon;
extern std::vector<std::string> TideCurrentDataSet;
extern wxString g_TCData_Dir;
extern TCMgr *ptcmgr;
extern bool g_bShowTrue;
extern bool g_bShowMag;
extern char nmea_tick_chars[];
extern double AnchorPointMinDist;
extern bool AnchorAlertOn1, AnchorAlertOn2;
extern wxString g_AW1GUID;
extern wxString g_AW2GUID;
extern bool g_bCruising;
extern double g_COGAvg;
extern int g_COGAvgSec;
extern double gQueryVar;
extern wxPrintData *g_printData;
extern wxPageSetupData *g_pageSetupData;
extern int g_ChartUpdatePeriod;
extern int g_SkewCompUpdatePeriod;
extern double g_VPRotate;
extern bool g_bCourseUp;
extern bool g_bLookAhead;
extern bool g_bskew_comp;
extern bool g_bopengl;
extern int g_unit_test_1;
extern int g_unit_test_2;
extern bool g_bPauseTest;
extern wxRect g_blink_rect;
extern bool g_bSleep;
extern bool g_bPlayShipsBells;
extern wxDateTime g_loglast_time;
extern int g_nAWDefault;
extern int g_nAWMax;
extern bool g_bDeferredStartTrack;
extern bool bDBUpdateInProgress;
extern int quitflag;
extern int g_tick;
extern ChartDB *ChartData;
extern bool g_boptionsactive;
extern bool g_bDeferredInitDone;
extern int options_lastPage;
extern int options_subpage;
extern bool b_reloadForPlugins;
extern ChartCanvas *g_focusCanvas;
extern wxVector<wxString> g_params;
extern bool g_bNeedDBUpdate;
extern bool g_bFullscreen;
extern wxString gWorldMapLocation, gDefaultWorldMapLocation;
extern ChartGroupArray *g_pGroupArray;
extern bool g_bEnableZoomToCursor;
extern double g_display_size_mm;
extern double g_config_display_size_mm;
extern int g_nTrackPrecision;
extern wxString ChartListFileName;
extern bool g_bFullscreenToolbar;
extern arrayofCanvasPtr g_canvasArray;
extern arrayofCanvasConfigPtr g_canvasConfigArray;
extern wxString g_lastAppliedTemplateGUID;
extern wxPoint options_lastWindowPos;
extern wxSize options_lastWindowSize;
extern unsigned int g_canvasConfig;
extern bool g_bFullScreenQuilt;
extern bool g_bQuiltEnable;
extern wxString *pInit_Chart_Dir;
extern bool g_bAIS_CPA_Alert;
extern bool g_bAIS_CPA_Alert_Audio;
extern bool g_bAISShowTracks;
extern bool g_bAllowShowScaled;
extern bool g_bHideMoored;
extern bool g_bShowScaled;
extern bool g_bShowAIS;
extern bool g_bShowOutlines;
extern bool g_bTempShowMenuBar;
extern bool g_bShowStatusBar;
extern int g_track_rotate_time;
extern int g_track_rotate_time_type;
extern bool g_bTrackCarryOver;
extern bool g_bTrackDaily;
extern bool g_FlushNavobjChanges;
extern int g_FlushNavobjChangesTimeout;
extern bool g_bShowChartBar;
extern double g_plus_minus_zoom_factor;
extern int g_nframewin_x;
extern int g_nframewin_y;
extern int g_nframewin_posx;
extern int g_nframewin_posy;
extern bool g_bframemax;
extern LayerList *pLayerList;
extern bool g_bAutoAnchorMark;
extern wxDateTime g_start_time;
extern bool g_bcompression_wait;
extern bool g_bquiting;
extern wxString g_AisTargetList_perspective;
extern bool b_inCloseWindow;
extern bool b_inCompressAllCharts;
extern int g_ais_query_dialog_x, g_ais_query_dialog_y;
extern wxAuiDefaultDockArt *g_pauidockart;
extern int g_click_stop;
extern wxString g_CmdSoundString;

extern int g_sticky_chart;
extern int g_sticky_projection;
extern bool g_bdisable_opengl;
extern int Usercolortable_index;
extern wxArrayPtrVoid *UserColorTableArray;
extern wxArrayPtrVoid *UserColourHashTableArray;
extern wxColorHashMap *pcurrent_user_color_hash;

// probable move to ocpn_app
extern bool g_bfilter_cogsog;
extern int g_COGFilterSec;
extern int g_SOGFilterSec;
extern bool g_own_ship_sog_cog_calc;
extern int g_own_ship_sog_cog_calc_damp_sec;
extern wxDateTime last_own_ship_sog_cog_calc_ts;
extern double last_own_ship_sog_cog_calc_lat, last_own_ship_sog_cog_calc_lon;
extern bool g_bHasHwClock;
extern bool s_bSetSystemTime;
extern bool bGPSValid;
extern bool bVelocityValid;
extern int g_total_NMEAerror_messages;
extern int gGPS_Watchdog;
extern int gHDx_Watchdog;
extern int gHDT_Watchdog;
extern int gVAR_Watchdog;
extern int gSAT_Watchdog;
extern bool g_bCPAWarn;

extern bool g_bUseGLL;
extern int g_MemFootSec;
extern int g_MemFootMB;
//extern Multiplexer *g_pMUX;
extern int g_memUsed;
extern int g_chart_zoom_modifier_vector;
extern bool g_config_display_size_manual;


#ifdef __WXMSW__
// System color control support

typedef DWORD(WINAPI *SetSysColors_t)(DWORD, DWORD *, DWORD *);
typedef DWORD(WINAPI *GetSysColor_t)(DWORD);

SetSysColors_t pSetSysColors;
GetSysColor_t pGetSysColor;

void SaveSystemColors(void);
void RestoreSystemColors(void);

DWORD color_3dface;
DWORD color_3dhilite;
DWORD color_3dshadow;
DWORD color_3ddkshadow;
DWORD color_3dlight;
DWORD color_activecaption;
DWORD color_gradientactivecaption;
DWORD color_captiontext;
DWORD color_windowframe;
DWORD color_inactiveborder;

#endif

#ifdef __MSVC__
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#if !defined(NAN)
static const long long lNaN = 0xfff8000000000000;
#define NAN (*(double *)&lNaN)
#endif

//    Some static helpers
void appendOSDirSlash(wxString *pString);

void InitializeUserColors(void);
void DeInitializeUserColors(void);
void SetSystemColors(ColorScheme cs);
//------------------------------------------------------------------------------
//    PNG Icon resources
//------------------------------------------------------------------------------

#if defined(__WXGTK__) || defined(__WXQT__)
#include "bitmaps/opencpn.xpm"
#endif

//------------------------------------------------------------------------------
//              Local constants
//------------------------------------------------------------------------------
// enum {
//     ID_PIANO_DISABLE_QUILT_CHART = 32000, ID_PIANO_ENABLE_QUILT_CHART
// };

// begin duplicated code
static double chart_dist(int index) {
	double d;
	//float clon;
	//float clat;
	//const ChartTableEntry &cte = ChartData->GetChartTableEntry(index);
	//// if the chart contains ownship position set the distance to 0
	//if (cte.GetBBox().Contains(gLat, gLon))
	d = 0.;
	//else {
	//  // find the nearest edge
	//  double t;
	//  clon = (cte.GetLonMax() + cte.GetLonMin()) / 2;
	//  d = DistGreatCircle(cte.GetLatMax(), clon, gLat, gLon);
	//  t = DistGreatCircle(cte.GetLatMin(), clon, gLat, gLon);
	//  if (t < d) d = t;

	//  clat = (cte.GetLatMax() + cte.GetLatMin()) / 2;
	//  t = DistGreatCircle(clat, cte.GetLonMin(), gLat, gLon);
	//  if (t < d) d = t;
	//  t = DistGreatCircle(clat, cte.GetLonMax(), gLat, gLon);
	//  if (t < d) d = t;
	//}
	return d;
}

WX_DEFINE_SORTED_ARRAY_INT(int, MySortedArrayInt);
static int CompareInts(int n1, int n2) {
	double d1 = chart_dist(n1);
	double d2 = chart_dist(n2);
	return (int)(d1 - d2);
}

class compress_target {
public:
	wxString chart_path;
	double distance;
};

WX_DECLARE_OBJARRAY(compress_target, ArrayOfCompressTargets);
WX_DEFINE_OBJARRAY(ArrayOfCompressTargets);

#include <wx/arrimpl.cpp>
// end duplicated code

void ParseAllENC(wxWindow * parent) {
	MySortedArrayInt idx_sorted_by_distance(CompareInts);

	// Building the cache may take a long time....
	// Be a little smarter.
	// Build a sorted array of chart database indices, sorted on distance from the
	// ownship currently. This way, a user may build a few chart SENCs for
	// immediate use, then "skip" or "cancel"out on the rest until later.
	int count = 0;
	for (int i = 0; i < ChartData->GetChartTableEntries(); i++) {
		/* skip if not ENC */
		const ChartTableEntry& cte = ChartData->GetChartTableEntry(i);
		if (CHART_TYPE_S57 != cte.GetChartType()) continue;

		idx_sorted_by_distance.Add(i);
		count++;
	}

	if (count == 0) return;

	wxLogMessage(wxString::Format(_T("ParseAllENC() count = %d"), count));

	//  Build another array of sorted compression targets.
	//  We need to do this, as the chart table will not be invariant
	//  after the compression threads start, so our index array will be invalid.

	ArrayOfCompressTargets ct_array;
	for (unsigned int j = 0; j < idx_sorted_by_distance.GetCount(); j++) {
		int i = idx_sorted_by_distance[j];

		const ChartTableEntry& cte = ChartData->GetChartTableEntry(i);
		double distance = chart_dist(i);

		wxString filename(cte.GetpFullPath(), wxConvUTF8);

		compress_target* pct = new compress_target;
		pct->distance = distance;
		pct->chart_path = filename;

		ct_array.push_back(pct);
	}

	int thread_count = 0;

	extern int g_nCPUCount;
	if (g_nCPUCount > 0)
		thread_count = g_nCPUCount;
	else
		thread_count = wxThread::GetCPUCount();

	if (thread_count < 1) {
		// obviously there's a least one CPU!
		thread_count = 1;
	}

	// parse targets
	count = 0;
	for (unsigned int j = 0; j < ct_array.size(); j++) {
		wxString filename = ct_array[j].chart_path;
		double distance = ct_array[j].distance;
		int index = ChartData->FinddbIndex(filename);
		if (index < 0) continue;
		const ChartTableEntry& cte = ChartData->GetChartTableEntry(index);
		Extent ext;
		ext.NLAT = cte.GetLatMax();
		ext.SLAT = cte.GetLatMin();
		ext.WLON = cte.GetLonMin();
		ext.ELON = cte.GetLonMax();

		int scale = cte.GetScale();

		wxString msg;
		msg.Printf(_("Distance from Ownship:  %4.0f NMi"), distance);

		count++;		

		if (ps52plib) {
			s57chart* newChart = new s57chart;

			newChart->SetNativeScale(scale);
			newChart->SetFullExtent(ext);
			newChart->DisableBackgroundSENC();

			newChart->FindOrCreateSenc(filename, false);  // no progress dialog required
			delete newChart;
		}

#if defined(__WXMSW__) || defined(__WXOSX__)
		::wxSafeYield();
#endif
	}

#if 0
	/* wait for workers to finish, and clean up after then */
	for (int t = 0; t < thread_count; t++) {
		if (workers[t]) {
			workers[t]->Wait();
			delete workers[t];
		}
	}
	delete[] workers;
#endif
}


//------------------------------------------------------------------------------
//              Fwd Refs
//------------------------------------------------------------------------------

int ShowNavWarning() {
  wxString msg0(
      _("\n\
OpenCPN is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied \
warranty of MERCHANTABILITY or FITNESS FOR A \
PARTICULAR PURPOSE.\n\n\
See the GNU General Public License for more details.\n\n\
OpenCPN must only be used in conjunction with approved \
paper charts and traditional methods of navigation.\n\n\
DO NOT rely upon OpenCPN for safety of life or property.\n\n\
Please click \"OK\" to agree and proceed, \"Cancel\" to quit.\n"));

  wxString vs = wxString::Format(wxT(" .. Version %s"), VERSION_FULL);

  return 0;
}

bool isSingleChart(ChartBase *chart) {
  if (chart == nullptr) return false;

  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc && cc->m_singleChart == chart) {
      return true;
    }
  }
  return false;
}

#if defined(__WXGTK__) && defined(OCPN_HAVE_X11)

// Note: use XFree to free this pointer. Use unique_ptr in the future.
static char *get_X11_property(Display *disp, Window win, Atom xa_prop_type,
                              const char *prop_name) {
  Atom xa_prop_name;
  Atom xa_ret_type;
  int ret_format;
  unsigned long ret_nitems;
  unsigned long ret_bytes_after;
  unsigned char *ret_prop;

  xa_prop_name = XInternAtom(disp, prop_name, False);

  // For XGetWindowProperty source see
  // https://github.com/mirror/libX11/blob/master/src/GetProp.c#L107
  // it is quite tricky. Some notes.
  // + Results are already NULL terminated.
  // + 32 as a ret_format means sizeof(long) in the API...
  // + but as xlib does the null termination we can just ignore the sizes.
  if (XGetWindowProperty(disp, win, xa_prop_name, 0, 1024, False, xa_prop_type,
                         &xa_ret_type, &ret_format, &ret_nitems,
                         &ret_bytes_after, &ret_prop) != Success)
    return NULL;

  if (xa_ret_type != xa_prop_type) {
    XFree(ret_prop);
    return NULL;
  }
  return (char *)ret_prop;
}
#endif

// Determine if a transparent toolbar is possible under linux with opengl
static bool isTransparentToolbarInOpenGLOK(void) {
#ifdef __WXOSX__
  return true;
#else
  bool status = false;
#ifndef __WXQT__
#ifdef OCPN_HAVE_X11
  if (!g_bdisable_opengl) {
    Display *disp = XOpenDisplay(NULL);
    Window *sup_window;
    if ((sup_window = (Window *)get_X11_property(disp, DefaultRootWindow(disp),
                                                 XA_WINDOW,
                                                 "_NET_SUPPORTING_WM_CHECK")) ||
        (sup_window = (Window *)get_X11_property(disp, DefaultRootWindow(disp),
                                                 XA_CARDINAL,
                                                 "_WIN_SUPPORTING_WM_CHECK"))) {
      /* WM_NAME */
      char *wm_name;
      if ((wm_name = get_X11_property(disp, *sup_window,
                                      XInternAtom(disp, "UTF8_STRING", False),
                                      "_NET_WM_NAME")) ||
          (wm_name = get_X11_property(disp, *sup_window, XA_STRING,
                                      "_NET_WM_NAME"))) {
        // we know it works in xfce4, add other checks as we can validate them
        if (strstr(wm_name, "Xfwm4") || strstr(wm_name, "Compiz"))
          status = true;

        XFree(wm_name);
      }
      XFree(sup_window);
    }
    XCloseDisplay(disp);
  }
#endif
#endif
  return status;
#endif
}

//------------------------------------------------------------------------------
// MyFrame
//------------------------------------------------------------------------------

//      Frame implementation
wxDEFINE_EVENT(BELLS_PLAYED_EVTYPE, wxCommandEvent);

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_CLOSE(MyFrame::OnCloseWindow)
EVT_MENU(wxID_EXIT, MyFrame::OnExit)
EVT_SIZE(MyFrame::OnSize)
EVT_MAXIMIZE(MyFrame::OnMaximize)
EVT_ERASE_BACKGROUND(MyFrame::OnEraseBackground)
EVT_TIMER(RECAPTURE_TIMER, MyFrame::OnRecaptureTimer)
EVT_COMMAND(wxID_ANY, BELLS_PLAYED_EVTYPE, MyFrame::OnBellsFinished)

#ifdef wxHAS_POWER_EVENTS
EVT_POWER_SUSPENDING(MyFrame::OnSuspending)
EVT_POWER_SUSPENDED(MyFrame::OnSuspended)
EVT_POWER_SUSPEND_CANCEL(MyFrame::OnSuspendCancel)
EVT_POWER_RESUME(MyFrame::OnResume)
#endif  // wxHAS_POWER_EVENTS

END_EVENT_TABLE()

/*
 * Direct callback from completed sound, possibly in an interrupt
 * context. Just post an event to be processed in main thread.
 */
static void onBellsFinishedCB(void *ptr) {
  auto framePtr = static_cast<MyFrame *>(ptr);
  if (framePtr) {
    wxCommandEvent ev(BELLS_PLAYED_EVTYPE);
    wxPostEvent(framePtr, ev);
  }
}

// My frame constructor
MyFrame::MyFrame(wxFrame *frame, const wxString &title, const wxPoint &pos, const wxSize &size, long style)
    : wxFrame(frame, -1, title, pos, size, style)  // wxSIMPLE_BORDER | wxCLIP_CHILDREN | wxRESIZE_BORDER)
      // wxCAPTION | wxSYSTEM_MENU | wxRESIZE_BORDER
      {
  m_last_track_rotation_ts = 0;
  m_ulLastNMEATicktime = 0;

  m_pStatusBar = NULL;
  m_StatusBarFieldCount = g_Platform->GetStatusBarFieldCount();

  m_pMenuBar = NULL;
  g_options = NULL;
  piano_ctx_menu = NULL;
  InitTimer.SetOwner(this, INIT_TIMER);
  m_iInitCount = 0;
  m_initializing = false;
  //      Set up some assorted member variables
  m_bTimeIsSet = false;
  m_bDateIsSet = false;
  nBlinkerTick = 0;

  m_bdefer_resize = false;

  //    Clear the NMEA Filter tables
  for (int i = 0; i < MAX_COGSOG_FILTER_SECONDS; i++) {
    COGFilterTable[i] = NAN;
    SOGFilterTable[i] = NAN;
  }
  m_last_bGPSValid = false;
  m_last_bVelocityValid = false;

  for (int i = 0; i < MAX_COG_AVERAGE_SECONDS; i++) COGTable[i] = NAN;

  m_fixtime = -1;

  m_bpersistent_quilt = false;

  m_ChartUpdatePeriod = 1;  // set the default (1 sec.) period
    
  b_autofind = false;

  //  And from the thread SENC creator
  Connect(wxEVT_OCPN_BUILDSENCTHREAD, (wxObjectEventFunction)(wxEventFunction)&MyFrame::OnSENCEvtThread);
  //        Establish the system icons for the frame.

#ifdef __WXMSW__
  SetIcon(wxICON(0));  // this grabs the first icon in the integrated MSW resource file
#endif

#if defined(__WXGTK__) || defined(__WXQT__)
  wxIcon app_icon(opencpn);  // This comes from opencpn.xpm inclusion above
  SetIcon(app_icon);
#endif

#ifdef __WXMSW__
  //    Establish the entry points in USER32.DLL for system color control
  wxDynamicLibrary dllUser32(_T("user32.dll"));

  pSetSysColors = (SetSysColors_t)dllUser32.GetSymbol(wxT("SetSysColors"));
  pGetSysColor = (GetSysColor_t)dllUser32.GetSymbol(wxT("GetSysColor"));

  SaveSystemColors();
#endif

  m_next_available_plugin_tool_id = ID_PLUGIN_BASE;

  g_sticky_chart = -1;
  g_sticky_projection = -1;
  m_BellsToPlay = 0;

  m_resizeTimer.SetOwner(this, RESIZE_TIMER);
  m_recaptureTimer.SetOwner(this, RECAPTURE_TIMER);
  m_tick_idx = 0;


#ifdef __WXOSX__
  // Enable native fullscreen on macOS
  EnableFullScreenView();
#endif
}

MyFrame::~MyFrame() {
  FrameTimer1.Stop();
  delete ChartData;  
}

void MyFrame::OnSENCEvtThread(OCPN_BUILDSENC_ThreadEvent &event) {
  s57chart *chart;
  switch (event.type) {
    case SENC_BUILD_STARTED:
      // printf("Myframe SENC build started\n");
      break;
    case SENC_BUILD_DONE_NOERROR:
      // printf("Myframe SENC build done no error\n");
      chart = event.m_ticket->m_chart;
      if (chart) {
        chart->PostInit(FULL_INIT, global_color_scheme);
        // ..For each canvas, force an S52PLIB reconfig...
        for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
          ChartCanvas *cc = g_canvasArray.Item(i);
          if (cc) cc->ClearS52PLIBStateHash();  // Force a S52 PLIB re-configure
        }
      }

      ReloadAllVP();
      delete event.m_ticket;
      break;
    case SENC_BUILD_DONE_ERROR:
      // printf("Myframe SENC build done ERROR\n");
      break;
    default:
      break;
  }
}

void MyFrame::RebuildChartDatabase() {
  bool b_SetInitialPoint = false;

  //   Build the initial chart dir array
  ArrayOfCDI ChartDirArray;
  pConfig->LoadChartDirArray(ChartDirArray);

  if (ChartDirArray.GetCount()) {
    //              Create and Save a new Chart Database based on the hints
    //              given in the config file
	  printf("OpenCPN needs to update the chart database from config file \n");

    delete ChartData;
    ChartData = new ChartDB();

    wxString line(_("Rebuilding chart database from configuration file entries..."));
    /* The following 3 strings are embeded in wxProgressDialog but must be
     * included by xgettext to be localized properly. See
     * {wxWidgets}src/generic/progdlgg.cpp:190 */
    wxString dummy1 = _("Elapsed time : ");
    wxString dummy2 = _("Estimated time : ");
    wxString dummy3 = _("Remaining time : ");    

    ChartData->Create(ChartDirArray);
    ChartData->SaveBinary(ChartListFileName);

    //  Apply the inital Group Array structure to the chart data base
    ChartData->ApplyGroupArray(g_pGroupArray);
  }
}

// play an arbitrary number of bells by using 1 and 2 bell sounds
void MyFrame::OnBellsFinished(wxCommandEvent &event) {
  int bells = wxMin(m_BellsToPlay, 2);
  if (bells <= 0) return;

  wxString soundfile = _T("sounds");
  appendOSDirSlash(&soundfile);
  soundfile.Prepend(g_Platform->GetSharedDataDir());
  wxLogMessage(_T("Using bells sound file: ") + soundfile);

  m_BellsToPlay -= bells;
}

void MyFrame::OnEraseBackground(wxEraseEvent &event) {}

void MyFrame::OnMaximize(wxMaximizeEvent &event) {
  g_click_stop = 0;
#ifdef __WXOSX__
  event.Skip();
#endif
}

ColorScheme GetColorScheme() { return global_color_scheme; }

ColorScheme MyFrame::GetColorScheme() { return global_color_scheme; }

void MyFrame::ReloadAllVP() {
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) cc->ReloadVP();
  }
}

void MyFrame::SetAndApplyColorScheme(ColorScheme cs) {
  global_color_scheme = cs;

  wxString SchemeName;
  switch (cs) {
    case GLOBAL_COLOR_SCHEME_DAY:
      SchemeName = _T("DAY");
      break;
    case GLOBAL_COLOR_SCHEME_DUSK:
      SchemeName = _T("DUSK");
      break;
    case GLOBAL_COLOR_SCHEME_NIGHT:
      SchemeName = _T("NIGHT");
      break;
    default:
      SchemeName = _T("DAY");
      break;
  }

  g_pauidockart->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_NONE);

  g_pauidockart->SetColour(wxAUI_DOCKART_BORDER_COLOUR, wxColour(0, 0, 0));
  g_pauidockart->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 1);
  g_pauidockart->SetColour(wxAUI_DOCKART_SASH_COLOUR, wxColour(0, 0, 0));
  g_pauidockart->SetMetric(wxAUI_DOCKART_SASH_SIZE, 0);
  g_pauidockart->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR,
                           wxColour(0, 0, 0));
  g_pauidockart->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, wxColour(0, 0, 0));

  //    if( cs == GLOBAL_COLOR_SCHEME_DUSK || cs == GLOBAL_COLOR_SCHEME_NIGHT )
  //    {
  //        g_pauidockart->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
  //        g_pauidockart->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR,
  //        wxColour(0,0,0));
  //        g_pauidockart->SetColour(wxAUI_DOCKART_BORDER_COLOUR,
  //        wxColour(0,0,0));
  //    }

  //      else{
  //          g_pauidockart->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE,
  //          g_grad_default);
  //          g_pauidockart->SetColour(wxAUI_DOCKART_BORDER_COLOUR,
  //          g_border_color_default);
  //          g_pauidockart->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE,
  //          g_border_size_default);
  //          g_pauidockart->SetColour(wxAUI_DOCKART_SASH_COLOUR,
  //          g_sash_color_default);
  //          g_pauidockart->SetMetric(wxAUI_DOCKART_SASH_SIZE,
  //          g_sash_size_default);
  //          g_pauidockart->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR,
  //          g_caption_color_default);
  //          g_pauidockart->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR,
  //          g_background_color_default);
  //
  //      }

  g_pauidockart->SetColour(wxAUI_DOCKART_SASH_COLOUR, wxColour(0, 0, 0));
  g_pauidockart->SetMetric(wxAUI_DOCKART_SASH_SIZE, 6);

  g_pauimgr->Update();

  g_StyleManager->GetCurrentStyle()->SetColorScheme(cs);

  // Search the user color table array to find the proper hash table
  Usercolortable_index = 0;
  for (unsigned int i = 0; i < UserColorTableArray->GetCount(); i++) {
    colTable *ct = (colTable *)UserColorTableArray->Item(i);
    if (SchemeName.IsSameAs(*ct->tableName)) {
      Usercolortable_index = i;
      break;
    }
  }

  if (ps52plib) ps52plib->SetPLIBColorScheme(SchemeName, ChartCtxFactory());

  //    Set up a pointer to the proper hash table
  pcurrent_user_color_hash =
      (wxColorHashMap *)UserColourHashTableArray->Item(Usercolortable_index);

  SetSystemColors(cs);

  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      cc->SetColorScheme(cs);
      cc->GetWorldBackgroundChart()->SetColorScheme(cs);
      cc->HideChartInfoWindow();
      cc->SetQuiltChartHiLiteIndex(-1);
    }
  }

  if (ChartData) ChartData->ApplyColorSchemeToCachedCharts(cs);

  if (g_options) {
    g_options->SetColorScheme(cs);
  }
  //    For the AIS target query dialog, we must rebuild it to incorporate the
  //    style desired for the colorscheme selected
  ApplyGlobalColorSchemetoStatusBar();

  UpdateAllToolbars(cs);
}

void MyFrame::ApplyGlobalColorSchemetoStatusBar(void) {
  if (m_pStatusBar != NULL) {
    m_pStatusBar->SetBackgroundColour(GetGlobalColor(_T("UIBDR")));  // UINFF
    m_pStatusBar->ClearBackground();
  }
}

ChartCanvas *MyFrame::GetPrimaryCanvas() {
  if (g_canvasArray.GetCount() > 0)
    return g_canvasArray.Item(0);
  else
    return NULL;
}
void MyFrame::CancelAllMouseRoute() {  
}

void MyFrame::NotifyChildrenResize() {}

void MyFrame::CreateCanvasLayout(bool b_useStoredSize) {
  //  Clear the cache, and thus close all charts to avoid memory leaks
  if (ChartData) ChartData->PurgeCache();

  // Detach all canvases from AUI manager
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray[i];
    if (cc) {
      g_pauimgr->DetachPane(cc);
    }
  }

  // Destroy any existing canvases, except for Primary canvas
  for (unsigned int i = 1; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      cc->Destroy();
    }
  }

  // Canvas pointers in config array are now invalid
  for (unsigned int i = 1; i < g_canvasConfigArray.GetCount(); i++) {
    g_canvasConfigArray.Item(i)->canvas = NULL;
  }

  //    g_canvasArray.Clear();

  // Clear the canvas Array, except for Primary canvas
  for (unsigned int i = 1; i < g_canvasArray.GetCount(); i++) {
    g_canvasArray.RemoveAt(i);
  }

  ChartCanvas *cc = NULL;
  switch (g_canvasConfig) {
    default:
    case 0:  // a single canvas
      if (!g_canvasArray.GetCount() || !g_canvasConfigArray.Item(0)) {
        cc = new ChartCanvas(this, 0);  // the chart display canvas
        g_canvasArray.Add(cc);
      } else {
        cc = g_canvasArray[0];
      }

      // Verify that glCanvas is ready, if necessary
      if (g_bopengl) {
        if (!cc->GetglCanvas()) cc->SetupGlCanvas();
        cc->GetglCanvas()->Show();
      }

      g_canvasConfigArray.Item(0)->canvas = cc;

      cc->SetDisplaySizeMM(g_display_size_mm);

      cc->ApplyCanvasConfig(g_canvasConfigArray.Item(0));

      cc->SetColorScheme(global_color_scheme);      

      g_pauimgr->AddPane(cc);
      g_pauimgr->GetPane(cc).Name(_T("ChartCanvas"));
      g_pauimgr->GetPane(cc).Fixed();
      g_pauimgr->GetPane(cc).CaptionVisible(false);
      g_pauimgr->GetPane(cc).CenterPane();

      break;

    case 1: {  // two canvas, horizontal
      break;
    }
    case 2:  // two canvas, vertical

      break;
  }

  g_focusCanvas = GetPrimaryCanvas();
}

void MyFrame::RequestNewToolbars(bool bforcenew) {
  if (b_inCloseWindow) {
    return;
  }
}

//      Update inplace the various controls with bitmaps corresponding to the
//      current color scheme
void MyFrame::UpdateAllToolbars(ColorScheme cs) {
  return;
}

ChartCanvas *MyFrame::GetCanvasUnderMouse() {
  wxPoint screenPoint = ::wxGetMousePosition();
  canvasConfig *cc;

  switch (g_canvasConfig) {
    case 1:
      cc = g_canvasConfigArray.Item(0);
      if (cc) {
        ChartCanvas *canvas = cc->canvas;
        if (canvas->GetScreenRect().Contains(
                /*canvas->ScreenToClient*/ (screenPoint)))
          return canvas;
      }
      cc = g_canvasConfigArray.Item(1);
      if (cc) {
        ChartCanvas *canvas = cc->canvas;
        if (canvas->GetScreenRect().Contains(
                /*canvas->ScreenToClient*/ (screenPoint)))
          return canvas;
      }
      break;

    default:
      cc = g_canvasConfigArray.Item(0);
      if (cc) {
        ChartCanvas *canvas = cc->canvas;
        if (canvas->GetScreenRect().Contains(
                canvas->ScreenToClient(screenPoint)))
          return canvas;
      }
  }

  return NULL;
}

int MyFrame::GetCanvasIndexUnderMouse() {
  wxPoint screenPoint = ::wxGetMousePosition();
  canvasConfig *cc;

  switch (g_canvasConfig) {
    case 1:
      cc = g_canvasConfigArray.Item(0);
      if (cc) {
        ChartCanvas *canvas = cc->canvas;
        if (canvas->GetScreenRect().Contains(
                /*canvas->ScreenToClient*/ (screenPoint)))
          return 0;
      }
      cc = g_canvasConfigArray.Item(1);
      if (cc) {
        ChartCanvas *canvas = cc->canvas;
        if (canvas->GetScreenRect().Contains(
                /*canvas->ScreenToClient*/ (screenPoint)))
          return 1;
      }
      break;

    default:
      cc = g_canvasConfigArray.Item(0);
      if (cc) {
        ChartCanvas *canvas = cc->canvas;
        if (canvas->GetScreenRect().Contains(
                canvas->ScreenToClient(screenPoint)))
          return 0;
      }
  }

  return -1;
}

bool MyFrame::DropMarker(bool atOwnShip) {
  double lat, lon;
  ChartCanvas *canvas = GetCanvasUnderMouse();
  
  if (!canvas) return false;

  lat = canvas->m_cursor_lat;
  lon = canvas->m_cursor_lon;

  InvalidateAllGL();
  RefreshAllCanvas(false);

  return true;
}

void MyFrame::SwitchKBFocus(ChartCanvas *pCanvas) {
  if (g_canvasConfig != 0) {  // multi-canvas?
    canvasConfig *cc;
    int nTarget = -1;
    int nTargetGTK = -1;
    ChartCanvas *target;
    wxWindow *source = FindFocus();
    ChartCanvas *test = wxDynamicCast(source, ChartCanvas);
    if (!test) return;

    // On linux(GTK), the TAB key causes a loss of focus immediately
    //  So the logic needs a switch
    switch (g_canvasConfig) {
      case 1:
        cc = g_canvasConfigArray.Item(0);
        if (cc) {
          ChartCanvas *canvas = cc->canvas;
          if (canvas && (canvas == test)) {
            nTarget = 1;
            nTargetGTK = 0;
          }
        }
        cc = g_canvasConfigArray.Item(1);
        if (cc) {
          ChartCanvas *canvas = cc->canvas;
          if (canvas && (canvas == test)) {
            nTarget = 0;
            nTargetGTK = 1;
          }
        }

        if (nTarget >= 0) {
          // printf("sw %d\n", nTarget);
          int nfinalTarget = nTarget;
#ifdef __WXGTK__
          nfinalTarget = nTargetGTK;
#endif
          target = g_canvasConfigArray.Item(nfinalTarget)->canvas;
          if (target) {
            wxWindow *win = wxDynamicCast(target, wxWindow);
            win->SetFocus();
            target->Refresh(true);
          }
        }
        break;

      default:
        break;
    }
  }
}

void MyFrame::FastClose() {
  FrameTimer1.Stop();
  quitflag++;            // signal to the timer loop
  FrameTimer1.Start(1);  // real quick now...
}

// Intercept menu commands
void MyFrame::OnExit(wxCommandEvent &event) {
  quitflag++;  // signal to the timer loop
}

void MyFrame::OnCloseWindow(wxCloseEvent &event) {
  //    It is possible that double clicks on application exit box could cause
  //    re-entrance here Not good, and don't need it anyway, so simply return.
  if (b_inCloseWindow) {
    //            wxLogMessage(_T("opencpn::MyFrame re-entering
    //            OnCloseWindow"));
    return;
  }

  // The Options dialog, and other deferred init items, are not fully
  // initialized. Best to just cancel the close request. This is probably only
  // reachable on slow hardware, or on Android life-cycle events...
#ifndef __OCPN__ANDROID__
  if (!g_bDeferredInitDone) return;
#endif

#ifndef __WXOSX__
  if (g_options) {
    delete g_options;
    g_options = NULL;
    g_pOptions = NULL;
  }
#endif

  //  If the multithread chart compressor engine is running, cancel the close
  //  command
  if (b_inCompressAllCharts) {
    return;
  }

  if (bDBUpdateInProgress) return;

  b_inCloseWindow = true;

  ::wxSetCursor(wxCURSOR_WAIT);
  
  // Make sure the saved perspective minimum canvas sizes are essentially
  // undefined
  for(unsigned int i=0 ; i < g_canvasArray.GetCount() ; i++){
      ChartCanvas *cc = g_canvasArray.Item(i);
      if(cc)
          g_pauimgr->GetPane( cc ).MinSize(10,10);
  }

  pConfig->SetPath(_T ( "/AUI" ));
  pConfig->Write(_T ( "AUIPerspective" ), g_pauimgr->SavePerspective());

  g_bquiting = true;

#ifdef ocpnUSE_GL
  // cancel compression jobs
  if (g_bopengl) {
    if (g_glTextureManager) {
      g_glTextureManager->PurgeJobList();

      if (g_glTextureManager->GetRunningJobCount()) g_bcompression_wait = true;
    }
  }
#endif

  SetCursor(wxCURSOR_WAIT);

  RefreshAllCanvas(true);

  //  This yield is not necessary, since the Update() proceeds syncronously...
  // wxYield();

  //   Save the saved Screen Brightness
  RestoreScreenBrightness();

  // Persist the toolbar locations

  g_bframemax = IsMaximized();

  FrameTimer1.Stop();
  FrameCOGTimer.Stop();

  /*
  Automatically drop an anchorage waypoint, if enabled
  On following conditions:
  1.  In "Cruising" mode, meaning that speed has at some point exceeded 3.0 kts.
  2.  Current speed is less than 0.5 kts.
  3.  Opencpn has been up at least 30 minutes
  4.  And, of course, opencpn is going down now.
  5.  And if there is no anchor watch set on "anchor..." icon mark           //
  pjotrc 2010.02.15
  */
  if (g_bAutoAnchorMark) {
    bool watching_anchor = false;  // pjotrc 2010.02.15
    wxDateTime now = wxDateTime::Now();
    wxTimeSpan uptime = now.Subtract(g_start_time);
  }

  // Provisionally save all settings before deactivating plugins
  pConfig->UpdateSettings();
  
  wxLogMessage(_T("opencpn::MyFrame exiting cleanly."));

  quitflag++;

  pConfig->UpdateNavObj();

  // Remove any leftover Routes and Waypoints from config file as they were
  // saved to navobj before
  pConfig->DeleteGroup(_T ( "/Routes" ));
  pConfig->DeleteGroup(_T ( "/Marks" ));
  pConfig->Flush();

  delete g_printData;
  delete g_pageSetupData;
#ifndef __WXQT__
  SetStatusBar(NULL);
#endif

  //  Clear the cache, and thus close all charts to avoid memory leaks
  if (ChartData) ChartData->PurgeCache();

  // pthumbwin is a canvas child
  // pthumbwin = NULL;

  // Finally ready to destroy the canvases
  g_focusCanvas = NULL;

  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) cc->Destroy();
  }

  g_canvasArray.Clear();

  g_pauimgr->UnInit();
  delete g_pauimgr;
  g_pauimgr = NULL;

  //    Unload the PlugIns
  //      Note that we are waiting until after the canvas is destroyed,
  //      since some PlugIns may have created children of canvas.
  //      Such a PlugIn must stay intact for the canvas dtor to call
  //      DestoryChildren()

  if (ChartData) ChartData->PurgeCachePlugins();

  delete pConfig;  // All done
  pConfig = NULL;
  InitBaseConfig(0);
  
  if (pLayerList) {
    LayerList::iterator it;
    while (pLayerList->GetCount()) {
      Layer *lay = pLayerList->GetFirst()->GetData();
      delete lay;  // automatically removes the layer from list, see Layer dtor
    }
  }

  g_bTempShowMenuBar = false;

#define THREAD_WAIT_SECONDS 5
#ifdef ocpnUSE_GL
  // The last thing we do is finish the compression threads.
  // This way the main window is already invisible and to the user
  // it appears to have finished rather than hanging for several seconds
  // while the compression threads exit
  if (g_bopengl && g_glTextureManager &&
      g_glTextureManager->GetRunningJobCount()) {
    g_glTextureManager->ClearAllRasterTextures();

    wxLogMessage(_T("Starting compressor pool drain"));
    wxDateTime now = wxDateTime::Now();
    time_t stall = now.GetTicks();
    time_t end = stall + THREAD_WAIT_SECONDS;

    int n_comploop = 0;
    while (stall < end) {
      wxDateTime later = wxDateTime::Now();
      stall = later.GetTicks();

      wxString msg;
      msg.Printf(_T("Time: %d  Job Count: %d"), n_comploop,
                 g_glTextureManager->GetRunningJobCount());
      wxLogMessage(msg);
      if (!g_glTextureManager->GetRunningJobCount()) break;
      wxYield();
      wxSleep(1);
    }

    wxString fmsg;
    fmsg.Printf(_T("Finished compressor pool drain..Time: %d  Job Count: %d"),
                n_comploop, g_glTextureManager->GetRunningJobCount());
    wxLogMessage(fmsg);
  }
  delete g_glTextureManager;
#endif

  this->Destroy();
  gFrame = NULL;

  wxLogMessage(_T("gFrame destroyed."));
}

void MyFrame::OnMove(wxMoveEvent &event) {
#ifdef __WXOSX__
  SendSizeEvent();
#endif
  //    Somehow, this method does not work right on Windows....
  //      g_nframewin_posx = event.GetPosition().x;
  //      g_nframewin_posy = event.GetPosition().y;

  g_nframewin_posx = GetPosition().x;
  g_nframewin_posy = GetPosition().y;
}

void MyFrame::ProcessCanvasResize(void) {
  TriggerRecaptureTimer();
}

void MyFrame::TriggerRecaptureTimer() {
  m_recaptureTimer.Start(
      1000, wxTIMER_ONE_SHOT);  // One second seems enough, on average
}

void MyFrame::OnRecaptureTimer(wxTimerEvent &event) { Raise(); }

void MyFrame::SetCanvasSizes(wxSize frameSize) {
  if (!g_canvasArray.GetCount()) return;  

#if 0
    int cccw = frameSize.x;
    int ccch = frameSize.y;
#endif

  // .. for each canvas...
  switch (g_canvasConfig) {
    default:
    case 0:
#if 0
            cc = g_canvasArray.Item(0);
            if( cc ) {
                cc->GetSize( &cur_width, &cur_height );
                if( ( cur_width != cccw ) || ( cur_height != ccch ) ) {
                    if( g_pauimgr->GetPane( cc ).IsOk() )
                        g_pauimgr->GetPane( cc ).BestSize( cccw, ccch );
                    else
                        cc->SetSize( 0, 0, cccw, ccch );
                }
            }
#endif
      break;

    case 1:
#if 0
            cc = g_canvasArray.Item(1);
            if( cc ) {
               int ccw = g_canvasConfigArray.Item(1)->canvasSize.x;
               int cch = g_canvasConfigArray.Item(1)->canvasSize.y;

               ccw = wxMin(ccw, cccw * 8 / 10);
               ccw = wxMax(ccw, cccw * 2 / 10);
               if(cccw < 100)
                   ccw = 20;

               g_canvasConfigArray.Item(1)->canvasSize = wxSize(ccw, cch);
//               g_pauimgr->GetPane(cc).MinSize(cccw * 2 / 10, ccch);

#if 1  // ndef __WXMSW__
       // wxAUI hack: This is needed to explicietly set a docked pane size
       // Set MinSize to desired value, then call wxAuiPaneInfo::Fixed() to
       // apply it
                g_pauimgr->GetPane(cc).MinSize(ccw, cch);
                g_pauimgr->GetPane(cc).Fixed();
                g_pauimgr->Update();

                //now make resizable again
                g_pauimgr->GetPane(cc).Resizable();
                ///g_pauimgr->GetPane(cc).MinSize(cccw * 2 / 10, ccch);
                //g_pauimgr->Update();  //Deferred
                //g_pauimgr->GetPane( cc ).BestSize( ccw, cch );
#endif
            }
#endif

      break;
  }
}

void MyFrame::OnSize(wxSizeEvent &event) { ODoSetSize(); }

void MyFrame::ODoSetSize(void) {
  int x, y;
  GetClientSize(&x, &y);
  //      Resize the children

  if (m_pStatusBar != NULL) {
    m_StatusBarFieldCount = g_Platform->GetStatusBarFieldCount();
    int currentCount = m_pStatusBar->GetFieldsCount();
    if (currentCount != m_StatusBarFieldCount) {
      if ((currentCount > 0) && (currentCount < 7)) {
        // reset the widths very small to avoid auto-resizing of the frame
        // The sizes will be reset later in this method
        int widths[] = {2, 2, 2, 2, 2, 2};
        m_pStatusBar->SetStatusWidths(currentCount, widths);
      }

      m_pStatusBar->SetFieldsCount(m_StatusBarFieldCount);
    }

    if (m_StatusBarFieldCount) {
      //  If the status bar layout is "complex", meaning more than two columns,
      //  then use custom crafted relative widths for the fields.
      //  Otherwise, just split the frame client width into equal spaces

      if (m_StatusBarFieldCount > 2) {
        int widths[] = {-6, -5, -5, -6, -4};
        m_pStatusBar->SetStatusWidths(m_StatusBarFieldCount, widths);
      } else if (m_StatusBarFieldCount == 2) {
        int cwidth = x * 90 / 100;
        int widths[] = {100, 100};
        widths[0] = cwidth * 6.4 / 10.0;
        widths[1] = cwidth * 3.6 / 10.0;
        m_pStatusBar->SetStatusWidths(m_StatusBarFieldCount, widths);
      } else {
        int widths[] = {100, 100};
        widths[0] = x * 90 / 100;
        m_pStatusBar->SetStatusWidths(m_StatusBarFieldCount, widths);
      }

      int styles[] = {wxSB_FLAT, wxSB_FLAT, wxSB_FLAT,
                      wxSB_FLAT, wxSB_FLAT, wxSB_FLAT};
      m_pStatusBar->SetStatusStyles(m_StatusBarFieldCount, styles);

      wxString sogcog(_T("SOG --- ") + getUsrSpeedUnit() + +_T("     ") +
                      _T(" COG ---\u00B0"));
      m_pStatusBar->SetStatusText(sogcog, STAT_FIELD_SOGCOG);
    }
  }

  if (m_pStatusBar) {
    //  Maybe resize the font so the text fits in the boxes

    wxRect stat_box;
    m_pStatusBar->GetFieldRect(0, stat_box);
    // maximum size is 1/28 of the box width, or the box height - whicever is
    // less
    int max_font_size = wxMin((stat_box.width / 28), (stat_box.height));

    wxFont sys_font = *wxNORMAL_FONT;
    int try_font_size = sys_font.GetPointSize();

#ifdef __WXOSX__
    int min_font_size = 10;  // much less than 10pt is unreadably small on OS X
    try_font_size += 1;      // default to 1pt larger than system UI font
#else
    int min_font_size =
        7;               // on Win/Linux the text does not shrink quite so fast
    try_font_size += 2;  // default to 2pt larger than system UI font
#endif
  }

  SetCanvasSizes(GetClientSize());

  // .. for each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) cc->FormatPianoKeys();
  }

  //  If global toolbar is shown, reposition it...
  //  Update the stored window size
  GetSize(&x, &y);
  g_nframewin_x = x;
  g_nframewin_y = y;

  // FIXME (dave)  Thumbwins are gone...
  // if (pthumbwin) pthumbwin->SetMaxSize(GetClientSize());

  //  Reset the options dialog size logic
  options_lastWindowSize = wxSize(0, 0);
  options_lastWindowPos = wxPoint(0, 0);

  if (g_pauimgr) g_pauimgr->Update();
}

void MyFrame::ResizeManually(int nWidth, int nHeight) {	
	//      Resize the children
	if (m_pStatusBar != NULL) {
		m_StatusBarFieldCount = g_Platform->GetStatusBarFieldCount();
		int currentCount = m_pStatusBar->GetFieldsCount();
		if (currentCount != m_StatusBarFieldCount) {
			if ((currentCount > 0) && (currentCount < 7)) {
				// reset the widths very small to avoid auto-resizing of the frame
				// The sizes will be reset later in this method
				int widths[] = { 2, 2, 2, 2, 2, 2 };
				m_pStatusBar->SetStatusWidths(currentCount, widths);
			}

			m_pStatusBar->SetFieldsCount(m_StatusBarFieldCount);
		}

		if (m_StatusBarFieldCount) {
			//  If the status bar layout is "complex", meaning more than two columns,
			//  then use custom crafted relative widths for the fields.
			//  Otherwise, just split the frame client width into equal spaces

			if (m_StatusBarFieldCount > 2) {
				int widths[] = { -6, -5, -5, -6, -4 };
				m_pStatusBar->SetStatusWidths(m_StatusBarFieldCount, widths);
			}
			else if (m_StatusBarFieldCount == 2) {
				int cwidth = nWidth * 90 / 100;
				int widths[] = { 100, 100 };
				widths[0] = cwidth * 6.4 / 10.0;
				widths[1] = cwidth * 3.6 / 10.0;
				m_pStatusBar->SetStatusWidths(m_StatusBarFieldCount, widths);
			}
			else {
				int widths[] = { 100, 100 };
				widths[0] = nWidth * 90 / 100;
				m_pStatusBar->SetStatusWidths(m_StatusBarFieldCount, widths);
			}

			int styles[] = { wxSB_FLAT, wxSB_FLAT, wxSB_FLAT,
							wxSB_FLAT, wxSB_FLAT, wxSB_FLAT };
			m_pStatusBar->SetStatusStyles(m_StatusBarFieldCount, styles);

			wxString sogcog(_T("SOG --- ") + getUsrSpeedUnit() + +_T("     ") +
				_T(" COG ---\u00B0"));
			m_pStatusBar->SetStatusText(sogcog, STAT_FIELD_SOGCOG);
		}
	}

	if (m_pStatusBar) {
		//  Maybe resize the font so the text fits in the boxes

		wxRect stat_box;
		m_pStatusBar->GetFieldRect(0, stat_box);
		// maximum size is 1/28 of the box width, or the box height - whicever is
		// less
		int max_font_size = wxMin((stat_box.width / 28), (stat_box.height));

		wxFont sys_font = *wxNORMAL_FONT;
		int try_font_size = sys_font.GetPointSize();

#ifdef __WXOSX__
		int min_font_size = 10;  // much less than 10pt is unreadably small on OS X
		try_font_size += 1;      // default to 1pt larger than system UI font
#else
		int min_font_size =
			7;               // on Win/Linux the text does not shrink quite so fast
		try_font_size += 2;  // default to 2pt larger than system UI font
#endif
	}

	wxSize xSize(nWidth, nHeight);
	SetCanvasSizes(xSize);

	// .. for each canvas...
	for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
		ChartCanvas* cc = g_canvasArray.Item(i);
		if (cc) cc->FormatPianoKeys();
	}

	//  If global toolbar is shown, reposition it...
	//  Update the stored window size
	g_nframewin_x = nWidth;
	g_nframewin_y = nHeight;

	// FIXME (dave)  Thumbwins are gone...
	// if (pthumbwin) pthumbwin->SetMaxSize(GetClientSize());

	//  Reset the options dialog size logic
	options_lastWindowSize = wxSize(0, 0);
	options_lastWindowPos = wxPoint(0, 0);

	if (g_pauimgr) g_pauimgr->Update();
}

void MyFrame::PositionConsole(void) {
  if (NULL == GetPrimaryCanvas()) return;
  //    Reposition console based on its size and chartcanvas size
  int ccx, ccy, ccsx, ccsy, consx, consy;
  ChartCanvas *consoleHost = GetPrimaryCanvas();
  if (g_canvasConfig > 0) consoleHost = g_canvasArray[1];

  if (consoleHost) {
    consoleHost->GetSize(&ccsx, &ccsy);
    consoleHost->GetPosition(&ccx, &ccy);
  } else {
    GetPrimaryCanvas()->GetSize(&ccsx, &ccsy);
    GetPrimaryCanvas()->GetPosition(&ccx, &ccy);
    consoleHost = GetPrimaryCanvas();
  }  
}

void MyFrame::UpdateAllFonts() {
  //  Close and destroy any persistent dialogs, so that new fonts will be
  //  utilized
  DestroyPersistentDialogs();
  RefreshAllCanvas();
}

void MyFrame::DestroyPersistentDialogs() {  
}

void MyFrame::RefreshGroupIndices(void) {
  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) cc->canvasRefreshGroupIndex();
  }
}

bool MyFrame::SetGlobalToolbarViz(bool viz) {
  bool viz_now = g_bmasterToolbarFull;

  wxString tip = _("Show Toolbar");
  if (viz) {
    tip = _("Hide Toolbar");    
  }

  bool toggle = false;
  if (viz && !g_bmasterToolbarFull)
    toggle = true;

  else if (!viz && g_bmasterToolbarFull)
    toggle = true;

  if (toggle) {
    g_bmasterToolbarFull = !g_bmasterToolbarFull;
    ToolbarAnimateTimer.Start(10, wxTIMER_ONE_SHOT);
  }

  return viz_now;
}

void MyFrame::ScheduleSettingsDialog() {
  wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED);
  evt.SetId(ID_SETTINGS /*ID_MENU_SETTINGS_BASIC*/);
  GetEventHandler()->AddPendingEvent(evt);
}

ChartCanvas *MyFrame::GetFocusCanvas() {
  if ((g_canvasConfig != 0) && g_focusCanvas)  // multi-canvas?
    return g_focusCanvas;
  else
    return GetPrimaryCanvas();
}

void MyFrame::OnToolbarAnimateTimer(wxTimerEvent &event) { 
}

void MyFrame::InvalidateAllGL() {
#ifdef ocpnUSE_GL
  // For each canvas
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      cc->InvalidateGL();
      cc->Refresh();
    }
  }
#endif
}

void MyFrame::RefreshAllCanvas(bool bErase) {
  // For each canvas
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      cc->Refresh(bErase);
    }
  }
}

void MyFrame::SetAISDisplayStyle(ChartCanvas *cc, int StyleIndx) {
  cc->SetAISCanvasDisplayStyle(StyleIndx);

  UpdateGlobalMenuItems();
  ReloadAllVP();
}

void MyFrame::setStringVP(wxString VPS) {
  ChartCanvas *cc = GetPrimaryCanvas();

  if (!cc) return;
}

void MyFrame::DoSettings() {
  if (g_boptionsactive) return;

  bool bnewtoolbar = !(DoOptionsDialog() == 0);

  //              Apply various system settings
  ApplyGlobalSettings(bnewtoolbar);
// ..For each canvas...
  bool b_loadHarmonics = false;
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      if (cc->GetbShowCurrent() || cc->GetbShowTide()) b_loadHarmonics = true;
    }
  }
  if (b_loadHarmonics) LoadHarmonics();

  //  The chart display options may have changed, especially on S57 ENC,
  //  So, flush the cache and redraw
  ReloadAllVP();
}

void MyFrame::ToggleChartBar(ChartCanvas *cc) {
  g_bShowChartBar = !g_bShowChartBar;

  if (g_bShowChartBar) cc->m_brepaint_piano = true;

  cc->ReloadVP();  // needed to set VP.pix_height
  Refresh();

  if (g_bShowChartBar) {
    DoChartUpdate();
    UpdateControlBar(cc);
  }

  SetMenubarItemState(ID_MENU_UI_CHARTBAR, g_bShowChartBar);
}

void MyFrame::ToggleColorScheme() {
  static bool lastIsNight;
  ColorScheme s = GetColorScheme();
  int is = (int)s;
  is++;
  if (lastIsNight && is == 3)  // Back from step 3
  {
    is = 1;
    lastIsNight = false;
  }                                 //      Goto to Day
  if (lastIsNight) is = 2;          // Back to Dusk on step 3
  if (is == 3) lastIsNight = true;  // Step 2 Night
  s = (ColorScheme)is;
  if (s == N_COLOR_SCHEMES) s = GLOBAL_COLOR_SCHEME_RGB;

  SetAndApplyColorScheme(s);
}

void MyFrame::ToggleFullScreen() {
  bool to = !IsFullScreen();

#ifdef __WXOSX__
  ShowFullScreen(to);
#else
  long style = wxFULLSCREEN_NOBORDER | wxFULLSCREEN_NOCAPTION;
  ;  // | wxFULLSCREEN_NOMENUBAR;
  ShowFullScreen(to, style);
#endif

  UpdateAllToolbars(global_color_scheme);
  UpdateControlBar(GetPrimaryCanvas());
  Layout();
  TriggerRecaptureTimer();
}

void MyFrame::TrackOn(void) {
  g_bTrackActive = true;
    
  // The main toolbar may still be NULL here and we will do nothing...
  SetMasterToolbarItemState(ID_TRACK, g_bTrackActive);  

  SetMenubarItemState(ID_MENU_NAV_TRACK, g_bTrackActive);

#ifdef __OCPN__ANDROID__
  androidSetTrackTool(true);
#endif

  wxJSONValue v;
  wxDateTime now;
  now = now.Now().ToUTC();  
  
  wxString msg_id(_T("OCPN_TRK_ACTIVATED"));
  
  g_FlushNavobjChangesTimeout = 30;  // Every thirty seconds, consider flushing navob changes
}

void MyFrame::TrackDailyRestart(void) {
  TrackOn();
}

void MyFrame::SetUpMode(ChartCanvas *cc, int mode) {
  if (cc) {
    cc->SetUpMode(mode);

    SetMenubarItemState(ID_MENU_CHART_COGUP, mode == COURSE_UP_MODE);
    SetMenubarItemState(ID_MENU_CHART_NORTHUP, mode == NORTH_UP_MODE);
    SetMenubarItemState(ID_MENU_CHART_HEADUP, mode == HEAD_UP_MODE);

    if (m_pMenuBar)
      m_pMenuBar->SetLabel(ID_MENU_CHART_NORTHUP, _("North Up Mode"));
  }
}

void MyFrame::ToggleENCText(ChartCanvas *cc) {
  cc->SetShowENCText(!cc->GetShowENCText());

  SetMenubarItemState(ID_MENU_ENC_TEXT, cc->GetShowENCText());

  //     if(g_pi_manager)
  //         g_pi_manager->SendConfigToAllPlugIns();

  ReloadAllVP();
}

void MyFrame::SetENCDisplayCategory(ChartCanvas *cc, enum _DisCat nset) {
  if (ps52plib) {
    if (cc) {
      cc->SetENCDisplayCategory(nset);

      UpdateGlobalMenuItems();

      /*            if(g_pi_manager)
                      g_pi_manager->SendConfigToAllPlugIns();
       */
      ReloadAllVP();
    }
  }
}

void MyFrame::ToggleSoundings(ChartCanvas *cc) {
  cc->SetShowENCDepth(!cc->GetShowENCDepth());

  SetMenubarItemState(ID_MENU_ENC_SOUNDINGS, cc->GetShowENCDepth());

  ReloadAllVP();
}

bool MyFrame::ToggleLights(ChartCanvas *cc) {
  cc->SetShowENCLights(!cc->GetShowENCLights());

  SetMenubarItemState(ID_MENU_ENC_LIGHTS, cc->GetShowENCLights());

  ReloadAllVP();

  return true;
}

#if 0
void MyFrame::ToggleRocks( void )
{
    if( ps52plib ) {
        int vis =  0;
        // Need to loop once for UWTROC, which is our "master", then for
        // other categories, since order is unknown?
        for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
            OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
            if( !strncmp( pOLE->OBJLName, "UWTROC", 6 ) ) {
                pOLE->nViz = !pOLE->nViz;
                vis = pOLE->nViz;
            }
        }
        for( unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount(); iPtr++ ) {
            OBJLElement *pOLE = (OBJLElement *) ( ps52plib->pOBJLArray->Item( iPtr ) );
            if( !strncmp( pOLE->OBJLName, "OBSTRN", 6 ) ) {
                pOLE->nViz = vis;
            }
            if( !strncmp( pOLE->OBJLName, "WRECKS", 6 ) ) {
                pOLE->nViz = vis;
            }
        }
        ps52plib->GenerateStateHash();
        ReloadAllVP();
    }
}
#endif

void MyFrame::ToggleAnchor(ChartCanvas *cc) {
  cc->SetShowENCAnchor(!cc->GetShowENCAnchor());

  SetMenubarItemState(ID_MENU_ENC_ANCHOR, cc->GetShowENCAnchor());

  ReloadAllVP();
}

void MyFrame::ToggleDataQuality(ChartCanvas *cc) {
  cc->SetShowENCDataQual(!cc->GetShowENCDataQual());

  SetMenubarItemState(ID_MENU_ENC_DATA_QUALITY, cc->GetShowENCDataQual());
  ReloadAllVP();
}

void MyFrame::TogglebFollow(ChartCanvas *cc) {
  if (!cc->m_bFollow)
    SetbFollow(cc);
  else
    ClearbFollow(cc);
}

void MyFrame::ToggleNavobjects(ChartCanvas *cc) {
  cc->m_bShowNavobjects = !cc->m_bShowNavobjects;
  SetMenubarItemState(ID_MENU_SHOW_NAVOBJECTS, cc->m_bShowNavobjects);
  cc->Refresh();
}

void MyFrame::ToggleAISDisplay(ChartCanvas *cc) {
  cc->SetShowAIS(!cc->GetShowAIS());
  SetMenubarItemState(ID_MENU_AIS_TARGETS, cc->GetShowAIS());
  cc->Refresh();
}

void MyFrame::ToggleAISMinimizeTargets(ChartCanvas *cc) {
  cc->SetAttenAIS(!cc->GetAttenAIS());
  SetMenubarItemState(ID_MENU_AIS_SCALED_TARGETS, cc->GetAttenAIS());
  cc->Refresh();
}

void MyFrame::SetbFollow(ChartCanvas *cc) {  
  cc->m_bFollow = true;

  SetMenubarItemState(ID_MENU_NAV_FOLLOW, true);

  DoChartUpdate();
  cc->ReloadVP();
  SetChartUpdatePeriod();
}

void MyFrame::ClearbFollow(ChartCanvas *cc) {
  //    Center the screen on the GPS position, for lack of a better place
  
  cc->m_bFollow = false;
  SetMenubarItemState(ID_MENU_NAV_FOLLOW, false);

  DoChartUpdate();
  cc->ReloadVP();
  SetChartUpdatePeriod();
}

void MyFrame::ToggleChartOutlines(ChartCanvas *cc) {
  cc->SetShowOutlines(!cc->GetShowOutlines());

  RefreshAllCanvas(false);

#ifdef ocpnUSE_GL  // opengl renders chart outlines as part of the chart this
                   // needs a full refresh
  if (g_bopengl) InvalidateAllGL();
#endif

  SetMenubarItemState(ID_MENU_CHART_OUTLINES, cc->GetShowOutlines());
}

void MyFrame::ToggleTestPause(void) { g_bPauseTest = !g_bPauseTest; }

void MyFrame::SetMenubarItemState(int item_id, bool state) {
  if (m_pMenuBar) {
    bool enabled = m_pMenuBar->IsEnabled(item_id);
    m_pMenuBar->Enable(item_id, false);
    m_pMenuBar->Check(item_id, state);
    m_pMenuBar->Enable(item_id, enabled);
  }
}

void MyFrame::SetMasterToolbarItemState(int tool_id, bool state) {  
}

void MyFrame::SetToolbarItemBitmaps(int tool_id, wxBitmap *bmp, wxBitmap *bmpRollover) {  
}

void MyFrame::SetToolbarItemSVG(int tool_id, wxString normalSVGfile, wxString rolloverSVGfile, wxString toggledSVGfile) {  
}

void MyFrame::ApplyGlobalSettings(bool bnewtoolbar) {
  //             ShowDebugWindow as a wxStatusBar
  m_StatusBarFieldCount = g_Platform->GetStatusBarFieldCount();

#ifdef __WXMSW__
  UseNativeStatusBar(false);  // better for MSW, undocumented in frame.cpp
#endif

  wxSize lastOptSize = options_lastWindowSize;
  SendSizeEvent();

  SendSizeEvent();
  options_lastWindowSize = lastOptSize;
}

wxString _menuText(wxString name, wxString shortcut) {
  wxString menutext;
  menutext << name;
#ifndef __OCPN__ANDROID__
  menutext << _T("\t") << shortcut;
#endif
  return menutext;
}

void MyFrame::BuildMenuBar(void) {
  /*
   * Menu Bar - add or remove it if necessary, and update the state of the menu
   * items
   */
#ifdef __WXOSX__
  bool showMenuBar = true;  // the menu bar is always visible in OS X
#else
  bool showMenuBar = g_bShowMenuBar;  // get visibility from options

  if (!showMenuBar &&
      g_bTempShowMenuBar)  // allows pressing alt to temporarily show
    showMenuBar = true;
#endif

  if (showMenuBar) {
    //  Menu bar has some dependencies on S52 PLIB, so be sure it is loaded.
    LoadS57();

    if (!m_pMenuBar) {  // add the menu bar if it is enabled
      m_pMenuBar = new wxMenuBar();
      RegisterGlobalMenuItems();
      SetMenuBar(m_pMenuBar);  // must be after RegisterGlobalMenuItems for wx
                               // to populate the OS X App Menu correctly
    }

    UpdateGlobalMenuItems();  // update the state of the menu items (checkmarks
                              // etc)
  } else {
    if (m_pMenuBar) {  // remove the menu bar if it is disabled
      SetMenuBar(NULL);
      m_pMenuBar->Destroy();
      m_pMenuBar = NULL;
    }
  }
}

void MyFrame::RegisterGlobalMenuItems() {
  if (!m_pMenuBar) return;  // if there isn't a menu bar

  wxMenu *nav_menu = new wxMenu();
  nav_menu->AppendCheckItem(ID_MENU_NAV_FOLLOW,
                            _menuText(_("Auto Follow"), _T("Ctrl-A")));
  nav_menu->AppendCheckItem(ID_MENU_NAV_TRACK, _("Enable Tracking"));
  nav_menu->AppendSeparator();
  nav_menu->AppendRadioItem(ID_MENU_CHART_NORTHUP, _("North Up Mode"));
  nav_menu->AppendRadioItem(ID_MENU_CHART_COGUP, _("Course Up Mode"));
  nav_menu->AppendRadioItem(ID_MENU_CHART_HEADUP, _("Head Up Mode"));
  nav_menu->AppendSeparator();
#ifndef __WXOSX__
  nav_menu->Append(ID_MENU_ZOOM_IN, _menuText(_("Zoom In"), _T("+")));
  nav_menu->Append(ID_MENU_ZOOM_OUT, _menuText(_("Zoom Out"), _T("-")));
#else
  nav_menu->Append(ID_MENU_ZOOM_IN, _menuText(_("Zoom In"), _T("Alt-+")));
  nav_menu->Append(ID_MENU_ZOOM_OUT, _menuText(_("Zoom Out"), _T("Alt--")));
#endif
  nav_menu->AppendSeparator();
  nav_menu->Append(ID_MENU_SCALE_IN,
                   _menuText(_("Larger Scale Chart"), _T("Ctrl-Left")));
  nav_menu->Append(ID_MENU_SCALE_OUT,
                   _menuText(_("Smaller Scale Chart"), _T("Ctrl-Right")));
#ifndef __WXOSX__
  nav_menu->AppendSeparator();
  nav_menu->Append(ID_MENU_OQUIT, _menuText(_("Exit OpenCPN"), _T("Ctrl-Q")));
#endif
  m_pMenuBar->Append(nav_menu, _("&Navigate"));

  wxMenu *view_menu = new wxMenu();
#ifndef __WXOSX__
  view_menu->AppendCheckItem(ID_MENU_CHART_QUILTING,
                             _menuText(_("Enable Chart Quilting"), _T("Q")));
  view_menu->AppendCheckItem(ID_MENU_CHART_OUTLINES,
                             _menuText(_("Show Chart Outlines"), _T("O")));
#else
  view_menu->AppendCheckItem(
      ID_MENU_CHART_QUILTING,
      _menuText(_("Enable Chart Quilting"), _T("Alt-Q")));
  view_menu->AppendCheckItem(ID_MENU_CHART_OUTLINES,
                             _menuText(_("Show Chart Outlines"), _T("Alt-O")));
#endif
  view_menu->AppendCheckItem(ID_MENU_UI_CHARTBAR,
                             _menuText(_("Show Chart Bar"), _T("Ctrl-B")));

  view_menu->AppendSeparator();
#ifndef __WXOSX__
  view_menu->AppendCheckItem(ID_MENU_ENC_TEXT,
                             _menuText(_("Show ENC text"), _T("T")));
  view_menu->AppendCheckItem(ID_MENU_ENC_LIGHTS,
                             _menuText(_("Show ENC Lights"), _T("L")));
  view_menu->AppendCheckItem(ID_MENU_ENC_SOUNDINGS,
                             _menuText(_("Show ENC Soundings"), _T("S")));
  view_menu->AppendCheckItem(ID_MENU_ENC_ANCHOR,
                             _menuText(_("Show ENC Anchoring Info"), _T("A")));
  view_menu->AppendCheckItem(ID_MENU_ENC_DATA_QUALITY,
                             _menuText(_("Show ENC Data Quality"), _T("U")));
  view_menu->AppendCheckItem(ID_MENU_SHOW_NAVOBJECTS,
                             _menuText(_("Show Navobjects"), _T("V")));
#else
  view_menu->AppendCheckItem(ID_MENU_ENC_TEXT,
                             _menuText(_("Show ENC text"), _T("Alt-T")));
  view_menu->AppendCheckItem(ID_MENU_ENC_LIGHTS,
                             _menuText(_("Show ENC Lights"), _T("Alt-L")));
  view_menu->AppendCheckItem(ID_MENU_ENC_SOUNDINGS,
                             _menuText(_("Show ENC Soundings"), _T("Alt-S")));
  view_menu->AppendCheckItem(
      ID_MENU_ENC_ANCHOR, _menuText(_("Show ENC Anchoring Info"), _T("Alt-A")));
  view_menu->AppendCheckItem(
      ID_MENU_ENC_DATA_QUALITY,
      _menuText(_("Show ENC Data Quality"), _T("Alt-U")));
  view_menu->AppendCheckItem(ID_MENU_SHOW_NAVOBJECTS,
                             _menuText(_("Show Navobjects"), _T("Alt-V")));
#endif
  view_menu->AppendSeparator();
  view_menu->AppendCheckItem(ID_MENU_SHOW_TIDES, _("Show Tides"));
  view_menu->AppendCheckItem(ID_MENU_SHOW_CURRENTS, _("Show Currents"));
  view_menu->AppendSeparator();
#ifndef __WXOSX__
  view_menu->Append(ID_MENU_UI_COLSCHEME,
                    _menuText(_("Change Color Scheme"), _T("C")));
#else
  view_menu->Append(ID_MENU_UI_COLSCHEME,
                    _menuText(_("Change Color Scheme"), _T("Alt-C")));
#endif

  view_menu->AppendSeparator();
#ifndef __WXOSX__
  view_menu->Append(ID_MENU_UI_FULLSCREEN,
                    _menuText(_("Toggle Full Screen"), _T("F11")));
#endif
  m_pMenuBar->Append(view_menu, _("&View"));

  wxMenu *ais_menu = new wxMenu();
  ais_menu->AppendCheckItem(ID_MENU_AIS_TARGETS, _("Show AIS Targets"));
  ais_menu->AppendCheckItem(ID_MENU_AIS_SCALED_TARGETS,
                            _("Attenuate less critical AIS targets"));
  ais_menu->AppendSeparator();
  ais_menu->AppendCheckItem(ID_MENU_AIS_MOORED_TARGETS,
                            _("Hide Moored AIS Targets"));
  ais_menu->AppendCheckItem(ID_MENU_AIS_TRACKS, _("Show AIS Target Tracks"));
  ais_menu->AppendCheckItem(ID_MENU_AIS_CPADIALOG, _("Show CPA Alert Dialogs"));
  ais_menu->AppendCheckItem(ID_MENU_AIS_CPASOUND, _("Sound CPA Alarms"));

#ifndef __WXOSX__
  ais_menu->AppendCheckItem(ID_MENU_AIS_CPAWARNING,
                    _menuText(_("Show CPA Warnings"), _T("W")));
#else
  ais_menu->AppendCheckItem(ID_MENU_AIS_CPAWARNING,
                    _menuText(_("Show CPA Warnings"), _T("Alt-W")));
#endif

  ais_menu->AppendSeparator();
  ais_menu->Append(ID_MENU_AIS_TARGETLIST, _("AIS target list") + _T("..."));
  m_pMenuBar->Append(ais_menu, _("&AIS"));

  wxMenu *tools_menu = new wxMenu();
#ifndef __WXOSX__
  tools_menu->Append(ID_MENU_TOOL_MEASURE,
                     _menuText(_("Measure Distance"), _T("M")));
#else
  tools_menu->Append(ID_MENU_TOOL_MEASURE,
                     _menuText(_("Measure Distance"), _T("Alt-M")));
#endif

  tools_menu->AppendSeparator();
  tools_menu->Append(ID_MENU_ROUTE_MANAGER, _("Route && Mark Manager..."));
  tools_menu->Append(ID_MENU_ROUTE_NEW,
                     _menuText(_("Create Route"), _T("Ctrl-R")));
  tools_menu->AppendSeparator();
  tools_menu->Append(ID_MENU_MARK_BOAT,
                     _menuText(_("Drop Mark at Boat"), _T("Ctrl-O")));
  tools_menu->Append(ID_MENU_MARK_CURSOR,
                     _menuText(_("Drop Mark at Cursor"), _T("Ctrl-M")));
  tools_menu->AppendSeparator();
#ifdef __WXOSX__
  tools_menu->Append(
      ID_MENU_MARK_MOB,
      _menuText(
          _("Drop MOB Marker"),
          _T("RawCtrl-Space")));  // NOTE Cmd+Space is reserved for Spotlight
  tools_menu->AppendSeparator();
  tools_menu->Append(wxID_PREFERENCES,
                     _menuText(_("Preferences") + _T("..."), _T("Ctrl-,")));
#else
  tools_menu->Append(ID_MENU_MARK_MOB,
                     _menuText(_("Drop MOB Marker"), _T("Ctrl-Space")));
  tools_menu->AppendSeparator();
  tools_menu->Append(wxID_PREFERENCES,
                     _menuText(_("Options") + _T("..."), _T("Ctrl-,")));
#endif
  m_pMenuBar->Append(tools_menu, _("&Tools"));

#ifdef __WXOSX__
  wxMenu *window_menu = new wxMenu();
  m_pMenuBar->Append(window_menu, _("&Window"));
#endif

  wxMenu *help_menu = new wxMenu();
  help_menu->Append(wxID_ABOUT, _("About OpenCPN"));
  help_menu->Append(wxID_HELP, _("OpenCPN Help"));
  m_pMenuBar->Append(help_menu, _("&Help"));

  // Set initial values for menu check items and radio items
  UpdateGlobalMenuItems();
}

void MyFrame::UpdateGlobalMenuItems() {
  if (!m_pMenuBar) return;  // if there isn't a menu bar

  m_pMenuBar->FindItem(ID_MENU_NAV_FOLLOW)
      ->Check(GetPrimaryCanvas()->m_bFollow);
  m_pMenuBar->FindItem(ID_MENU_CHART_NORTHUP)
      ->Check(GetPrimaryCanvas()->GetUpMode() == NORTH_UP_MODE);
  m_pMenuBar->FindItem(ID_MENU_CHART_COGUP)
      ->Check(GetPrimaryCanvas()->GetUpMode() == COURSE_UP_MODE);
  m_pMenuBar->FindItem(ID_MENU_CHART_HEADUP)
      ->Check(GetPrimaryCanvas()->GetUpMode() == HEAD_UP_MODE);
  m_pMenuBar->FindItem(ID_MENU_NAV_TRACK)->Check(g_bTrackActive);
  m_pMenuBar->FindItem(ID_MENU_CHART_OUTLINES)->Check(g_bShowOutlines);
  m_pMenuBar->FindItem(ID_MENU_CHART_QUILTING)->Check(g_bQuiltEnable);
  m_pMenuBar->FindItem(ID_MENU_UI_CHARTBAR)->Check(g_bShowChartBar);
  m_pMenuBar->FindItem(ID_MENU_AIS_TARGETS)->Check(g_bShowAIS);
  m_pMenuBar->FindItem(ID_MENU_AIS_MOORED_TARGETS)->Check(g_bHideMoored);
  m_pMenuBar->FindItem(ID_MENU_AIS_SCALED_TARGETS)->Check(g_bShowScaled);
  m_pMenuBar->FindItem(ID_MENU_AIS_SCALED_TARGETS)->Enable(g_bAllowShowScaled);
  m_pMenuBar->FindItem(ID_MENU_AIS_TRACKS)->Check(g_bAISShowTracks);
  m_pMenuBar->FindItem(ID_MENU_AIS_CPADIALOG)->Check(g_bAIS_CPA_Alert);
  m_pMenuBar->FindItem(ID_MENU_AIS_CPASOUND)->Check(g_bAIS_CPA_Alert_Audio);
  m_pMenuBar->FindItem(ID_MENU_AIS_CPAWARNING)->Check(g_bCPAWarn);
  m_pMenuBar->FindItem(ID_MENU_SHOW_NAVOBJECTS)
      ->Check(GetPrimaryCanvas()->m_bShowNavobjects);

  if (ps52plib) {
    m_pMenuBar->FindItem(ID_MENU_ENC_TEXT)->Check(ps52plib->GetShowS57Text());
    m_pMenuBar->FindItem(ID_MENU_ENC_SOUNDINGS)
        ->Check(ps52plib->GetShowSoundings());

    bool light_state = false;
    if (ps52plib) {
      for (unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount();
           iPtr++) {
        OBJLElement *pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(iPtr));
        if (!strncmp(pOLE->OBJLName, "LIGHTS", 6)) {
          light_state = (pOLE->nViz == 1);
          break;
        }
      }
    }
    m_pMenuBar->FindItem(ID_MENU_ENC_LIGHTS)
        ->Check((!ps52plib->IsObjNoshow("LIGHTS")) && light_state);

    // Menu "Anchor Info" entry is only accessible in "All" or "User Standard"
    // categories
    DisCat nset = ps52plib->GetDisplayCategory();
    if ((nset == MARINERS_STANDARD) || (nset == OTHER)) {
      m_pMenuBar->FindItem(ID_MENU_ENC_ANCHOR)
          ->Check(!ps52plib->IsObjNoshow("SBDARE"));
      m_pMenuBar->Enable(ID_MENU_ENC_ANCHOR, true);
      m_pMenuBar->FindItem(ID_MENU_ENC_DATA_QUALITY)
          ->Check(!ps52plib->IsObjNoshow("M_QUAL"));
      m_pMenuBar->Enable(ID_MENU_ENC_DATA_QUALITY, true);
    } else {
      m_pMenuBar->FindItem(ID_MENU_ENC_ANCHOR)->Check(false);
      m_pMenuBar->Enable(ID_MENU_ENC_ANCHOR, false);
      m_pMenuBar->Enable(ID_MENU_ENC_DATA_QUALITY, false);
    }
  }
}

void MyFrame::UpdateGlobalMenuItems(ChartCanvas *cc) {
  if (!m_pMenuBar) return;  // if there isn't a menu bar

  m_pMenuBar->FindItem(ID_MENU_NAV_FOLLOW)->Check(cc->m_bFollow);

  if (cc->GetUpMode() == NORTH_UP_MODE)
    m_pMenuBar->FindItem(ID_MENU_CHART_NORTHUP)->Check(true);
  else if (cc->GetUpMode() == COURSE_UP_MODE)
    m_pMenuBar->FindItem(ID_MENU_CHART_COGUP)->Check(true);
  else
    m_pMenuBar->FindItem(ID_MENU_CHART_HEADUP)->Check(true);

  m_pMenuBar->FindItem(ID_MENU_NAV_TRACK)->Check(g_bTrackActive);
  m_pMenuBar->FindItem(ID_MENU_CHART_OUTLINES)->Check(cc->GetShowOutlines());
  m_pMenuBar->FindItem(ID_MENU_CHART_QUILTING)->Check(cc->GetQuiltMode());
  m_pMenuBar->FindItem(ID_MENU_UI_CHARTBAR)->Check(cc->GetShowChartbar());
  m_pMenuBar->FindItem(ID_MENU_AIS_TARGETS)->Check(cc->GetShowAIS());
  m_pMenuBar->FindItem(ID_MENU_AIS_MOORED_TARGETS)->Check(g_bHideMoored);
  m_pMenuBar->FindItem(ID_MENU_AIS_SCALED_TARGETS)->Check(cc->GetAttenAIS());
  m_pMenuBar->FindItem(ID_MENU_AIS_SCALED_TARGETS)->Enable(g_bAllowShowScaled);
  m_pMenuBar->FindItem(ID_MENU_AIS_TRACKS)->Check(g_bAISShowTracks);
  m_pMenuBar->FindItem(ID_MENU_AIS_CPADIALOG)->Check(g_bAIS_CPA_Alert);
  m_pMenuBar->FindItem(ID_MENU_AIS_CPASOUND)->Check(g_bAIS_CPA_Alert_Audio);
  m_pMenuBar->FindItem(ID_MENU_AIS_CPAWARNING)->Check(g_bCPAWarn);
  m_pMenuBar->FindItem(ID_MENU_SHOW_NAVOBJECTS)->Check(cc->m_bShowNavobjects);
  m_pMenuBar->FindItem(ID_MENU_SHOW_TIDES)->Check(cc->GetbShowTide());
  m_pMenuBar->FindItem(ID_MENU_SHOW_CURRENTS)->Check(cc->GetbShowCurrent());

  if (ps52plib) {
    m_pMenuBar->FindItem(ID_MENU_ENC_TEXT)->Check(cc->GetShowENCText());
    m_pMenuBar->FindItem(ID_MENU_ENC_SOUNDINGS)->Check(cc->GetShowENCDepth());

    if (ps52plib) {
      for (unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount();
           iPtr++) {
        OBJLElement *pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(iPtr));
        if (!strncmp(pOLE->OBJLName, "LIGHTS", 6)) {
          break;
        }
      }
    }
    m_pMenuBar->FindItem(ID_MENU_ENC_LIGHTS)->Check(cc->GetShowENCLights());

    // Menu "Anchor Info" entry is only accessible in "All" or "UserStandard"
    // categories
    DisCat nset = (DisCat)cc->GetENCDisplayCategory();
    if ((nset == MARINERS_STANDARD) || (nset == OTHER)) {
      m_pMenuBar->FindItem(ID_MENU_ENC_ANCHOR)->Check(cc->GetShowENCAnchor());
      m_pMenuBar->Enable(ID_MENU_ENC_ANCHOR, true);
      m_pMenuBar->FindItem(ID_MENU_ENC_DATA_QUALITY)
          ->Check(cc->GetShowENCDataQual());
      m_pMenuBar->Enable(ID_MENU_ENC_DATA_QUALITY, true);
    } else {
      m_pMenuBar->FindItem(ID_MENU_ENC_ANCHOR)->Check(false);
      m_pMenuBar->Enable(ID_MENU_ENC_ANCHOR, false);
      m_pMenuBar->Enable(ID_MENU_ENC_DATA_QUALITY, false);
    }
  }
}

void MyFrame::JumpToPosition(ChartCanvas *cc, double lat, double lon,
                             double scale) {
  if (lon > 180.0) lon -= 360.0;
  // XXX is vLat/vLon always equal to cc m_vLat, m_vLon after SetViewPoint? Does
  // it matter?
  vLat = lat;
  vLon = lon;
  cc->JumpToPosition(lat, lon, scale);
}

void MyFrame::UpdateCanvasConfigDescriptors() {
  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasConfigArray.GetCount(); i++) {
    canvasConfig *cc = g_canvasConfigArray.Item(i);
    if (cc) {
      ChartCanvas *chart = cc->canvas;
      if (chart) {
        cc->iLat = chart->GetVP().clat;
        cc->iLon = chart->GetVP().clon;
        cc->iRotation = chart->GetVP().rotation;
        cc->iScale = chart->GetVP().view_scale_ppm;
        cc->DBindex = chart->GetQuiltReferenceChartIndex();
        cc->GroupID = chart->m_groupIndex;
        cc->canvasSize = chart->GetSize();

        cc->bQuilt = chart->GetQuiltMode();
        cc->bShowTides = chart->GetbShowTide();
        cc->bShowCurrents = chart->GetbShowCurrent();
        cc->bShowGrid = chart->GetShowGrid();
        cc->bShowOutlines = chart->GetShowOutlines();
        cc->bShowDepthUnits = chart->GetShowDepthUnits();

        cc->bFollow = chart->m_bFollow;
        cc->bLookahead = chart->m_bLookAhead;
        cc->bCourseUp = false;
        cc->bHeadUp = false;;
        int upmode = chart->GetUpMode();
        if (upmode == COURSE_UP_MODE)
          cc->bCourseUp = true;
        else if (upmode == HEAD_UP_MODE)
          cc->bHeadUp = true;
      }
    }
  }
}

void MyFrame::CenterView(ChartCanvas *cc, const LLBBox &RBBox, int nWidth, int nHeight) {
  if (!RBBox.GetValid()) return;
  // Calculate bbox center
  double clat = (RBBox.GetMinLat() + RBBox.GetMaxLat()) / 2;
  double clon = (RBBox.GetMinLon() + RBBox.GetMaxLon()) / 2;
  double ppm;  // final ppm scale to use

  if (RBBox.GetMinLat() == RBBox.GetMaxLat() &&
      RBBox.GetMinLon() == RBBox.GetMaxLon()) {
    // only one point, (should be a box?)
    ppm = cc->GetVPScale();
  } else {
    // Calculate ppm
    double rw, rh;  // route width, height
    int ww, wh;     // chart window width, height
    // route bbox width in nm
    DistanceBearingMercator(RBBox.GetMinLat(), RBBox.GetMinLon(),
                            RBBox.GetMinLat(), RBBox.GetMaxLon(), NULL, &rw);
    // route bbox height in nm
    DistanceBearingMercator(RBBox.GetMinLat(), RBBox.GetMinLon(),
                            RBBox.GetMaxLat(), RBBox.GetMinLon(), NULL, &rh);

    cc->GetSize(&ww, &wh);
	/*ww = nWidth;
	wh = nHeight;*/

    ppm = wxMin(ww / (rw * 1852), wh / (rh * 1852)) * (100 - fabs(clat)) / 90;

    ppm = wxMin(ppm, 1.0);
  }

  JumpToPosition(cc, clat, clon, ppm);
}

int MyFrame::DoOptionsDialog() {
  if (g_boptionsactive) return 0;

  g_boptionsactive = true;
  g_last_ChartScaleFactor = g_ChartScaleFactor;

  if (NULL == g_options) {
    int sx, sy;
    pConfig->SetPath("/Settings");
    pConfig->Read("OptionsSizeX", &sx, -1);
    pConfig->Read("OptionsSizeY", &sy, -1);

    wxWindow *optionsParent = this;
#ifdef __WXOSX__
    optionsParent = GetPrimaryCanvas();
#endif
    g_options = new options(optionsParent, -1, _("Options"), wxPoint(-1, -1), wxSize(sx, sy));
  }

  //    Set initial Chart Dir
  g_options->SetInitChartDir(*pInit_Chart_Dir);

  //      Pass two working pointers for Chart Dir Dialog
  g_options->SetCurrentDirList(ChartData->GetChartDirArray());
  ArrayOfCDI *pWorkDirArray = new ArrayOfCDI;
  g_options->SetWorkDirListPtr(pWorkDirArray);

  //      Pass a ptr to MyConfig, for updates
  g_options->SetConfigPtr(pConfig);

  g_options->SetInitialSettings();

  bPrevQuilt = g_bQuiltEnable;
  bPrevFullScreenQuilt = g_bFullScreenQuilt;
  bPrevOGL = g_bopengl;

  prev_locale = g_locale;

  bool b_sub = false;

#if defined(__WXOSX__) || defined(__WXQT__)
  bool b_restoreAIS = false;
  if (g_pAISTargetList && g_pAISTargetList->IsShown()) {
    b_restoreAIS = true;
    g_pAISTargetList->Shutdown();
    g_pAISTargetList = NULL;
  }
#endif

#ifdef __WXOSX__
  // ..For each canvas...
  
  SubmergeAllCanvasToolbars();  
#endif

  g_options->SetInitialPage(options_lastPage, options_subpage);

#ifndef __OCPN__ANDROID__  //    if(!g_bresponsive){
  g_options->lastWindowPos = options_lastWindowPos;
  if (options_lastWindowPos != wxPoint(0, 0)) {
    g_options->Move(options_lastWindowPos);
    g_options->SetSize(options_lastWindowSize);
  } else {
    g_options->CenterOnScreen();
  }
  if (options_lastWindowSize != wxSize(0, 0)) {
    g_options->SetSize(options_lastWindowSize);
  }

  // Correct some fault in Options dialog layout logic on GTK3 by forcing a
  // re-layout to new slightly reduced size.
#ifdef __WXGTK3__
  if (options_lastWindowSize != wxSize(0, 0))
    g_options->SetSize(options_lastWindowSize.x - 1, options_lastWindowSize.y);
#endif

#endif

  unsigned int last_canvasConfig = g_canvasConfig;
  wxSize cc1SizeBefore;
  if (g_canvasConfig > 0) {
    canvasConfig *cc = g_canvasConfigArray.Item(0);
    if (cc) cc1SizeBefore = g_canvasArray.Item(0)->GetSize();
  }

  //  Capture the full path names and VPScale of charts currently shown in all
  //  canvases
  wxArrayString pathArray;
  double restoreScale[4];

  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      wxString chart_file_name;
      if (cc->GetQuiltMode()) {
        int dbi = cc->GetQuiltRefChartdbIndex();
        chart_file_name = ChartData->GetDBChartFileName(dbi);
      } else {
        if (cc->m_singleChart)
          chart_file_name = cc->m_singleChart->GetFullPath();
      }

      pathArray.Add(chart_file_name);
      restoreScale[i] = cc->GetVPScale();
    }
  }

  int rr = g_options->ShowModal();

  options_lastPage = g_options->lastPage;
  options_subpage = g_options->lastSubPage;

  options_lastWindowPos = g_options->lastWindowPos;
  options_lastWindowSize = g_options->lastWindowSize;

  if (1) {  // always surface toolbar, and restart the timer if needed
    GetPrimaryCanvas()->SetFocus();
  }

#ifdef __WXGTK__
  Raise();  // I dunno why...
#endif

  bool ret_val = false;
  rr = g_options->GetReturnCode();

  if (g_last_ChartScaleFactor != g_ChartScaleFactor) rr |= S52_CHANGED;

  bool b_refresh = true;

#if 0
    bool ccRightSizeChanged = false;
    if( g_canvasConfig > 0 ){
        canvasConfig *cc = g_canvasConfigArray.Item(0);
        if(cc ){
            wxSize cc1Size = cc->canvasSize;
            if(cc1Size.x != cc1SizeBefore.x)
                ccRightSizeChanged = true;
        }
    }
#endif

  if ((g_canvasConfig != last_canvasConfig) || (rr & GL_CHANGED)) {
    UpdateCanvasConfigDescriptors();

    if ((g_canvasConfig > 0) && (last_canvasConfig == 0))
      CreateCanvasLayout(true);
    else
      CreateCanvasLayout();

    SendSizeEvent();

    g_pauimgr->Update();

    // We need a yield() here to pick up the size event
    // so that the toolbars will be sized correctly
    wxYield();

    rr |= GENERIC_CHANGED;

    if (g_bopengl)  // Force mark/waypoint icon reload
      rr |= S52_CHANGED;

    b_refresh = true;
  }

  if (rr & CONFIG_CHANGED) {
    // Apply the changed canvas configs to each canvas
    // ..For each canvas...
    for (unsigned int i = 0; i < g_canvasConfigArray.GetCount(); i++) {
      canvasConfig *cc = g_canvasConfigArray.Item(i);
      if (cc) {
        ChartCanvas *chartCanvas = cc->canvas;
        if (chartCanvas) {
          chartCanvas->ApplyCanvasConfig(cc);
        }
      }
    }
  }

  if (rr) {
    bDBUpdateInProgress = true;
    b_refresh |= ProcessOptionsDialog(rr, g_options->GetWorkDirListPtr());
    ChartData->GetChartDirArray() =
        *(g_options->GetWorkDirListPtr());  // Perform a deep copy back to main
                                            // database.
    bDBUpdateInProgress = false;
    ret_val = true;
  }

  delete pWorkDirArray;

  DoChartUpdate();

  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {      
      cc->UpdateCanvasControlBar();
    }
  }
  
  RequestNewToolbars();

  //  Rebuild cursors
  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      cc->RebuildCursors();
    }
  }

  // Change of master toolbar scale?
  bool b_masterScaleChange = false;  

  if ((rr & TOOLBAR_CHANGED) || b_masterScaleChange)
    RequestNewMasterToolbar(true);

  bool bMuiChange = false;

  // Inform the canvases
  if (b_masterScaleChange || bMuiChange) {
    // ..For each canvas...
    for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
      ChartCanvas *cc = g_canvasArray.Item(i);
      if (cc) {
        cc->ProcessNewGUIScale();
      }
    }
  }
    
#if defined(__WXOSX__) || defined(__WXQT__)
  if (b_restoreAIS) {
    g_pAISTargetList = new AISTargetListDialog(this, g_pauimgr, g_pAIS);
    g_pAISTargetList->UpdateAISTargetList();
  }
#endif
  
  //  Force reload of options dialog to pick up font changes or other major
  //  layout changes
  if ((rr & FONT_CHANGED) || (rr & NEED_NEW_OPTIONS)) {
    delete g_options;
    g_options = NULL;
    g_pOptions = NULL;
  }
#if wxUSE_XLOCALE
  if (rr & LOCALE_CHANGED) {
    g_Platform->ChangeLocale(g_locale, plocale_def_lang, &plocale_def_lang);
    ApplyLocale();
  }
#endif

  // If needed, refresh each canvas,
  // trying to reload the previously displayed chart by name as saved in
  // pathArray Also, restoring the previous chart VPScale, if possible
  if (b_refresh) {
    // ..For each canvas...
    for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
      ChartCanvas *cc = g_canvasArray.Item(i);
      if (cc) {
        int index_hint = -1;
        if (i < pathArray.GetCount())
          index_hint = ChartData->FinddbIndex(pathArray.Item(i));
        cc->canvasChartsRefresh(index_hint);
        if (index_hint != -1) cc->SetVPScale(restoreScale[i]);
      }
    }
  }

  g_boptionsactive = false;

  //  If we had a config chamge, then schedule a re-entry to the settings dialog
  if (rr & CONFIG_CHANGED) {
    options_subpage = 3;  // Back to the "templates" page
    ScheduleSettingsDialog();
  } else
    options_subpage = 0;

  return ret_val;
}

bool MyFrame::ProcessOptionsDialog(int rr, ArrayOfCDI *pNewDirArray) {
  bool b_need_refresh = false;  // Do we need a full reload?

  if ((rr & VISIT_CHARTS) &&
      ((rr & CHANGE_CHARTS) || (rr & FORCE_UPDATE) || (rr & SCAN_UPDATE))) {
    if (pNewDirArray) {
      b_need_refresh = true;
    }
  }

  if (rr & STYLE_CHANGED) {
	  printf("Please restart OpenCPN to activate language or style changes.\n");    
  }

  bool b_groupchange = false;
  if (((rr & VISIT_CHARTS) &&
       ((rr & CHANGE_CHARTS) || (rr & FORCE_UPDATE) || (rr & SCAN_UPDATE))) ||
      (rr & GROUPS_CHANGED)) {
    b_groupchange = ScrubGroupArray();
    ChartData->ApplyGroupArray(g_pGroupArray);
    RefreshGroupIndices();
  }

  if (rr & GROUPS_CHANGED || b_groupchange) {
    pConfig->DestroyConfigGroups();
    pConfig->CreateConfigGroups(g_pGroupArray);
  }

  if (rr & TIDES_CHANGED) {
    LoadHarmonics();
  }

  pConfig->UpdateSettings();
#if 0
//TODO Not need with per-canvas CourseUp
    if( g_bCourseUp ) {
        //    Stuff the COGAvg table in case COGUp is selected
        double stuff = NAN;
        if( !std::isnan(gCog) ) stuff = gCog;
        if( g_COGAvgSec > 0 ) {
            for( int i = 0; i < g_COGAvgSec; i++ )
                COGTable[i] = stuff;
        }

        g_COGAvg = stuff;

        DoCOGSet();
    }
#endif

  //    Stuff the Filter tables  
  SetChartUpdatePeriod();  // Pick up changes to skew compensator

  if (rr & GL_CHANGED) {
    //    Refresh the chart display, after flushing cache.
    //      This will allow all charts to recognise new OpenGL configuration, if
    //      any
    b_need_refresh = true;
  }

  if (rr & S52_CHANGED) {
    b_need_refresh = true;
  }

#ifdef ocpnUSE_GL
  if (rr & REBUILD_RASTER_CACHE) {
    if (g_glTextureManager) {
      GetPrimaryCanvas()->Disable();
      g_glTextureManager->BuildCompressedCache();
      GetPrimaryCanvas()->Enable();
    }
  }
#endif

  if ((g_config_display_size_mm > 0)  && g_config_display_size_manual){
    g_display_size_mm = g_config_display_size_mm;
  } else {
    g_display_size_mm = wxMax(50, g_Platform->GetDisplaySizeMM());
  }

  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) cc->SetDisplaySizeMM(g_display_size_mm);
  }

  // update S52 PLIB scale factors
  if (ps52plib){
    ps52plib->SetScaleFactorExp(g_Platform->GetChartScaleFactorExp(g_ChartScaleFactor));
    ps52plib-> SetScaleFactorZoomMod(g_chart_zoom_modifier_vector);
  }

  // Apply any needed updates to each canvas
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) cc->ApplyGlobalSettings();
  }

  //    Do a full Refresh, trying to open the last open chart
// TODO  This got move up a level.  FIX ANDROID codepath
#if 0
    if(b_need_refresh){
        int index_hint = ChartData->FinddbIndex( chart_file_name );
        if( -1 == index_hint )
            b_autofind = true;
        ChartsRefresh( );
    }
#endif

  //  The zoom-scale factor may have changed
  //  so, trigger a recalculation of the reference chart

  bool ztc = g_bEnableZoomToCursor;  // record the present state
  g_bEnableZoomToCursor =
      false;  // since we don't want to pan to an unknown cursor position

  //  This is needed to recognise changes in zoom-scale factors
  GetPrimaryCanvas()->DoZoomCanvas(1.0001);

  g_bEnableZoomToCursor = ztc;

  g_last_ChartScaleFactor = g_ChartScaleFactor;

  return b_need_refresh;
}

wxString MyFrame::GetGroupName(int igroup) {
  ChartGroup *pGroup = g_pGroupArray->Item(igroup - 1);
  return pGroup->m_group_name;
}

bool MyFrame::CheckGroup(int igroup) {
  if (igroup == 0) return true;  // "all charts" is always OK

  ChartGroup *pGroup = g_pGroupArray->Item(igroup - 1);

  if (!pGroup->m_element_array.size())  //  truly empty group is OK
    return true;

  for (const auto &elem : pGroup->m_element_array) {
    for (unsigned int ic = 0;
         ic < (unsigned int)ChartData->GetChartTableEntries(); ic++) {
      ChartTableEntry *pcte = ChartData->GetpChartTableEntry(ic);
      wxString chart_full_path(pcte->GetpFullPath(), wxConvUTF8);

      if (chart_full_path.StartsWith(elem.m_element_name)) return true;
    }
  }

  return false;  // this group is empty
}

bool MyFrame::ScrubGroupArray() {
  //    For each group,
  //    make sure that each group element (dir or chart) references at least
  //    oneitem in the database. If not, remove the element.

  bool b_change = false;
  unsigned int igroup = 0;
  while (igroup < g_pGroupArray->GetCount()) {
    bool b_chart_in_element = false;
    ChartGroup *pGroup = g_pGroupArray->Item(igroup);

    for (unsigned int j = 0; j < pGroup->m_element_array.size(); j++) {
      const wxString &element_root = pGroup->m_element_array[j].m_element_name;

      for (unsigned int ic = 0;
           ic < (unsigned int)ChartData->GetChartTableEntries(); ic++) {
        ChartTableEntry *pcte = ChartData->GetpChartTableEntry(ic);
        wxString chart_full_path = pcte->GetFullSystemPath();

        if (chart_full_path.StartsWith(element_root)) {
          b_chart_in_element = true;
          break;
        }
      }

      // Explicit check to avoid removing a group containing only GSHHS
      if (!b_chart_in_element) {
        wxString test_string = _T("GSHH");
        if (element_root.Upper().Contains(test_string))
          b_chart_in_element = true;
      }

      if (!b_chart_in_element)  // delete the element
      {
        pGroup->m_element_array.erase(pGroup->m_element_array.begin() + j);
        j--;
        b_change = true;
      }
    }

    igroup++;  // next group
  }

  return b_change;
}

void MyFrame::RefreshCanvasOther(ChartCanvas *ccThis) {
  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc && (cc != ccThis)) cc->Refresh();
  }
}

// Flav: This method reloads all charts for convenience
void MyFrame::ChartsRefresh() {
  if (!ChartData) return;

  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      int currentIndex = cc->GetpCurrentStack()->GetCurrentEntrydbIndex();
      if (cc->GetQuiltMode()) {
        currentIndex = cc->GetQuiltReferenceChartIndex();
      }
      cc->canvasChartsRefresh(currentIndex);
    }
  }
}

void MyFrame::InvalidateAllQuilts() {
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      cc->InvalidateQuilt();
      cc->SetQuiltRefChart(-1);
      cc->m_singleChart = NULL;
    }
  }
}

void MyFrame::ToggleQuiltMode(ChartCanvas *cc) {
  if (cc) {
    cc->ToggleCanvasQuiltMode();
#if 0
        bool cur_mode = cc->GetQuiltMode();

        if( !cc->GetQuiltMode() )
            cc->SetQuiltMode( true );
        else
            if( cc->GetQuiltMode() ) {
                cc->SetQuiltMode( false );
                g_sticky_chart = cc->GetQuiltReferenceChartIndex();
            }


        if( cur_mode != cc->GetQuiltMode() ){
            //TODO >>SetupQuiltMode();
            DoChartUpdate();
            cc->InvalidateGL();
            Refresh();
        }
        g_bQuiltEnable = cc->GetQuiltMode();

        // Recycle the S52 PLIB so that vector charts will flush caches and re-render
        if(ps52plib)
            ps52plib->GenerateStateHash();
#endif
  }
}

void MyFrame::DoStackDown(ChartCanvas *cc) { DoStackDelta(cc, -1); }

void MyFrame::DoStackUp(ChartCanvas *cc) { DoStackDelta(cc, 1); }

void MyFrame::DoStackDelta(ChartCanvas *cc, int direction) {
  if (cc) {
    cc->DoCanvasStackDelta(direction);
  }
}

wxDEFINE_EVENT(EVT_BASIC_NAV_DATA, ObservedEvt);
wxDEFINE_EVENT(EVT_GPS_WATCHDOG, ObservedEvt);

void MyFrame::UpdateDB_Canvas() {
	ParseAllENC(NULL);
	g_Platform->SetFullscreen(g_bFullscreen);
	
	pConfig->LoadNavObjects();
	//    Re-enable anchor watches if set in config file      

	// Import Layer-wise any .gpx files from /layers directory
	wxString layerdir = g_Platform->GetPrivateDataDir();
	appendOSDirSlash(&layerdir);
	layerdir.Append(_T("layers"));

	if (wxDir::Exists(layerdir)) {
		wxString laymsg;
		laymsg.Printf(wxT("Getting .gpx layer files from: %s"),
			layerdir.c_str());
		wxLogMessage(laymsg);
		pConfig->LoadLayers(layerdir);
	}

	g_bDeferredInitDone = true;

	GetPrimaryCanvas()->SetFocus();
	g_focusCanvas = GetPrimaryCanvas();

	if (b_reloadForPlugins) {
		DoChartUpdate();
		ChartsRefresh();
	}

	printf("OnInitTimer...Finalize Canvases \n");

	for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
		ChartCanvas* cc = g_canvasArray.Item(i);
		if (cc) {
			cc->CheckGroupValid();
		}
	}

	SendSizeEvent();

	RefreshAllCanvas(true);
}

//    Manage the application memory footprint on a periodic schedule
void MyFrame::OnMemFootTimer(wxTimerEvent &event) {
  MemFootTimer.Stop();

  int memsize = GetApplicationMemoryUse();

  g_MemFootMB = 100;
  printf("Memsize: %d  \n", memsize);
  // The application memory usage has exceeded the target, so try to manage it
  // down....
  if (memsize > (g_MemFootMB * 1000)) {
    ChartCanvas *cc = GetPrimaryCanvas();
    if (ChartData && cc) {
      //    Get a local copy of the cache info
      wxArrayPtrVoid *pCache = ChartData->GetChartCache();
      unsigned int nCache = pCache->GetCount();
      CacheEntry *pcea = new CacheEntry[nCache];

      for (unsigned int i = 0; i < nCache; i++) {
        CacheEntry *pce = (CacheEntry *)(pCache->Item(i));
        pcea[i] = *pce;  // ChartBase *Ch = (ChartBase *)pce->pChart;
      }

      if (nCache > 1) {
        //    Bubble Sort the local cache entry array
        bool b_cont = true;
        while (b_cont) {
          b_cont = false;
          for (unsigned int i = 0; i < nCache - 1; i++) {
            if (pcea[i].RecentTime > pcea[i + 1].RecentTime) {
              CacheEntry tmp = pcea[i];
              pcea[i] = pcea[i + 1];
              pcea[i + 1] = tmp;
              b_cont = true;
              break;
            }
          }
        }

        //    Free up some chart cache entries until the memory footprint target
        //    is realized

        unsigned int idelete = 0;  // starting at top. which is oldest
        unsigned int idelete_max = pCache->GetCount();

        //    How many can be deleted?
        unsigned int minimum_cache = 1;
        if (cc->GetQuiltMode()) minimum_cache = cc->GetQuiltChartCount();

        while ((memsize > (g_MemFootMB * 1000)) &&
               (pCache->GetCount() > minimum_cache) &&
               (idelete < idelete_max)) {
          int memsizeb = memsize;

          ChartData->DeleteCacheChart((ChartBase *)pcea[idelete].pChart);
          idelete++;
          memsize = GetApplicationMemoryUse();
          printf("delete, before: %d  after: %d\n", memsizeb, memsize);
        }
      }

      delete[] pcea;
    }
  }

  MemFootTimer.Start(9000, wxTIMER_CONTINUOUS);
}

int ut_index;

void MyFrame::CheckToolbarPosition() {
#ifdef __WXMAC__
  // Manage Full Screen mode on Mac Mojave 10.14
  static bool bMaximized;

  if (IsMaximized() && !bMaximized) {
    bMaximized = true;    
  } else if (!IsMaximized() && bMaximized) {
    bMaximized = false;    
  }
#endif
}

void MyFrame::OnFrameTimer1(wxTimerEvent &event) {
  CheckToolbarPosition();

  if (!g_bPauseTest && (g_unit_test_1 || g_unit_test_2)) {
    //            if((0 == ut_index) && GetQuiltMode())
    //                  ToggleQuiltMode();

    // We use only one canvas for the unit tests, so far...
    ChartCanvas *cc = GetPrimaryCanvas();

    cc->m_bFollow = false;
    
    int ut_index_max = ((g_unit_test_1 > 0) ? (g_unit_test_1 - 1) : INT_MAX);

    if (ChartData) {
      if (cc->m_groupIndex > 0) {
        while (ut_index < ChartData->GetChartTableEntries() &&
               !ChartData->IsChartInGroup(ut_index, cc->m_groupIndex)) {
          ut_index++;
        }
      }
      if (ut_index < ChartData->GetChartTableEntries()) {
        // printf("%d / %d\n", ut_index, ChartData->GetChartTableEntries());
        const ChartTableEntry *cte = &ChartData->GetChartTableEntry(ut_index);

        double clat = (cte->GetLatMax() + cte->GetLatMin()) / 2;
        double clon = (cte->GetLonMax() + cte->GetLonMin()) / 2;

        vLat = clat;
        vLon = clon;

        cc->SetViewPoint(clat, clon);

        if (cc->GetQuiltMode()) {
          if (cc->IsChartQuiltableRef(ut_index))
            cc->SelectQuiltRefdbChart(ut_index);
        } else
          cc->SelectdbChart(ut_index);

        double ppm;  // final ppm scale to use
        if (g_unit_test_1) {
          ppm = cc->GetCanvasScaleFactor() / cte->GetScale();
          ppm /= 2;
        } else {
          double rw, rh;  // width, height
          int ww, wh;     // chart window width, height

          // width in nm
          DistanceBearingMercator(cte->GetLatMin(), cte->GetLonMin(),
                                  cte->GetLatMin(), cte->GetLonMax(), NULL,
                                  &rw);

          // height in nm
          DistanceBearingMercator(cte->GetLatMin(), cte->GetLonMin(),
                                  cte->GetLatMax(), cte->GetLonMin(), NULL,
                                  &rh);

          cc->GetSize(&ww, &wh);
          ppm = wxMin(ww / (rw * 1852), wh / (rh * 1852)) * (100 - fabs(clat)) /
                90;
          ppm = wxMin(ppm, 1.0);
        }
        cc->SetVPScale(ppm);

        cc->ReloadVP();

        ut_index++;
        if (ut_index > ut_index_max) exit(0);
      } else {
        _exit(0);
      }
    }
  }
  g_tick++;

  //      Listen for quitflag to be set, requesting application close
  if (quitflag) {
    wxLogMessage(_T("Got quitflag from SIGNAL"));
    FrameTimer1.Stop();
    Close();
    return;
  }

  if (bDBUpdateInProgress) return;

  FrameTimer1.Stop();

  //  If tracking carryover was found in config file, enable tracking as soon as
  //  GPS become valid
  if (g_bDeferredStartTrack) {
    if (!g_bTrackActive) {
      if (bGPSValid) {
        gFrame->TrackOn();
        g_bDeferredStartTrack = false;
      }
    } else {  // tracking has been manually activated
      g_bDeferredStartTrack = false;
    }
  }

//  Send current nav status data to log file on every half hour   // pjotrc
  //  2010.02.09

  wxDateTime lognow = wxDateTime::Now();  // pjotrc 2010.02.09
  int hourLOC = lognow.GetHour();
  int minuteLOC = lognow.GetMinute();
  lognow.MakeGMT();
  int minuteUTC = lognow.GetMinute();
  int second = lognow.GetSecond();

  wxTimeSpan logspan = lognow.Subtract(g_loglast_time);
  if ((logspan.IsLongerThan(wxTimeSpan(0, 30, 0, 0))) || (minuteUTC == 0) ||
      (minuteUTC == 30)) {
    if (logspan.IsLongerThan(wxTimeSpan(0, 1, 0, 0))) {
      wxString day = lognow.FormatISODate();
      wxString utc = lognow.FormatISOTime();
      wxString navmsg = _T("LOGBOOK:  ");
      navmsg += day;
      navmsg += _T(" ");
      navmsg += utc;
      navmsg += _T(" UTC ");

      if (bGPSValid) {        
      } else {        
      }
      wxLogMessage(navmsg);
      g_loglast_time = lognow;

      int bells = (hourLOC % 4) * 2;  // 2 bells each hour
      if (minuteLOC != 0) bells++;    // + 1 bell on 30 minutes
      if (!bells) bells = 8;          // 0 is 8 bells

      if (g_bPlayShipsBells && ((minuteLOC == 0) || (minuteLOC == 30))) {
        m_BellsToPlay = bells;
        wxCommandEvent ev(BELLS_PLAYED_EVTYPE);
        wxPostEvent(this, ev);
      }
    }
  }

  // If no alerts are on, then safe to resume sleeping
  if (g_bSleep && !AnchorAlertOn1 && !AnchorAlertOn2) {
    FrameTimer1.Start(TIMER_GFRAME_1, wxTIMER_CONTINUOUS);
    return;
  }

  //  Update the Toolbar Status windows and lower status bar
  //  just after start of ticks.

  if (g_tick == 2) {
    wxString sogcog(_T("SOG --- ") + getUsrSpeedUnit() + +_T("     ") +
                    _T(" COG ---\u00B0"));
    if (GetStatusBar()) SetStatusText(sogcog, STAT_FIELD_SOGCOG);
  }

// TODO
//  Not needed?
#if 0
#if !defined(__WXGTK__) && !defined(__WXQT__)
    {
        double cursor_lat, cursor_lon;
        GetPrimaryCanvas()->GetCursorLatLon( &cursor_lat, &cursor_lon );
        GetPrimaryCanvas()->SetCursorStatus(cursor_lat, cursor_lon);
    }
#endif
#endif

  //      Update the chart database and displayed chart
  bool bnew_view = false;

  //    Do the chart update based on the global update period currently set
  //    If in COG UP mode, the chart update is handled by COG Update timer
  if (/*!g_bCourseUp &&*/ (0 != g_ChartUpdatePeriod)) {
    if (0 == m_ChartUpdatePeriod--) {
      bnew_view = DoChartUpdate();
      m_ChartUpdatePeriod = g_ChartUpdatePeriod;
    }
  }

  nBlinkerTick++;

  // This call sends autopilot output strings to output ports.

  // For each canvas....
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      cc->DrawBlinkObjects();;

      if (cc->GetQuiltMode()) {
        double erf = cc->GetQuiltMaxErrorFactor();        
      } else {        
      }

      if ((bGPSValid != m_last_bGPSValid) || (bVelocityValid != m_last_bVelocityValid)) {        
        bnew_view = true;  // force a full Refresh()
      }
    }
  }

  m_last_bGPSValid = bGPSValid;
  m_last_bVelocityValid = bVelocityValid;

  //    If any PlugIn requested dynamic overlay callbacks, force a full canvas
  //    refresh thus, ensuring at least 1 Hz. callback.
  bool brq_dynamic = false;
  //  Make sure we get a redraw and alert sound on AnchorWatch excursions.
  if (AnchorAlertOn1 || AnchorAlertOn2) bnew_view = true;

  // For each canvas....
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      if (g_bopengl) {
#ifdef ocpnUSE_GL
        if (cc->GetglCanvas()) {
          if (m_fixtime - cc->GetglCanvas()->m_last_render_time > 0)
            bnew_view = true;
        }
#endif
      } else {
        //  Invalidate the ChartCanvas window appropriately
        //    In non-follow mode, invalidate the rectangles containing the AIS
        //    targets and the ownship, etc... In follow mode, if there has
        //    already been a full screen refresh, there is no need to check
        //    ownship or AIS,
        //       since they will be always drawn on the full screen paint.

        if ((!cc->m_bFollow) || (cc->GetUpMode() != NORTH_UP_MODE)) {          
          cc->UpdateAlerts();
        } else {
          if (!bnew_view) {  // There has not been a Refresh() yet.....
            cc->UpdateAlerts();
          }
        }
      }
    }
  }

  //  Pick up any change Toolbar status displays  
  UpdateAISTool();

  //  This little hack fixes a problem seen with some UniChrome OpenGL drivers
  //  We need a deferred resize to get glDrawPixels() to work right.
  //  So we set a trigger to generate a resize after 5 seconds....
  //  See the "UniChrome" hack elsewhere
  if (m_bdefer_resize) {
    if (0 == (g_tick % (5))) {
      printf("___RESIZE\n");
      SetSize(m_defer_size);
      g_pauimgr->Update();
      m_bdefer_resize = false;
    }
  }

#ifdef __OCPN__ANDROID__

  // Update the navobj file on a fixed schedule (5 minutes)
  // This will do nothing if the navobj.changes file is empty and clean
  if (((g_tick % g_FlushNavobjChangesTimeout) == 0) || g_FlushNavobjChanges) {
    if (pConfig && pConfig->IsChangesFileDirty()) {
      androidShowBusyIcon();
      wxStopWatch update_sw;
      pConfig->UpdateNavObj(true);
      wxString msg = wxString::Format(
          _T("OpenCPN periodic navobj update took %ld ms."), update_sw.Time());
      wxLogMessage(msg);
      qDebug() << msg.mb_str();
      g_FlushNavobjChanges = false;
      androidHideBusyIcon();
    }
  }

#endif

  // Reset pending next AppMsgBus notification
  m_b_new_data = false;

  if (g_unit_test_2)
    FrameTimer1.Start(TIMER_GFRAME_1 * 3, wxTIMER_CONTINUOUS);
  else
    FrameTimer1.Start(TIMER_GFRAME_1, wxTIMER_CONTINUOUS);
}

bool MyFrame::SendJSON_WMM_Var_Request(double lat, double lon,
                                       wxDateTime date) {
    return false;
}

void MyFrame::TouchAISActive(void) {  
}

void MyFrame::UpdateAISTool(void) {  
}

//    Cause refresh of active Tide/Current data, if displayed
void MyFrame::OnFrameTCTimer(wxTimerEvent &event) {
  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) cc->SetbTCUpdate(true);
  }

  RefreshAllCanvas(false);
}

//    Keep and update the Viewport rotation angle according to average COG for
//    COGUP mode
void MyFrame::OnFrameCOGTimer(wxTimerEvent &event) {
  // ..For each canvas...
  bool b_rotate = false;
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) b_rotate |= (cc->GetUpMode() != NORTH_UP_MODE);
  }

  if (!b_rotate) {
    FrameCOGTimer.Stop();
    return;
  }

  DoCOGSet();

  //    Restart the timer, max frequency is 10 hz.
  int period_ms = 100;
  //if (g_COGAvgSec > 0) period_ms = g_COGAvgSec * 1000;
  FrameCOGTimer.Start(period_ms, wxTIMER_CONTINUOUS);
}

void MyFrame::DoCOGSet(void) {
  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) cc->DoCanvasCOGSet();
  }
}

void RenderShadowText(wxDC *pdc, wxFont *pFont, wxString &str, int x, int y) {
#ifdef DrawText
#undef DrawText
#define FIXIT
#endif

  wxFont oldfont = pdc->GetFont();  // save current font

  pdc->SetFont(*pFont);
  pdc->SetTextForeground(GetGlobalColor(_T("CHGRF")));
  pdc->SetBackgroundMode(wxTRANSPARENT);

  pdc->DrawText(str, x, y + 1);
  pdc->DrawText(str, x, y - 1);
  pdc->DrawText(str, x + 1, y);
  pdc->DrawText(str, x - 1, y);

  pdc->SetTextForeground(GetGlobalColor(_T("CHBLK")));

  pdc->DrawText(str, x, y);

  pdc->SetFont(oldfont);  // restore last font
}

// TODO How does this relate to per-canvas rotation?
void MyFrame::UpdateRotationState(double rotation) {
  //  If rotated manually, we switch to NORTHUP
  g_bCourseUp = false;

  if (fabs(rotation) > .001) {
    SetMenubarItemState(ID_MENU_CHART_COGUP, false);
    SetMenubarItemState(ID_MENU_CHART_NORTHUP, true);
    if (m_pMenuBar) {
      m_pMenuBar->SetLabel(ID_MENU_CHART_NORTHUP, _("Rotated Mode"));
    }
  } else {
    SetMenubarItemState(ID_MENU_CHART_COGUP, g_bCourseUp);
    SetMenubarItemState(ID_MENU_CHART_NORTHUP, !g_bCourseUp);
    if (m_pMenuBar) {
      m_pMenuBar->SetLabel(ID_MENU_CHART_NORTHUP, _("North Up Mode"));
    }
  }

  DoChartUpdate();
}

//    Application memory footprint management

int MyFrame::GetApplicationMemoryUse(void) {
  int memsize = -1;
#ifdef __linux__

  //    Use a contrived ps command to get the virtual memory size associated
  //    with this process
  wxWindow *fWin = wxWindow::FindFocus();

  wxArrayString outputArray;
  wxString cmd(_T("ps --no-headers -o vsize "));
  unsigned long pid = wxGetProcessId();
  wxString cmd1;
  cmd1.Printf(_T("%ld"), pid);
  cmd += cmd1;
  wxExecute(cmd, outputArray);

  if (outputArray.GetCount()) {
    wxString s = outputArray.Item(0);
    long vtmp;
    if (s.ToLong(&vtmp)) memsize = vtmp;
  }

  if (fWin) fWin->SetFocus();

#endif

#ifdef __WXMSW__
  HANDLE hProcess;
  PROCESS_MEMORY_COUNTERS pmc;

  unsigned long processID = wxGetProcessId();

  hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
                         processID);
  if (NULL == hProcess) return 0;

  if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
    /*
     printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
     printf( "\tPeakWorkingSetSize: 0x%08X\n",
     pmc.PeakWorkingSetSize );
     printf( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
     printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n",
     pmc.QuotaPeakPagedPoolUsage );
     printf( "\tQuotaPagedPoolUsage: 0x%08X\n",
     pmc.QuotaPagedPoolUsage );
     printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n",
     pmc.QuotaPeakNonPagedPoolUsage );
     printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n",
     pmc.QuotaNonPagedPoolUsage );
     printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage );
     printf( "\tPeakPagefileUsage: 0x%08X\n",
     pmc.PeakPagefileUsage );
     */
    memsize = pmc.WorkingSetSize / 1024;
  }

  CloseHandle(hProcess);

#endif

  return memsize;
}

double MyFrame::GetBestVPScale(ChartBase *pchart) {
  return GetPrimaryCanvas()->GetBestVPScale(pchart);
}

void MyFrame::SetChartUpdatePeriod() {
  //    Set the chart update period based upon chart skew and skew compensator

  g_ChartUpdatePeriod = 0;  // General default

  // In non-GL, singlele-chart mode, rotation of skewed charts is very slow
  //  So we need to use a slower update time constant to preserve adequate UI
  //  performance
  bool bskewdc = false;
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) {
      if (!g_bopengl && !cc->GetVP().b_quilt) {
        if (fabs(cc->GetVP().skew) > 0.0001) bskewdc = true;
      }
      if (cc->m_bFollow) g_ChartUpdatePeriod = 1;
    }
  }

  if (bskewdc) g_ChartUpdatePeriod = g_SkewCompUpdatePeriod;

  m_ChartUpdatePeriod = g_ChartUpdatePeriod;
}

void MyFrame::UpdateControlBar(ChartCanvas *cc) {
  if (!cc) return;
  cc->UpdateCanvasControlBar();
}

void MyFrame::selectChartDisplay(int type, int family) {
  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) cc->selectCanvasChartDisplay(type, family);
  }

  UpdateGlobalMenuItems();  // update the state of the menu items (checkmarks
                            // etc)
}

//----------------------------------------------------------------------------------
//      DoChartUpdate
//      Create a chartstack based on current lat/lon.
//      Return true if a Refresh(false) was called within.
//----------------------------------------------------------------------------------
bool MyFrame::DoChartUpdate(void) {
  bool return_val = false;

  // ..For each canvas...
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) return_val |= cc->DoCanvasUpdate();
  }

  return return_val;
}

void MyFrame::MouseEvent(wxMouseEvent &event) {
  int x, y;
  event.GetPosition(&x, &y);
}

#ifdef __WXGTK__
#include <malloc.h>
#endif

#if defined(__linux__)
#include "sys/types.h"
#include "sys/sysinfo.h"
#endif /* __linux__ */

int g_lastMemTick = -1;

/* Return total system RAM and size of program */
/* Values returned are in kilobytes            */
bool GetMemoryStatus(int *mem_total, int *mem_used) {
#if defined(__linux__)
  // Use sysinfo to obtain total RAM
  if (mem_total) {
    *mem_total = 0;
    struct sysinfo sys_info;
    if (sysinfo(&sys_info) != -1)
      *mem_total = ((uint64_t)sys_info.totalram * sys_info.mem_unit) / 1024;
  }
  //      Use filesystem /proc/self/statm to determine memory status
  //  Provides information about memory usage, measured in pages.  The columns
  //  are: size       total program size (same as VmSize in /proc/[pid]/status)
  //  resident   resident set size (same as VmRSS in /proc/[pid]/status)
  //  share      shared pages (from shared mappings)
  //  text       text (code)
  //  lib        library (unused in Linux 2.6)
  //  data       data + stack
  //  dt         dirty pages (unused in Linux 2.6)

  if (mem_used) {
    *mem_used = 0;
    FILE *file = fopen("/proc/self/statm", "r");
    if (file) {
      if (fscanf(file, "%d", mem_used) != 1) {
        wxLogWarning("Cannot parse /proc/self/statm (!)");
      }
      *mem_used *= 4;  // XXX assume 4K page
      fclose(file);
    }
  }

  return true;

#endif /* __linux__ */

#ifdef __WXMSW__
  HANDLE hProcess;
  PROCESS_MEMORY_COUNTERS pmc;

  unsigned long processID = wxGetProcessId();

  if (mem_used) {
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE,
                           processID);

    if (hProcess && GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
      /*
       printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
       printf( "\tPeakWorkingSetSize: 0x%08X\n",
       pmc.PeakWorkingSetSize );
       printf( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
       printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n",
       pmc.QuotaPeakPagedPoolUsage );
       printf( "\tQuotaPagedPoolUsage: 0x%08X\n",
       pmc.QuotaPagedPoolUsage );
       printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n",
       pmc.QuotaPeakNonPagedPoolUsage );
       printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n",
       pmc.QuotaNonPagedPoolUsage );
       printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage );
       printf( "\tPeakPagefileUsage: 0x%08X\n",
       pmc.PeakPagefileUsage );
       */
      *mem_used = pmc.WorkingSetSize / 1024;
    }

    CloseHandle(hProcess);
  }

  if (mem_total) {
    MEMORYSTATUSEX statex;

    statex.dwLength = sizeof(statex);

    GlobalMemoryStatusEx(&statex);
    /*
     _tprintf (TEXT("There is  %*ld percent of memory in use.\n"),
     WIDTH, statex.dwMemoryLoad);
     _tprintf (TEXT("There are %*I64d total Kbytes of physical memory.\n"),
     WIDTH, statex.ullTotalPhys/DIV);
     _tprintf (TEXT("There are %*I64d free Kbytes of physical memory.\n"),
     WIDTH, statex.ullAvailPhys/DIV);
     _tprintf (TEXT("There are %*I64d total Kbytes of paging file.\n"),
     WIDTH, statex.ullTotalPageFile/DIV);
     _tprintf (TEXT("There are %*I64d free Kbytes of paging file.\n"),
     WIDTH, statex.ullAvailPageFile/DIV);
     _tprintf (TEXT("There are %*I64d total Kbytes of virtual memory.\n"),
     WIDTH, statex.ullTotalVirtual/DIV);
     _tprintf (TEXT("There are %*I64d free Kbytes of virtual memory.\n"),
     WIDTH, statex.ullAvailVirtual/DIV);
     */

    *mem_total = statex.ullTotalPhys / 1024;
  }
  return true;
#endif

#ifdef __WXMAC__

  if (g_tick != g_lastMemTick) {
    malloc_zone_pressure_relief(NULL, 0);

    int bytesInUse = 0;
    int blocksInUse = 0;
    int sizeAllocated = 0;

    malloc_statistics_t stats;
    stats.blocks_in_use = 0;
    stats.size_in_use = 0;
    stats.max_size_in_use = 0;
    stats.size_allocated = 0;
    malloc_zone_statistics(NULL, &stats);
    bytesInUse += stats.size_in_use;
    blocksInUse += stats.blocks_in_use;
    sizeAllocated += stats.size_allocated;

    g_memUsed = sizeAllocated >> 10;

    // printf("mem_used (Mb):  %d   %d \n", g_tick, g_memUsed / 1024);
    g_lastMemTick = g_tick;
  }

  if (mem_used) *mem_used = g_memUsed;
  if (mem_total) {
    *mem_total = 4000;
    FILE *fpIn = popen("sysctl -n hw.memsize", "r");
    if (fpIn) {
      double pagesUsed = 0.0, totalPages = 0.0;
      char buf[64];
      if (fgets(buf, sizeof(buf), fpIn) != NULL) {
        *mem_total = atol(buf) >> 10;
      }
    }
  }

  return true;
#endif

  if (mem_used) *mem_used = 0;
  if (mem_total) *mem_total = 0;
  return false;
}

void MyFrame::DoPrint(void) {
  if (NULL == g_printData) {
    g_printData = new wxPrintData;
    g_printData->SetOrientation(wxLANDSCAPE);
    g_pageSetupData = new wxPageSetupDialogData;
  }

  wxPrintDialogData printDialogData(*g_printData);
  printDialogData.EnablePageNumbers(false);
#ifdef __WXGTK__
  GetPrimaryCanvas()->SetFocus();
  Raise();  // I dunno why...
#endif
}

wxDateTime gTimeSource;

static void UpdatePositionCalculatedSogCog() {
}

void MyFrame::FilterCogSog(void) {  
}

void MyFrame::StopSockets(void) {
  // TODO: Can be removed?
}

void MyFrame::ResumeSockets(void) {
  // TODO: Can be removed?
}

void MyFrame::LoadHarmonics() {
  if (!ptcmgr) {
    ptcmgr = new TCMgr;
    ptcmgr->LoadDataSources(TideCurrentDataSet);
  } else {
    bool b_newdataset = false;

    //      Test both ways
    for (auto a : ptcmgr->GetDataSet()) {
      bool b_foundi = false;
      for (auto b : TideCurrentDataSet) {
        if (a == b) {
          b_foundi = true;
          break;  // j loop
        }
      }
      if (!b_foundi) {
        b_newdataset = true;
        break;  //  i loop
      }
    }

    for (auto a : TideCurrentDataSet) {
      bool b_foundi = false;
      for (auto b : ptcmgr->GetDataSet()) {
        if (a == b) {
          b_foundi = true;
          break;  // j loop
        }
      }
      if (!b_foundi) {
        b_newdataset = true;
        break;  //  i loop
      }
    }

    if (b_newdataset) ptcmgr->LoadDataSources(TideCurrentDataSet);
  }
}

Route *pAISMOBRoute;

void MyFrame::applySettingsString(wxString settings) {
  //  Save some present values
  int last_UIScaleFactor = g_GUIScaleFactor;
  bool previous_expert = g_bUIexpert;
  g_last_ChartScaleFactor = g_ChartScaleFactor;
  ArrayOfCDI *pNewDirArray = new ArrayOfCDI;

  int rr = g_Platform->platformApplyPrivateSettingsString(settings, pNewDirArray); //rr : 0

  // And apply the changes
  pConfig->UpdateSettings();

  //  Might need to rebuild symbols
  if (g_last_ChartScaleFactor != g_ChartScaleFactor) rr |= S52_CHANGED;  //rr : 2

  if (rr & S52_CHANGED) {
    if (ps52plib) {
      ps52plib->FlushSymbolCaches(ChartCtxFactory());
      ps52plib
          ->ClearCNSYLUPArray();  // some CNSY depends on renderer (e.g. CARC)
      ps52plib->GenerateStateHash();
    }
  }

  ProcessOptionsDialog(rr, pNewDirArray);

  // Try to detect if the toolbar is changing, to avoid a rebuild if not
  // necessary.

  bool b_newToolbar = false;

  if (g_GUIScaleFactor != last_UIScaleFactor) b_newToolbar = true;

  if (previous_expert != g_bUIexpert) b_newToolbar = true;

  if (rr & TOOLBAR_CHANGED) b_newToolbar = true;

  if (previous_expert != g_bUIexpert) g_Platform->applyExpertMode(g_bUIexpert);

  if (b_newToolbar) {
    RequestNewToolbars(true);  // Force rebuild, to pick up bGUIexpert and scale settings.

    RequestNewMasterToolbar(true);
  }

  gFrame->Raise();

  InvalidateAllGL();
  DoChartUpdate();
  UpdateControlBar(GetPrimaryCanvas());
  Refresh();

#if defined(__WXOSX__) || defined(__WXQT__)  
#endif

  Refresh(false);
}

#ifdef wxHAS_POWER_EVENTS
void MyFrame::OnSuspending(wxPowerEvent &event) {
  //   wxDateTime now = wxDateTime::Now();
  //   printf("OnSuspending...%d\n", now.GetTicks());

  wxLogMessage(_T("System suspend starting..."));
}

void MyFrame::OnSuspended(wxPowerEvent &WXUNUSED(event)) {
  //    wxDateTime now = wxDateTime::Now();
  //    printf("OnSuspended...%d\n", now.GetTicks());
  wxLogMessage(_T("System is going to suspend."));
}

void MyFrame::OnSuspendCancel(wxPowerEvent &WXUNUSED(event)) {
  //    wxDateTime now = wxDateTime::Now();
  //    printf("OnSuspendCancel...%d\n", now.GetTicks());
  wxLogMessage(_T("System suspend was cancelled."));
}

int g_last_resume_ticks;
void MyFrame::OnResume(wxPowerEvent &WXUNUSED(event)) {
  wxDateTime now = wxDateTime::Now();
  //    printf("OnResume...%d\n", now.GetTicks());
  wxLogMessage(_T("System resumed from suspend."));

  if ((now.GetTicks() - g_last_resume_ticks) > 5) {
    wxLogMessage(_T("Restarting streams."));
    //       printf("   Restarting streams\n");
    g_last_resume_ticks = now.GetTicks();
//FIXME (dave)
#if 0
    if (g_pMUX) {
      g_pMUX->ClearStreams();

      g_pMUX->StartAllStreams();
    }
#endif
  }

  //  If OpenGL is enabled, Windows Resume does not properly refresh the
  //  application GL context. We need to force a Resize event that actually does
  //  something.
  if (g_bopengl) {
    if (IsMaximized()) {  // This is not real pretty on-screen, but works
      Maximize(false);
      wxYield();
      Maximize(true);
    } else {
      wxSize sz = GetSize();
      SetSize(wxSize(sz.x - 1, sz.y));
      wxYield();
      SetSize(sz);
    }
  }
}
#endif  // wxHAS_POWER_EVENTS

//----------------------------------------------------------------------------------------------------------
//   Master Toolbar support
//----------------------------------------------------------------------------------------------------------

void MyFrame::RequestNewMasterToolbar(bool bforcenew) {
  bool btbRebuild = false;

  bool b_reshow = true;
}

bool MyFrame::CollapseGlobalToolbar() {
    return false;
}

bool MyFrame::GetMasterToolItemShow(int toolid) {
  if (g_bmasterToolbarFull)
    return true;
  else
    return false;
}

bool MyFrame::AddDefaultPositionPlugInTools() {
  bool bret = false;

  return bret;
}

/*************************************************************************
 * Global color management routines
 *
 *************************************************************************/

wxColour GetGlobalColor(wxString colorName);  // -> color_handler

static const char *usercolors[] = {
    "Table:DAY",
    "GREEN1;120;255;120;",
    "GREEN2; 45;150; 45;",
    "GREEN3;200;220;200;",
    "GREEN4;  0;255;  0;",
    "BLUE1; 170;170;255;",
    "BLUE2;  45; 45;170;",
    "BLUE3;   0;  0;255;",
    "GREY1; 200;200;200;",
    "GREY2; 230;230;230;",
    "RED1;  220;200;200;",
    "UBLCK;   0;  0;  0;",
    "UWHIT; 255;255;255;",
    "URED;  255;  0;  0;",
    "UGREN;   0;255;  0;",
    "YELO1; 243;229; 47;",
    "YELO2; 128; 80;  0;",
    "TEAL1;   0;128;128;",
    "GREEN5;170;254;  0;",
    "COMPT; 245;247;244",
#ifdef __WXOSX__
    "DILG0; 255;255;255;",  // Dialog Background white
#else
    "DILG0; 238;239;242;",  // Dialog Background white
#endif
    "DILG1; 212;208;200;",  // Dialog Background
    "DILG2; 255;255;255;",  // Control Background
    "DILG3;   0;  0;  0;",  // Text
    "UITX1;   0;  0;  0;",  // Menu Text, derived from UINFF

    "CHGRF; 163; 180; 183;",
    "UINFM; 197;  69; 195;",
    "UINFG; 104; 228;  86;",
    "UINFF; 125; 137; 140;",
    "UINFR; 241;  84; 105;",
    "SHIPS;   7;   7;   7;",
    "CHYLW; 244; 218;  72;",
    "CHWHT; 212; 234; 238;",

    "UDKRD; 124; 16;  0;",
    "UARTE; 200;  0;  0;",  // Active Route, Grey on Dusk/Night

    "NODTA; 163; 180; 183;",
    "CHBLK;   7;   7;   7;",
    "SNDG1; 125; 137; 140;",
    "SNDG2;   7;   7;   7;",
    "SCLBR; 235; 125;  54;",
    "UIBDR; 125; 137; 140;",
    "UINFB;  58; 120; 240;",
    "UINFD;   7;   7;   7;",
    "UINFO; 235; 125;  54;",
    "PLRTE; 220;  64;  37;",
    "CHMGD; 197; 69; 195;",
    "UIBCK; 212; 234; 238;",

    "DASHB; 255;255;255;",  // Dashboard Instr background
    "DASHL; 175;175;175;",  // Dashboard Instr Label
    "DASHF;  50; 50; 50;",  // Dashboard Foreground
    "DASHR; 200;  0;  0;",  // Dashboard Red
    "DASHG;   0;200;  0;",  // Dashboard Green
    "DASHN; 200;120;  0;",  // Dashboard Needle
    "DASH1; 204;204;255;",  // Dashboard Illustrations
    "DASH2; 122;131;172;",  // Dashboard Illustrations
    "COMP1; 211;211;211;",  // Compass Window Background

    "GREY3;  40; 40; 40;",  // MUIBar/TB background
    "BLUE4; 100;100;200;",  // Canvas Focus Bar
    "VIO01; 171; 33;141;",
    "VIO02; 209;115;213;",

    "Table:DUSK",
    "GREEN1; 60;128; 60;",
    "GREEN2; 22; 75; 22;",
    "GREEN3; 80;100; 80;",
    "GREEN4;  0;128;  0;",
    "BLUE1;  80; 80;160;",
    "BLUE2;  30; 30;120;",
    "BLUE3;   0;  0;128;",
    "GREY1; 100;100;100;",
    "GREY2; 128;128;128;",
    "RED1;  150;100;100;",
    "UBLCK;   0;  0;  0;",
    "UWHIT; 255;255;255;",
    "URED;  120; 54; 11;",
    "UGREN;  35;110; 20;",
    "YELO1; 120;115; 24;",
    "YELO2;  64; 40;  0;",
    "TEAL1;   0; 64; 64;",
    "GREEN5; 85;128; 0;",
    "COMPT; 124;126;121",

    "CHGRF;  41; 46; 46;",
    "UINFM;  58; 20; 57;",
    "UINFG;  35; 76; 29;",
    "UINFF;  41; 46; 46;",
    "UINFR;  80; 28; 35;",
    "SHIPS;  71; 78; 79;",
    "CHYLW;  81; 73; 24;",
    "CHWHT;  71; 78; 79;",

    "DILG0; 110;110;110;",  // Dialog Background
    "DILG1; 110;110;110;",  // Dialog Background
    "DILG2;   0;  0;  0;",  // Control Background
    "DILG3; 130;130;130;",  // Text
    "UITX1;  41; 46; 46;",  // Menu Text, derived from UINFF
    "UDKRD;  80;  0;  0;",
    "UARTE;  64; 64; 64;",  // Active Route, Grey on Dusk/Night

    "NODTA;  41;  46;  46;",
    "CHBLK;  54;  60;  61;",
    "SNDG1;  41;  46;  46;",
    "SNDG2;  71;  78;  79;",
    "SCLBR;  75;  38;  19;",
    "UIBDR;  54;  60;  61;",
    "UINFB;  19;  40;  80;",
    "UINFD;  71;  78;  79;",
    "UINFO;  75;  38;  19;",
    "PLRTE;  73;  21;  12;",
    "CHMGD; 74; 58; 81;",
    "UIBCK; 7; 7; 7;",

    "DASHB;  77; 77; 77;",  // Dashboard Instr background
    "DASHL;  54; 54; 54;",  // Dashboard Instr Label
    "DASHF;   0;  0;  0;",  // Dashboard Foreground
    "DASHR;  58; 21; 21;",  // Dashboard Red
    "DASHG;  21; 58; 21;",  // Dashboard Green
    "DASHN; 100; 50;  0;",  // Dashboard Needle
    "DASH1;  76; 76;113;",  // Dashboard Illustrations
    "DASH2;  48; 52; 72;",  // Dashboard Illustrations
    "COMP1; 107;107;107;",  // Compass Window Background

    "GREY3;  20; 20; 20;",  // MUIBar/TB background
    "BLUE4;  80; 80;160;",  // Canvas Focus Bar
    "VIO01; 128; 25;108;",
    "VIO02; 171; 33;141;",

    "Table:NIGHT",
    "GREEN1; 30; 80; 30;",
    "GREEN2; 15; 60; 15;",
    "GREEN3; 12; 23;  9;",
    "GREEN4;  0; 64;  0;",
    "BLUE1;  60; 60;100;",
    "BLUE2;  22; 22; 85;",
    "BLUE3;   0;  0; 40;",
    "GREY1;  48; 48; 48;",
    "GREY2;  32; 32; 32;",
    "RED1;  100; 50; 50;",
    "UWHIT; 255;255;255;",
    "UBLCK;   0;  0;  0;",
    "URED;   60; 27;  5;",
    "UGREN;  17; 55; 10;",
    "YELO1;  60; 65; 12;",
    "YELO2;  32; 20;  0;",
    "TEAL1;   0; 32; 32;",
    "GREEN5; 44; 64; 0;",
    "COMPT;  48; 49; 51",
    "DILG0;  80; 80; 80;",  // Dialog Background
    "DILG1;  80; 80; 80;",  // Dialog Background
    "DILG2;   0;  0;  0;",  // Control Background
    "DILG3;  65; 65; 65;",  // Text
    "UITX1;  31; 34; 35;",  // Menu Text, derived from UINFF
    "UDKRD;  50;  0;  0;",
    "UARTE;  64; 64; 64;",  // Active Route, Grey on Dusk/Night

    "CHGRF;  16; 18; 18;",
    "UINFM;  52; 18; 52;",
    "UINFG;  22; 24;  7;",
    "UINFF;  31; 34; 35;",
    "UINFR;  59; 17; 10;",
    "SHIPS;  37; 41; 41;",
    "CHYLW;  31; 33; 10;",
    "CHWHT;  37; 41; 41;",

    "NODTA;   7;   7;   7;",
    "CHBLK;  31;  34;  35;",
    "SNDG1;  31;  34;  35;",
    "SNDG2;  43;  48;  48;",
    "SCLBR;  52;  28;  12;",
    "UIBDR;  31;  34;  35;",
    "UINFB;  21;  29;  69;",
    "UINFD;  43;  48;  58;",
    "UINFO;  52;  28;  12;",
    "PLRTE;  66;  19;  11;",
    "CHMGD; 52; 18; 52;",
    "UIBCK; 7; 7; 7;",

    "DASHB;   0;  0;  0;",  // Dashboard Instr background
    "DASHL;  20; 20; 20;",  // Dashboard Instr Label
    "DASHF;  64; 64; 64;",  // Dashboard Foreground
    "DASHR;  70; 15; 15;",  // Dashboard Red
    "DASHG;  15; 70; 15;",  // Dashboard Green
    "DASHN;  17; 80; 56;",  // Dashboard Needle
    "DASH1;  48; 52; 72;",  // Dashboard Illustrations
    "DASH2;  36; 36; 53;",  // Dashboard Illustrations
    "COMP1;  24; 24; 24;",  // Compass Window Background

    "GREY3;  10; 10; 10;",  // MUIBar/TB background
    "BLUE4;  70; 70;140;",  // Canvas Focus Bar
    "VIO01;  85; 16; 72;",
    "VIO02; 128; 25;108;",

    "*****"};

int get_static_line(char *d, const char **p, int index, int n) {
  if (!strcmp(p[index], "*****")) return 0;

  strncpy(d, p[index], n);
  return strlen(d);
}

void InitializeUserColors(void) {
  const char **p = usercolors;
  char buf[81];
  int index = 0;
  char TableName[20];
  colTable *ctp;
  colTable *ct;
  int R, G, B;

  UserColorTableArray = new wxArrayPtrVoid;
  UserColourHashTableArray = new wxArrayPtrVoid;

  //    Create 3 color table entries
  ct = new colTable;
  ct->tableName = new wxString(_T("DAY"));
  ct->color = new wxArrayPtrVoid;
  UserColorTableArray->Add((void *)ct);

  ct = new colTable;
  ct->tableName = new wxString(_T("DUSK"));
  ct->color = new wxArrayPtrVoid;
  UserColorTableArray->Add((void *)ct);

  ct = new colTable;
  ct->tableName = new wxString(_T("NIGHT"));
  ct->color = new wxArrayPtrVoid;
  UserColorTableArray->Add((void *)ct);

  while ((get_static_line(buf, p, index, sizeof(buf) - 1))) {
    if (!strncmp(buf, "Table", 5)) {
      sscanf(buf, "Table:%s", TableName);

      for (unsigned int it = 0; it < UserColorTableArray->GetCount(); it++) {
        ctp = (colTable *)(UserColorTableArray->Item(it));
        if (!strcmp(TableName, ctp->tableName->mb_str())) {
          ct = ctp;
          break;
        }
      }

    } else {
      char name[21];
      int j = 0;
      while (buf[j] != ';' && j < 20) {
        name[j] = buf[j];
        j++;
      }
      name[j] = 0;

      S52color *c = new S52color;
      strcpy(c->colName, name);

      sscanf(&buf[j], ";%i;%i;%i", &R, &G, &B);
      c->R = (char)R;
      c->G = (char)G;
      c->B = (char)B;

      ct->color->Add(c);
    }

    index++;
  }

  //    Now create the Hash tables

  for (unsigned int its = 0; its < UserColorTableArray->GetCount(); its++) {
    wxColorHashMap *phash = new wxColorHashMap;
    UserColourHashTableArray->Add((void *)phash);

    colTable *ctp = (colTable *)(UserColorTableArray->Item(its));

    for (unsigned int ic = 0; ic < ctp->color->GetCount(); ic++) {
      S52color *c2 = (S52color *)(ctp->color->Item(ic));

      wxColour c(c2->R, c2->G, c2->B);
      wxString key(c2->colName, wxConvUTF8);
      (*phash)[key] = c;
    }
  }

  //    Establish a default hash table pointer
  //    in case a color is needed before ColorScheme is set
  pcurrent_user_color_hash =
      (wxColorHashMap *)UserColourHashTableArray->Item(0);
}

void DeInitializeUserColors(void) {
  unsigned int i;
  for (i = 0; i < UserColorTableArray->GetCount(); i++) {
    colTable *ct = (colTable *)UserColorTableArray->Item(i);

    for (unsigned int j = 0; j < ct->color->GetCount(); j++) {
      S52color *c = (S52color *)ct->color->Item(j);
      delete c;  // color
    }

    delete ct->tableName;  // wxString
    delete ct->color;      // wxArrayPtrVoid

    delete ct;  // colTable
  }

  delete UserColorTableArray;

  for (i = 0; i < UserColourHashTableArray->GetCount(); i++) {
    wxColorHashMap *phash = (wxColorHashMap *)UserColourHashTableArray->Item(i);
    delete phash;
  }

  delete UserColourHashTableArray;
}

#ifdef __WXMSW__

#define NCOLORS 40

typedef struct _MSW_COLOR_SPEC {
  int COLOR_NAME;
  wxString S52_RGB_COLOR;
  int SysRGB_COLOR;
} MSW_COLOR_SPEC;

MSW_COLOR_SPEC color_spec[] = {{COLOR_MENU, _T("UIBCK"), 0},
                               {COLOR_MENUTEXT, _T("UITX1"), 0},
                               {COLOR_BTNSHADOW, _T("UIBCK"), 0},  // Menu Frame
                               {-1, _T(""), 0}};

void SaveSystemColors() {
  /*
   color_3dface = pGetSysColor(COLOR_3DFACE);
   color_3dhilite = pGetSysColor(COLOR_3DHILIGHT);
   color_3dshadow = pGetSysColor(COLOR_3DSHADOW);
   color_3ddkshadow = pGetSysColor(COLOR_3DDKSHADOW);
   color_3dlight = pGetSysColor(COLOR_3DLIGHT);
   color_activecaption = pGetSysColor(COLOR_ACTIVECAPTION);
   color_gradientactivecaption = pGetSysColor(27); //COLOR_3DLIGHT);
   color_captiontext = pGetSysColor(COLOR_CAPTIONTEXT);
   color_windowframe = pGetSysColor(COLOR_WINDOWFRAME);
   color_inactiveborder = pGetSysColor(COLOR_INACTIVEBORDER);
   */
  //    Record the default system color in my substitution structure
  MSW_COLOR_SPEC *pcspec = &color_spec[0];
  while (pcspec->COLOR_NAME != -1) {
    pcspec->SysRGB_COLOR = pGetSysColor(pcspec->COLOR_NAME);
    pcspec++;
  }
}

void RestoreSystemColors() {
  int element[NCOLORS];
  int rgbcolor[NCOLORS];
  int i = 0;

  MSW_COLOR_SPEC *pcspec = &color_spec[0];
  while (pcspec->COLOR_NAME != -1) {
    element[i] = pcspec->COLOR_NAME;
    rgbcolor[i] = pcspec->SysRGB_COLOR;

    pcspec++;
    i++;
  }

  pSetSysColors(i, (unsigned long *)&element[0], (unsigned long *)&rgbcolor[0]);
}

#endif

void SetSystemColors(ColorScheme cs) {  //---------------
#ifdef __WXMSW__
  int element[NCOLORS];
  int rgbcolor[NCOLORS];
  int i = 0;
  if ((GLOBAL_COLOR_SCHEME_DUSK == cs) || (GLOBAL_COLOR_SCHEME_NIGHT == cs)) {
    MSW_COLOR_SPEC *pcspec = &color_spec[0];
    while (pcspec->COLOR_NAME != -1) {
      wxColour color = GetGlobalColor(pcspec->S52_RGB_COLOR);
      rgbcolor[i] = (color.Red() << 16) + (color.Green() << 8) + color.Blue();
      element[i] = pcspec->COLOR_NAME;

      i++;
      pcspec++;
    }

    pSetSysColors(i, (unsigned long *)&element[0],
                  (unsigned long *)&rgbcolor[0]);

  } else {  // for daylight colors, use default windows colors as saved....

    RestoreSystemColors();
  }
#endif
}

wxColor GetDimColor(wxColor c) {
  if ((global_color_scheme == GLOBAL_COLOR_SCHEME_DAY) ||
      (global_color_scheme == GLOBAL_COLOR_SCHEME_RGB))
    return c;

  float factor = 1.0;
  if (global_color_scheme == GLOBAL_COLOR_SCHEME_DUSK) factor = 0.5;
  if (global_color_scheme == GLOBAL_COLOR_SCHEME_NIGHT) factor = 0.25;

  wxImage::RGBValue rgb(c.Red(), c.Green(), c.Blue());
  wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
  hsv.value = hsv.value * factor;
  wxImage::RGBValue nrgb = wxImage::HSVtoRGB(hsv);

  return wxColor(nrgb.red, nrgb.green, nrgb.blue);
}


//               A helper function to check for proper parameters of anchor
//               watch
//
double AnchorDistFix(double const d, double const AnchorPointMinDist,
                     double const AnchorPointMaxDist)  //  pjotrc 2010.02.22
{
  if (d >= 0.0)
    if (d < AnchorPointMinDist)
      return AnchorPointMinDist;
    else if (d > AnchorPointMaxDist)
      return AnchorPointMaxDist;
    else
      return d;

  else
    // if ( d < 0.0 )
    if (d > -AnchorPointMinDist)
      return -AnchorPointMinDist;
    else if (d < -AnchorPointMaxDist)
      return -AnchorPointMaxDist;
    else
      return d;
}
//      Console supporting printf functionality for Windows GUI app

#ifdef __WXMSW__
static const WORD MAX_CONSOLE_LINES =
    500;  // maximum mumber of lines the output console should have

//#ifdef _DEBUG

void RedirectIOToConsole()

{
  int hConHandle;

  wxIntPtr lStdHandle;

  CONSOLE_SCREEN_BUFFER_INFO coninfo;

  FILE *fp;

  // allocate a console for this app

  AllocConsole();

  // set the screen buffer to be big enough to let us scroll text

  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
  coninfo.dwSize.Y = MAX_CONSOLE_LINES;
  SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

  // redirect unbuffered STDOUT to the console

  lStdHandle = (wxIntPtr)GetStdHandle(STD_OUTPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stdout = *fp;
  setvbuf(stdout, NULL, _IONBF, 0);

  // redirect unbuffered STDIN to the console

  lStdHandle = (wxIntPtr)GetStdHandle(STD_INPUT_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "r");
  *stdin = *fp;
  setvbuf(stdin, NULL, _IONBF, 0);

  // redirect unbuffered STDERR to the console

  lStdHandle = (wxIntPtr)GetStdHandle(STD_ERROR_HANDLE);
  hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
  fp = _fdopen(hConHandle, "w");
  *stderr = *fp;
  setvbuf(stderr, NULL, _IONBF, 0);

  // make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console
  // as well

  // ios::sync_with_stdio();
}

//#endif
#endif

#ifdef __WXMSW__
bool TestGLCanvas(wxString prog_dir) {
#ifdef __MSVC__
  wxString test_app = prog_dir;
  test_app += _T("ocpn_gltest1.exe");

  if (::wxFileExists(test_app)) {
    long proc_return = ::wxExecute(test_app, wxEXEC_SYNC);
    printf("OpenGL Test Process returned %0X\n", proc_return);
    if (proc_return == 0)
      printf("GLCanvas OK\n");
    else
      printf("GLCanvas failed to start, disabling OpenGL.\n");

    return (proc_return == 0);
  } else
    return true;
#else
  /* until we can get the source to ocpn_gltest1 assume true for mingw */
  return true;
#endif
}
#endif

#if 0
/*************************************************************************
 * Serial port enumeration routines
 *
 * The EnumSerialPort function will populate an array of SSerInfo structs,
 * each of which contains information about one serial port present in
 * the system. Note that this code must be linked with setupapi.lib,
 * which is included with the Win32 SDK.
 *
 * by Zach Gorman <gormanjz@hotmail.com>
 *
 * Copyright (c) 2002 Archetype Auction Software, Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is
 * met: Redistributions of source code must retain the above copyright
 * notice, this condition and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ARCHETYPE AUCTION SOFTWARE OR ITS
 * AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ************************************************************************/

// For MFC
#include <stdafx.h>

// The next 3 includes are needed for serial port enumeration
#include <objbase.h>
#include <initguid.h>
#include <Setupapi.h>

#include "EnumSerial.h"

// The following define is from ntddser.h in the DDK. It is also
// needed for serial port enumeration.
#ifndef GUID_CLASS_COMPORT
DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, \
0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
#endif


struct SSerInfo {
    SSerInfo() : bUsbDevice(FALSE) {}
    CString strDevPath;          // Device path for use with CreateFile()
    CString strPortName;         // Simple name (i.e. COM1)
    CString strFriendlyName;     // Full name to be displayed to a user
    BOOL bUsbDevice;             // Provided through a USB connection?
    CString strPortDesc;         // friendly name without the COMx
};

//---------------------------------------------------------------
// Helpers for enumerating the available serial ports.
// These throw a CString on failure, describing the nature of
// the error that occurred.

void EnumPortsWdm(CArray<SSerInfo,SSerInfo&> &asi);
void EnumPortsWNt4(CArray<SSerInfo,SSerInfo&> &asi);
void EnumPortsW9x(CArray<SSerInfo,SSerInfo&> &asi);
void SearchPnpKeyW9x(HKEY hkPnp, BOOL bUsbDevice,
                     CArray<SSerInfo,SSerInfo&> &asi);


//---------------------------------------------------------------
// Routine for enumerating the available serial ports.
// Throws a CString on failure, describing the error that
// occurred. If bIgnoreBusyPorts is TRUE, ports that can't
// be opened for read/write access are not included.

void EnumSerialPorts(CArray<SSerInfo,SSerInfo&> &asi, BOOL bIgnoreBusyPorts)
{
    // Clear the output array
    asi.RemoveAll();

    // Use different techniques to enumerate the available serial
    // ports, depending on the OS we're using
    OSVERSIONINFO vi;
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (!::GetVersionEx(&vi)) {
        CString str;
        str.Format("Could not get OS version. (err=%lx)",
                   GetLastError());
        throw str;
    }
    // Handle windows 9x and NT4 specially
    if (vi.dwMajorVersion < 5) {
        if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
            EnumPortsWNt4(asi);
        else
            EnumPortsW9x(asi);
    }
    else {
        // Win2k and later support a standard API for
        // enumerating hardware devices.
        EnumPortsWdm(asi);
    }

    for (int ii=0; ii<asi.GetSize(); ii++)
    {
        SSerInfo& rsi = asi[ii];
        if (bIgnoreBusyPorts) {
            // Only display ports that can be opened for read/write
            HANDLE hCom = CreateFile(rsi.strDevPath,
                                     GENERIC_READ | GENERIC_WRITE,
                                     0,    /* comm devices must be opened w/exclusive-access */
                                     NULL, /* no security attrs */
                                     OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
                                     0,    /* not overlapped I/O */
                                     NULL  /* hTemplate must be NULL for comm devices */
            );
            if (hCom == INVALID_HANDLE_VALUE) {
                // It can't be opened; remove it.
                asi.RemoveAt(ii);
                ii--;
                continue;
            }
            else {
                // It can be opened! Close it and add it to the list
                ::CloseHandle(hCom);
            }
        }

        // Come up with a name for the device.
        // If there is no friendly name, use the port name.
        if (rsi.strFriendlyName.IsEmpty())
            rsi.strFriendlyName = rsi.strPortName;

        // If there is no description, try to make one up from
            // the friendly name.
            if (rsi.strPortDesc.IsEmpty()) {
                // If the port name is of the form "ACME Port (COM3)"
                // then strip off the " (COM3)"
                rsi.strPortDesc = rsi.strFriendlyName;
                int startdex = rsi.strPortDesc.Find(" (");
                int enddex = rsi.strPortDesc.Find(")");
                if (startdex > 0 && enddex ==
                    (rsi.strPortDesc.GetLength()-1))
                    rsi.strPortDesc = rsi.strPortDesc.Left(startdex);
            }
    }
}

// Helpers for EnumSerialPorts

void EnumPortsWdm(CArray<SSerInfo,SSerInfo&> &asi)
{
    CString strErr;
    // Create a device information set that will be the container for
    // the device interfaces.
    GUID *guidDev = (GUID*) &GUID_CLASS_COMPORT;

    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
    SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;

    try {
        hDevInfo = SetupDiGetClassDevs( guidDev,
                                        NULL,
                                        NULL,
                                        DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
        );

        if(hDevInfo == INVALID_HANDLE_VALUE)
        {
            strErr.Format("SetupDiGetClassDevs failed. (err=%lx)",
                          GetLastError());
            throw strErr;
        }

        // Enumerate the serial ports
        BOOL bOk = TRUE;
        SP_DEVICE_INTERFACE_DATA ifcData;
        DWORD dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
        pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
        // This is required, according to the documentation. Yes,
        // it's weird.
        ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        for (DWORD ii=0; bOk; ii++) {
            bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
                                              NULL, guidDev, ii, &ifcData);
            if (bOk) {
                // Got a device. Get the details.
                SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};
                bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
                                                      &ifcData, pDetData, dwDetDataSize, NULL, &devdata);
                if (bOk) {
                    CString strDevPath(pDetData->DevicePath);
                    // Got a path to the device. Try to get some more info.
                    TCHAR fname[256];
                    TCHAR desc[256];
                    BOOL bSuccess = SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_FRIENDLYNAME, NULL,
                        (PBYTE)fname, sizeof(fname), NULL);
                    bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
                        (PBYTE)desc, sizeof(desc), NULL);
                    BOOL bUsbDevice = FALSE;
                    TCHAR locinfo[256];
                    if (SetupDiGetDeviceRegistryProperty(
                        hDevInfo, &devdata, SPDRP_LOCATION_INFORMATION, NULL,
                        (PBYTE)locinfo, sizeof(locinfo), NULL))
                    {
                        // Just check the first three characters to determine
                        // if the port is connected to the USB bus. This isn't
                        // an infallible method; it would be better to use the
                        // BUS GUID. Currently, Windows doesn't let you query
                        // that though (SPDRP_BUSTYPEGUID seems to exist in
                        // documentation only).
                        bUsbDevice = (strncmp(locinfo, "USB", 3)==0);
                    }
                    if (bSuccess) {
                        // Add an entry to the array
                        SSerInfo si;
                        si.strDevPath = strDevPath;
                        si.strFriendlyName = fname;
                        si.strPortDesc = desc;
                        si.bUsbDevice = bUsbDevice;
                        asi.Add(si);
                    }

                }
                else {
                    strErr.Format("SetupDiGetDeviceInterfaceDetail failed. (err=%lx)",
                                  GetLastError());
                    throw strErr;
                }
            }
            else {
                DWORD err = GetLastError();
                if (err != ERROR_NO_MORE_ITEMS) {
                    strErr.Format("SetupDiEnumDeviceInterfaces failed. (err=%lx)", err);
                    throw strErr;
                }
            }
        }
    }
    catch (CString strCatchErr) {
        strErr = strCatchErr;
    }

    if (pDetData != NULL)
        delete [] (char*)pDetData;
    if (hDevInfo != INVALID_HANDLE_VALUE)
        SetupDiDestroyDeviceInfoList(hDevInfo);

    if (!strErr.IsEmpty())
        throw strErr;
}

void EnumPortsWNt4(CArray<SSerInfo,SSerInfo&> &asi)
{
    // NT4's driver model is totally different, and not that
    // many people use NT4 anymore. Just try all the COM ports
    // between 1 and 16
    SSerInfo si;
    for (int ii=1; ii<=16; ii++) {
        CString strPort;
        strPort.Format("COM%d",ii);
        si.strDevPath = CString("\\\\.\\") + strPort;
        si.strPortName = strPort;
        asi.Add(si);
    }
}

void EnumPortsW9x(CArray<SSerInfo,SSerInfo&> &asi)
{
    // Look at all keys in HKLM\Enum, searching for subkeys named
    // *PNP0500 and *PNP0501. Within these subkeys, search for
    // sub-subkeys containing value entries with the name "PORTNAME"
    // Search all subkeys of HKLM\Enum\USBPORTS for PORTNAME entries.

    // First, open HKLM\Enum
    HKEY hkEnum = NULL;
    HKEY hkSubEnum = NULL;
    HKEY hkSubSubEnum = NULL;

    try {
        if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Enum", 0, KEY_READ,
            &hkEnum) != ERROR_SUCCESS)
            throw CString("Could not read from HKLM\\Enum");

        // Enumerate the subkeys of HKLM\Enum
            char acSubEnum[128];
            DWORD dwSubEnumIndex = 0;
            DWORD dwSize = sizeof(acSubEnum);
            while (RegEnumKeyEx(hkEnum, dwSubEnumIndex++, acSubEnum, &dwSize,
                NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
            {
                HKEY hkSubEnum = NULL;
                if (RegOpenKeyEx(hkEnum, acSubEnum, 0, KEY_READ,
                    &hkSubEnum) != ERROR_SUCCESS)
                    throw CString("Could not read from HKLM\\Enum\\")+acSubEnum;

                // Enumerate the subkeys of HKLM\Enum\*\, looking for keys
                    // named *PNP0500 and *PNP0501 (or anything in USBPORTS)
                    BOOL bUsbDevice = (strcmp(acSubEnum,"USBPORTS")==0);
                    char acSubSubEnum[128];
                    dwSize = sizeof(acSubSubEnum);  // set the buffer size
                    DWORD dwSubSubEnumIndex = 0;
                    while (RegEnumKeyEx(hkSubEnum, dwSubSubEnumIndex++, acSubSubEnum,
                        &dwSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
                    {
                        BOOL bMatch = (strcmp(acSubSubEnum,"*PNP0500")==0 ||
                        strcmp(acSubSubEnum,"*PNP0501")==0 ||
                        bUsbDevice);
                        if (bMatch) {
                            HKEY hkSubSubEnum = NULL;
                            if (RegOpenKeyEx(hkSubEnum, acSubSubEnum, 0, KEY_READ,
                                &hkSubSubEnum) != ERROR_SUCCESS)
                                throw CString("Could not read from HKLM\\Enum\\") +
                                acSubEnum + "\\" + acSubSubEnum;
                            SearchPnpKeyW9x(hkSubSubEnum, bUsbDevice, asi);
                            RegCloseKey(hkSubSubEnum);
                            hkSubSubEnum = NULL;
                        }

                        dwSize = sizeof(acSubSubEnum);  // restore the buffer size
                    }

                    RegCloseKey(hkSubEnum);
                    hkSubEnum = NULL;
                    dwSize = sizeof(acSubEnum); // restore the buffer size
            }
    }
    catch (CString strError) {
        if (hkEnum != NULL)
            RegCloseKey(hkEnum);
        if (hkSubEnum != NULL)
            RegCloseKey(hkSubEnum);
        if (hkSubSubEnum != NULL)
            RegCloseKey(hkSubSubEnum);
        throw strError;
    }

    RegCloseKey(hkEnum);
}

void SearchPnpKeyW9x(HKEY hkPnp, BOOL bUsbDevice,
                     CArray<SSerInfo,SSerInfo&> &asi)
{
    // Enumerate the subkeys of the given PNP key, looking for values with
    // the name "PORTNAME"
    // First, open HKLM\Enum
    HKEY hkSubPnp = NULL;

    try {
        // Enumerate the subkeys of HKLM\Enum\*\PNP050[01]
        char acSubPnp[128];
        DWORD dwSubPnpIndex = 0;
        DWORD dwSize = sizeof(acSubPnp);
        while (RegEnumKeyEx(hkPnp, dwSubPnpIndex++, acSubPnp, &dwSize,
            NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
        {
            HKEY hkSubPnp = NULL;
            if (RegOpenKeyEx(hkPnp, acSubPnp, 0, KEY_READ,
                &hkSubPnp) != ERROR_SUCCESS)
                throw CString("Could not read from HKLM\\Enum\\...\\")
                + acSubPnp;

            // Look for the PORTNAME value
                char acValue[128];
                dwSize = sizeof(acValue);
                if (RegQueryValueEx(hkSubPnp, "PORTNAME", NULL, NULL, (BYTE*)acValue,
                    &dwSize) == ERROR_SUCCESS)
                {
                    CString strPortName(acValue);

                    // Got the portname value. Look for a friendly name.
                    CString strFriendlyName;
                    dwSize = sizeof(acValue);
                    if (RegQueryValueEx(hkSubPnp, "FRIENDLYNAME", NULL, NULL, (BYTE*)acValue,
                        &dwSize) == ERROR_SUCCESS)
                        strFriendlyName = acValue;

                    // Prepare an entry for the output array.
                        SSerInfo si;
                        si.strDevPath = CString("\\\\.\\") + strPortName;
                        si.strPortName = strPortName;
                        si.strFriendlyName = strFriendlyName;
                        si.bUsbDevice = bUsbDevice;

                        // Overwrite duplicates.
                        BOOL bDup = FALSE;
                        for (int ii=0; ii<asi.GetSize() && !bDup; ii++)
                        {
                            if (asi[ii].strPortName == strPortName) {
                                bDup = TRUE;
                                asi[ii] = si;
                            }
                        }
                        if (!bDup) {
                            // Add an entry to the array
                            asi.Add(si);
                        }
                }

                RegCloseKey(hkSubPnp);
                hkSubPnp = NULL;
                dwSize = sizeof(acSubPnp);  // restore the buffer size
        }
    }
    catch (CString strError) {
        if (hkSubPnp != NULL)
            RegCloseKey(hkSubPnp);
        throw strError;
    }
}

#endif

bool ReloadLocale() {
  bool ret = false;

#if wxUSE_XLOCALE
  ret =
      (!g_Platform->ChangeLocale(g_locale, plocale_def_lang, &plocale_def_lang)
            .IsEmpty());
#endif
  return ret;
}

void ApplyLocale() {
	FontMgr::Get().SetLocale(g_locale);
	FontMgr::Get().ScrubList();
  //  Close and re-init various objects to allow new locale to show.
  delete g_options;
  g_options = NULL;
  g_pOptions = NULL;

  //  Process the menubar, if present.
  if (gFrame->m_pMenuBar) {  // remove the menu bar if it is presently enabled
    gFrame->SetMenuBar(NULL);
    gFrame->m_pMenuBar->Destroy();
    gFrame->m_pMenuBar = NULL;
  }
  gFrame->BuildMenuBar();

  //  Give all canvas a chance to update, if needed
  for (unsigned int i = 0; i < g_canvasArray.GetCount(); i++) {
    ChartCanvas *cc = g_canvasArray.Item(i);
    if (cc) cc->CanvasApplyLocale();
  }

  // Capture a copy of the current perspective
  //  So that we may restore PlugIn window sizes, position, visibility, etc.
  wxString perspective;
  pConfig->SetPath(_T ( "/AUI" ));
  pConfig->Read(_T ( "AUIPerspective" ), &perspective);

  //  Compliant Plugins will reload their locale message catalog during the
  //  Init() method. So it is sufficient to simply deactivate, and then
  //  re-activate, all "active" plugins.    

  //         // Make sure the perspective saved in the config file is
  //         "reasonable"
  //         // In particular, the perspective should have an entry for every
  //         // windows added to the AUI manager so far.
  //         // If any are not found, then use the default layout
  //
  bool bno_load = false;
  wxAuiPaneInfoArray pane_array_val = g_pauimgr->GetAllPanes();

  for (unsigned int i = 0; i < pane_array_val.GetCount(); i++) {
    wxAuiPaneInfo pane = pane_array_val[i];
    if (perspective.Find(pane.name) == wxNOT_FOUND) {
      bno_load = true;
      break;
    }
  }

  if (!bno_load) g_pauimgr->LoadPerspective(perspective, false);

  g_pauimgr->Update();

  if (gFrame) {
    gFrame->RequestNewToolbars(true);
    gFrame->RequestNewMasterToolbar(true);
  }
}

extern s57RegistrarMgr *m_pRegistrarMan;
extern wxString g_UserPresLibData;
extern wxString g_SENCPrefix;
extern bool g_bportable;
extern wxString g_csv_locn;
extern SENCThreadManager *g_SencThreadManager;

void LoadS57() {
  if (ps52plib)  // already loaded?
    return;

  //  Start a SENC Thread manager
  g_SencThreadManager = new SENCThreadManager();

  //      Set up a useable CPL library error handler for S57 stuff
  // FIXME (dave) Verify after moving LoadS57
  // CPLSetErrorHandler(MyCPLErrorHandler);

  //      Init the s57 chart object, specifying the location of the required csv
  //      files
  g_csv_locn = g_Platform->GetSharedDataDir();
  g_csv_locn.Append(_T("s57data"));

  if (g_bportable) {
    g_csv_locn = _T(".");
    appendOSDirSlash(&g_csv_locn);
    g_csv_locn.Append(_T("s57data"));
  }

  //      If the config file contains an entry for SENC file prefix, use it.
  //      Otherwise, default to PrivateDataDir
  if (g_SENCPrefix.IsEmpty()) {
    g_SENCPrefix = g_Platform->GetPrivateDataDir();
    appendOSDirSlash(&g_SENCPrefix);
    g_SENCPrefix.Append(_T("SENC"));
  }

  if (g_bportable) {
    wxFileName f(g_SENCPrefix);
    if (f.MakeRelativeTo(g_Platform->GetPrivateDataDir()))
      g_SENCPrefix = f.GetFullPath();
    else
      g_SENCPrefix = _T("SENC");
  }

  //      If the config file contains an entry for PresentationLibraryData, use
  //      it. Otherwise, default to conditionally set spot under g_pcsv_locn
  wxString plib_data;
  bool b_force_legacy = false;

  if (g_UserPresLibData.IsEmpty()) {
    plib_data = g_csv_locn;
    appendOSDirSlash(&plib_data);
    plib_data.Append(_T("S52RAZDS.RLE"));
  } else {
    plib_data = g_UserPresLibData;
    b_force_legacy = true;
  }

  ps52plib = new s52plib(plib_data, g_Platform, b_force_legacy);

  //  If the library load failed, try looking for the s57 data elsewhere

  //  First, look in UserDataDir
  /*    From wxWidgets documentation

   wxStandardPaths::GetUserDataDir
   wxString GetUserDataDir() const
   Return the directory for the user-dependent application data files:
   * Unix: ~/.appname
   * Windows: C:\Documents and Settings\username\Application Data\appname
   * Mac: ~/Library/Application Support/appname
   */

  if (!ps52plib->m_bOK) {
    delete ps52plib;

    wxStandardPaths &std_path = g_Platform->GetStdPaths();

    wxString look_data_dir;
    look_data_dir.Append(std_path.GetUserDataDir());
    appendOSDirSlash(&look_data_dir);
    wxString tentative_SData_Locn = look_data_dir;
    look_data_dir.Append(_T("s57data"));

    plib_data = look_data_dir;
    appendOSDirSlash(&plib_data);
    plib_data.Append(_T("S52RAZDS.RLE"));

    wxLogMessage(_T("Looking for s57data in ") + look_data_dir);
    ps52plib = new s52plib(plib_data, g_Platform);

    if (ps52plib->m_bOK) {
      g_csv_locn = look_data_dir;
      ///???            g_SData_Locn = tentative_SData_Locn;
    }
  }

  //  And if that doesn't work, look again in the original SData Location
  //  This will cover the case in which the .ini file entry is corrupted or
  //  moved

  if (!ps52plib->m_bOK) {
    delete ps52plib;

    wxString look_data_dir;
    look_data_dir = g_Platform->GetSharedDataDir();
    look_data_dir.Append(_T("s57data"));

    plib_data = look_data_dir;
    appendOSDirSlash(&plib_data);
    plib_data.Append(_T("S52RAZDS.RLE"));

    wxLogMessage(_T("Looking for s57data in ") + look_data_dir);
    ps52plib = new s52plib(plib_data, g_Platform);

    if (ps52plib->m_bOK) g_csv_locn = look_data_dir;
  }

  if (ps52plib->m_bOK) {
    wxLogMessage(_T("Using s57data in ") + g_csv_locn);
    m_pRegistrarMan =
        new s57RegistrarMgr(g_csv_locn, g_Platform->GetLogFilePtr());

    //    Preset some object class visibilites for "User Standard" disply
    //    category
    //  They may be overridden in LoadS57Config
    for (unsigned int iPtr = 0; iPtr < ps52plib->pOBJLArray->GetCount();
         iPtr++) {
      OBJLElement *pOLE = (OBJLElement *)(ps52plib->pOBJLArray->Item(iPtr));
      if (!strncmp(pOLE->OBJLName, "DEPARE", 6)) pOLE->nViz = 1;
      if (!strncmp(pOLE->OBJLName, "LNDARE", 6)) pOLE->nViz = 1;
      if (!strncmp(pOLE->OBJLName, "COALNE", 6)) pOLE->nViz = 1;
    }

    pConfig->LoadS57Config();
    ps52plib->SetPLIBColorScheme(global_color_scheme, ChartCtxFactory());

    if (gFrame){
      ps52plib->SetPPMM(g_BasePlatform->GetDisplayDPmm());
      double dip_factor = g_BasePlatform->GetDisplayDIPMult(gFrame);
      ps52plib->SetDIPFactor(dip_factor);
      ps52plib->SetContentScaleFactor(OCPN_GetDisplayContentScaleFactor());
    }

    // preset S52 PLIB scale factors
    ps52plib->SetScaleFactorExp(g_Platform->GetChartScaleFactorExp(g_ChartScaleFactor));
    ps52plib-> SetScaleFactorZoomMod(g_chart_zoom_modifier_vector);

#ifdef ocpnUSE_GL

    // Setup PLIB OpenGL options, if enabled
    extern bool g_b_EnableVBO;
    extern GLenum g_texture_rectangle_format;    

    if (g_bopengl){      
      ps52plib->SetGLOptions(
          glChartCanvas::s_b_useStencil, glChartCanvas::s_b_useStencilAP,
          glChartCanvas::s_b_useScissorTest, glChartCanvas::s_b_useFBO,
          g_b_EnableVBO, g_texture_rectangle_format, 1, 1);

    }
#endif

  } else {
    printf("   S52PLIB Initialization failed, disabling Vector charts.\n");
    
    delete ps52plib;
    ps52plib = NULL;
  }
}

class ParseENCWorkerThread : public wxThread {
public:
  ParseENCWorkerThread(wxString filename, Extent &ext, int scale)
      : wxThread(wxTHREAD_JOINABLE) {
    m_filename = filename;
    m_ext = ext;
    m_scale = scale;
    Create();
  }

  void *Entry() {
    //         ChartBase *pchart = ChartData->OpenChartFromDB(m_filename,
    //         FULL_INIT); ChartData->DeleteCacheChart(pchart);
    s57chart *newChart = new s57chart;

    newChart->SetNativeScale(m_scale);
    newChart->SetFullExtent(m_ext);

    newChart->FindOrCreateSenc(m_filename);
    delete newChart;
    return 0;
  }

  wxString m_filename;
  Extent m_ext;
  int m_scale;
};