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

#include <algorithm>
#include <limits.h>
#include <memory>

#ifdef __WXMSW__
#include <math.h>
#include <psapi.h>
#include <stdlib.h>
#include <time.h>
#endif

#include <wx/app.h>
#include <wx/apptrait.h>
#include <wx/arrimpl.cpp>
#include <wx/artprov.h>
#include <wx/aui/aui.h>
#include <wx/clrpicker.h>
#include <wx/dialog.h>
#include <wx/dialog.h>
#include <wx/dir.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/ipc.h>
#include <wx/jsonreader.h>
#include <wx/listctrl.h>
#include <wx/printdlg.h>
#include <wx/print.h>
#include <wx/progdlg.h>
#include <wx/settings.h>
#include <wx/stdpaths.h>
#include <wx/tokenzr.h>
#include <wx/cmdline.h>
#include <wx/display.h>

#include <iostream>
#include <locale>
#include "config_vars.h"
#include "conn_params.h"
#include "dychart.h"
#include "chartdb.h"
#include "chcanv.h"
#include "navutil.h"
#include "OCPNPlatform.h"
#include "base_platform.h"
#include "ocpn_frame.h"
#include "logger.h"
#include "OCPN_AUIManager.h"
#include "ConfigMgr.h"

#include "SencManager.h"
#include "Layer.h"
#include "s57RegistrarMgr.h"
#include "S57ClassRegistrar.h"
#include "thumbwin.h"
#include "s52plib.h"
#include "s57chart.h"
#include "chartdbs.h"
#include "tcmgr.h"
#include "styles.h"
#include "comm_vars.h"
#include "FontMgr.h"

#include "ais_decoder.h"
#include "AISTargetAlertDialog.h"
#include "AISTargetQueryDialog.h"
#include "MainApp.h"

#ifndef __WXMSW__
#include <setjmp.h>
#include <signal.h>
#endif

#ifdef OCPN_HAVE_X11
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#endif


#ifdef __linux__
#include "udev_rule_mgr.h"
#endif

#ifdef ocpnUSE_GL
#include "glChartCanvas.h"
#endif

#ifdef __WXOSX__
#include "macutils.h"
#endif

#ifdef __WXMSW__
void RedirectIOToConsole();
#endif

#if defined(__WXMSW__) && defined (__MSVC__LEAK)
#include "Stackwalker.h"
#endif

#ifdef LINUX_CRASHRPT
#include "crashprint.h"
#endif

WX_DEFINE_OBJARRAY(ArrayOfCDI);

OCPNPlatform* g_Platform;
BasePlatform* g_BasePlatform;
MyFrame* gFrame;
MyConfig* pConfig;

options* g_options;
wxString g_uiStyle;

wxString ChartListFileName;
wxString* pInit_Chart_Dir;
wxString g_winPluginDir;  // Base plugin directory on Windows.

ThumbWin* pthumbwin;
S57ClassRegistrar* g_poRegistrar;

wxDateTime g_start_time;
wxDateTime g_loglast_time;

int Usercolortable_index;
wxArrayPtrVoid* UserColorTableArray;
wxArrayPtrVoid* UserColourHashTableArray;
wxColorHashMap* pcurrent_user_color_hash;

wxPageSetupData* g_pageSetupData = (wxPageSetupData*)NULL;

WX_DEFINE_ARRAY_PTR(ChartCanvas*, arrayofCanvasPtr);

arrayofCanvasPtr g_canvasArray;
ocpnStyle::StyleManager* g_StyleManager;

bool g_bShowOutlines;
bool g_bShowDepthUnits;
bool g_bDisplayGrid;  // Flag indicating weather the lat/lon grid should be
					  // displayed

int g_sticky_chart;
int g_sticky_projection;

int g_nAWMax;
bool g_bPlayShipsBells;
bool g_bFullscreenToolbar;
bool g_bShowLayers;
int g_nAutoHideToolbar;
bool g_bAutoHideToolbar;

int g_pNavAidRadarRingsStepUnits;
int g_iWaypointRangeRingsNumber;
int g_iWaypointRangeRingsStepUnits;
wxColour g_colourWaypointRangeRingsColour;
bool g_bWayPointPreventDragging;
wxColour g_colourOwnshipRangeRingsColour;
bool g_bShowWptName;
int g_maxzoomin;

bool g_bTrackActive;
bool g_bportable;
bool g_benableUDPNullHeader;
bool g_btrackContinuous;
bool bGPSValid;
bool bVelocityValid;
bool AnchorAlertOn1, AnchorAlertOn2;
bool s_bSetSystemTime;

float g_selection_radius_mm = 2.0;
float g_selection_radius_touch_mm = 10.0;

double vLat, vLon;
double initial_scale_ppm, initial_rotation;

ColorScheme global_color_scheme = GLOBAL_COLOR_SCHEME_DAY;

AISTargetAlertDialog* g_pais_alert_dialog_active;
AISTargetQueryDialog* g_pais_query_dialog_active;

bool g_bShowCurrent;
double g_ChartNotRenderScaleFactor;
int g_LayerIdx;

int g_restore_stackindex;
int g_restore_dbindex;
int g_AndroidVersionCode;

int g_last_ChartScaleFactor;
int g_nbrightness = 100;

int g_nframewin_x;
int g_nframewin_y;
int g_nframewin_posx;
int g_nframewin_posy;
bool g_bframemax;
int g_nDepthUnitDisplay;

wxString g_UserPresLibData;

bool g_b_overzoom_x = true;  // Allow high overzoom

int g_click_stop;

bool g_bPauseTest;
bool g_bSleep;
bool g_boptionsactive;

bool g_bShowTrue = true;
bool g_bShowMag;
bool g_bShowStatusBar;

bool g_bUIexpert;
bool g_bFullscreen;

OCPN_AUIManager* g_pauimgr;
wxAuiDefaultDockArt* g_pauidockart;

char nmea_tick_chars[] = { '|', '/', '-', '\\', '|', '/', '-', '\\' };

LayerList* pLayerList;
s52plib* ps52plib;
s57RegistrarMgr* m_pRegistrarMan;
ChartDB* ChartData;
SENCThreadManager* g_SencThreadManager;

wxString g_vs;
bool g_bFirstRun;
bool g_bUpgradeInProcess;

wxString g_compatOS;
wxString g_compatOsVersion;

ChartCanvas* g_focusCanvas;
ChartCanvas* g_overlayCanvas;
TCMgr* ptcmgr;

int g_unit_test_1;
int g_unit_test_2;
bool g_start_fullscreen;
bool g_rebuild_gl_cache;
bool g_parse_all_enc;

// Files specified on the command line, if any.
wxVector<wxString> g_params;

ChartBase* Current_Vector_Ch;
wxString* pdir_list[20];

bool g_bIsNewLayer;
bool g_bLayerViz;

bool bDBUpdateInProgress;

bool g_bshowToolbar = true;
bool g_bexpert = true;
bool g_bBasicMenus = false;

bool bDrawCurrentValues;

wxString AISTargetNameFileName;
wxString gWorldMapLocation, gDefaultWorldMapLocation;
wxString g_csv_locn;
wxString g_SENCPrefix;
wxString g_VisibleLayers;
wxString g_InvisibleLayers;
wxString g_VisiNameinLayers;
wxString g_InVisiNameinLayers;

AisDecoder* g_pAIS;

bool g_bcompression_wait;
bool g_FlushNavobjChanges;
int g_FlushNavobjChangesTimeout;

wxString g_uploadConnection;

int user_user_id;
int file_user_id;

int quitflag;
int g_tick = 0;
int g_mem_total, g_mem_used, g_mem_initial;

static unsigned int malloc_max;

wxString g_anchorwatch_sound_file;
wxString g_DSC_sound_file;
wxString g_SART_sound_file;
wxString g_AIS_sound_file;

double AnchorPointMinDist;
bool g_bCruising;

ChartDummy* pDummyChart;

// Global print data, to remember settings during the session
wxPrintData* g_printData = (wxPrintData*)NULL;

// Global page setup data
					  // displayed
bool g_bShowChartBar;
bool g_bShowActiveRouteHighway;
int g_nAWDefault;
bool g_bTransparentToolbar;
bool g_bTransparentToolbarInOpenGLOK;

bool g_bPermanentMOBIcon;
bool g_bTempShowMenuBar;

int g_iSDMMFormat;
int g_iDistanceFormat;
int g_iSpeedFormat;
int g_iTempFormat;

int g_iNavAidRadarRingsNumberVisible;
float g_fNavAidRadarRingsStep;
float g_fWaypointRangeRingsStep;
bool g_bConfirmObjectDelete;
int g_iWpt_ScaMin;
bool g_bUseWptScaMin;
// Set default color scheme
int gGPS_Watchdog;

int gHDx_Watchdog;
int gHDT_Watchdog;
int gVAR_Watchdog;

int gSAT_Watchdog;

bool g_bDebugCM93;
bool g_bDebugS57;

bool g_bfilter_cogsog;
int g_COGFilterSec = 1;
int g_SOGFilterSec;

int g_ChartUpdatePeriod;
int g_SkewCompUpdatePeriod;

int g_lastClientRectx;
int g_lastClientRecty;
int g_lastClientRectw;
int g_lastClientRecth;
double g_display_size_mm;
double g_config_display_size_mm;
bool g_config_display_size_manual;

int g_GUIScaleFactor;
int g_ChartScaleFactor;
float g_ChartScaleFactorExp;
float g_MarkScaleFactorExp;
int g_ShipScaleFactor;
float g_ShipScaleFactorExp;
int g_ENCSoundingScaleFactor;
int g_ENCTextScaleFactor;

bool g_bShowTide;

#ifndef __WXMSW__
sigjmp_buf env;  // the context saved by sigsetjmp();
#endif

#ifdef __WXOSX__
#include "macutils.h"
#endif

// begin rms
#ifdef __WXOSX__
#ifdef __WXMSW__
#ifdef USE_GLU_TESS
#ifdef USE_GLU_DLL
// end rms
extern bool s_glu_dll_ready;
extern HINSTANCE s_hGLU_DLL;  // Handle to DLL
#endif
#endif
#endif
#endif

double g_ownship_predictor_minutes;
double g_ownship_HDTpredictor_miles;

bool g_own_ship_sog_cog_calc;
int g_own_ship_sog_cog_calc_damp_sec;
wxDateTime last_own_ship_sog_cog_calc_ts;
double last_own_ship_sog_cog_calc_lat, last_own_ship_sog_cog_calc_lon;

bool g_bAIS_CPA_Alert;
bool g_bAIS_CPA_Alert_Audio;
int g_iSoundDeviceIndex;

int g_ais_alert_dialog_x, g_ais_alert_dialog_y;
int g_ais_alert_dialog_sx, g_ais_alert_dialog_sy;
int g_ais_query_dialog_x, g_ais_query_dialog_y;

int g_S57_dialog_sx, g_S57_dialog_sy;

bool g_bAutoAnchorMark;

wxRect g_blink_rect;
double g_PlanSpeed;
wxDateTime g_StartTime;
int g_StartTimeTZ;
IDX_entry* gpIDX;
int gpIDXn;
long gStart_LMT_Offset;

wxArrayString* pMessageOnceArray;

FILE* s_fpdebug;
bool bAutoOpen;

bool g_bUseGLL = true;

int g_nCacheLimit;
int g_memCacheLimit;
bool g_bGDAL_Debug;

double g_VPRotate;  // Viewport rotation angle, used on "Course Up" mode
bool g_bCourseUp;
int g_COGAvgSec = 15;  // COG average period (sec.) for Course Up Mode
double g_COGAvg;
bool g_bLookAhead;
bool g_bskew_comp;
bool g_bopengl;
bool g_bSoftwareGL;
bool g_bShowFPS;
bool g_bsmoothpanzoom;
bool g_fog_overzoom;
double g_overzoom_emphasis_base;
bool g_oz_vector_scale;
double g_plus_minus_zoom_factor;

int g_nCOMPortCheck = 32;

bool g_b_legacy_input_filter_behaviour;  // Support original input filter
										 // process or new process

bool g_bbigred;

bool g_bAISRolloverShowClass;
bool g_bAISRolloverShowCOG;
bool g_bAISRolloverShowCPA;

bool g_bDebugGPSD;

bool g_bFullScreenQuilt = true;
bool g_bQuiltEnable;
bool g_bQuiltStart;

bool g_bdisable_opengl;

ChartGroupArray* g_pGroupArray;

std::vector<std::string> TideCurrentDataSet;
wxString g_TCData_Dir;

bool g_bDeferredInitDone;
int options_lastPage = 0;
int options_subpage = 0;

wxPoint options_lastWindowPos(0, 0);
wxSize options_lastWindowSize(0, 0);

bool g_bsimplifiedScalebar;

int g_grad_default;
wxColour g_border_color_default;
int g_border_size_default;
int g_sash_size_default;
wxColour g_caption_color_default;
wxColour g_sash_color_default;
wxColour g_background_color_default;

int osMajor, osMinor;

bool GetMemoryStatus(int* mem_total, int* mem_used);
bool g_bHasHwClock;


// AIS Global configuration
bool g_bShowAIS;
bool g_bCPAMax;
double g_CPAMax_NM;
bool g_bCPAWarn;
double g_CPAWarn_NM;
bool g_bTCPA_Max;
double g_TCPA_Max;
bool g_bMarkLost;
double g_MarkLost_Mins;
bool g_bRemoveLost;
double g_RemoveLost_Mins;
bool g_bShowCOG;
bool g_bSyncCogPredictors;
double g_ShowCOG_Mins;
bool g_bAISShowTracks;
double g_AISShowTracks_Mins;
double g_AISShowTracks_Limit;
bool g_bHideMoored;
bool g_bAllowShowScaled;
double g_ShowMoored_Kts;
wxString g_sAIS_Alert_Sound_File;
bool g_bAIS_CPA_Alert_Suppress_Moored;
bool g_bAIS_ACK_Timeout;
double g_AckTimeout_Mins;
bool g_bShowScaled;
bool g_bShowAreaNotices;
bool g_bDrawAISSize;
bool g_bDrawAISRealtime;
double g_AIS_RealtPred_Kts;
bool g_bShowAISName;
int g_Show_Target_Name_Scale;
int g_WplAction;

int g_nAIS_activity_timer;

bool g_bEnableZoomToCursor;

bool g_bTrackCarryOver;
bool g_bDeferredStartTrack;
bool g_bTrackDaily;
int g_track_rotate_time;
int g_track_rotate_time_type;
bool g_bHighliteTracks;
int g_route_line_width;
int g_track_line_width;
wxColour g_colourTrackLineColour;
wxString g_default_wp_icon;
wxString g_default_routepoint_icon;

double g_TrackIntervalSeconds;
double g_TrackDeltaDistance;
int g_nTrackPrecision;

int g_total_NMEAerror_messages;

int g_cm93_zoom_factor;
bool g_bShowDetailSlider;
int g_detailslider_dialog_x, g_detailslider_dialog_y;

bool g_bUseGreenShip;

wxString g_AW1GUID;
wxString g_AW2GUID;

int g_OwnShipIconType;
double g_n_ownship_length_meters;
double g_n_ownship_beam_meters;
double g_n_gps_antenna_offset_y;
double g_n_gps_antenna_offset_x;
int g_n_ownship_min_mm;

double g_n_arrival_circle_radius;

bool g_bNeedDBUpdate;
bool g_bPreserveScaleOnX;

#if wxUSE_XLOCALE || !wxCHECK_VERSION(3, 0, 0)
wxLocale* plocale_def_lang;
#endif

wxString g_locale;
wxString g_localeOverride;
bool g_b_assume_azerty;

bool g_bUseRaster;
bool g_bUseVector;
bool g_bUseCM93;

int g_MemFootSec;
int g_MemFootMB;

wxStaticBitmap* g_pStatBoxTool;
bool g_bquiting;
int g_BSBImgDebug;

wxString g_AisTargetList_perspective;
int g_AisTargetList_range;
int g_AisTargetList_sortColumn;
bool g_bAisTargetList_sortReverse;
wxString g_AisTargetList_column_spec;
wxString g_AisTargetList_column_order;
int g_AisTargetList_count;
bool g_bAisTargetList_autosort;

wxString g_toolbarConfig = _T("XXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
wxString g_toolbarConfigSecondary = _T("....XX..X........XXXXXXXXXXXX");

int g_maintoolbar_x;
int g_maintoolbar_y;
long g_maintoolbar_orient;
float g_toolbar_scalefactor;

float g_compass_scalefactor;
bool g_bShowMenuBar;
bool g_bShowCompassWin;

bool g_benable_rotate;

bool g_bMagneticAPB;

bool g_bInlandEcdis;

int g_GPU_MemSize;

double gQueryVar = 361.0;

char bells_sound_file_name[2][12] = { "1bells.wav", "2bells.wav" };

int portaudio_initialized;

bool g_bAIS_GCPA_Alert_Audio;
bool g_bAIS_SART_Alert_Audio;
bool g_bAIS_DSC_Alert_Audio;
bool g_bAnchor_Alert_Audio;

int n_NavMessageShown;
wxString g_config_version_string;

wxString g_CmdSoundString;

bool g_bresponsive;
bool g_bRollover;

bool b_inCompressAllCharts;
bool g_bGLexpert;

int g_chart_zoom_modifier_raster;
int g_chart_zoom_modifier_vector;


bool g_bAdvanceRouteWaypointOnArrivalOnly;

bool g_bSpaceDropMark;

wxArrayString g_locale_catalog_array;
bool b_reloadForPlugins;

unsigned int g_canvasConfig;
bool g_useMUI;
bool g_bmasterToolbarFull = true;

int g_memUsed;

wxString g_lastAppliedTemplateGUID;
bool b_inCloseWindow;

extern ColorScheme GetColorScheme();

void InitializeUserColors(void);

//------------------------------------------------------------------------------
//    PNG Icon resources
//------------------------------------------------------------------------------

#if defined(__WXGTK__) || defined(__WXQT__)
#include "../src/bitmaps/opencpn.xpm"
#endif



wxString newPrivateFileName(wxString home_locn, const char* name,
	const char* windowsName) {
	wxString fname = wxString::FromUTF8(name);
	wxString fwname = wxString::FromUTF8(windowsName);
	wxString filePathAndName;

	filePathAndName = g_Platform->GetPrivateDataDir();
	if (filePathAndName.Last() != wxFileName::GetPathSeparator())
		filePathAndName.Append(wxFileName::GetPathSeparator());

#ifdef __WXMSW__
	filePathAndName.Append(fwname);
#else
	filePathAndName.Append(fname);
#endif

	return filePathAndName;
}

void AddChartCacheFromDB(ChartDB* pChartDB) {
	wxArrayPtrVoid* pChartCacheFromDB = pChartDB->GetChartCache();

	for (unsigned int i = 0; i < pChartDB->active_chartTable.GetCount(); i++) {
		const ChartTableEntry& cte = pChartDB->active_chartTable[i];
		wxString ChartFullPath = cte.GetFullSystemPath();
		ChartTypeEnum chart_type = (ChartTypeEnum)cte.GetChartType();
		ChartFamilyEnum chart_family = (ChartFamilyEnum)cte.GetChartFamily();

		if (!pChartDB->IsCacheLocked()) {
			if (g_memCacheLimit) {
				int mem_used;
				GetMemoryStatus(0, &mem_used);

				//printf("AddChartCacheFromDB : cache size: %d, chart type: %d\n", (int)pChartCacheFromDB->GetCount(), chart_type);
				if ((mem_used > g_memCacheLimit * 8 / 10) && (pChartCacheFromDB->GetCount() > 2)) {
					wxString msg(_T("Removing oldest chart from cache: "));
					while (1) {
						CacheEntry* pce = pChartDB->FindOldestDeleteCandidate(true);
						if (pce == 0) break;  // no possible delete candidate

						// purge texture cache, really need memory here
						pChartDB->DeleteCacheEntry(pce, true, msg);

						GetMemoryStatus(0, &mem_used);
						if ((mem_used < g_memCacheLimit * 8 / 10) || (pChartCacheFromDB->GetCount() <= 2)) break;
					}  // while
				}
			}
			else {  // Use n chart cache policy, if memory-limit  policy is not used			
			 //      Limit cache to n charts, tossing out the oldest when space is
			 //      needed
				unsigned int nCache = pChartCacheFromDB->GetCount();
				if (nCache > (unsigned int)g_nCacheLimit && nCache > 2) {
					wxString msg(_T("Removing oldest chart from cache: "));
					while (nCache > (unsigned int)g_nCacheLimit) {
						CacheEntry* pce = pChartDB->FindOldestDeleteCandidate(true);
						if (pce == 0) break;

						pChartDB->DeleteCacheEntry(pce, true, msg);
						nCache--;
					}
				}
			}
		}

		LoadS57();
		ChartBase* pChartBase = new s57chart();
		s57chart* pENCChart = static_cast<s57chart*>(pChartBase);

		pENCChart->SetNativeScale(cte.GetScale());

		//    Explicitely set the chart extents from the database to
		//    support the case wherein the SENC file has not yet been built
		Extent ext;
		ext.NLAT = cte.GetLatMax();
		ext.SLAT = cte.GetLatMin();
		ext.WLON = cte.GetLonMin();
		ext.ELON = cte.GetLonMax();
		pENCChart->SetFullExtent(ext);

		InitReturn ir;
		s52plib* plib = ps52plib;
		wxString msg_fn(ChartFullPath);
		msg_fn.Replace(_T("%"), _T("%%"));

		if ((chart_family != CHART_FAMILY_VECTOR) || ((chart_family == CHART_FAMILY_VECTOR) && plib)) {
			wprintf(_T("AddChartCacheFromDB : Initializing Chart %s\n"), msg_fn.wc_str());

			ir = pChartBase->Init(ChartFullPath, FULL_INIT);  // using the passed flag
			pChartBase->SetColorScheme(GetColorScheme());
		}
		else {
			wprintf(_T("AddChartCacheFromDB : No PLIB, Skipping vector chart  %s\n"), msg_fn.wc_str());
		}

		if (INIT_OK == ir) {
			CacheEntry* pce = new CacheEntry;
			pce->FullPath = ChartFullPath;
			pce->pChart = pChartBase;
			pce->dbIndex = i;

			pce->n_lock = 0;

			printf("AddChartCacheFromDB : entry is created for %s\n", (const char*)(ChartFullPath.mb_str(wxConvUTF8)));			
		}
		else {
			delete pChartBase;
			pChartBase = NULL;
		}
	}
}

bool ConfigureDB(ArrayOfCDI& sENCDirPath, std::string& sDBFilePath, ChartDB* pChartDB) {
	if (!pChartDB->Create(sENCDirPath)) {
		return false;
	}

	pChartDB->PurgeCache();
	if (!pChartDB->SaveBinary(sDBFilePath)) {
		printf("Fail to produce the DB.\n");
		return false;
	}

	printf("Success to produce the DB.\n");
	return true;
}

bool InitChart(std::string& sDBFilePath, ArrayOfCDI& arrayChartDirs, bool rebuildChartFlag) {
	ChartData = new ChartDB();
	bool bDBBuildFlag = false;
	if (rebuildChartFlag) {
		bDBBuildFlag = ConfigureDB(arrayChartDirs, sDBFilePath, ChartData);
	}
	else {
		FILE* pTestFile = fopen(sDBFilePath.c_str(), "rb");
		if (!pTestFile) {
			bDBBuildFlag = ConfigureDB(arrayChartDirs, sDBFilePath, ChartData);
		}
		else {
			fclose(pTestFile);

			ChartData->PurgeCache();
			bDBBuildFlag = !ChartData->LoadBinary(sDBFilePath, arrayChartDirs);
		}
	}

	AddChartCacheFromDB(ChartData);
	return bDBBuildFlag;
}

// `Main program' equivalent, creating windows and returning main app frame
//------------------------------------------------------------------------------
// MyApp
//------------------------------------------------------------------------------
#ifdef WIN32
IMPLEMENT_APP(MainApp)
#endif

#include <wx/dynlib.h>

#if wxUSE_CMDLINE_PARSER
void MainApp::OnInitCmdLine(wxCmdLineParser & parser) {
	//    Add some OpenCPN specific command line options
	parser.AddSwitch(_T("h"), _T("help"), _("Show usage syntax."),
		wxCMD_LINE_OPTION_HELP);
	parser.AddSwitch(_T("p"), wxEmptyString, _("Run in portable mode."));
	parser.AddSwitch(_T("fullscreen"), wxEmptyString,
		_("Switch to full screen mode on start."));
	parser.AddSwitch(
		_T("no_opengl"), wxEmptyString,
		_("Disable OpenGL video acceleration. This setting will be remembered."));
	parser.AddSwitch(_T("rebuild_gl_raster_cache"), wxEmptyString,
		_T("Rebuild OpenGL raster cache on start."));
	parser.AddSwitch(
		_T("parse_all_enc"), wxEmptyString,
		_T("Convert all S-57 charts to OpenCPN's internal format on start."));
	parser.AddOption(
		_T("l"), _T("loglevel"),
		_("Amount of logging: error, warning, message, info, debug or trace"));
	parser.AddOption(_T("unit_test_1"), wxEmptyString,
		_("Display a slideshow of <num> charts and then exit. Zero "
			"or negative <num> specifies no limit."),
		wxCMD_LINE_VAL_NUMBER);
	parser.AddSwitch(_T("unit_test_2"));
	parser.AddParam("import GPX files", wxCMD_LINE_VAL_STRING,
		wxCMD_LINE_PARAM_OPTIONAL | wxCMD_LINE_PARAM_MULTIPLE);
	parser.AddLongSwitch("unit_test_2");
	parser.AddSwitch("safe_mode");
}

/** Parse --loglevel and set up logging, falling back to defaults. */
static void ParseLoglevel(wxCmdLineParser & parser) {
	const char* strLevel = std::getenv("OPENCPN_LOGLEVEL");
	strLevel = strLevel ? strLevel : "info";
	wxString wxLevel;
	if (parser.Found("l", &wxLevel)) {
		strLevel = wxLevel.c_str();
	}
	wxLogLevel level = OcpnLog::str2level(strLevel);
	if (level == OcpnLog::LOG_BADLEVEL) {
		fprintf(stderr, "Bad loglevel %s, using \"info\"", strLevel);
		strLevel = "info";
		level = wxLOG_Info;
	}
	wxLog::SetLogLevel(level);
}

bool MainApp::OnCmdLineParsed(wxCmdLineParser & parser) {
	long number;
	wxString repo;
	wxString plugin;

	g_unit_test_2 = parser.Found(_T("unit_test_2"));
	g_bportable = parser.Found(_T("p"));
	g_start_fullscreen = parser.Found(_T("fullscreen"));
	g_bdisable_opengl = parser.Found(_T("no_opengl"));
	g_rebuild_gl_cache = parser.Found(_T("rebuild_gl_raster_cache"));
	g_parse_all_enc = parser.Found(_T("parse_all_enc"));
	if (parser.Found(_T("unit_test_1"), &number)) {
		g_unit_test_1 = static_cast<int>(number);
		if (g_unit_test_1 == 0) g_unit_test_1 = -1;
	}
	
	ParseLoglevel(parser);

	for (size_t paramNr = 0; paramNr < parser.GetParamCount(); ++paramNr)
		g_params.push_back(parser.GetParam(paramNr));

	return true;
}
#endif

#ifdef __WXMSW__
//  Handle any exception not handled by CrashRpt
//  Most probable:  Malloc/new failure

bool MainApp::OnExceptionInMainLoop() {
	wxLogWarning(_T("Caught MainLoopException, continuing..."));
	return true;
}
#endif

static wxStopWatch init_sw;

MainApp::MainApp()
{
	m_bDBCreateResult = false;
#ifdef __linux__
	// Handle e. g., wayland default display -- see #1166.

	if (wxGetEnv("WAYLAND_DISPLAY", NULL))
		setenv("GDK_BACKEND", "x11", 1);

#endif   // __linux__
}

bool MainApp::OnInit(std::string& sENCDirPath, bool bRebuildChart) {
//#ifdef __linux__
//	gtk_init();
//#endif
	if (!wxApp::OnInit()) return false;
		g_unit_test_2 = 0;
		g_bportable = true;
		g_start_fullscreen = false;
		g_bdisable_opengl = false;
		g_rebuild_gl_cache = false;
		g_parse_all_enc = false;
		g_unit_test_1 = 0;

// #ifndef WIN32
// 	g_bportable = true;
// #endif

	GpxDocument::SeedRandom();
	last_own_ship_sog_cog_calc_ts = wxInvalidDateTime;

#if defined(__WXGTK__) && defined(ocpnUSE_GLES) && defined(__ARM_ARCH)
	// There is a race condition between cairo which is used for text rendering
	// by gtk and EGL which without the below code causes a bus error and the
	// program aborts before startup
	// this hack forces cairo to load right now by rendering some text

	wxBitmap bmp(10, 10, -1);
	wxMemoryDC dc;
	dc.SelectObject(bmp);
	dc.DrawText(_T("X"), 0, 0);
#endif
	m_checker = 0;	

	g_Platform = new OCPNPlatform;
	g_BasePlatform = g_Platform;

  g_Platform->GetPrivateDataDir();
  //  Perform first stage initialization
  OCPNPlatform::Initialize_1();

	setlocale(LC_NUMERIC, "C");

	g_start_time = wxDateTime::Now();

	AnchorPointMinDist = 5.0;
	malloc_max = 0;

	GetMemoryStatus(&g_mem_total, &g_mem_initial);

	wxPlatformInfo platforminfo = wxPlatformInfo::Get();

	wxString os_name;
	os_name = platforminfo.GetOperatingSystemIdName();

	::wxGetOsVersion(&osMajor, &osMinor);
	OCPN_OSDetail* detail = g_Platform->GetOSDetail();

	::wxInitAllImageHandlers();

	g_Platform->GetSharedDataDir();

	pInit_Chart_Dir = new wxString();
	g_pGroupArray = new ChartGroupArray;

	g_Platform->GetPrivateDataDir();

	pMessageOnceArray = new wxArrayString;

	g_bShowAIS = true;

	pLayerList = new LayerList;
#ifndef __WXMSW__
#ifdef PROBE_PORTS__WITH_HELPER
	user_user_id = getuid();
	file_user_id = geteuid();
#endif
#endif

	bool b_initial_load = false;

	wxFileName config_test_file_name(g_Platform->GetConfigFileName());
	if (config_test_file_name.FileExists()) {
		printf("Using existing Config_File: %s\n", (const char*)(g_Platform->GetConfigFileName().mb_str(wxConvUTF8)));
	}
	else {
		printf("Creating new Config_File: %s\n", (const char*)(g_Platform->GetConfigFileName().mb_str(wxConvUTF8)));

		b_initial_load = true;

		if (true != config_test_file_name.DirExists(config_test_file_name.GetPath())) {
			if (!config_test_file_name.Mkdir(config_test_file_name.GetPath())) {
				printf("Cannot create config file directory for %s\n", (const char *)(g_Platform->GetConfigFileName().mb_str(wxConvUTF8)));
			}
		}
	}

	pConfig = g_Platform->GetConfigObject();
	InitBaseConfig(pConfig);
	pConfig->LoadMyConfig(sENCDirPath);

	if (b_initial_load) g_Platform->SetDefaultOptions();

	g_Platform->applyExpertMode(g_bUIexpert);

	g_StyleManager = new ocpnStyle::StyleManager();
	g_StyleManager->SetStyle(_T("MUI_flat"));
	if (!g_StyleManager->IsOK()) {
		printf("Failed to initialize style manager.\n");
		exit(EXIT_FAILURE);
	}

	g_display_size_mm = wxMax(50, g_Platform->GetDisplaySizeMM());

	if ((g_config_display_size_mm > 0) && (g_config_display_size_manual)) {
		g_display_size_mm = g_config_display_size_mm;
		printf("Display size (horizontal) config override: %d mm\n", (int)g_display_size_mm);
		g_Platform->SetDisplaySizeMM(g_display_size_mm);
	}

	g_display_size_mm = wxMax(50, g_display_size_mm);

	if (!n_NavMessageShown) g_bFirstRun = true;

#if wxUSE_XLOCALE || !wxCHECK_VERSION(3, 0, 0)
	g_Platform->SetLocaleSearchPrefixes();

  wxString cflmsg = _T("Config file language:  ") + g_locale;
  wxLogMessage(cflmsg);

  //  Make any adjustments necessary
  g_locale = g_Platform->GetAdjustedAppLocale();
  cflmsg = _T("Adjusted App language:  ") + g_locale;
  wxLogMessage(cflmsg);

	g_Platform->ChangeLocale(g_locale, plocale_def_lang, &plocale_def_lang);

	if (g_locale == _T("fr_FR")) g_b_assume_azerty = true;
#endif

	ConfigMgr::Get();

	wxString vs = wxString("Version ") + VERSION_FULL + " Build " + VERSION_DATE;
	g_bUpgradeInProcess = (vs != g_config_version_string);

	g_Platform->SetUpgradeOptions(vs, g_config_version_string);

#ifdef ocpnUSE_GL

#ifdef __WXMSW__
#if !wxCHECK_VERSION( \
    2, 9, 0)  // The OpenGL test app only runs on wx 2.8, unavailable on wx3.x

	if (/*g_bopengl &&*/ !g_bdisable_opengl) {
		wxFileName fn(g_Platform->GetExePath());
		bool b_test_result = TestGLCanvas(fn.GetPathWithSep());

		if (!b_test_result)
			wxLogMessage(_T("OpenGL disabled due to test app failure."));

		g_bdisable_opengl = !b_test_result;
	}
#endif
#endif

#else
	g_bdisable_opengl = true;
#endif

	g_bdisable_opengl = true;
	if (g_bdisable_opengl) g_bopengl = false;

#if defined(__UNIX__) && !defined(__OCPN__ANDROID__) && !defined(__WXOSX__)
	if (g_bSoftwareGL) setenv("LIBGL_ALWAYS_SOFTWARE", "1", 1);
#endif

#ifdef __WXMSW__
	if (0 == g_memCacheLimit) g_memCacheLimit = (int)(g_mem_total * 0.5);
	g_memCacheLimit = wxMin(g_memCacheLimit, 1024 * 1024);  // math in kBytes, Max is 1 GB
#else
	g_memCacheLimit = 0;
	if (0 == g_nCacheLimit) {
		g_nCacheLimit = CACHE_N_LIMIT_DEFAULT;
	}
#endif

	ChartListFileName = newPrivateFileName(g_Platform->GetPrivateDataDir(), "chartlist.dat", "CHRTLIST.DAT");

	//      Establish location and name of AIS MMSI -> Target Name mapping
	AISTargetNameFileName = newPrivateFileName(g_Platform->GetPrivateDataDir(), "mmsitoname.csv", "MMSINAME.CSV");

	if (pInit_Chart_Dir->IsEmpty()) {
		wxStandardPaths& std_path = g_Platform->GetStdPaths();

		if (!g_bportable) {
			pInit_Chart_Dir->Append(std_path.GetDocumentsDir());
		}
	}

	gDefaultWorldMapLocation = "gshhs";
	gDefaultWorldMapLocation.Prepend(g_Platform->GetSharedDataDir());
	gDefaultWorldMapLocation.Append(wxFileName::GetPathSeparator());
	if (gWorldMapLocation == wxEmptyString) {
		gWorldMapLocation = gDefaultWorldMapLocation;
	}

	//  Check the global Tide/Current data source array
	//  If empty, preset default (US + ROW) data sources
	wxString default_tcdata0 =
		(g_Platform->GetSharedDataDir() + _T("tcdata") +
			wxFileName::GetPathSeparator() + _T("harmonics-dwf-20210110-free.tcd"));
	wxString default_tcdata1 =
		(g_Platform->GetSharedDataDir() + _T("tcdata") +
			wxFileName::GetPathSeparator() + _T("HARMONICS_NO_US.IDX"));

	if (TideCurrentDataSet.empty()) {
		TideCurrentDataSet.push_back(g_Platform->NormalizePath(default_tcdata0).ToStdString());
		TideCurrentDataSet.push_back(g_Platform->NormalizePath(default_tcdata1).ToStdString());
	}

	gpIDX = NULL;
	gpIDXn = 0;

	g_Platform->Initialize_2();

	wxSize new_frame_size(-1, -1);
	int cx, cy, cw, ch;
	::wxClientDisplayRect(&cx, &cy, &cw, &ch);

	InitializeUserColors();

	if ((g_nframewin_x > 100) && (g_nframewin_y > 100) && (g_nframewin_x <= cw) && (g_nframewin_y <= ch)) {
		cw = g_nframewin_x;
		ch = g_nframewin_y;
		new_frame_size.Set(g_nframewin_x, g_nframewin_y);
	}
	else {
		new_frame_size.Set(cw * 7 / 10, ch * 7 / 10);
	}

	if ((g_lastClientRectx != cx) || (g_lastClientRecty != cy) ||
		(g_lastClientRectw != cw) || (g_lastClientRecth != ch)) {
		new_frame_size.Set(cw * 7 / 10, ch * 7 / 10);
		g_bframemax = false;
	}

	g_lastClientRectx = cx;
	g_lastClientRecty = cy;
	g_lastClientRectw = cw;
	g_lastClientRecth = ch;

	wxPoint position(0, 0);
	wxSize dsize = wxGetDisplaySize();

#ifdef __WXMAC__
	g_nframewin_posy = wxMax(g_nframewin_posy, 22);
#endif

	if ((g_nframewin_posx < dsize.x) && (g_nframewin_posy < dsize.y)) {
		position = wxPoint(g_nframewin_posx, g_nframewin_posy);
	}

#ifdef __WXMSW__
	//  Support MultiMonitor setups which can allow negative window positions.
	RECT frame_rect;
	frame_rect.left = position.x;
	frame_rect.top = position.y;
	frame_rect.right = position.x + new_frame_size.x;
	frame_rect.bottom = position.y + new_frame_size.y;

	//  If the requested frame window does not intersect any installed monitor,
	//  then default to simple primary monitor positioning.
	if (NULL == MonitorFromRect(&frame_rect, MONITOR_DEFAULTTONULL))
		position = wxPoint(10, 10);
#endif

#ifdef __WXOSX__
	//  Support MultiMonitor setups which can allow negative window positions.
	const wxPoint ptScreen(position.x, position.y);
	const int displayIndex = wxDisplay::GetFromPoint(ptScreen);

	if (displayIndex == wxNOT_FOUND) {
		position = wxPoint(10, 30);
	}
#endif

	g_nframewin_posx = position.x;
	g_nframewin_posy = position.y;

	long app_style = wxDEFAULT_FRAME_STYLE;
	app_style |= wxWANTS_CHARS;

	try {
		gFrame = new MyFrame(NULL, "", position, new_frame_size, app_style);
	}
	catch (std::exception & ex) {
		printf("Create Frame Failed! : %s\n", ex.what());
		return false;
	}

	g_Platform->Initialize_3();

	g_pauimgr = new OCPN_AUIManager;
	g_pauidockart = new wxAuiDefaultDockArt;
	g_pauimgr->SetArtProvider(g_pauidockart);
	g_pauimgr->SetDockSizeConstraint(.9, .9);

	g_grad_default = g_pauidockart->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE);
	g_border_color_default =
		g_pauidockart->GetColour(wxAUI_DOCKART_BORDER_COLOUR);
	g_border_size_default =
		g_pauidockart->GetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE);
	g_sash_size_default = g_pauidockart->GetMetric(wxAUI_DOCKART_SASH_SIZE);
	g_caption_color_default =
		g_pauidockart->GetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR);
	g_sash_color_default = g_pauidockart->GetColour(wxAUI_DOCKART_SASH_COLOUR);
	g_background_color_default =
		g_pauidockart->GetColour(wxAUI_DOCKART_BACKGROUND_COLOUR);

	g_pauimgr->SetManagedWindow(gFrame);
	gFrame->CreateCanvasLayout();
	gFrame->SetChartUpdatePeriod();

	gFrame->Enable();

	gFrame->GetPrimaryCanvas()->SetFocus();
	pthumbwin = new ThumbWin(gFrame->GetPrimaryCanvas());

	gFrame->ApplyGlobalSettings(false);  // done once on init with resize

	gFrame->SetAndApplyColorScheme(global_color_scheme);

	if (g_bframemax) gFrame->Maximize(true);

	Yield();

	ArrayOfCDI ChartDirArray;
	pConfig->LoadChartDirArray(ChartDirArray);

	if (!ChartDirArray.GetCount()) {
		if (::wxFileExists(ChartListFileName)) ::wxRemoveFile(ChartListFileName);
	}

	if (bRebuildChart) {		
		gFrame->RebuildChartDatabase();		
		g_bNeedDBUpdate = false;		
	}
	else {
		ChartData = new ChartDB();
		if (!ChartData->LoadBinary(ChartListFileName, ChartDirArray)) {
			gFrame->RebuildChartDatabase();
			g_bNeedDBUpdate = false;
		}
	}
	
	//  Verify any saved chart database startup index
	if (g_restore_dbindex >= 0) {
		if (ChartData->GetChartTableEntries() == 0)
			g_restore_dbindex = -1;

		else if (g_restore_dbindex > (ChartData->GetChartTableEntries() - 1))
			g_restore_dbindex = 0;
	}

	//  Apply the inital Group Array structure to the chart data base
	ChartData->ApplyGroupArray(g_pGroupArray);

	//      All set to go.....

	// Process command line option to rebuild cache
#ifdef ocpnUSE_GL
	extern ocpnGLOptions g_GLOptions;

	if (g_rebuild_gl_cache && g_bopengl && g_GLOptions.m_bTextureCompression &&
		g_GLOptions.m_bTextureCompressionCaching) {
		gFrame->ReloadAllVP();  //  Get a nice chart background loaded

		//      Turn off the toolbar as a clear signal that the system is busy right
		//      now.
		// Note: I commented this out because the toolbar never comes back for me
		// and is unusable until I restart opencpn without generating the cache
		//        if( g_MainToolbar )
		//            g_MainToolbar->Hide();

		if (g_glTextureManager) g_glTextureManager->BuildCompressedCache();
	}
#endif

	if ((gps_watchdog_timeout_ticks > 60) || (gps_watchdog_timeout_ticks <= 0))
		gps_watchdog_timeout_ticks = (GPS_TIMEOUT_SECONDS * 1000) / TIMER_GFRAME_1;

	sat_watchdog_timeout_ticks = gps_watchdog_timeout_ticks;

	g_priSats = 99;

	////  Most likely installations have no ownship heading information
	g_bVAR_Rx = false;

	//  Start up a new track if enabled in config file
	if (g_bTrackCarryOver) g_bDeferredStartTrack = true;

	Yield();

	gFrame->DoChartUpdate();

	FontMgr::Get().ScrubList();  // Clean the font list, removing nonsensical entries

	gFrame->ReloadAllVP();  // once more, and good to go

	gFrame->Refresh(false);
	gFrame->Raise();

	gFrame->GetPrimaryCanvas()->Enable();
	gFrame->GetPrimaryCanvas()->SetFocus();

#ifdef ocpnUSE_GL
	if (!g_bdisable_opengl) {
		glChartCanvas* pgl =
			(glChartCanvas*)gFrame->GetPrimaryCanvas()->GetglCanvas();
		if (pgl &&
			(pgl->GetRendererString().Find(_T("UniChrome")) != wxNOT_FOUND)) {
			gFrame->m_defer_size = gFrame->GetSize();
			gFrame->SetSize(gFrame->m_defer_size.x - 10, gFrame->m_defer_size.y);
			g_pauimgr->Update();
			gFrame->m_bdefer_resize = true;
		}
	}
#endif

	//  
	gFrame->Raise();
	gFrame->GetPrimaryCanvas()->Enable();
	gFrame->GetPrimaryCanvas()->SetFocus();

	printf("Initialize 4 \n");
	OCPNPlatform::Initialize_4();

	wxMilliSleep(500);

	g_bHasHwClock = true;  // by default most computers do have a hwClock
#if defined(__UNIX__) && !defined(__OCPN__ANDROID__)
	struct stat buffer;
	g_bHasHwClock =
		((stat("/dev/rtc", &buffer) == 0) || (stat("/dev/rtc0", &buffer) == 0) ||
		(stat("/dev/misc/rtc", &buffer) == 0));
#endif	
	g_config_version_string = vs;

	pConfig->UpdateSettings();

	printf("MainApp : pauiMGR update\n");
	g_pauimgr->Update();	

	printf("Wait for minutes to prepare... \n");
	gFrame->UpdateDB_Canvas();
	printf("pre-initialize finished! \n");

	return true;
}

char my_tolower(char ch)
{
	return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

void toLowerCase(std::string& str) {
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
}

int MainApp::GetLayerIndex(std::string& sLayerCaption) {	
	toLowerCase(sLayerCaption);

	if (sLayerCaption.compare(LAYER_TEXT_CAPTION) == 0) return LAYER_TEXT;
	else if (sLayerCaption.compare(LAYER_DEPTHS_CAPTION) == 0) return LAYER_DEPTHS;
	else if (sLayerCaption.compare(LAYER_BLLABELS_CAPTION) == 0) return LAYER_BLLABELS;
	else if (sLayerCaption.compare(LAYER_LIGHTS_CAPTION) == 0) return LAYER_LIGHTS;
	else if (sLayerCaption.compare(LAYER_LDESCR_CAPTION) == 0) return LAYER_LDESCR;
	else if (sLayerCaption.compare(LAYER_AINFO_CAPTION) == 0) return LAYER_AINFO;
	else if (sLayerCaption.compare(LAYER_SLVIS_CAPTION) == 0) return LAYER_SLVIS;
	else if (sLayerCaption.compare(LAYER_GRID_CAPTION) == 0) return LAYER_GRID;
	
	return -1;
}

bool MainApp::UpdateFrameCanvas(std::string& sBBox, int nWidth, int nHeight, std::string& sLayers, std::string& sIMGFilePath, bool bPNGFlag)
{
	if (!gFrame || sBBox.empty() || sLayers.empty() || nWidth < 1 || nHeight < 1) return false;

	std::vector<std::string> bbox_parts;
	std::string delimiter = ",";
	size_t pos = 0;

	// Split the sBBox string into four parts using the delimiter ","
	while ((pos = sBBox.find(delimiter)) != std::string::npos) {
		std::string part = sBBox.substr(0, pos);
		bbox_parts.push_back(part);
		sBBox.erase(0, pos + delimiter.length());
	}
	bbox_parts.push_back(sBBox);

	if (bbox_parts.size() != 4) return false;

	// Convert the four parts to double values
	double dMinLat = std::stod(bbox_parts[0]);
	double dMinLon = std::stod(bbox_parts[1]);
	double dMaxLat = std::stod(bbox_parts[2]);
	double dMaxLon = std::stod(bbox_parts[3]);

	LLBBox llbBox;
	llbBox.Set(dMinLat, dMinLon, dMaxLat, dMaxLon);

	std::vector<int> vnLayers;

	pos = 0;
	int nLayerIndex = -1;
	while ((pos = sLayers.find(delimiter)) != std::string::npos) {
		std::string part = sLayers.substr(0, pos);
		nLayerIndex = GetLayerIndex(part);
		if (nLayerIndex >= 0) vnLayers.push_back(nLayerIndex);
		sLayers.erase(0, pos + delimiter.length());
	}

	nLayerIndex = GetLayerIndex(sLayers);
	if (nLayerIndex >= 0) vnLayers.push_back(nLayerIndex);

	ChartCanvas* pCC = gFrame->GetPrimaryCanvas();
	//pCC->SetSize(nWidth, nHeight);
	gFrame->CenterView(pCC, llbBox, nWidth, nHeight);
	gFrame->DoChartUpdate();
	gFrame->ChartsRefresh();

	//gFrame->ResizeManually(nWidth, nHeight);
	//adjust bbox with viewport's size
	/*wxPoint xMin, xMax, xCenter;
	pCC->GetCanvasPointPix(dMinLat, dMinLon, &xMin);
	pCC->GetCanvasPointPix(dMaxLat, dMaxLon, &xMax);
	pCC->GetCanvasPointPix((dMaxLat + dMinLat) / 2, (dMaxLon + dMinLon) / 2, &xCenter);

	int nVWidth, nVHeight;
	nVWidth = pCC->GetVP().pix_width;
	nVHeight = pCC->GetVP().pix_height;*/

	pCC->m_b_paint_enable = true;
	pCC->DrawCanvasData(llbBox, nWidth, nHeight, vnLayers, sIMGFilePath, bPNGFlag);

	return true;
}