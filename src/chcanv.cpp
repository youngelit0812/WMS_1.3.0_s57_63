#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif  // precompiled headers
#include <wx/image.h>
#include <wx/graphics.h>
#include <wx/clipbrd.h>
#include <wx/aui/aui.h>

#include "config.h"

#include <wx/listimpl.cpp>

#include "chcanv.h"

#include "geodesic.h"
#include "styles.h"
#include "piano.h"
#include "navutil.h"
#include "navutil_base.h"
#include "concanv.h"
#include "thumbwin.h"
#include "chartdb.h"
#include "chartimg.h"
#include "cutil.h"
#include "MarkInfo.h"
#include "nav_object_database.h"
#include "tcmgr.h"
#include "ocpn_pixel.h"
#include "ocpndc.h"
#include "multiplexer.h"
#include "timers.h"
#include "tide_time.h"
#include "glTextureDescriptor.h"
#include "ChInfoWin.h"
#include "Quilt.h"
#include "SystemCmdSound.h"
#include "FontMgr.h"

#include "SendToGpsDlg.h"

#include "OCPNRegion.h"
#include "gshhs.h"
#include "wx28compat.h"
#include "OCPN_AUIManager.h"
#include "CanvasConfig.h"
#include "CanvasOptions.h"
#include "ocpn_frame.h"
#include "idents.h"
#include "line_clip.h"

#ifdef ocpnUSE_GL
#include "glChartCanvas.h"
#endif

#include "cm93.h"      // for chart outline draw
#include "s57chart.h"  // for ArrayOfS57Obj
#include "s52plib.h"
#include "s52utils.h"

#include "ais.h"

#ifdef __MSVC__
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new (_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW
#endif

#ifndef __WXMSW__
#include <signal.h>
#include <setjmp.h>

#endif

extern float g_ChartScaleFactorExp;
extern float g_ShipScaleFactorExp;
extern double g_mouse_zoom_sensitivity;

#include <vector>
//#include <wx-3.0/wx/aui/auibar.h>

#if defined(__MSVC__) && (_MSC_VER < 1700)
#define trunc(d) ((d > 0) ? floor(d) : ceil(d))
#endif

//  Define to enable the invocation of a temporary menubar by pressing the Alt
//  key. Not implemented for Windows XP, as it interferes with Alt-Tab
//  processing.
#define OCPN_ALT_MENUBAR 1

//    Profiling support
//#include "/usr/include/valgrind/callgrind.h"

// ----------------------------------------------------------------------------
// Useful Prototypes
// ----------------------------------------------------------------------------
extern bool G_FloatPtInPolygon(MyFlPoint *rgpts, int wnumpts, float x, float y);
extern void catch_signals(int signo);

extern void AlphaBlending(ocpnDC &dc, int x, int y, int size_x, int size_y,
                          float radius, wxColour color,
                          unsigned char transparency);

extern double g_ChartNotRenderScaleFactor;
extern double gLat, gLon, gCog, gSog, gHdt;

extern ChartDB *ChartData;
extern bool bDBUpdateInProgress;
extern ColorScheme global_color_scheme;
extern int g_nbrightness;

extern ConsoleCanvas *console;
extern OCPNPlatform *g_Platform;

extern RouteList *pRouteList;
extern std::vector<Track*> g_TrackList;
extern MyConfig *pConfig;

extern Routeman *g_pRouteMan;
extern ThumbWin *pthumbwin;
extern TCMgr *ptcmgr;
extern MarkInfoDlg *g_pMarkInfoDialog;
extern double AnchorPointMinDist;
extern bool AnchorAlertOn1;
extern bool AnchorAlertOn2;
extern int g_nAWMax;
extern int g_iDistanceFormat;
extern wxString GetLayerName(int id);
extern wxString g_uploadConnection;
extern bool g_bsimplifiedScalebar;

extern bool bDrawCurrentValues;

extern s52plib *ps52plib;

extern bool bGPSValid;
extern bool g_bTempShowMenuBar;
extern bool g_bShowMenuBar;

extern bool g_bShowAreaNotices;
extern int g_Show_Target_Name_Scale;
extern bool g_bCPAWarn;
extern bool g_bTCPA_Max;

extern MyFrame *gFrame;

extern int g_iNavAidRadarRingsNumberVisible;
extern float g_fNavAidRadarRingsStep;
extern int g_pNavAidRadarRingsStepUnits;
extern bool g_bWayPointPreventDragging;
extern bool g_bEnableZoomToCursor;
extern bool g_bShowChartBar;
extern bool g_bInlandEcdis;
extern int g_ENCSoundingScaleFactor;
extern int g_ENCTextScaleFactor;
extern int g_maxzoomin;

extern float g_GLMinSymbolLineWidth;
bool g_bAllowShipToActive;
bool g_bShowShipToActive;
int g_shipToActiveStyle;
int g_shipToActiveColor;

extern int g_ais_query_dialog_x, g_ais_query_dialog_y;

extern int g_S57_dialog_sx, g_S57_dialog_sy;
extern int g_detailslider_dialog_x, g_detailslider_dialog_y;

extern bool g_b_overzoom_x;  // Allow high overzoom
extern double g_plus_minus_zoom_factor;

extern int g_OwnShipIconType;
extern double g_n_ownship_length_meters;
extern double g_n_ownship_beam_meters;
extern double g_n_gps_antenna_offset_y;
extern double g_n_gps_antenna_offset_x;
extern int g_n_ownship_min_mm;

extern double g_COGAvg;  // only needed for debug....

extern int g_click_stop;
extern double g_ownship_predictor_minutes;
extern double g_ownship_HDTpredictor_miles;

extern bool g_bquiting;
extern wxString g_sAIS_Alert_Sound_File;
extern wxString g_anchorwatch_sound_file;

extern OCPN_AUIManager *g_pauimgr;

extern bool g_bopengl;
extern bool g_bdisable_opengl;

extern bool g_bFullScreenQuilt;

extern bool g_bsmoothpanzoom;

bool g_bDebugOGL;

extern bool g_b_assume_azerty;

extern ChartGroupArray *g_pGroupArray;
extern wxString g_default_routepoint_icon;

extern ocpnStyle::StyleManager *g_StyleManager;

extern OcpnSound *g_anchorwatch_sound;

extern bool g_bShowTrue, g_bShowMag;
extern bool g_bresponsive;
extern int g_chart_zoom_modifier_raster;
extern int g_chart_zoom_modifier_vector;
extern int g_ChartScaleFactor;


#ifdef ocpnUSE_GL
#endif

extern bool g_bShowFPS;
extern double g_gl_ms_per_frame;
extern bool g_benable_rotate;
extern bool g_bRollover;

extern bool g_bSpaceDropMark;
extern bool g_bAutoHideToolbar;
extern int g_nAutoHideToolbar;
extern bool g_bDeferredInitDone;

extern wxString g_CmdSoundString;
extern bool g_boptionsactive;

//  TODO why are these static?
static int mouse_x;
static int mouse_y;
static bool mouse_leftisdown;

bool g_brouteCreating;

bool g_bShowTrackPointTime;

int r_gamma_mult;
int g_gamma_mult;
int b_gamma_mult;
int gamma_state;
bool g_brightness_init;
int last_brightness;

int g_cog_predictor_width;
extern double g_display_size_mm;

extern wxColour g_colourOwnshipRangeRingsColour;

// LIVE ETA OPTION
bool g_bShowLiveETA;
double g_defaultBoatSpeed;
double g_defaultBoatSpeedUserUnit;

extern int g_nAIS_activity_timer;
extern bool g_bskew_comp;
extern float g_compass_scalefactor;
extern int g_COGAvgSec;  // COG average period (sec.) for Course Up Mode

wxGLContext *g_pGLcontext;  // shared common context

extern bool g_useMUI;
extern unsigned int g_canvasConfig;
extern wxString g_lastPluginMessage;

extern ChartCanvas *g_focusCanvas;
extern ChartCanvas *g_overlayCanvas;

extern float g_toolbar_scalefactor;
extern SENCThreadManager *g_SencThreadManager;

wxString g_ObjQFileExt;

// "Curtain" mode parameters
wxDialog *g_pcurtain;
extern double gLat, gLat;

extern int g_GUIScaleFactor;
// Win DPI scale factor
double g_scaler;
wxString g_lastS52PLIBPluginMessage;

#define MIN_BRIGHT 10
#define MAX_BRIGHT 100

//------------------------------------------------------------------------------
//    ChartCanvas Implementation
//------------------------------------------------------------------------------
BEGIN_EVENT_TABLE(ChartCanvas, wxWindow)
EVT_ACTIVATE(ChartCanvas::OnActivate)
EVT_SIZE(ChartCanvas::OnSize)

EVT_TIMER(PAN_TIMER, ChartCanvas::PanTimerEvent)
EVT_TIMER(MOVEMENT_TIMER, ChartCanvas::MovementTimerEvent)
EVT_TIMER(MOVEMENT_STOP_TIMER, ChartCanvas::MovementStopTimerEvent)
EVT_TIMER(ROT_TIMER, ChartCanvas::RotateTimerEvent)
EVT_MOUSE_CAPTURE_LOST(ChartCanvas::LostMouseCapture)

EVT_TIMER(DEFERRED_FOCUS_TIMER, ChartCanvas::OnDeferredFocusTimerEvent)

END_EVENT_TABLE()

// Define a constructor for my canvas
ChartCanvas::ChartCanvas(wxFrame *frame, int canvasIndex)
    : wxWindow(frame, wxID_ANY, wxPoint(20, 20), wxSize(5, 5), wxNO_BORDER) {
  parent_frame = (MyFrame *)frame;  // save a pointer to parent
  m_canvasIndex = canvasIndex;

  pscratch_bm = NULL;

  SetBackgroundColour(wxColour(0, 0, 0));
  SetBackgroundStyle(wxBG_STYLE_CUSTOM);  // on WXMSW, this prevents flashing on
                                          // color scheme change

  m_groupIndex = 0;
  m_bIsInRadius = false;
  m_bMayToggleMenuBar = true;

  m_bFollow = false;
  m_bShowNavobjects = true;
  m_bTCupdate = false;
  m_bAppendingRoute = false;  // was true in MSW, why??
  pThumbDIBShow = NULL;
  m_bShowCurrent = false;
  m_bShowTide = false;
  bShowingCurrent = false;
  //pCwin = NULL;
  warp_flag = false;
  m_bzooming = false;
  m_b_paint_enable = true;
  m_routeState = 0;

  pss_overlay_bmp = NULL;
  pss_overlay_mask = NULL;
  m_bChartDragging = false;
  m_bMeasure_Active = false;
  m_bMeasure_DistCircle = false;
  m_bedge_pan = false;
  m_disable_edge_pan = false;
  m_dragoffsetSet = false;
  m_bautofind = false;
  m_bFirstAuto = true;
  m_groupIndex = 0;
  m_singleChart = NULL;
  m_upMode = NORTH_UP_MODE;
  m_bShowAIS = true;
  m_bShowAISScaled = false;

  m_vLat = 0.;
  m_vLon = 0.;

  m_pCIWin = NULL;

  m_FinishRouteOnKillFocus = true;

  m_bsectors_shown = false;

  m_bbrightdir = false;
  r_gamma_mult = 1;
  g_gamma_mult = 1;
  b_gamma_mult = 1;

  m_pos_image_user_day = NULL;
  m_pos_image_user_dusk = NULL;
  m_pos_image_user_night = NULL;
  m_pos_image_user_grey_day = NULL;
  m_pos_image_user_grey_dusk = NULL;
  m_pos_image_user_grey_night = NULL;

  m_zoom_factor = 1;
  m_rotation_speed = 0;
  m_mustmove = 0;

  m_OSoffsetx = 0.;
  m_OSoffsety = 0.;

  m_pos_image_user_yellow_day = NULL;
  m_pos_image_user_yellow_dusk = NULL;
  m_pos_image_user_yellow_night = NULL;

  VPoint.Invalidate();

  m_glcc = NULL;

  m_toolbar_scalefactor = 1.0;
  m_toolbarOrientation = wxTB_HORIZONTAL;
  m_focus_indicator_pix = 1;

  m_pCurrentStack = NULL;
  m_bpersistent_quilt = false;
  m_piano_ctx_menu = NULL;  

  g_ChartNotRenderScaleFactor = 2.0;
  m_bShowScaleInStatusBar = true;

  m_bShowScaleInStatusBar = false;

  m_bShowOutlines = false;
  m_bDisplayGrid = false;
  m_bShowDepthUnits = true;
  m_encDisplayCategory = (int)STANDARD;

  m_encShowLights = true;
  m_encShowAnchor = true;
  m_encShowDataQual = false;
  m_bShowGPS = true;
  m_pQuilt = new Quilt(this);
  SetQuiltMode(true);
  SetAlertString(_T(""));
  m_sector_glat = 0;
  m_sector_glon = 0;

#ifdef HAVE_WX_GESTURE_EVENTS
  m_oldVPSScale = -1.0;
  m_popupWanted = false;
  m_leftdown = false;
#endif /* HAVE_WX_GESTURE_EVENTS */

  SetupGlCanvas();
  
  singleClickEventIsValid = false;

  //    Build the cursors
  pCursorLeft = NULL;
  pCursorRight = NULL;
  pCursorUp = NULL;
  pCursorDown = NULL;
  pCursorArrow = NULL;
  pCursorPencil = NULL;
  pCursorCross = NULL;

  RebuildCursors();

  SetCursor(*pCursorArrow);

  pPanTimer = new wxTimer(this, m_MouseDragging);
  pPanTimer->Stop();

  pMovementTimer = new wxTimer(this, MOVEMENT_TIMER);
  pMovementTimer->Stop();

  pMovementStopTimer = new wxTimer(this, MOVEMENT_STOP_TIMER);
  pMovementStopTimer->Stop();

  pRotDefTimer = new wxTimer(this, ROT_TIMER);
  pRotDefTimer->Stop();

  m_DoubleClickTimer = new wxTimer(this, DBLCLICK_TIMER);
  m_DoubleClickTimer->Stop();

  m_panx = m_pany = 0;
  m_panspeed = 0;

  pCurTrackTimer = new wxTimer(this, CURTRACK_TIMER);
  pCurTrackTimer->Stop();
  m_curtrack_timer_msec = 10;

  m_wheelzoom_stop_oneshot = 0;
  m_last_wheel_dir = 0;

  m_RolloverPopupTimer.SetOwner(this, ROPOPUP_TIMER);

  m_deferredFocusTimer.SetOwner(this, DEFERRED_FOCUS_TIMER);

  m_rollover_popup_timer_msec = 20;

  m_routeFinishTimer.SetOwner(this, ROUTEFINISH_TIMER);

  m_b_rot_hidef = true;

  proute_bm = NULL;
  m_prot_bm = NULL;

  m_upMode = NORTH_UP_MODE;
  m_bLookAhead = false;
  m_VPRotate = 0;

  // Set some benign initial values

  m_cs = GLOBAL_COLOR_SCHEME_DAY;
  VPoint.clat = 0;
  VPoint.clon = 0;
  VPoint.view_scale_ppm = 1;
  VPoint.Invalidate();
  m_nMeasureState = 0;

  m_canvas_scale_factor = 1.;

  m_canvas_width = 1000;

  m_overzoomTextWidth = 0;
  m_overzoomTextHeight = 0;

  //    Create the default world chart
  pWorldBackgroundChart = new GSHHSChart;

  //    Create the default depth unit emboss maps
  m_pEM_Feet = NULL;
  m_pEM_Meters = NULL;
  m_pEM_Fathoms = NULL;

  CreateDepthUnitEmbossMaps(GLOBAL_COLOR_SCHEME_DAY);

  m_pEM_OverZoom = NULL;
  SetOverzoomFont();
  CreateOZEmbossMapData(GLOBAL_COLOR_SCHEME_DAY);

  //    Build icons for tide/current points
  ocpnStyle::Style *style = g_StyleManager->GetCurrentStyle();
  m_bmTideDay = style->GetIconScaled(_T("tidesml"), 1. / g_Platform->GetDisplayDIPMult(this));

  //    Dusk
  m_bmTideDusk = CreateDimBitmap(m_bmTideDay, .50);

  //    Night
  m_bmTideNight = CreateDimBitmap(m_bmTideDay, .20);

  //    Build Dusk/Night  ownship icons
  double factor_dusk = 0.5;
  double factor_night = 0.25;

  // Red
  m_os_image_red_day = style->GetIcon(_T("ship-red")).ConvertToImage();

  int rimg_width = m_os_image_red_day.GetWidth();
  int rimg_height = m_os_image_red_day.GetHeight();

  m_os_image_red_dusk = m_os_image_red_day.Copy();
  m_os_image_red_night = m_os_image_red_day.Copy();

  for (int iy = 0; iy < rimg_height; iy++) {
    for (int ix = 0; ix < rimg_width; ix++) {
      if (!m_os_image_red_day.IsTransparent(ix, iy)) {
        wxImage::RGBValue rgb(m_os_image_red_day.GetRed(ix, iy),
                              m_os_image_red_day.GetGreen(ix, iy),
                              m_os_image_red_day.GetBlue(ix, iy));
        wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
        hsv.value = hsv.value * factor_dusk;
        wxImage::RGBValue nrgb = wxImage::HSVtoRGB(hsv);
        m_os_image_red_dusk.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);

        hsv = wxImage::RGBtoHSV(rgb);
        hsv.value = hsv.value * factor_night;
        nrgb = wxImage::HSVtoRGB(hsv);
        m_os_image_red_night.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);
      }
    }
  }

  // Grey
  m_os_image_grey_day =
      style->GetIcon(_T("ship-red")).ConvertToImage().ConvertToGreyscale();

  int gimg_width = m_os_image_grey_day.GetWidth();
  int gimg_height = m_os_image_grey_day.GetHeight();

  m_os_image_grey_dusk = m_os_image_grey_day.Copy();
  m_os_image_grey_night = m_os_image_grey_day.Copy();

  for (int iy = 0; iy < gimg_height; iy++) {
    for (int ix = 0; ix < gimg_width; ix++) {
      if (!m_os_image_grey_day.IsTransparent(ix, iy)) {
        wxImage::RGBValue rgb(m_os_image_grey_day.GetRed(ix, iy),
                              m_os_image_grey_day.GetGreen(ix, iy),
                              m_os_image_grey_day.GetBlue(ix, iy));
        wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
        hsv.value = hsv.value * factor_dusk;
        wxImage::RGBValue nrgb = wxImage::HSVtoRGB(hsv);
        m_os_image_grey_dusk.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);

        hsv = wxImage::RGBtoHSV(rgb);
        hsv.value = hsv.value * factor_night;
        nrgb = wxImage::HSVtoRGB(hsv);
        m_os_image_grey_night.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);
      }
    }
  }

  // Yellow
  m_os_image_yellow_day = m_os_image_red_day.Copy();

  gimg_width = m_os_image_yellow_day.GetWidth();
  gimg_height = m_os_image_yellow_day.GetHeight();

  m_os_image_yellow_dusk = m_os_image_red_day.Copy();
  m_os_image_yellow_night = m_os_image_red_day.Copy();

  for (int iy = 0; iy < gimg_height; iy++) {
    for (int ix = 0; ix < gimg_width; ix++) {
      if (!m_os_image_yellow_day.IsTransparent(ix, iy)) {
        wxImage::RGBValue rgb(m_os_image_yellow_day.GetRed(ix, iy),
                              m_os_image_yellow_day.GetGreen(ix, iy),
                              m_os_image_yellow_day.GetBlue(ix, iy));
        wxImage::HSVValue hsv = wxImage::RGBtoHSV(rgb);
        hsv.hue += 60. / 360.;  // shift to yellow
        wxImage::RGBValue nrgb = wxImage::HSVtoRGB(hsv);
        m_os_image_yellow_day.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);

        hsv = wxImage::RGBtoHSV(rgb);
        hsv.value = hsv.value * factor_dusk;
        hsv.hue += 60. / 360.;  // shift to yellow
        nrgb = wxImage::HSVtoRGB(hsv);
        m_os_image_yellow_dusk.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);

        hsv = wxImage::RGBtoHSV(rgb);
        hsv.hue += 60. / 360.;  // shift to yellow
        hsv.value = hsv.value * factor_night;
        nrgb = wxImage::HSVtoRGB(hsv);
        m_os_image_yellow_night.SetRGB(ix, iy, nrgb.red, nrgb.green, nrgb.blue);
      }
    }
  }

  //  Set initial pointers to ownship images
  m_pos_image_red = &m_os_image_red_day;
  m_pos_image_yellow = &m_os_image_yellow_day;
  m_pos_image_grey = &m_os_image_grey_day;

  //m_pBrightPopup = NULL;

#ifdef ocpnUSE_GL
  if (!g_bdisable_opengl) m_pQuilt->EnableHighDefinitionZoom(true);
#endif

  int gridFontSize = 8;
#if defined(__WXOSX__) || defined(__WXGTK3__)
  // Support scaled HDPI displays.
  gridFontSize *= GetContentScaleFactor();
#endif

  m_pgridFont = FontMgr::Get().FindOrCreateFont(
      gridFontSize, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL, FALSE,
      wxString(_T ( "Arial" )));

  m_Piano = new Piano(this);

  m_bToolbarEnable = false;
  m_pianoFrozen = false;

  SetMinSize(wxSize(200, 200));

  m_displayScale = 1.0;
#if defined(__WXOSX__) || defined(__WXGTK3__)
  // Support scaled HDPI displays.
  m_displayScale = GetContentScaleFactor();
#endif


#ifdef HAVE_WX_GESTURE_EVENTS
  if (!EnableTouchEvents(wxTOUCH_ZOOM_GESTURE |
                         wxTOUCH_PRESS_GESTURES)) {
    wxLogError("Failed to enable touch events");
  }

  Bind(wxEVT_GESTURE_ZOOM, &ChartCanvas::OnZoom, this);

  Bind(wxEVT_LONG_PRESS, &ChartCanvas::OnLongPress, this);
  Bind(wxEVT_PRESS_AND_TAP, &ChartCanvas::OnPressAndTap, this);


#endif
}

ChartCanvas::~ChartCanvas() {
  delete pThumbDIBShow;

  //    Delete Cursors
  delete pCursorLeft;
  delete pCursorRight;
  delete pCursorUp;
  delete pCursorDown;
  delete pCursorArrow;
  delete pCursorPencil;
  delete pCursorCross;

  delete pPanTimer;
  delete pMovementTimer;
  delete pMovementStopTimer;
  delete pCurTrackTimer;
  delete pRotDefTimer;
  delete m_DoubleClickTimer;

  //delete m_pBrightPopup;

  delete m_pCIWin;

  delete pscratch_bm;

  m_dc_route.SelectObject(wxNullBitmap);
  delete proute_bm;

  delete pWorldBackgroundChart;
  delete pss_overlay_bmp;

  delete m_pEM_Feet;
  delete m_pEM_Meters;
  delete m_pEM_Fathoms;

  delete m_pEM_OverZoom;
  //        delete m_pEM_CM93Offset;

  delete m_prot_bm;

  delete m_pos_image_user_day;
  delete m_pos_image_user_dusk;
  delete m_pos_image_user_night;
  delete m_pos_image_user_grey_day;
  delete m_pos_image_user_grey_dusk;
  delete m_pos_image_user_grey_night;
  delete m_pos_image_user_yellow_day;
  delete m_pos_image_user_yellow_dusk;
  delete m_pos_image_user_yellow_night;

#ifdef ocpnUSE_GL
  if (!g_bdisable_opengl) {
    delete m_glcc;

#if wxCHECK_VERSION(2, 9, 0)
    if (IsPrimaryCanvas() && g_bopengl) delete g_pGLcontext;
#endif
  }
#endif

  // Delete the MUI bar, but make sure there is no pointer to it during destroy.
  // wx tries to deliver events to this canvas during destroy.
  delete m_pQuilt;
  delete m_pCurrentStack;  
  delete m_Piano;
}

void ChartCanvas::RebuildCursors() {
  delete pCursorLeft;
  delete pCursorRight;
  delete pCursorUp;
  delete pCursorDown;
  delete pCursorArrow;
  delete pCursorPencil;
  delete pCursorCross;

  ocpnStyle::Style *style = g_StyleManager->GetCurrentStyle();
  double cursorScale = exp(g_GUIScaleFactor * (0.693 / 5.0));

  double pencilScale = 1.0 / g_Platform->GetDisplayDIPMult(gFrame);

  wxImage ICursorLeft = style->GetIcon(_T("left")).ConvertToImage();
  wxImage ICursorRight = style->GetIcon(_T("right")).ConvertToImage();
  wxImage ICursorUp = style->GetIcon(_T("up")).ConvertToImage();
  wxImage ICursorDown = style->GetIcon(_T("down")).ConvertToImage();
  wxImage ICursorPencil = style->GetIconScaled(_T("pencil"), pencilScale).ConvertToImage();
  wxImage ICursorCross = style->GetIcon(_T("cross")).ConvertToImage();

#if !defined(__WXMSW__) && !defined(__WXQT__)
  ICursorLeft.ConvertAlphaToMask(128);
  ICursorRight.ConvertAlphaToMask(128);
  ICursorUp.ConvertAlphaToMask(128);
  ICursorDown.ConvertAlphaToMask(128);
  ICursorPencil.ConvertAlphaToMask(10);
  ICursorCross.ConvertAlphaToMask(10);
#endif

  if (ICursorLeft.Ok()) {
    ICursorLeft.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
    ICursorLeft.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);
    pCursorLeft = new wxCursor(ICursorLeft);
  } else
    pCursorLeft = new wxCursor(wxCURSOR_ARROW);

  if (ICursorRight.Ok()) {
    ICursorRight.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 31);
    ICursorRight.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 15);
    pCursorRight = new wxCursor(ICursorRight);
  } else
    pCursorRight = new wxCursor(wxCURSOR_ARROW);

  if (ICursorUp.Ok()) {
    ICursorUp.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 15);
    ICursorUp.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0);
    pCursorUp = new wxCursor(ICursorUp);
  } else
    pCursorUp = new wxCursor(wxCURSOR_ARROW);

  if (ICursorDown.Ok()) {
    ICursorDown.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 15);
    ICursorDown.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 31);
    pCursorDown = new wxCursor(ICursorDown);
  } else
    pCursorDown = new wxCursor(wxCURSOR_ARROW);

  if (ICursorPencil.Ok()) {
    ICursorPencil.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0 * pencilScale);
    ICursorPencil.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 16 * pencilScale);
    pCursorPencil = new wxCursor(ICursorPencil);
  } else
    pCursorPencil = new wxCursor(wxCURSOR_ARROW);

  if (ICursorCross.Ok()) {
    ICursorCross.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 13);
    ICursorCross.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 12);
    pCursorCross = new wxCursor(ICursorCross);
  } else
    pCursorCross = new wxCursor(wxCURSOR_ARROW);

  pCursorArrow = new wxCursor(wxCURSOR_ARROW);
  pPlugIn_Cursor = NULL;
}

void ChartCanvas::CanvasApplyLocale() {
  CreateDepthUnitEmbossMaps(m_cs);
  CreateOZEmbossMapData(m_cs);
}

void ChartCanvas::SetupGlCanvas() {
#ifndef __OCPN__ANDROID__
#ifdef ocpnUSE_GL
  if (!g_bdisable_opengl) {
    if (g_bopengl) {
      wxLogMessage(_T("Creating glChartCanvas"));
      m_glcc = new glChartCanvas(this);

      // We use one context for all GL windows, so that textures etc will be
      // automatically shared
      if (IsPrimaryCanvas()) {
        // qDebug() << "Creating Primary Context";

        //             wxGLContextAttrs ctxAttr;
        //             ctxAttr.PlatformDefaults().CoreProfile().OGLVersion(3,
        //             2).EndList(); wxGLContext *pctx = new wxGLContext(m_glcc,
        //             NULL, &ctxAttr);
        wxGLContext *pctx = new wxGLContext(m_glcc);
        m_glcc->SetContext(pctx);
        g_pGLcontext = pctx;  // Save a copy of the common context
      } else {
#ifdef __WXOSX__
        m_glcc->SetContext(new wxGLContext(m_glcc, g_pGLcontext));
#else
        m_glcc->SetContext(g_pGLcontext);  // If not primary canvas, use the
                                           // saved common context
#endif
      }
    }
  }
#endif
#endif
}

#ifdef HAVE_WX_GESTURE_EVENTS
void ChartCanvas::OnLongPress(wxLongPressEvent &event) {
  /* we defer the popup menu call upon the leftup event
  else the menu disappears immediately,
  (see
  http://wxwidgets.10942.n7.nabble.com/Popupmenu-disappears-immediately-if-called-from-QueueEvent-td92572.html)
  */
  m_popupWanted = true;
}

void ChartCanvas::OnPressAndTap(wxPressAndTapEvent &event) {
  // not implemented yet
}

void ChartCanvas::OnZoom(wxZoomGestureEvent &event) {
  /* there are spurious end zoom events upon right-click */
  if (event.IsGestureEnd()) return;

  double factor = event.GetZoomFactor();

  if (event.IsGestureStart() || m_oldVPSScale < 0) {
    m_oldVPSScale = GetVPScale();
  }

  double current_vps = GetVPScale();
  double wanted_factor = m_oldVPSScale / current_vps * factor;

  ZoomCanvas(wanted_factor, true, false);

  //  Allow combined zoom/pan operation
  if (event.IsGestureStart()) {
    m_zoomStartPoint = event.GetPosition();
  } else {
    wxPoint delta = event.GetPosition() - m_zoomStartPoint;
    PanCanvas(-delta.x, -delta.y);
    m_zoomStartPoint = event.GetPosition();
  }
}


void ChartCanvas::OnDoubleLeftClick(wxMouseEvent &event) {
  DoRotateCanvas(0.0);
}
#endif /* HAVE_WX_GESTURE_EVENTS */

void ChartCanvas::ApplyCanvasConfig(canvasConfig *pcc) {
  SetViewPoint(pcc->iLat, pcc->iLon, pcc->iScale, 0., pcc->iRotation);
  m_vLat = pcc->iLat;
  m_vLon = pcc->iLon;

  m_restore_dbindex = pcc->DBindex;
  m_bFollow = pcc->bFollow;
  if (pcc->GroupID < 0) pcc->GroupID = 0;

  if (pcc->GroupID > (int)g_pGroupArray->GetCount())
    m_groupIndex = 0;
  else
    m_groupIndex = pcc->GroupID;

  if (pcc->bQuilt != GetQuiltMode()) ToggleCanvasQuiltMode();

  ShowTides(pcc->bShowTides);
  ShowCurrents(pcc->bShowCurrents);

  SetShowDepthUnits(pcc->bShowDepthUnits);
  SetShowGrid(pcc->bShowGrid);
  SetShowOutlines(pcc->bShowOutlines);

  SetShowAIS(pcc->bShowAIS);
  SetAttenAIS(pcc->bAttenAIS);

  // ENC options
  SetShowENCText(pcc->bShowENCText);
  m_encDisplayCategory = pcc->nENCDisplayCategory;
  m_encShowDepth = pcc->bShowENCDepths;
  m_encShowLightDesc = pcc->bShowENCLightDescriptions;
  m_encShowBuoyLabels = pcc->bShowENCBuoyLabels;
  m_encShowLights = pcc->bShowENCLights;
  m_bShowVisibleSectors = pcc->bShowENCVisibleSectorLights;
  m_encShowAnchor = pcc->bShowENCAnchorInfo;
  m_encShowDataQual = pcc->bShowENCDataQuality;

  bool courseUp = pcc->bCourseUp;
  bool headUp = pcc->bHeadUp;
  m_upMode = NORTH_UP_MODE;
  if (courseUp)
    m_upMode = COURSE_UP_MODE;
  else if (headUp)
    m_upMode = HEAD_UP_MODE;

  m_bLookAhead = pcc->bLookahead;

  m_singleChart = NULL;
}

void ChartCanvas::ApplyGlobalSettings() {

}

void ChartCanvas::CheckGroupValid(bool showMessage, bool switchGroup0) {
  bool groupOK = CheckGroup(m_groupIndex);

  if (!groupOK) {
    SetGroupIndex(m_groupIndex, true);
  }
}

int ChartCanvas::GetPianoHeight() {
  int height = 0;
  if (g_bShowChartBar && GetPiano()) height = m_Piano->GetHeight();

  return height;
}

void ChartCanvas::ShowTides(bool bShow) {
  gFrame->LoadHarmonics();

  if (ptcmgr->IsReady()) {
    SetbShowTide(bShow);

    parent_frame->SetMenubarItemState(ID_MENU_SHOW_TIDES, bShow);
  } else {
    wxLogMessage(_T("Chart1::Event...TCMgr Not Available"));
    SetbShowTide(false);
    parent_frame->SetMenubarItemState(ID_MENU_SHOW_TIDES, false);
  }
}

void ChartCanvas::ShowCurrents(bool bShow) {
  gFrame->LoadHarmonics();

  if (ptcmgr->IsReady()) {
    SetbShowCurrent(bShow);

    parent_frame->SetMenubarItemState(ID_MENU_SHOW_CURRENTS, bShow);
  } else {
    wxLogMessage(_T("Chart1::Event...TCMgr Not Available"));
    SetbShowCurrent(false);
    parent_frame->SetMenubarItemState(ID_MENU_SHOW_CURRENTS, false);
  }
}

// TODO
extern bool g_bPreserveScaleOnX;
extern ChartDummy *pDummyChart;
extern int g_sticky_chart;

void ChartCanvas::canvasRefreshGroupIndex(void) { SetGroupIndex(m_groupIndex); }

void ChartCanvas::SetGroupIndex(int index, bool autoSwitch) {
  SetAlertString(_T(""));

  int new_index = index;
  if (index > (int)g_pGroupArray->GetCount()) new_index = 0;

  bool bgroup_override = false;
  int old_group_index = new_index;

  if (!CheckGroup(new_index)) {
    new_index = 0;
    bgroup_override = true;
  }

  if (!autoSwitch && (index <= (int)g_pGroupArray->GetCount()))
    new_index = index;

  //    Get the currently displayed chart native scale, and the current ViewPort
  int current_chart_native_scale = GetCanvasChartNativeScale();
  ViewPort vp = GetVP();

  m_groupIndex = new_index;

  // Are there  ENCs in this group
  if (ChartData) m_bENCGroup = ChartData->IsENCInGroup(m_groupIndex);

  //  Allow the chart database to pre-calculate the MBTile inclusion test
  //  boolean...
  ChartData->CheckExclusiveTileGroup(m_canvasIndex);

  //  Invalidate the "sticky" chart on group change, since it might not be in
  //  the new group
  g_sticky_chart = -1;

  //    We need a chartstack and quilt to figure out which chart to open in the
  //    new group
  UpdateCanvasOnGroupChange();

  int dbi_now = -1;
  if (GetQuiltMode()) dbi_now = GetQuiltReferenceChartIndex();

  int dbi_hint = FindClosestCanvasChartdbIndex(current_chart_native_scale);

  // If a new reference chart is indicated, set a good scale for it.
  if ((dbi_now != dbi_hint) || !GetQuiltMode()) {
    double best_scale = GetBestStartScale(dbi_hint, vp);
    SetVPScale(best_scale);
  }

  if (GetQuiltMode()) dbi_hint = GetQuiltReferenceChartIndex();
  //    Refresh the canvas, selecting the "best" chart,
  //    applying the prior ViewPort exactly
  canvasChartsRefresh(dbi_hint);

  if (!autoSwitch && bgroup_override) {
    return;
  }
}

bool ChartCanvas::CheckGroup(int igroup) {
  if (!ChartData) return true;  //  Not known yet...

  if (igroup == 0) return true;  // "all charts" is always OK

  if (igroup < 0)  // negative group is an error
    return false;

  ChartGroup *pGroup = g_pGroupArray->Item(igroup - 1);

  if (pGroup->m_element_array.empty())  //  truly empty group prompts a warning,
                                        //  and auto-shift to group 0
    return false;

  for (const auto &elem : pGroup->m_element_array) {
    for (unsigned int ic = 0;
         ic < (unsigned int)ChartData->GetChartTableEntries(); ic++) {
      ChartTableEntry *pcte = ChartData->GetpChartTableEntry(ic);
      wxString chart_full_path(pcte->GetpFullPath(), wxConvUTF8);

      if (chart_full_path.StartsWith(elem.m_element_name)) return true;
    }
  }

  //  If necessary, check for GSHHS
  for (const auto &elem : pGroup->m_element_array) {
    const wxString &element_root = elem.m_element_name;
    wxString test_string = _T("GSHH");
    if (element_root.Upper().Contains(test_string)) return true;
  }

  return false;
}

void ChartCanvas::canvasChartsRefresh(int dbi_hint) {
  if (!ChartData) return;

  double old_scale = GetVPScale();
  InvalidateQuilt();
  SetQuiltRefChart(-1);

  m_singleChart = NULL;

  // delete m_pCurrentStack;
  // m_pCurrentStack = NULL;

  //    Build a new ChartStack
  if (!m_pCurrentStack) {
    m_pCurrentStack = new ChartStack;
    ChartData->BuildChartStack(m_pCurrentStack, m_vLat, m_vLon, m_groupIndex);
  }

  if (-1 != dbi_hint) {
    if (GetQuiltMode()) {
      GetpCurrentStack()->SetCurrentEntryFromdbIndex(dbi_hint);
      SetQuiltRefChart(dbi_hint);
    } else {
      //      Open the saved chart
      ChartBase *pTentative_Chart;
      pTentative_Chart = ChartData->OpenChartFromDB(dbi_hint, FULL_INIT);

      if (pTentative_Chart) {
        /* m_singleChart is always NULL here, (set above) should this go before
         * that? */
        if (m_singleChart) m_singleChart->Deactivate();

        m_singleChart = pTentative_Chart;
        m_singleChart->Activate();

        GetpCurrentStack()->CurrentStackEntry = ChartData->GetStackEntry(
            GetpCurrentStack(), m_singleChart->GetFullPath());
      }
    }

    // refresh_Piano();
  } else {
    //    Select reference chart from the stack, as though clicked by user
    //    Make it the smallest scale chart on the stack
    GetpCurrentStack()->CurrentStackEntry = GetpCurrentStack()->nEntry - 1;
    int selected_index = GetpCurrentStack()->GetCurrentEntrydbIndex();
    SetQuiltRefChart(selected_index);
  }

  //    Validate the correct single chart, or set the quilt mode as appropriate
  SetupCanvasQuiltMode();
  if (!GetQuiltMode() && m_singleChart == 0) {
    // use a dummy like in DoChartUpdate
    if (NULL == pDummyChart) pDummyChart = new ChartDummy;
    m_singleChart = pDummyChart;
    SetVPScale(old_scale);
  }

  ReloadVP();

  UpdateCanvasControlBar();  

  SetCursor(wxCURSOR_ARROW);
}

bool ChartCanvas::DoCanvasUpdate(void) {
  double tLat, tLon;    // Chart Stack location
  double vpLat, vpLon;  // ViewPort location

  bool bNewChart = false;
  bool bNewView = false;
  bool bCanvasChartAutoOpen = true;  // debugging

  bool bNewPiano = false;
  bool bOpenSpecified;
  ChartStack LastStack;
  ChartBase *pLast_Ch;

  ChartStack WorkStack;

  if (bDBUpdateInProgress) return false;
  if (!ChartData) return false;

  if (ChartData->IsBusy()) return false;

  //    Startup case:
  //    Quilting is enabled, but the last chart seen was not quiltable
  //    In this case, drop to single chart mode, set persistence flag,
  //    And open the specified chart
  // TODO implement this
  //     if( m_bFirstAuto && ( g_restore_dbindex >= 0 ) ) {
  //         if( GetQuiltMode() ) {
  //             if( !IsChartQuiltableRef( g_restore_dbindex ) ) {
  //                 gFrame->ToggleQuiltMode();
  //                 m_bpersistent_quilt = true;
  //                 m_singleChart = NULL;
  //             }
  //         }
  //     }

  //      If in auto-follow mode, use the current glat,glon to build chart
  //      stack. Otherwise, use vLat, vLon gotten from click on chart canvas, or
  //      other means

  if (m_bFollow) {
    tLat = gLat;
    tLon = gLon;

    // Set the ViewPort center based on the OWNSHIP offset
    double dx = m_OSoffsetx;
    double dy = m_OSoffsety;
    double d_east = dx / GetVP().view_scale_ppm;
    double d_north = dy / GetVP().view_scale_ppm;

    if (GetUpMode() == NORTH_UP_MODE) {
      fromSM(d_east, d_north, gLat, gLon, &vpLat, &vpLon);
    } else {
      double offset_angle = atan2(d_north, d_east);
      double offset_distance = sqrt((d_north * d_north) + (d_east * d_east));
      double chart_angle = GetVPRotation();
      double target_angle = chart_angle + offset_angle;
      double d_east_mod = offset_distance * cos(target_angle);
      double d_north_mod = offset_distance * sin(target_angle);
      fromSM(d_east_mod, d_north_mod, gLat, gLon, &vpLat, &vpLon);
    }

    // on lookahead mode, adjust the vp center point
    if (m_bLookAhead && bGPSValid && !m_MouseDragging) {
      double angle = g_COGAvg + (GetVPRotation() * 180. / PI);

      double pixel_deltay =
          fabs(cos(angle * PI / 180.)) * GetCanvasHeight() / 4;
      double pixel_deltax = fabs(sin(angle * PI / 180.)) * GetCanvasWidth() / 4;

      double pixel_delta_tent =
          sqrt((pixel_deltay * pixel_deltay) + (pixel_deltax * pixel_deltax));

      double pixel_delta = 0;

      //    The idea here is to cancel the effect of LookAhead for slow gSog, to
      //    avoid jumping of the vp center point during slow maneuvering, or at
      //    anchor....
      if (!std::isnan(gSog)) {
        if (gSog < 1.0)
          pixel_delta = 0.;
        else if (gSog >= 3.0)
          pixel_delta = pixel_delta_tent;
        else
          pixel_delta = pixel_delta_tent * (gSog - 1.0) / 2.0;
      }

      double meters_to_shift =
          cos(gLat * PI / 180.) * pixel_delta / GetVPScale();

      double dir_to_shift = g_COGAvg;

      ll_gc_ll(gLat, gLon, dir_to_shift, meters_to_shift / 1852., &vpLat,
               &vpLon);
    } else if (m_bLookAhead && !bGPSValid) {
      m_OSoffsetx = 0;  // center ownship on loss of GPS
      m_OSoffsety = 0;
      vpLat = gLat;
      vpLon = gLon;
    }

  } else {
    tLat = m_vLat;
    tLon = m_vLon;
    vpLat = m_vLat;
    vpLon = m_vLon;
  }

  if (GetQuiltMode()) {
    int current_db_index = -1;
    if (m_pCurrentStack)
      current_db_index =
          m_pCurrentStack
              ->GetCurrentEntrydbIndex();  // capture the currently selected Ref
                                           // chart dbIndex
    else
      m_pCurrentStack = new ChartStack;

    //  This logic added to enable opening a chart when there is no
    //  previous chart indication, either from inital startup, or from adding
    //  new chart directory
    if (m_bautofind && (-1 == GetQuiltReferenceChartIndex()) && m_pCurrentStack) {
      if (m_pCurrentStack->nEntry) {
        int new_dbIndex = m_pCurrentStack->GetDBIndex(m_pCurrentStack->nEntry - 1);  // smallest scale
        SelectQuiltRefdbChart(new_dbIndex, true);
        m_bautofind = false;
      }
    }

    ChartData->BuildChartStack(m_pCurrentStack, tLat, tLon, m_groupIndex);
    m_pCurrentStack->SetCurrentEntryFromdbIndex(current_db_index);

    if (m_bFirstAuto) {
      //  Allow the chart database to pre-calculate the MBTile inclusion test
      //  boolean...
      ChartData->CheckExclusiveTileGroup(m_canvasIndex);

      double proposed_scale_onscreen =
          GetCanvasScaleFactor() / GetVPScale();  // as set from config load

      int initial_db_index = m_restore_dbindex;
      if (initial_db_index < 0) {
        if (m_pCurrentStack->nEntry) {
          initial_db_index = m_pCurrentStack->GetDBIndex(m_pCurrentStack->nEntry - 1);
        } else
          m_bautofind = true;  // initial_db_index = 0;
      }

      if (m_pCurrentStack->nEntry) {
        int initial_type = ChartData->GetDBChartType(initial_db_index);

        //    Check to see if the target new chart is quiltable as a reference
        //    chart

        if (!IsChartQuiltableRef(initial_db_index)) {
          // If it is not quiltable, then walk the stack up looking for a
          // satisfactory chart i.e. one that is quiltable and of the same type
          // XXX if there's none?
          int stack_index = 0;

          if (stack_index >= 0) {
            while ((stack_index < m_pCurrentStack->nEntry - 1)) {
              int test_db_index = m_pCurrentStack->GetDBIndex(stack_index);
              if (IsChartQuiltableRef(test_db_index) &&
                  (initial_type ==
                   ChartData->GetDBChartType(initial_db_index))) {
                initial_db_index = test_db_index;
                break;
              }
              stack_index++;
            }
          }
        }

        ChartBase *pc = ChartData->OpenChartFromDB(initial_db_index, FULL_INIT);
        if (pc) {
          SetQuiltRefChart(initial_db_index);
          m_pCurrentStack->SetCurrentEntryFromdbIndex(initial_db_index);
        }

        // Check proposed scale, see how much underzoom results
        // Adjust as necessary to prevent slow loading on initial startup
        // For MBTILES we skip this test because they are always shown in
        // reasonable range of scale
        if (pc) {
          if (pc->GetChartType() != CHART_TYPE_MBTILES)
            proposed_scale_onscreen =
                wxMin(proposed_scale_onscreen, 4.0 * pc->GetNativeScale());
          else
            proposed_scale_onscreen =
                wxMin(proposed_scale_onscreen, 32.0 * pc->GetNativeScale());
        }
      }

      bNewView |= SetViewPoint(vpLat, vpLon,
                               GetCanvasScaleFactor() / proposed_scale_onscreen,
                               0, GetVPRotation());
    }
    // else
    bNewView |= SetViewPoint(vpLat, vpLon, GetVPScale(), 0, GetVPRotation());

    goto update_finish;
  }

  //  Single Chart Mode from here....
  pLast_Ch = m_singleChart;
  ChartTypeEnum new_open_type;
  ChartFamilyEnum new_open_family;
  if (pLast_Ch) {
    new_open_type = pLast_Ch->GetChartType();
    new_open_family = pLast_Ch->GetChartFamily();
  } else {
    new_open_type = CHART_TYPE_KAP;
    new_open_family = CHART_FAMILY_RASTER;
  }

  bOpenSpecified = m_bFirstAuto;

  //  Make sure the target stack is valid
  if (NULL == m_pCurrentStack) m_pCurrentStack = new ChartStack;

  // Build a chart stack based on tLat, tLon
  if (0 == ChartData->BuildChartStack(&WorkStack, tLat, tLon, g_sticky_chart,
                                      m_groupIndex)) {  // Bogus Lat, Lon?
    if (NULL == pDummyChart) {
      pDummyChart = new ChartDummy;
      bNewChart = true;
    }

    if (m_singleChart)
      if (m_singleChart->GetChartType() != CHART_TYPE_DUMMY) bNewChart = true;

    m_singleChart = pDummyChart;

    //    If the current viewpoint is invalid, set the default scale to
    //    something reasonable.
    double set_scale = GetVPScale();
    if (!GetVP().IsValid()) set_scale = 1. / 20000.;

    bNewView |= SetViewPoint(tLat, tLon, set_scale, 0, GetVPRotation());

    //      If the chart stack has just changed, there is new status
    if (WorkStack.nEntry && m_pCurrentStack->nEntry) {
      if (!ChartData->EqualStacks(&WorkStack, m_pCurrentStack)) {
        bNewPiano = true;
        bNewChart = true;
      }
    }

    //      Copy the new (by definition empty) stack into the target stack
    ChartData->CopyStack(m_pCurrentStack, &WorkStack);

    goto update_finish;
  }

  //              Check to see if Chart Stack has changed
  if (!ChartData->EqualStacks(&WorkStack, m_pCurrentStack)) {
    //      New chart stack, so...
    bNewPiano = true;

    //      Save a copy of the current stack
    ChartData->CopyStack(&LastStack, m_pCurrentStack);

    //      Copy the new stack into the target stack
    ChartData->CopyStack(m_pCurrentStack, &WorkStack);

    //  Is Current Chart in new stack?

    int tEntry = -1;
    if (NULL != m_singleChart)  // this handles startup case
      tEntry = ChartData->GetStackEntry(m_pCurrentStack,
                                        m_singleChart->GetFullPath());

    if (tEntry != -1) {  // m_singleChart is in the new stack
      m_pCurrentStack->CurrentStackEntry = tEntry;
      bNewChart = false;
    }

    else  // m_singleChart is NOT in new stack
    {     // So, need to open a new chart
      //      Find the largest scale raster chart that opens OK

      ChartBase *pProposed = NULL;

      if (bCanvasChartAutoOpen) {
        bool search_direction =
            false;  // default is to search from lowest to highest
        int start_index = 0;

        //    A special case:  If panning at high scale, open largest scale
        //    chart first
        if ((LastStack.CurrentStackEntry == LastStack.nEntry - 1) ||
            (LastStack.nEntry == 0)) {
          search_direction = true;
          start_index = m_pCurrentStack->nEntry - 1;
        }

        //    Another special case, open specified index on program start
        if (bOpenSpecified) {
          search_direction = false;
          start_index = 0;
          if ((start_index < 0) | (start_index >= m_pCurrentStack->nEntry))
            start_index = 0;

          new_open_type = CHART_TYPE_DONTCARE;
        }

        pProposed = ChartData->OpenStackChartConditional(
            m_pCurrentStack, start_index, search_direction, new_open_type,
            new_open_family);

        //    Try to open other types/families of chart in some priority
        if (NULL == pProposed)
          pProposed = ChartData->OpenStackChartConditional(
              m_pCurrentStack, start_index, search_direction,
              CHART_TYPE_CM93COMP, CHART_FAMILY_VECTOR);

        if (NULL == pProposed)
          pProposed = ChartData->OpenStackChartConditional(
              m_pCurrentStack, start_index, search_direction,
              CHART_TYPE_CM93COMP, CHART_FAMILY_RASTER);

        bNewChart = true;

      }  // bCanvasChartAutoOpen

      else
        pProposed = NULL;

      //  If no go, then
      //  Open a Dummy Chart
      if (NULL == pProposed) {
        if (NULL == pDummyChart) {
          pDummyChart = new ChartDummy;
          bNewChart = true;
        }

        if (pLast_Ch)
          if (pLast_Ch->GetChartType() != CHART_TYPE_DUMMY) bNewChart = true;

        pProposed = pDummyChart;
      }

      // Arriving here, pProposed points to an opened chart, or NULL.
      if (m_singleChart) m_singleChart->Deactivate();
      m_singleChart = pProposed;

      if (m_singleChart) {
        m_singleChart->Activate();
        m_pCurrentStack->CurrentStackEntry = ChartData->GetStackEntry(
            m_pCurrentStack, m_singleChart->GetFullPath());
      }
    }  // need new chart

    // Arriving here, m_singleChart is opened and OK, or NULL
    if (NULL != m_singleChart) {
      //      Setup the view using the current scale
      double set_scale = GetVPScale();

      //    If the current viewpoint is invalid, set the default scale to
      //    something reasonable.
      if (!GetVP().IsValid())
        set_scale = 1. / 20000.;
      else {  // otherwise, match scale if elected.
        double proposed_scale_onscreen;

        if (m_bFollow) {  // autoset the scale only if in autofollow
          double new_scale_ppm =
              m_singleChart->GetNearestPreferredScalePPM(GetVPScale());
          proposed_scale_onscreen = GetCanvasScaleFactor() / new_scale_ppm;
        } else
          proposed_scale_onscreen = GetCanvasScaleFactor() / set_scale;

        //  This logic will bring a new chart onscreen at roughly twice the true
        //  paper scale equivalent. Note that first chart opened on application
        //  startup (bOpenSpecified = true) will open at the config saved scale
        if (bNewChart && !g_bPreserveScaleOnX && !bOpenSpecified) {
          proposed_scale_onscreen = m_singleChart->GetNativeScale() / 2;
          double equivalent_vp_scale =
              GetCanvasScaleFactor() / proposed_scale_onscreen;
          double new_scale_ppm =
              m_singleChart->GetNearestPreferredScalePPM(equivalent_vp_scale);
          proposed_scale_onscreen = GetCanvasScaleFactor() / new_scale_ppm;
        }

        if (m_bFollow) {  // bounds-check the scale only if in autofollow
          proposed_scale_onscreen =
              wxMin(proposed_scale_onscreen,
                    m_singleChart->GetNormalScaleMax(GetCanvasScaleFactor(),
                                                     GetCanvasWidth()));
          proposed_scale_onscreen =
              wxMax(proposed_scale_onscreen,
                    m_singleChart->GetNormalScaleMin(GetCanvasScaleFactor(), g_b_overzoom_x));
        }

        set_scale = GetCanvasScaleFactor() / proposed_scale_onscreen;
      }

      bNewView |= SetViewPoint(vpLat, vpLon, set_scale,
                               m_singleChart->GetChartSkew() * PI / 180.,
                               GetVPRotation());
    }
  }  // new stack

  else  // No change in Chart Stack
  {
    if ((m_bFollow) && m_singleChart)
      bNewView |= SetViewPoint(vpLat, vpLon, GetVPScale(),
                               m_singleChart->GetChartSkew() * PI / 180.,
                               GetVPRotation());
  }

update_finish:
  //  Update the ownship position on thumbnail chart, if shown
  if (pthumbwin && pthumbwin->IsShown()) {
    if (pthumbwin->pThumbChart) {
      if (pthumbwin->pThumbChart->UpdateThumbData(gLat, gLon))
        pthumbwin->Refresh(TRUE);
    }
  }

  //m_bFirstAuto = false;  // Auto open on program start

  //  If we need a Refresh(), do it here...
  //  But don't duplicate a Refresh() done by SetViewPoint()
  if (bNewChart && !bNewView) Refresh(false);

#ifdef ocpnUSE_GL
  // If a new chart, need to invalidate gl viewport for refresh
  // so the fbo gets flushed
  if (m_glcc && g_bopengl && bNewChart) GetglCanvas()->Invalidate();
#endif

  return bNewChart | bNewView;
}

void ChartCanvas::SelectQuiltRefdbChart(int db_index, bool b_autoscale) {
  if (m_pCurrentStack) m_pCurrentStack->SetCurrentEntryFromdbIndex(db_index);

  SetQuiltRefChart(db_index);
  if (ChartData) {
    ChartBase *pc = ChartData->OpenChartFromDB(db_index, FULL_INIT);
    if (pc) {
      if (b_autoscale) {
        double best_scale_ppm = GetBestVPScale(pc);
        SetVPScale(best_scale_ppm);
      }
    } else
      SetQuiltRefChart(-1);
  } else
    SetQuiltRefChart(-1);
}

void ChartCanvas::SelectQuiltRefChart(int selected_index) {
  std::vector<int> piano_chart_index_array =
      GetQuiltExtendedStackdbIndexArray();
  int current_db_index = piano_chart_index_array[selected_index];

  SelectQuiltRefdbChart(current_db_index);
}

double ChartCanvas::GetBestVPScale(ChartBase *pchart) {
  if (pchart) {
    double proposed_scale_onscreen = GetCanvasScaleFactor() / GetVPScale();

    if ((g_bPreserveScaleOnX) ||
        (CHART_TYPE_CM93COMP == pchart->GetChartType())) {
      double new_scale_ppm = GetVPScale();
      proposed_scale_onscreen = GetCanvasScaleFactor() / new_scale_ppm;
    } else {
      //  This logic will bring the new chart onscreen at roughly twice the true
      //  paper scale equivalent.
      proposed_scale_onscreen = pchart->GetNativeScale() / 2;
      double equivalent_vp_scale =
          GetCanvasScaleFactor() / proposed_scale_onscreen;
      double new_scale_ppm =
          pchart->GetNearestPreferredScalePPM(equivalent_vp_scale);
      proposed_scale_onscreen = GetCanvasScaleFactor() / new_scale_ppm;
    }

    // Do not allow excessive underzoom, even if the g_bPreserveScaleOnX flag is
    // set. Otherwise, we get severe performance problems on all platforms

    double max_underzoom_multiplier = 2.0;
    if (GetVP().b_quilt) {
      double scale_max = m_pQuilt->GetNomScaleMin(pchart->GetNativeScale(),
                                                  pchart->GetChartType(),
                                                  pchart->GetChartFamily());
      max_underzoom_multiplier = scale_max / pchart->GetNativeScale();
    }

    proposed_scale_onscreen = wxMin(
        proposed_scale_onscreen,
        pchart->GetNormalScaleMax(GetCanvasScaleFactor(), GetCanvasWidth()) *
            max_underzoom_multiplier);

    //  And, do not allow excessive overzoom either
    proposed_scale_onscreen =
        wxMax(proposed_scale_onscreen,
              pchart->GetNormalScaleMin(GetCanvasScaleFactor(), false));

    return GetCanvasScaleFactor() / proposed_scale_onscreen;
  } else
    return 1.0;
}

void ChartCanvas::SetupCanvasQuiltMode(void) {
  if (GetQuiltMode())  // going to quilt mode
  {
    ChartData->LockCache();

    m_Piano->SetNoshowIndexArray(m_quilt_noshow_index_array);

    ocpnStyle::Style *style = g_StyleManager->GetCurrentStyle();

    /*m_Piano->SetVizIcon(new wxBitmap(style->GetIcon(_T("viz"))));
    m_Piano->SetInVizIcon(new wxBitmap(style->GetIcon(_T("redX"))));
    m_Piano->SetTMercIcon(new wxBitmap(style->GetIcon(_T("tmercprj"))));
    m_Piano->SetSkewIcon(new wxBitmap(style->GetIcon(_T("skewprj"))));*/

    m_Piano->SetRoundedRectangles(true);

    //    Select the proper Ref chart
    int target_new_dbindex = -1;
    if (m_pCurrentStack) {
      target_new_dbindex =
          GetQuiltReferenceChartIndex();  // m_pCurrentStack->GetCurrentEntrydbIndex();

      if (-1 != target_new_dbindex) {
        if (!IsChartQuiltableRef(target_new_dbindex)) {
          int proj = ChartData->GetDBChartProj(target_new_dbindex);
          int type = ChartData->GetDBChartType(target_new_dbindex);

          // walk the stack up looking for a satisfactory chart
          int stack_index = m_pCurrentStack->CurrentStackEntry;

          while ((stack_index < m_pCurrentStack->nEntry - 1) &&
                 (stack_index >= 0)) {
            int proj_tent = ChartData->GetDBChartProj(
                m_pCurrentStack->GetDBIndex(stack_index));
            int type_tent = ChartData->GetDBChartType(
                m_pCurrentStack->GetDBIndex(stack_index));

            if (IsChartQuiltableRef(m_pCurrentStack->GetDBIndex(stack_index))) {
              if ((proj == proj_tent) && (type_tent == type)) {
                target_new_dbindex = m_pCurrentStack->GetDBIndex(stack_index);
                break;
              }
            }
            stack_index++;
          }
        }
      }
    }

    if (IsChartQuiltableRef(target_new_dbindex))
      SelectQuiltRefdbChart(target_new_dbindex,
                            false);  // Try not to allow a scale change
    else
      SelectQuiltRefdbChart(-1, false);

    m_singleChart = NULL;  // Bye....

    //  Re-qualify the quilt reference chart selection
    AdjustQuiltRefChart();

    //  Restore projection type saved on last quilt mode toggle
    // TODO
    //             if(g_sticky_projection != -1)
    //                 GetVP().SetProjectionType(g_sticky_projection);
    //             else
    //                 GetVP().SetProjectionType(PROJECTION_MERCATOR);
    GetVP().SetProjectionType(PROJECTION_UNKNOWN);

  } else  // going to SC Mode
  {
    std::vector<int> empty_array;
    m_Piano->SetActiveKeyArray(empty_array);
    m_Piano->SetNoshowIndexArray(empty_array);
    m_Piano->SetEclipsedIndexArray(empty_array);

    ocpnStyle::Style *style = g_StyleManager->GetCurrentStyle();
    m_Piano->SetVizIcon(new wxBitmap(style->GetIcon(_T("viz"))));
    m_Piano->SetInVizIcon(new wxBitmap(style->GetIcon(_T("redX"))));
    m_Piano->SetTMercIcon(new wxBitmap(style->GetIcon(_T("tmercprj"))));
    m_Piano->SetSkewIcon(new wxBitmap(style->GetIcon(_T("skewprj"))));

    m_Piano->SetRoundedRectangles(false);
    // TODO  Make this a member g_sticky_projection = GetVP().m_projection_type;
  }

  //    When shifting from quilt to single chart mode, select the "best" single
  //    chart to show
  if (!GetQuiltMode()) {
    if (ChartData && ChartData->IsValid()) {
      UnlockQuilt();

      double tLat, tLon;
      if (m_bFollow == true) {
        tLat = gLat;
        tLon = gLon;
      } else {
        tLat = m_vLat;
        tLon = m_vLon;
      }

      if (!m_singleChart) {
        // Build a temporary chart stack based on tLat, tLon
        ChartStack TempStack;
        ChartData->BuildChartStack(&TempStack, tLat, tLon, g_sticky_chart,
                                   m_groupIndex);

        //    Iterate over the quilt charts actually shown, looking for the
        //    largest scale chart that will be in the new chartstack.... This
        //    will (almost?) always be the reference chart....

        ChartBase *Candidate_Chart = NULL;
        int cur_max_scale = (int)1e8;

        ChartBase *pChart = GetFirstQuiltChart();
        while (pChart) {
          //  Is this pChart in new stack?
          int tEntry =
              ChartData->GetStackEntry(&TempStack, pChart->GetFullPath());
          if (tEntry != -1) {
            if (pChart->GetNativeScale() < cur_max_scale) {
              Candidate_Chart = pChart;
              cur_max_scale = pChart->GetNativeScale();
            }
          }
          pChart = GetNextQuiltChart();
        }

        m_singleChart = Candidate_Chart;

        //    If the quilt is empty, there is no "best" chart.
        //    So, open the smallest scale chart in the current stack
        if (NULL == m_singleChart) {
          m_singleChart = ChartData->OpenStackChartConditional(
              &TempStack, TempStack.nEntry - 1, true, CHART_TYPE_DONTCARE,
              CHART_FAMILY_DONTCARE);
        }
      }

      //  Invalidate all the charts in the quilt,
      // as any cached data may be region based and not have fullscreen coverage
      InvalidateAllQuiltPatchs();

      if (m_singleChart) {
        int dbi = ChartData->FinddbIndex(m_singleChart->GetFullPath());
        std::vector<int> one_array;
        one_array.push_back(dbi);
        m_Piano->SetActiveKeyArray(one_array);
      }

      if (m_singleChart) {
        GetVP().SetProjectionType(m_singleChart->GetChartProjectionType());
      }
    }
    //    Invalidate the current stack so that it will be rebuilt on next tick
    if (m_pCurrentStack) m_pCurrentStack->b_valid = false;
  }
}

bool ChartCanvas::IsTempMenuBarEnabled() {
#ifdef __WXMSW__
  int major;
  wxGetOsVersion(&major);
  return (major >
          5);  //  For Windows, function is only available on Vista and above
#else
  return true;
#endif
}

double ChartCanvas::GetCanvasRangeMeters() {
  int width, height;
  GetSize(&width, &height);
  int minDimension = wxMin(width, height);

  double range = (minDimension / GetVP().view_scale_ppm) / 2;
  range *= cos(GetVP().clat * PI / 180.);
  return range;
}

void ChartCanvas::SetCanvasRangeMeters(double range) {
  int width, height;
  GetSize(&width, &height);
  int minDimension = wxMin(width, height);

  double scale_ppm = minDimension / (range / cos(GetVP().clat * PI / 180.));
  SetVPScale(scale_ppm / 2);
}

void ChartCanvas::SetDisplaySizeMM(double size) {
  m_display_size_mm = size;

  // int sx, sy;
  // wxDisplaySize( &sx, &sy );

  // Calculate pixels per mm for later reference
  wxSize sd = g_Platform->getDisplaySize();
  double horizontal = sd.x;
  // Set DPI (Win) scale factor
  g_scaler = g_Platform->GetDisplayDIPMult(this);

  m_pix_per_mm = (horizontal) / ((double)m_display_size_mm);
  m_canvas_scale_factor = (horizontal) / (m_display_size_mm / 1000.); //

  if (ps52plib) ps52plib->SetPPMM(m_pix_per_mm);

  wxString msg;
  msg.Printf(
      _T("Metrics:  m_display_size_mm: %g     g_Platform->getDisplaySize():  ")
      _T("%d:%d   "),
      m_display_size_mm, sd.x, sd.y);
  wxLogMessage(msg);

  int ssx, ssy;
  ::wxDisplaySize(&ssx, &ssy);
  msg.Printf(_T("wxDisplaySize(): %d %d"), ssx, ssy);
  wxLogMessage(msg);

  m_focus_indicator_pix = /*std::round*/ wxRound(1 * GetPixPerMM());
}
#if 0
void ChartCanvas::OnEvtCompressProgress( OCPN_CompressProgressEvent & event )
{
    wxString msg(event.m_string.c_str(), wxConvUTF8);
    // if cpus are removed between runs
    if(pprog_threads > 0 && compress_msg_array.GetCount() >  (unsigned int)pprog_threads) {
        compress_msg_array.RemoveAt(pprog_threads, compress_msg_array.GetCount() - pprog_threads);
    }

    if(compress_msg_array.GetCount() > (unsigned int)event.thread )
    {
        compress_msg_array.RemoveAt(event.thread);
        compress_msg_array.Insert( msg, event.thread);
    }
    else
        compress_msg_array.Add(msg);


    wxString combined_msg;
    for(unsigned int i=0 ; i < compress_msg_array.GetCount() ; i++) {
        combined_msg += compress_msg_array[i];
        combined_msg += _T("\n");
    }

    bool skip = false;
    pprog->Update(pprog_count, combined_msg, &skip );
    pprog->SetSize(pprog_size);
    if(skip)
        b_skipout = skip;
}
#endif
void ChartCanvas::InvalidateGL() {
  if (!m_glcc) return;
#ifdef ocpnUSE_GL
  if (g_bopengl) m_glcc->Invalidate();
#endif  
}

int ChartCanvas::GetCanvasChartNativeScale() {
  int ret = 1;
  if (!VPoint.b_quilt) {
    if (m_singleChart) ret = m_singleChart->GetNativeScale();
  } else
    ret = (int)m_pQuilt->GetRefNativeScale();

  return ret;
}

ChartBase *ChartCanvas::GetChartAtCursor() {
  ChartBase *target_chart;
  if (m_singleChart && (m_singleChart->GetChartFamily() == CHART_FAMILY_VECTOR))
    target_chart = m_singleChart;
  else if (VPoint.b_quilt)
    target_chart = m_pQuilt->GetChartAtPix(VPoint, wxPoint(mouse_x, mouse_y));
  else
    target_chart = NULL;
  return target_chart;
}

ChartBase *ChartCanvas::GetOverlayChartAtCursor() {
  ChartBase *target_chart;
  if (VPoint.b_quilt)
    target_chart =
        m_pQuilt->GetOverlayChartAtPix(VPoint, wxPoint(mouse_x, mouse_y));
  else
    target_chart = NULL;
  return target_chart;
}
int ChartCanvas::FindClosestCanvasChartdbIndex(int scale) {
  int new_dbIndex = -1;
  if (!VPoint.b_quilt) {
    if (m_pCurrentStack) {
      for (int i = 0; i < m_pCurrentStack->nEntry; i++) {
        int sc = ChartData->GetStackChartScale(m_pCurrentStack, i, NULL, 0);
        if (sc >= scale) {
          new_dbIndex = m_pCurrentStack->GetDBIndex(i);
          break;
        }
      }
    }
  } else {
    //    Using the current quilt, select a useable reference chart
    //    Said chart will be in the extended (possibly full-screen) stack,
    //    And will have a scale equal to or just greater than the stipulated
    //    value
    unsigned int im = m_pQuilt->GetExtendedStackIndexArray().size();
    if (im > 0) {
      for (unsigned int is = 0; is < im; is++) {
        const ChartTableEntry &m = ChartData->GetChartTableEntry(
            m_pQuilt->GetExtendedStackIndexArray()[is]);
        if ((m.Scale_ge(
                scale)) /* && (m_reference_family == m.GetChartFamily())*/) {
          new_dbIndex = m_pQuilt->GetExtendedStackIndexArray()[is];
          break;
        }
      }
    }
  }

  return new_dbIndex;
}

void ChartCanvas::EnablePaint(bool b_enable) {
  m_b_paint_enable = b_enable;
#ifdef ocpnUSE_GL
  if (m_glcc) m_glcc->EnablePaint(b_enable);
#endif
}

bool ChartCanvas::IsQuiltDelta() { return m_pQuilt->IsQuiltDelta(VPoint); }

void ChartCanvas::UnlockQuilt() { m_pQuilt->UnlockQuilt(); }

std::vector<int> ChartCanvas::GetQuiltIndexArray(void) {
  return m_pQuilt->GetQuiltIndexArray();  
}
//set quilt at VPoint
void ChartCanvas::SetQuiltMode(bool b_quilt) {
  VPoint.b_quilt = b_quilt;
  VPoint.b_FullScreenQuilt = g_bFullScreenQuilt;
}

bool ChartCanvas::GetQuiltMode(void) { return VPoint.b_quilt; }

int ChartCanvas::GetQuiltReferenceChartIndex(void) {
  return m_pQuilt->GetRefChartdbIndex();
}

void ChartCanvas::InvalidateAllQuiltPatchs(void) {
  m_pQuilt->InvalidateAllQuiltPatchs();
}

ChartBase *ChartCanvas::GetLargestScaleQuiltChart() {
  return m_pQuilt->GetLargestScaleChart();
}

ChartBase *ChartCanvas::GetFirstQuiltChart() {
  return m_pQuilt->GetFirstChart();
}

ChartBase *ChartCanvas::GetNextQuiltChart() { return m_pQuilt->GetNextChart(); }

int ChartCanvas::GetQuiltChartCount() { return m_pQuilt->GetnCharts(); }

void ChartCanvas::SetQuiltChartHiLiteIndex(int dbIndex) {
  m_pQuilt->SetHiliteIndex(dbIndex);
}

std::vector<int> ChartCanvas::GetQuiltCandidatedbIndexArray(bool flag1,
                                                            bool flag2) {
  return m_pQuilt->GetCandidatedbIndexArray(flag1, flag2);
}

int ChartCanvas::GetQuiltRefChartdbIndex(void) {
  return m_pQuilt->GetRefChartdbIndex();
}

std::vector<int> ChartCanvas::GetQuiltExtendedStackdbIndexArray() {
  return m_pQuilt->GetExtendedStackIndexArray();
}

std::vector<int> ChartCanvas::GetQuiltEclipsedStackdbIndexArray() {
  return m_pQuilt->GetEclipsedStackIndexArray();
}

void ChartCanvas::InvalidateQuilt(void) { return m_pQuilt->Invalidate(); }

double ChartCanvas::GetQuiltMaxErrorFactor() {
  return m_pQuilt->GetMaxErrorFactor();
}

bool ChartCanvas::IsChartQuiltableRef(int db_index) {
  return m_pQuilt->IsChartQuiltableRef(db_index);
}

bool ChartCanvas::IsChartLargeEnoughToRender(ChartBase *chart, ViewPort &vp) {
  double chartMaxScale =
      chart->GetNormalScaleMax(GetCanvasScaleFactor(), GetCanvasWidth());
  return (chartMaxScale * g_ChartNotRenderScaleFactor > vp.chart_scale);
}

ViewPort &ChartCanvas::GetVP() { return VPoint; }

void ChartCanvas::SetVP(ViewPort &vp) { VPoint = vp; }

void ChartCanvas::TriggerDeferredFocus() {
  //#if defined(__WXGTK__) || defined(__WXOSX__)

  m_deferredFocusTimer.Start(20, true);

#if defined(__WXGTK__) || defined(__WXOSX__)
  gFrame->Raise();
#endif

  //    gFrame->Raise();
  //#else
  //    SetFocus();
  //    Refresh(true);
  //#endif
}

void ChartCanvas::OnDeferredFocusTimerEvent(wxTimerEvent &event) {
  SetFocus();
  Refresh(true);
}

void ChartCanvas::ToggleChartOutlines(void) {
  m_bShowOutlines = !m_bShowOutlines;

  Refresh(false);

#ifdef ocpnUSE_GL  // opengl renders chart outlines as part of the chart this
                   // needs a full refresh
  if (g_bopengl) InvalidateGL();
#endif
}

void ChartCanvas::ToggleLookahead() {
  m_bLookAhead = !m_bLookAhead;
  m_OSoffsetx = 0;  // center ownship
  m_OSoffsety = 0;
}

void ChartCanvas::SetUpMode(int mode) {
  m_upMode = mode;

  if (mode != NORTH_UP_MODE) {
    //    Stuff the COGAvg table in case COGUp is selected
    double stuff = 0;
    if (!std::isnan(gCog)) stuff = gCog;

    if (g_COGAvgSec > 0) {
      for (int i = 0; i < g_COGAvgSec; i++) gFrame->COGTable[i] = stuff;
    }
    g_COGAvg = stuff;
    gFrame->FrameCOGTimer.Start(100, wxTIMER_CONTINUOUS);
  } else {
    if (!g_bskew_comp && (fabs(GetVPSkew()) > 0.0001))
      SetVPRotation(GetVPSkew());
    else
      SetVPRotation(0); /* reset to north up */
  }

  gFrame->DoChartUpdate();
}

bool ChartCanvas::DoCanvasCOGSet(void) {
  if (GetUpMode() == NORTH_UP_MODE) return false;

  if (std::isnan(g_COGAvg)) return true;

  double old_VPRotate = m_VPRotate;

  if ((GetUpMode() == HEAD_UP_MODE) && !std::isnan(gHdt)) {
    m_VPRotate = -gHdt * PI / 180.;
  } else if (GetUpMode() == COURSE_UP_MODE)
    m_VPRotate = -g_COGAvg * PI / 180.;

  SetVPRotation(m_VPRotate);
  bool bnew_chart = DoCanvasUpdate();

  if ((bnew_chart) || (old_VPRotate != m_VPRotate)) ReloadVP();

  return true;
}

void ChartCanvas::StopMovement() {
  m_panx = m_pany = 0;
  m_panspeed = 0;
  m_zoom_factor = 1;
  m_rotation_speed = 0;
  m_mustmove = 0;
#if 0
#if !defined(__WXGTK__) && !defined(__WXQT__)
    SetFocus();
    gFrame->Raise();
#endif
#endif
}

/* instead of integrating in timer callbacks
   (which do not always get called fast enough)
   we can perform the integration of movement
   at each render frame based on the time change */
bool ChartCanvas::StartTimedMovement(bool stoptimer) {
  // Start/restart the stop movement timer
  if (stoptimer) pMovementStopTimer->Start(800, wxTIMER_ONE_SHOT);

  if (!pMovementTimer->IsRunning()) {
    //        printf("timer not running, starting\n");
    pMovementTimer->Start(1, wxTIMER_ONE_SHOT);
  }

  if (m_panx || m_pany || m_zoom_factor != 1 || m_rotation_speed) {
    // already moving, gets called again because of key-repeat event
    return false;
  }

  m_last_movement_time = wxDateTime::UNow();

  /* jumpstart because paint gets called right away, if we want first frame to
   * move */
  //    m_last_movement_time -= wxTimeSpan::Milliseconds(100);

  //    Refresh( false );

  return true;
}

void ChartCanvas::DoTimedMovement() {
  if (m_pan_drag == wxPoint(0, 0) && !m_panx && !m_pany && m_zoom_factor == 1 &&
      !m_rotation_speed)
    return; /* not moving */

  wxDateTime now = wxDateTime::UNow();
  long dt = 0;
  if (m_last_movement_time.IsValid())
    dt = (now - m_last_movement_time).GetMilliseconds().ToLong();

  m_last_movement_time = now;

  if (dt > 500) /* if we are running very slow, don't integrate too fast */
    dt = 500;

  DoMovement(dt);
}

void ChartCanvas::DoMovement(long dt) {
  /* if we get here quickly assume 1ms so that some movement occurs */
  if (dt == 0) dt = 1;

  m_mustmove -= dt;
  if (m_mustmove < 0) m_mustmove = 0;

  if (m_pan_drag.x || m_pan_drag.y) {
    PanCanvas(m_pan_drag.x, m_pan_drag.y);
    m_pan_drag.x = m_pan_drag.y = 0;
  }

  if (m_panx || m_pany) {
    const double slowpan = .1, maxpan = 2;
    if (m_modkeys == wxMOD_ALT)
      m_panspeed = slowpan;
    else {
      m_panspeed += (double)dt / 500; /* apply acceleration */
      m_panspeed = wxMin(maxpan, m_panspeed);
    }
    PanCanvas(m_panspeed * m_panx * dt, m_panspeed * m_pany * dt);
  }

  if (m_zoom_factor != 1) {
    double alpha = 400, beta = 1.5;
    double zoom_factor = (exp(dt / alpha) - 1) / beta + 1;

    if (m_modkeys == wxMOD_ALT) zoom_factor = pow(zoom_factor, .15);

    if (m_zoom_factor < 1) zoom_factor = 1 / zoom_factor;

    //  Try to hit the zoom target exactly.
    // if(m_wheelzoom_stop_oneshot > 0)
    {
      if (zoom_factor > 1) {
        if (VPoint.chart_scale / zoom_factor <= m_zoom_target)
          zoom_factor = VPoint.chart_scale / m_zoom_target;
      }

      else if (zoom_factor < 1) {
        if (VPoint.chart_scale / zoom_factor >= m_zoom_target)
          zoom_factor = VPoint.chart_scale / m_zoom_target;
      }
    }

    if (fabs(zoom_factor - 1) > 1e-4)
      DoZoomCanvas(zoom_factor, m_bzooming_to_cursor);

    if (m_wheelzoom_stop_oneshot > 0) {
      if (m_wheelstopwatch.Time() > m_wheelzoom_stop_oneshot) {
        m_wheelzoom_stop_oneshot = 0;
        StopMovement();
      }

      //      Don't overshoot the zoom target.
      if (zoom_factor > 1) {
        if (VPoint.chart_scale <= m_zoom_target) {
          m_wheelzoom_stop_oneshot = 0;
          StopMovement();
        }
      } else if (zoom_factor < 1) {
        if (VPoint.chart_scale >= m_zoom_target) {
          m_wheelzoom_stop_oneshot = 0;
          StopMovement();
        }
      }
    }
  }

  if (m_rotation_speed) { /* in degrees per second */
    double speed = m_rotation_speed;
    if (m_modkeys == wxMOD_ALT) speed /= 10;
    DoRotateCanvas(VPoint.rotation + speed * PI / 180 * dt / 1000.0);
  }
}

void ChartCanvas::SetColorScheme(ColorScheme cs) {
  SetAlertString(_T(""));

  //    Setup ownship image pointers
  switch (cs) {
    case GLOBAL_COLOR_SCHEME_DAY:
      m_pos_image_red = &m_os_image_red_day;
      m_pos_image_grey = &m_os_image_grey_day;
      m_pos_image_yellow = &m_os_image_yellow_day;
      m_pos_image_user = m_pos_image_user_day;
      m_pos_image_user_grey = m_pos_image_user_grey_day;
      m_pos_image_user_yellow = m_pos_image_user_yellow_day;
      m_cTideBitmap = m_bmTideDay;
      m_cCurrentBitmap = m_bmCurrentDay;

      break;
    case GLOBAL_COLOR_SCHEME_DUSK:
      m_pos_image_red = &m_os_image_red_dusk;
      m_pos_image_grey = &m_os_image_grey_dusk;
      m_pos_image_yellow = &m_os_image_yellow_dusk;
      m_pos_image_user = m_pos_image_user_dusk;
      m_pos_image_user_grey = m_pos_image_user_grey_dusk;
      m_pos_image_user_yellow = m_pos_image_user_yellow_dusk;
      m_cTideBitmap = m_bmTideDusk;
      m_cCurrentBitmap = m_bmCurrentDusk;
      break;
    case GLOBAL_COLOR_SCHEME_NIGHT:
      m_pos_image_red = &m_os_image_red_night;
      m_pos_image_grey = &m_os_image_grey_night;
      m_pos_image_yellow = &m_os_image_yellow_night;
      m_pos_image_user = m_pos_image_user_night;
      m_pos_image_user_grey = m_pos_image_user_grey_night;
      m_pos_image_user_yellow = m_pos_image_user_yellow_night;
      m_cTideBitmap = m_bmTideNight;
      m_cCurrentBitmap = m_bmCurrentNight;
      break;
    default:
      m_pos_image_red = &m_os_image_red_day;
      m_pos_image_grey = &m_os_image_grey_day;
      m_pos_image_yellow = &m_os_image_yellow_day;
      m_pos_image_user = m_pos_image_user_day;
      m_pos_image_user_grey = m_pos_image_user_grey_day;
      m_pos_image_user_yellow = m_pos_image_user_yellow_day;
      m_cTideBitmap = m_bmTideDay;
      m_cCurrentBitmap = m_bmCurrentDay;
      break;
  }

  CreateDepthUnitEmbossMaps(cs);
  CreateOZEmbossMapData(cs);

  //  Set up fog effect base color
  m_fog_color = wxColor(
      170, 195, 240);  // this is gshhs (backgound world chart) ocean color
  float dim = 1.0;
  switch (cs) {
    case GLOBAL_COLOR_SCHEME_DUSK:
      dim = 0.5;
      break;
    case GLOBAL_COLOR_SCHEME_NIGHT:
      dim = 0.25;
      break;
    default:
      break;
  }
  m_fog_color.Set(m_fog_color.Red() * dim, m_fog_color.Green() * dim,
                  m_fog_color.Blue() * dim);

  //  Really dark
#if 0
    if( cs == GLOBAL_COLOR_SCHEME_DUSK || cs == GLOBAL_COLOR_SCHEME_NIGHT ) {
        SetBackgroundColour( wxColour(0,0,0) );

        SetWindowStyleFlag( (GetWindowStyleFlag() & ~wxSIMPLE_BORDER) | wxNO_BORDER);
    }
    else{
        SetWindowStyleFlag( (GetWindowStyleFlag() & ~wxNO_BORDER) | wxSIMPLE_BORDER);
#ifndef __WXMAC__
        SetBackgroundColour( wxNullColour );
#endif
    }
#endif

  m_Piano->SetColorScheme(cs);

  if (pWorldBackgroundChart) pWorldBackgroundChart->SetColorScheme(cs);
#ifdef ocpnUSE_GL
  if (g_bopengl && m_glcc) {
    m_glcc->SetColorScheme(cs);
    g_glTextureManager->ClearAllRasterTextures();
    // m_glcc->FlushFBO();
  }
#endif
  SetbTCUpdate(true);  // force re-render of tide/current locators
  m_brepaint_piano = true;

  ReloadVP();

  m_cs = cs;
}

wxBitmap ChartCanvas::CreateDimBitmap(wxBitmap &Bitmap, double factor) {
  wxImage img = Bitmap.ConvertToImage();
  int sx = img.GetWidth();
  int sy = img.GetHeight();

  wxImage new_img(img);

  for (int i = 0; i < sx; i++) {
    for (int j = 0; j < sy; j++) {
      if (!img.IsTransparent(i, j)) {
        new_img.SetRGB(i, j, (unsigned char)(img.GetRed(i, j) * factor),
                       (unsigned char)(img.GetGreen(i, j) * factor),
                       (unsigned char)(img.GetBlue(i, j) * factor));
      }
    }
  }

  wxBitmap ret = wxBitmap(new_img);

  return ret;
}

void ChartCanvas::RotateTimerEvent(wxTimerEvent &event) {
  m_b_rot_hidef = true;
  ReloadVP();
}

void ChartCanvas::SetCursorStatus(double cursor_lat, double cursor_lon) {
  if (!parent_frame->m_pStatusBar) return;

  wxString s1;
  s1 += _T(" ");
  s1 += toSDMM(1, cursor_lat);
  s1 += _T("   ");
  s1 += toSDMM(2, cursor_lon);

  if (STAT_FIELD_CURSOR_LL >= 0)
    parent_frame->SetStatusText(s1, STAT_FIELD_CURSOR_LL);

  if (STAT_FIELD_CURSOR_BRGRNG < 0) return;

  double brg, dist;
  wxString s = "";
  DistanceBearingMercator(cursor_lat, cursor_lon, gLat, gLon, &brg, &dist);  

  s << FormatDistanceAdaptive(dist);

  // CUSTOMIZATION - LIVE ETA OPTION
  // -------------------------------------------------------
  // Calculate an "live" ETA based on route starting from the current
  // position of the boat and goes to the cursor of the mouse.
  // In any case, an standard ETA will be calculated with a default speed
  // of the boat to give an estimation of the route (in particular if GPS
  // is off).

  // Display only if option "live ETA" is selected in Settings > Display >
  // General.
  if (g_bShowLiveETA) {
    float realTimeETA;
    float boatSpeed;
    float boatSpeedDefault = g_defaultBoatSpeed;

    // Calculate Estimate Time to Arrival (ETA) in minutes
    // Check before is value not closed to zero (it will make an very big
    // number...)
    if (!std::isnan(gSog)) {
      boatSpeed = gSog;
      if (boatSpeed < 0.5) {
        realTimeETA = 0;
      } else {
        realTimeETA = dist / boatSpeed * 60;
      }
    } else {
      realTimeETA = 0;
    }

    // Add space after distance display
    s << " ";
    // Display ETA
    s << minutesToHoursDays(realTimeETA);

    // In any case, display also an ETA with default speed at 6knts

    s << " [@";
    s << wxString::Format(_T("%d"), (int)toUsrSpeed(boatSpeedDefault, -1));
    s << wxString::Format(_T("%s"), getUsrSpeedUnit(-1));
    s << " ";
    s << minutesToHoursDays(dist / boatSpeedDefault * 60);
    s << "]";
  }
  // END OF - LIVE ETA OPTION

  parent_frame->SetStatusText(s, STAT_FIELD_CURSOR_BRGRNG);
}

// CUSTOMIZATION - FORMAT MINUTES
// -------------------------------------------------------
// New function to format minutes into a more readable format:
//  * Hours + minutes, or
//  * Days + hours.
wxString minutesToHoursDays(float timeInMinutes) {
  wxString s;

  if (timeInMinutes == 0) {
    s << "--min";
  }

  // Less than 60min, keep time in minutes
  else if (timeInMinutes < 60 && timeInMinutes != 0) {
    s << wxString::Format(_T("%d"), (int)timeInMinutes);
    s << "min";
  }

  // Between 1h and less than 24h, display time in hours, minutes
  else if (timeInMinutes >= 60 && timeInMinutes < 24 * 60) {
    int hours;
    int min;
    hours = (int)timeInMinutes / 60;
    min = (int)timeInMinutes % 60;

    if (min == 0) {
      s << wxString::Format(_T("%d"), hours);
      s << "h";
    } else {
      s << wxString::Format(_T("%d"), hours);
      s << "h";
      s << wxString::Format(_T("%d"), min);
      s << "min";
    }

  }

  // More than 24h, display time in days, hours
  else if (timeInMinutes > 24 * 60) {
    int days;
    int hours;
    days = (int)(timeInMinutes / 60) / 24;
    hours = (int)(timeInMinutes / 60) % 24;

    if (hours == 0) {
      s << wxString::Format(_T("%d"), days);
      s << "d";
    } else {
      s << wxString::Format(_T("%d"), days);
      s << "d";
      s << wxString::Format(_T("%d"), hours);
      s << "h";
    }
  }

  return s;
}

// END OF CUSTOMIZATION - FORMAT MINUTES
// Thanks open source code ;-)
// -------------------------------------------------------

void ChartCanvas::GetCursorLatLon(double *lat, double *lon) {
  double clat, clon;
  GetCanvasPixPoint(mouse_x, mouse_y, clat, clon);
  *lat = clat;
  *lon = clon;
}

void ChartCanvas::GetDoubleCanvasPointPix(double rlat, double rlon,
                                          wxPoint2DDouble *r) {
  return GetDoubleCanvasPointPixVP(GetVP(), rlat, rlon, r);
}

void ChartCanvas::GetDoubleCanvasPointPixVP(ViewPort &vp, double rlat,
                                            double rlon, wxPoint2DDouble *r) {
  // If the Current Chart is a raster chart, and the
  // requested lat/long is within the boundaries of the chart,
  // and the VP is not rotated,
  // then use the embedded BSB chart georeferencing algorithm
  // for greater accuracy
  // Additionally, use chart embedded georef if the projection is TMERC
  //  i.e. NOT MERCATOR and NOT POLYCONIC

  // If for some reason the chart rejects the request by returning an error,
  // then fall back to Viewport Projection estimate from canvas parameters
  if (!g_bopengl && m_singleChart &&
      (m_singleChart->GetChartFamily() == CHART_FAMILY_RASTER) &&
      (((fabs(vp.rotation) < .0001) && (fabs(vp.skew) < .0001)) ||
       ((m_singleChart->GetChartProjectionType() != PROJECTION_MERCATOR) &&
        (m_singleChart->GetChartProjectionType() !=
         PROJECTION_TRANSVERSE_MERCATOR) &&
        (m_singleChart->GetChartProjectionType() != PROJECTION_POLYCONIC))) &&
      (m_singleChart->GetChartProjectionType() == vp.m_projection_type) &&
      (m_singleChart->GetChartType() != CHART_TYPE_PLUGIN)) {
    ChartBaseBSB *Cur_BSB_Ch = dynamic_cast<ChartBaseBSB *>(m_singleChart);
    //                        bool bInside = G_FloatPtInPolygon ( ( MyFlPoint *
    //                        ) Cur_BSB_Ch->GetCOVRTableHead ( 0 ),
    //                                                            Cur_BSB_Ch->GetCOVRTablenPoints
    //                                                            ( 0 ), rlon,
    //                                                            rlat );
    //                        bInside = true;
    //                        if ( bInside )
    if (Cur_BSB_Ch) {
      //    This is a Raster chart....
      //    If the VP is changing, the raster chart parameters may not yet be
      //    setup So do that before accessing the chart's embedded
      //    georeferencing
      Cur_BSB_Ch->SetVPRasterParms(vp);
      double rpixxd, rpixyd;
      if (0 == Cur_BSB_Ch->latlong_to_pix_vp(rlat, rlon, rpixxd, rpixyd, vp)) {
        r->m_x = rpixxd;
        r->m_y = rpixyd;
        return;
      }
    }
  }

  //    if needed, use the VPoint scaling estimator,
  *r = vp.GetDoublePixFromLL(rlat, rlon);
}

// This routine might be deleted and all of the rendering improved
// to have floating point accuracy
bool ChartCanvas::GetCanvasPointPix(double rlat, double rlon, wxPoint *r) {
  return GetCanvasPointPixVP(GetVP(), rlat, rlon, r);
}

bool ChartCanvas::GetCanvasPointPixVP(ViewPort &vp, double rlat, double rlon,
                                      wxPoint *r) {
  wxPoint2DDouble p;
  GetDoubleCanvasPointPixVP(vp, rlat, rlon, &p);

  // some projections give nan values when invisible values (other side of
  // world) are requested we should stop using integer coordinates or return
  // false here (and test it everywhere)
  if (std::isnan(p.m_x)) {
    *r = wxPoint(INVALID_COORD, INVALID_COORD);
    return false;
  }

  if( (abs(p.m_x) < 1e6) && (abs(p.m_y) < 1e6) )
    *r = wxPoint(wxRound(p.m_x), wxRound(p.m_y));
  else
    *r = wxPoint(INVALID_COORD, INVALID_COORD);

  return true;
}

void ChartCanvas::GetCanvasPixPoint(double x, double y, double &lat,
                                    double &lon) {
  // If the Current Chart is a raster chart, and the
  // requested x,y is within the boundaries of the chart,
  // and the VP is not rotated,
  // then use the embedded BSB chart georeferencing algorithm
  // for greater accuracy
  // Additionally, use chart embedded georef if the projection is TMERC
  //  i.e. NOT MERCATOR and NOT POLYCONIC

  // If for some reason the chart rejects the request by returning an error,
  // then fall back to Viewport Projection  estimate from canvas parameters
  bool bUseVP = true;

  if (!g_bopengl && m_singleChart &&
      (m_singleChart->GetChartFamily() == CHART_FAMILY_RASTER) &&
      (((fabs(GetVP().rotation) < .0001) && (fabs(GetVP().skew) < .0001)) ||
       ((m_singleChart->GetChartProjectionType() != PROJECTION_MERCATOR) &&
        (m_singleChart->GetChartProjectionType() !=
         PROJECTION_TRANSVERSE_MERCATOR) &&
        (m_singleChart->GetChartProjectionType() != PROJECTION_POLYCONIC))) &&
      (m_singleChart->GetChartProjectionType() == GetVP().m_projection_type) &&
      (m_singleChart->GetChartType() != CHART_TYPE_PLUGIN)) {
    ChartBaseBSB *Cur_BSB_Ch = dynamic_cast<ChartBaseBSB *>(m_singleChart);

    // TODO     maybe need iterative process to validate bInside
    //          first pass is mercator, then check chart boundaries

    if (Cur_BSB_Ch) {
      //    This is a Raster chart....
      //    If the VP is changing, the raster chart parameters may not yet be
      //    setup So do that before accessing the chart's embedded
      //    georeferencing
      Cur_BSB_Ch->SetVPRasterParms(GetVP());

      double slat, slon;
      if (0 == Cur_BSB_Ch->vp_pix_to_latlong(GetVP(), x, y, &slat, &slon)) {
        lat = slat;

        if (slon < -180.)
          slon += 360.;
        else if (slon > 180.)
          slon -= 360.;

        lon = slon;
        bUseVP = false;
      }
    }
  }

  //    if needed, use the VPoint scaling estimator
  if (bUseVP) {
    GetVP().GetLLFromPix(wxPoint2DDouble(x, y), &lat, &lon);
  }
}

void ChartCanvas::ZoomCanvasSimple(double factor) {
  DoZoomCanvas(factor, false);
  extendedSectorLegs.clear();
}

void ChartCanvas::ZoomCanvas(double factor, bool can_zoom_to_cursor,
                             bool stoptimer) {
  m_bzooming_to_cursor = can_zoom_to_cursor && g_bEnableZoomToCursor;

  if (g_bsmoothpanzoom) {
    if (StartTimedMovement(stoptimer)) {
      m_mustmove += 150; /* for quick presses register as 200 ms duration */
      m_zoom_factor = factor;
    }

    m_zoom_target = VPoint.chart_scale / factor;
  } else {
    if (m_modkeys == wxMOD_ALT) factor = pow(factor, .15);

    DoZoomCanvas(factor, can_zoom_to_cursor);
  }

  extendedSectorLegs.clear();
}

void ChartCanvas::DoZoomCanvas(double factor, bool can_zoom_to_cursor) {
  // possible on startup
  if (!ChartData) return;
  if (!m_pCurrentStack) return;

  /* TODO: queue the quilted loading code to a background thread
     so yield is never called from here, and also rendering is not delayed */

  //    Cannot allow Yield() re-entrancy here
  if (m_bzooming) return;
  m_bzooming = true;

  double old_ppm = GetVP().view_scale_ppm;

  //  Capture current cursor position for zoom to cursor
  double zlat = m_cursor_lat;
  double zlon = m_cursor_lon;

  double proposed_scale_onscreen =
      GetVP().chart_scale /
      factor;  // GetCanvasScaleFactor() / ( GetVPScale() * factor );
  bool b_do_zoom = false;

  if (factor > 1) {
    b_do_zoom = true;

    // double zoom_factor = factor;

    ChartBase *pc = NULL;

    if (!VPoint.b_quilt) {
      pc = m_singleChart;
    } else {
      int new_db_index = m_pQuilt->AdjustRefOnZoomIn(proposed_scale_onscreen);
      if (new_db_index >= 0)
        pc = ChartData->OpenChartFromDB(new_db_index, FULL_INIT);

      if (m_pCurrentStack)
        m_pCurrentStack->SetCurrentEntryFromdbIndex(
            new_db_index);  // highlite the correct bar entry
    }

    if (pc) {
      //             double target_scale_ppm = GetVPScale() * zoom_factor;
      //             proposed_scale_onscreen = GetCanvasScaleFactor() /
      //             target_scale_ppm;

      //  Query the chart to determine the appropriate zoom range
      double min_allowed_scale =
          g_maxzoomin;  // Roughly, latitude dependent for mercator charts

      if (proposed_scale_onscreen < min_allowed_scale) {
        if (min_allowed_scale == GetCanvasScaleFactor() / (GetVPScale())) {
          m_zoom_factor = 1; /* stop zooming */
          b_do_zoom = false;
        } else
          proposed_scale_onscreen = min_allowed_scale;
      }

    } else {
      proposed_scale_onscreen = wxMax(proposed_scale_onscreen, g_maxzoomin);
    }

  } else if (factor < 1) {
    b_do_zoom = true;

    ChartBase *pc = NULL;

    bool b_smallest = false;

    if (!VPoint.b_quilt) {  // not quilted
      pc = m_singleChart;

      if (pc) {
        //      If m_singleChart is not on the screen, unbound the zoomout
        LLBBox viewbox = VPoint.GetBBox();
        //                BoundingBox chart_box;
        int current_index = ChartData->FinddbIndex(pc->GetFullPath());
        double max_allowed_scale;

        max_allowed_scale = GetCanvasScaleFactor() / m_absolute_min_scale_ppm;

        //  We can allow essentially unbounded zoomout in single chart mode
        //                if( ChartData->GetDBBoundingBox( current_index,
        //                &chart_box ) &&
        //                    !viewbox.IntersectOut( chart_box ) )
        //                    //  Clamp the minimum scale zoom-out to the value
        //                    specified by the chart max_allowed_scale =
        //                    wxMin(max_allowed_scale, 4.0 *
        //                                              pc->GetNormalScaleMax(
        //                                              GetCanvasScaleFactor(),
        //                                                                     GetCanvasWidth() ) );
        if (proposed_scale_onscreen > max_allowed_scale) {
          m_zoom_factor = 1; /* stop zooming */
          proposed_scale_onscreen = max_allowed_scale;
        }
      }

    } else {
      int new_db_index = m_pQuilt->AdjustRefOnZoomOut(proposed_scale_onscreen);
      if (new_db_index >= 0)
        pc = ChartData->OpenChartFromDB(new_db_index, FULL_INIT);

      if (m_pCurrentStack)
        m_pCurrentStack->SetCurrentEntryFromdbIndex(
            new_db_index);  // highlite the correct bar entry

      b_smallest = m_pQuilt->IsChartSmallestScale(new_db_index);

      if (b_smallest || (0 == m_pQuilt->GetExtendedStackCount()))
        proposed_scale_onscreen =
            wxMin(proposed_scale_onscreen,
                  GetCanvasScaleFactor() / m_absolute_min_scale_ppm);
    }

    // set a minimum scale
    if ((GetCanvasScaleFactor() / proposed_scale_onscreen) <
        m_absolute_min_scale_ppm)
      b_do_zoom = false;
  }

  double new_scale =
      GetVPScale() * (GetVP().chart_scale / proposed_scale_onscreen);

  if (b_do_zoom) {
    if (can_zoom_to_cursor && g_bEnableZoomToCursor) {
      //  Arrange to combine the zoom and pan into one operation for smoother
      //  appearance
      SetVPScale(new_scale, false);  // adjust, but deferred refresh

      wxPoint r;
      GetCanvasPointPix(zlat, zlon, &r);
      PanCanvas(r.x - mouse_x, r.y - mouse_y);  // this will give the Refresh()
    } else {
      SetVPScale(new_scale);

      if (m_bFollow) DoCanvasUpdate();
    }
  }

  m_bzooming = false;
}
int rot;
void ChartCanvas::RotateCanvas(double dir) {
  //SetUpMode(NORTH_UP_MODE);

  if (g_bsmoothpanzoom) {
    if (StartTimedMovement()) {
      m_mustmove += 150; /* for quick presses register as 200 ms duration */
      m_rotation_speed = dir * 60;
    }
  } else {
    double speed = dir * 10;
    if (m_modkeys == wxMOD_ALT) speed /= 20;
    DoRotateCanvas(VPoint.rotation + PI / 180 * speed);
  }
}

void ChartCanvas::DoRotateCanvas(double rotation) {
  while (rotation < 0) rotation += 2 * PI;
  while (rotation > 2 * PI) rotation -= 2 * PI;

  if (rotation == VPoint.rotation || std::isnan(rotation)) return;

  SetVPRotation(rotation);
  parent_frame->UpdateRotationState(VPoint.rotation);
}

void ChartCanvas::DoTiltCanvas(double tilt) {
  while (tilt < 0) tilt = 0;
  while (tilt > .95) tilt = .95;

  if (tilt == VPoint.tilt || std::isnan(tilt)) return;

  VPoint.tilt = tilt;
  Refresh(false);
}

void ChartCanvas::TogglebFollow(void) {
  if (!m_bFollow)
    SetbFollow();
  else
    ClearbFollow();
}

void ChartCanvas::ClearbFollow(void) {
  m_bFollow = false;  // update the follow flag

  parent_frame->SetMenubarItemState(ID_MENU_NAV_FOLLOW, false);

  DoCanvasUpdate();
  ReloadVP();
  parent_frame->SetChartUpdatePeriod();
}

void ChartCanvas::SetbFollow(void) {
  JumpToPosition(gLat, gLon, GetVPScale());
  m_bFollow = true;

  parent_frame->SetMenubarItemState(ID_MENU_NAV_FOLLOW, true);
  // Is the OWNSHIP on-screen?
  // If not, then reset the OWNSHIP offset to 0 (center screen)
  if ((fabs(m_OSoffsetx) > VPoint.pix_width / 2) ||
      (fabs(m_OSoffsety) > VPoint.pix_height / 2)) {
    m_OSoffsetx = 0;
    m_OSoffsety = 0;
  }

  DoCanvasUpdate();
  ReloadVP();
  parent_frame->SetChartUpdatePeriod();
}

void ChartCanvas::JumpToPosition(double lat, double lon, double scale_ppm) {
  if (lon > 180.0) lon -= 360.0;
  m_vLat = lat;
  m_vLon = lon;
  StopMovement();
  m_bFollow = false;

  if (!GetQuiltMode()) {
    double skew = 0;
    if (m_singleChart) skew = m_singleChart->GetChartSkew() * PI / 180.;
    SetViewPoint(lat, lon, scale_ppm, skew, GetVPRotation());
  } else {
    if (scale_ppm != GetVPScale()) {
      // XXX should be done in SetViewPoint
      VPoint.chart_scale = m_canvas_scale_factor / (scale_ppm);
      AdjustQuiltRefChart();
    }
    SetViewPoint(lat, lon, scale_ppm, 0, GetVPRotation());
  }
}

bool ChartCanvas::PanCanvas(double dx, double dy) {
  if (!ChartData) return false;

  extendedSectorLegs.clear();

  // double clat = VPoint.clat, clon = VPoint.clon;
  double dlat, dlon;
  wxPoint2DDouble p(VPoint.pix_width / 2.0, VPoint.pix_height / 2.0);

  int iters = 0;
  for (;;) {
    GetCanvasPixPoint(p.m_x + trunc(dx), p.m_y + trunc(dy), dlat, dlon);

    if (iters++ > 5) return false;
    if (!std::isnan(dlat)) break;

    dx *= .5, dy *= .5;
    if (fabs(dx) < 1 && fabs(dy) < 1) return false;
  }

  // avoid overshooting the poles
  if (dlat > 90)
    dlat = 90;
  else if (dlat < -90)
    dlat = -90;

  if (dlon > 360.) dlon -= 360.;
  if (dlon < -360.) dlon += 360.;

  //    This should not really be necessary, but round-trip georef on some
  //    charts is not perfect, So we can get creep on repeated unidimensional
  //    pans, and corrupt chart cacheing.......

  //    But this only works on north-up projections
  // TODO: can we remove this now?
  //     if( ( ( fabs( GetVP().skew ) < .001 ) ) && ( fabs( GetVP().rotation ) <
  //     .001 ) ) {
  //
  //         if( dx == 0 ) dlon = clon;
  //         if( dy == 0 ) dlat = clat;
  //     }

  int cur_ref_dbIndex = m_pQuilt->GetRefChartdbIndex();

  SetViewPoint(dlat, dlon, VPoint.view_scale_ppm, VPoint.skew, VPoint.rotation);

  if (VPoint.b_quilt) {
    int new_ref_dbIndex = m_pQuilt->GetRefChartdbIndex();
    if ((new_ref_dbIndex != cur_ref_dbIndex) && (new_ref_dbIndex != -1)) {
      // Tweak the scale slightly for a new ref chart
      ChartBase *pc = ChartData->OpenChartFromDB(new_ref_dbIndex, FULL_INIT);
      if (pc) {
        double tweak_scale_ppm =
            pc->GetNearestPreferredScalePPM(VPoint.view_scale_ppm);
        SetVPScale(tweak_scale_ppm);
      }
    }
  }

  //  Turn off bFollow only if the ownship has left the screen
  double offx, offy;
  toSM(dlat, dlon, gLat, gLon, &offx, &offy);

  double offset_angle = atan2(offy, offx);
  double offset_distance = sqrt((offy * offy) + (offx * offx));
  double chart_angle = GetVPRotation();
  double target_angle = chart_angle - offset_angle;
  double d_east_mod = offset_distance * cos(target_angle);
  double d_north_mod = offset_distance * sin(target_angle);

  m_OSoffsetx = d_east_mod * VPoint.view_scale_ppm;
  m_OSoffsety = -d_north_mod * VPoint.view_scale_ppm;

  if (m_bFollow && ((fabs(m_OSoffsetx) > VPoint.pix_width / 2) || (fabs(m_OSoffsety) > VPoint.pix_height / 2))) {
    m_bFollow = false;  // update the follow flag
  }

  Refresh(false);

  pCurTrackTimer->Start(m_curtrack_timer_msec, wxTIMER_ONE_SHOT);

  return true;
}

void ChartCanvas::ReloadVP(bool b_adjust) {
  if (g_brightness_init) SetScreenBrightness(g_nbrightness);

  LoadVP(VPoint, b_adjust);
}

void ChartCanvas::LoadVP(ViewPort &vp, bool b_adjust) {
#ifdef ocpnUSE_GL
  if (g_bopengl && m_glcc) {
    m_glcc->Invalidate();
    if (m_glcc->GetSize() != GetSize()) {
      m_glcc->SetSize(GetSize());
    }
  } else
#endif
  {
    m_cache_vp.Invalidate(); //bValid : false
    m_bm_cache_vp.Invalidate(); //bValid : false
  }

  VPoint.Invalidate();

  if (m_pQuilt) m_pQuilt->Invalidate();
  
  SetViewPoint(vp.clat, vp.clon, vp.view_scale_ppm, vp.skew, vp.rotation, vp.m_projection_type, b_adjust);
}

void ChartCanvas::SetQuiltRefChart(int dbIndex) {
  m_pQuilt->SetReferenceChart(dbIndex);
  VPoint.Invalidate();
  m_pQuilt->Invalidate();
}

double ChartCanvas::GetBestStartScale(int dbi_hint, const ViewPort &vp) {
  if (m_pQuilt)
    return m_pQuilt->GetBestStartScale(dbi_hint, vp);
  else
    return vp.view_scale_ppm;
}

//      Verify and adjust the current reference chart,
//      so that it will not lead to excessive overzoom or underzoom onscreen
int ChartCanvas::AdjustQuiltRefChart() {
  int ret = -1;
  if (m_pQuilt) {
    wxASSERT(ChartData);
    ChartBase *pc =
        ChartData->OpenChartFromDB(m_pQuilt->GetRefChartdbIndex(), FULL_INIT);
    if (pc) {
      double min_ref_scale =
          pc->GetNormalScaleMin(m_canvas_scale_factor, false);
      double max_ref_scale =
          pc->GetNormalScaleMax(m_canvas_scale_factor, m_canvas_width);

      if (VPoint.chart_scale < min_ref_scale) {
        ret = m_pQuilt->AdjustRefOnZoomIn(VPoint.chart_scale);
      } else if (VPoint.chart_scale > max_ref_scale) {
        ret = m_pQuilt->AdjustRefOnZoomOut(VPoint.chart_scale);
      } else {
        bool brender_ok = IsChartLargeEnoughToRender(pc, VPoint);

        int ref_family = pc->GetChartFamily();

        if (!brender_ok) {
          unsigned int target_stack_index = 0;
          int target_stack_index_check =
              m_pQuilt->GetExtendedStackIndexArray()
                  [m_pQuilt->GetRefChartdbIndex()];  // Lookup

          if (wxNOT_FOUND != target_stack_index_check)
            target_stack_index = target_stack_index_check;

          int extended_array_count =
              m_pQuilt->GetExtendedStackIndexArray().size();
          while ((!brender_ok) &&
                 ((int)target_stack_index < (extended_array_count - 1))) {
            target_stack_index++;
            int test_db_index =
                m_pQuilt->GetExtendedStackIndexArray()[target_stack_index];

            if ((ref_family == ChartData->GetDBChartFamily(test_db_index)) &&
                IsChartQuiltableRef(test_db_index)) {
              //    open the target, and check the min_scale
              ChartBase *ptest_chart =
                  ChartData->OpenChartFromDB(test_db_index, FULL_INIT);
              if (ptest_chart) {
                brender_ok = IsChartLargeEnoughToRender(ptest_chart, VPoint);
              }
            }
          }

          if (brender_ok) {  // found a better reference chart
            int new_db_index =
                m_pQuilt->GetExtendedStackIndexArray()[target_stack_index];
            if ((ref_family == ChartData->GetDBChartFamily(new_db_index)) &&
                IsChartQuiltableRef(new_db_index)) {
              m_pQuilt->SetReferenceChart(new_db_index);
              ret = new_db_index;
            } else
              ret = m_pQuilt->GetRefChartdbIndex();
          } else
            ret = m_pQuilt->GetRefChartdbIndex();

        } else
          ret = m_pQuilt->GetRefChartdbIndex();
      }
    } else
      ret = -1;
  }

  return ret;
}

void ChartCanvas::UpdateCanvasOnGroupChange(void) {
  delete m_pCurrentStack;
  m_pCurrentStack = new ChartStack;
  wxASSERT(ChartData);
  ChartData->BuildChartStack(m_pCurrentStack, VPoint.clat, VPoint.clon,
                             m_groupIndex);

  if (m_pQuilt) {
    m_pQuilt->Compose(VPoint);
    SetFocus();
  }
}

bool ChartCanvas::SetViewPointByCorners(double latSW, double lonSW,
                                        double latNE, double lonNE) {
  // Center Point
  double latc = (latSW + latNE) / 2.0;
  double lonc = (lonSW + lonNE) / 2.0;

  // Get scale in ppm (latitude)
  double ne_easting, ne_northing;
  toSM(latNE, lonNE, latc, lonc, &ne_easting, &ne_northing);

  double sw_easting, sw_northing;
  toSM(latSW, lonSW, latc, lonc, &sw_easting, &sw_northing);

  double scale_ppm = VPoint.pix_height / fabs(ne_northing - sw_northing);

  return SetViewPoint(latc, lonc, scale_ppm, VPoint.skew, VPoint.rotation);
}

bool ChartCanvas::SetVPScale(double scale, bool refresh) {
  return SetViewPoint(VPoint.clat, VPoint.clon, scale, VPoint.skew,
                      VPoint.rotation, VPoint.m_projection_type, true, refresh);
}

bool ChartCanvas::SetVPProjection(int projection) {
  if (!g_bopengl)  // alternative projections require opengl
    return false;

  // the view scale varies depending on geographic location and projection
  // rescale to keep the relative scale on the screen the same
  double prev_true_scale_ppm = m_true_scale_ppm;
  return SetViewPoint(VPoint.clat, VPoint.clon, VPoint.view_scale_ppm,
                      VPoint.skew, VPoint.rotation, projection) &&
         SetVPScale(wxMax(
             VPoint.view_scale_ppm * prev_true_scale_ppm / m_true_scale_ppm,
             m_absolute_min_scale_ppm));
}

bool ChartCanvas::SetViewPoint(double lat, double lon) {
  return SetViewPoint(lat, lon, VPoint.view_scale_ppm, VPoint.skew,
                      VPoint.rotation);
}

bool ChartCanvas::SetVPRotation(double angle) {
  return SetViewPoint(VPoint.clat, VPoint.clon, VPoint.view_scale_ppm, VPoint.skew,
                      angle);
}

bool ChartCanvas::SetViewPoint(double lat, double lon, double scale_ppm,
                               double skew, double rotation, int projection,
                               bool b_adjust, bool b_refresh) {
  bool b_ret = false;

  if (skew > PI) /* so our difference tests work, put in range of +-Pi */
    skew -= 2 * PI;

  //  Any sensible change?
  if (VPoint.IsValid()) {
    if ((fabs(VPoint.view_scale_ppm - scale_ppm) / scale_ppm < 1e-5) &&
        (fabs(VPoint.skew - skew) < 1e-9) &&
        (fabs(VPoint.rotation - rotation) < 1e-9) &&
        (fabs(VPoint.clat - lat) < 1e-9) && (fabs(VPoint.clon - lon) < 1e-9) &&
        (VPoint.m_projection_type == projection ||
         projection == PROJECTION_UNKNOWN))
      return false;
  }

  if (VPoint.m_projection_type != projection)
    VPoint.InvalidateTransformCache();  // invalidate

  //    Take a local copy of the last viewport
  ViewPort last_vp = VPoint;

  VPoint.skew = skew;
  VPoint.clat = lat;
  VPoint.clon = lon;
  VPoint.rotation = rotation;
  VPoint.view_scale_ppm = scale_ppm;
  if (projection != PROJECTION_UNKNOWN) VPoint.SetProjectionType(projection);
  else if (VPoint.m_projection_type == PROJECTION_UNKNOWN) VPoint.SetProjectionType(PROJECTION_MERCATOR);

  // don't allow latitude above 88 for mercator (90 is infinity)
  if (VPoint.m_projection_type == PROJECTION_MERCATOR || VPoint.m_projection_type == PROJECTION_TRANSVERSE_MERCATOR) {
    if (VPoint.clat > 89.5)
      VPoint.clat = 89.5;
    else if (VPoint.clat < -89.5)
      VPoint.clat = -89.5;
  }

  // don't zoom out too far for transverse mercator polyconic until we resolve
  // issues
  if (VPoint.m_projection_type == PROJECTION_POLYCONIC ||
      VPoint.m_projection_type == PROJECTION_TRANSVERSE_MERCATOR)
    VPoint.view_scale_ppm = wxMax(VPoint.view_scale_ppm, 2e-4);

  if (!g_bopengl)  // tilt is not possible without opengl
    VPoint.tilt = 0;

  if ((VPoint.pix_width <= 0) ||
      (VPoint.pix_height <= 0))  // Canvas parameters not yet set
    return false;

  bool bwasValid = VPoint.IsValid();
  VPoint.Validate();  // Mark this ViewPoint as OK

  //  Has the Viewport scale changed?  If so, invalidate the vp
  if (last_vp.view_scale_ppm != scale_ppm) {
    m_cache_vp.Invalidate();
    InvalidateGL();
  }

  //  A preliminary value, may be tweaked below
  VPoint.chart_scale = m_canvas_scale_factor / (scale_ppm);

  if (!VPoint.b_quilt && m_singleChart) {
    VPoint.SetBoxes();

    //  Allow the chart to adjust the new ViewPort for performance optimization
    //  This will normally be only a fractional (i.e.sub-pixel) adjustment...
    if (b_adjust) m_singleChart->AdjustVP(last_vp, VPoint);

    // If there is a sensible change in the chart render, refresh the whole
    // screen
    if ((!m_cache_vp.IsValid()) || (m_cache_vp.view_scale_ppm != VPoint.view_scale_ppm)) {
      Refresh(false);
      b_ret = true;
    } else {
      wxPoint cp_last, cp_this;
      GetCanvasPointPix(m_cache_vp.clat, m_cache_vp.clon, &cp_last);
      GetCanvasPointPix(VPoint.clat, VPoint.clon, &cp_this);

      if (cp_last != cp_this) {
        Refresh(false);
        b_ret = true;
      }
    }
    //  Create the stack
    if (m_pCurrentStack) {
      assert(ChartData != 0);
      int current_db_index;
      current_db_index =
          m_pCurrentStack->GetCurrentEntrydbIndex();  // capture the current

      ChartData->BuildChartStack(m_pCurrentStack, lat, lon, current_db_index,
                                 m_groupIndex);
      m_pCurrentStack->SetCurrentEntryFromdbIndex(current_db_index);
    }

    if (!g_bopengl) VPoint.b_MercatorProjectionOverride = false;
  }

  //  Handle the quilted case
  if (VPoint.b_quilt) {
    if (last_vp.view_scale_ppm != scale_ppm)
      m_pQuilt->InvalidateAllQuiltPatchs();

    //  Create the quilt
    if (ChartData /*&& ChartData->IsValid()*/) {
      if (!m_pCurrentStack) return false;

      int current_db_index;
      current_db_index = m_pCurrentStack->GetCurrentEntrydbIndex();  // capture the current

      ChartData->BuildChartStack(m_pCurrentStack, lat, lon, m_groupIndex);
      m_pCurrentStack->SetCurrentEntryFromdbIndex(current_db_index);

      //   Check to see if the current quilt reference chart is in the new stack
      int current_ref_stack_index = -1;
      for (int i = 0; i < m_pCurrentStack->nEntry; i++) {
        if (m_pQuilt->GetRefChartdbIndex() == m_pCurrentStack->GetDBIndex(i))
          current_ref_stack_index = i;
      }

      if (g_bFullScreenQuilt) {
        current_ref_stack_index = m_pQuilt->GetRefChartdbIndex();
      }

      // We might need a new Reference Chart
      bool b_needNewRef = false;

      //    If the new stack does not contain the current ref chart....
      if ((-1 == current_ref_stack_index) && (m_pQuilt->GetRefChartdbIndex() >= 0)) b_needNewRef = true;

      // Would the current Ref Chart be excessively underzoomed?
      // We need to check this here to be sure, since we cannot know where the
      // reference chart was assigned. For instance, the reference chart may
      // have been selected from the config file, or from a long jump with a
      // chart family switch implicit. Anyway, we check to be sure....
      bool renderable = true;
      ChartBase *referenceChart = ChartData->OpenChartFromDB(m_pQuilt->GetRefChartdbIndex(), FULL_INIT);
      if (referenceChart) {
        double chartMaxScale = referenceChart->GetNormalScaleMax(
            GetCanvasScaleFactor(), GetCanvasWidth());
        renderable = chartMaxScale * 64 >= VPoint.chart_scale;
      }
      if (!renderable) b_needNewRef = true;

      //    Need new refchart?
      if (b_needNewRef) {
        const ChartTableEntry &cte_ref =
            ChartData->GetChartTableEntry(m_pQuilt->GetRefChartdbIndex());
        int target_scale = cte_ref.GetScale();
        int target_type = cte_ref.GetChartType();
        int candidate_stack_index;

        //    reset the ref chart in a way that does not lead to excessive
        //    underzoom, for performance reasons Try to find a chart that is the
        //    same type, and has a scale of just smaller than the current ref
        //    chart

        candidate_stack_index = 0;
        while (candidate_stack_index <= m_pCurrentStack->nEntry - 1) {
          const ChartTableEntry &cte_candidate = ChartData->GetChartTableEntry(
              m_pCurrentStack->GetDBIndex(candidate_stack_index));
          int candidate_scale = cte_candidate.GetScale();
          int candidate_type = cte_candidate.GetChartType();

          if ((candidate_scale >= target_scale) &&
              (candidate_type == target_type)) {
            bool renderable = true;
            ChartBase *tentative_referenceChart = ChartData->OpenChartFromDB(
                m_pCurrentStack->GetDBIndex(candidate_stack_index), FULL_INIT);
            if (tentative_referenceChart) {
              double chartMaxScale =
                  tentative_referenceChart->GetNormalScaleMax(
                      GetCanvasScaleFactor(), GetCanvasWidth());
              renderable = chartMaxScale * 1.5 > VPoint.chart_scale;
            }

            if (renderable) break;
          }

          candidate_stack_index++;
        }

        //    If that did not work, look for a chart of just larger scale and
        //    same type
        if (candidate_stack_index >= m_pCurrentStack->nEntry) {
          candidate_stack_index = m_pCurrentStack->nEntry - 1;
          while (candidate_stack_index >= 0) {
            int idx = m_pCurrentStack->GetDBIndex(candidate_stack_index);
            if (idx >= 0) {
              const ChartTableEntry &cte_candidate = ChartData->GetChartTableEntry(idx);
              int candidate_scale = cte_candidate.GetScale();
              int candidate_type = cte_candidate.GetChartType();

              if ((candidate_scale <= target_scale) &&
                  (candidate_type == target_type))
                break;
            }
            candidate_stack_index--;
          }
        }

        // and if that did not work, chose stack entry 0
        if ((candidate_stack_index >= m_pCurrentStack->nEntry) ||
            (candidate_stack_index < 0))
          candidate_stack_index = 0;

        int new_ref_index = m_pCurrentStack->GetDBIndex(candidate_stack_index);

        m_pQuilt->SetReferenceChart(new_ref_index);  // maybe???
      }

      if (!g_bopengl) {
        // Preset the VPoint projection type to match what the quilt projection
        // type will be
        int ref_db_index = m_pQuilt->GetRefChartdbIndex(), proj;

        // Always keep the default Mercator projection if the reference chart is
        // not in the PatchList or the scale is too small for it to render.

        bool renderable = true;
        ChartBase *referenceChart =
            ChartData->OpenChartFromDB(ref_db_index, FULL_INIT);
        if (referenceChart) {
          double chartMaxScale = referenceChart->GetNormalScaleMax(
              GetCanvasScaleFactor(), GetCanvasWidth());
          renderable = chartMaxScale * 1.5 > VPoint.chart_scale;
          proj = ChartData->GetDBChartProj(ref_db_index);
        } else
          proj = PROJECTION_MERCATOR;

        VPoint.b_MercatorProjectionOverride =
            (m_pQuilt->GetnCharts() == 0 || !renderable);

        if (VPoint.b_MercatorProjectionOverride) proj = PROJECTION_MERCATOR;

        VPoint.SetProjectionType(proj);
      }

      VPoint.SetBoxes();

      //    If this quilt will be a perceptible delta from the existing quilt,
      //    then refresh the entire screen
      if (m_pQuilt->IsQuiltDelta(VPoint)) {
        //  Allow the quilt to adjust the new ViewPort for performance
        //  optimization This will normally be only a fractional (i.e.
        //  sub-pixel) adjustment...
        if (b_adjust) m_pQuilt->AdjustQuiltVP(last_vp, VPoint);

        m_pQuilt->Compose(VPoint);

        ChartData->PurgeCacheUnusedCharts(0.7);

        if (b_refresh) Refresh(false);

        b_ret = true;
      }
    }

    VPoint.skew = 0.;  // Quilting supports 0 Skew
  } else if (!g_bopengl) {
    OcpnProjType projection = PROJECTION_UNKNOWN;
    if (m_singleChart)  // viewport projection must match chart projection
                        // without opengl
      projection = m_singleChart->GetChartProjectionType();
    if (projection == PROJECTION_UNKNOWN) projection = PROJECTION_MERCATOR;
    VPoint.SetProjectionType(projection);
  }

  //  Has the Viewport projection changed?  If so, invalidate the vp
  if (last_vp.m_projection_type != VPoint.m_projection_type) {
    m_cache_vp.Invalidate();
    InvalidateGL();
  }

  UpdateCanvasControlBar();  // Refresh the Piano

  VPoint.chart_scale = 1.0;  // fallback default value
  VPoint.SetBoxes();

  if (VPoint.GetBBox().GetValid()) {
    //      Update the viewpoint reference scale
    if (m_singleChart)
      VPoint.ref_scale = m_singleChart->GetNativeScale();
    else {
      VPoint.ref_scale = m_pQuilt->GetRefNativeScale();
    }

    //    Calculate the on-screen displayed actual scale
    //    by a simple traverse northward from the center point
    //    of roughly one eighth of the canvas height
    wxPoint2DDouble r, r1;

    double delta_check = (VPoint.pix_height / VPoint.view_scale_ppm) / (1852. * 60);
    delta_check /= 8.;

    double check_point = wxMin(89., VPoint.clat);

    while ((delta_check + check_point) > 90.) delta_check /= 2.;

    double rhumbDist;
    DistanceBearingMercator(check_point, VPoint.clon, check_point + delta_check, VPoint.clon, 0, &rhumbDist);

    GetDoubleCanvasPointPix(check_point, VPoint.clon, &r1);
    GetDoubleCanvasPointPix(check_point + delta_check, VPoint.clon, &r);
    double delta_p = sqrt(((r1.m_y - r.m_y) * (r1.m_y - r.m_y)) + ((r1.m_x - r.m_x) * (r1.m_x - r.m_x)));

    m_true_scale_ppm = delta_p / (rhumbDist * 1852);

    //        A fall back in case of very high zoom-out, giving delta_y == 0
    //        which can probably only happen with vector charts
    if (0.0 == m_true_scale_ppm) m_true_scale_ppm = scale_ppm;

    //        Another fallback, for highly zoomed out charts
    //        This adjustment makes the displayed TrueScale correspond to the
    //        same algorithm used to calculate the chart zoom-out limit for
    //        ChartDummy.
    if (scale_ppm < 1e-4) m_true_scale_ppm = scale_ppm;

    if (m_true_scale_ppm)
      VPoint.chart_scale = m_canvas_scale_factor / (m_true_scale_ppm);
    else
      VPoint.chart_scale = 1.0;

    // Create a nice renderable string
    double round_factor = 1000.;
    if (VPoint.chart_scale <= 1000.)
      round_factor = 10.;
    else if (VPoint.chart_scale <= 10000.)
      round_factor = 100.;
    else if (VPoint.chart_scale <= 100000.)
      round_factor = 1000.;

    // Fixme: Workaround the wrongly calculated scale on Retina displays (#3117)
    double retina_coef = 1;
    #ifdef ocpnUSE_GL
    #ifdef __WXOSX__
    if (g_bopengl) {
      retina_coef = GetContentScaleFactor();;
    }
    #endif
    #endif

    double true_scale_display =
        wxRound(VPoint.chart_scale / round_factor) * round_factor * retina_coef;
    wxString text;

    m_displayed_scale_factor = VPoint.ref_scale / VPoint.chart_scale;

    if (m_displayed_scale_factor > 10.0)
      text.Printf(_T("%s %4.0f (%1.0fx)"), _("Scale"), true_scale_display,
                  m_displayed_scale_factor);
    else if (m_displayed_scale_factor > 1.0)
      text.Printf(_T("%s %4.0f (%1.1fx)"), _("Scale"), true_scale_display,
                  m_displayed_scale_factor);
    else if (m_displayed_scale_factor > 0.1) {
      double sfr = wxRound(m_displayed_scale_factor * 10.) / 10.;
      text.Printf(_T("%s %4.0f (%1.2fx)"), _("Scale"), true_scale_display, sfr);
    } else if (m_displayed_scale_factor > 0.01) {
      double sfr = wxRound(m_displayed_scale_factor * 100.) / 100.;
      text.Printf(_T("%s %4.0f (%1.2fx)"), _("Scale"), true_scale_display, sfr);
    } else {
      text.Printf(
          _T("%s %4.0f (---)"), _("Scale"),
          true_scale_display);  // Generally, no chart, so no chart scale factor
    }

#ifdef ocpnUSE_GL
    if (g_bopengl && g_bShowFPS) {
      wxString fps_str;
      double fps = 0.;
      if (g_gl_ms_per_frame > 0) {
        fps = 1000. / g_gl_ms_per_frame;
        fps_str.Printf(_T("  %3d fps"), (int)fps);
      }
      text += fps_str;
    }
#endif

    m_scaleValue = true_scale_display;
    m_scaleText = text;    
  }

  //  Maintain member vLat/vLon
  m_vLat = VPoint.clat;
  m_vLon = VPoint.clon;

  return b_ret;
}

//      This icon was adapted and scaled from the S52 Presentation Library
//      version 3_03.
//     Symbol VECGND02

static int s_png_pred_icon[] = {-10, -10, -10, 10, 10, 10, 10, -10};

//      This ownship icon was adapted and scaled from the S52 Presentation
//      Library version 3_03 Symbol OWNSHP05
static int s_ownship_icon[] = {5,  -42, 11,  -28, 11, 42, -11, 42, -11, -28,
                               -5, -42, -11, 0,   11, 0,  0,   42, 0,   -42};

wxColour ChartCanvas::PredColor() {
  //  RAdjust predictor color change on LOW_ACCURACY ship state in interests of
  //  visibility.
  if (SHIP_NORMAL == m_ownship_state)
    return GetGlobalColor(_T ( "URED" ));

  else if (SHIP_LOWACCURACY == m_ownship_state)
    return GetGlobalColor(_T ( "YELO1" ));

  return GetGlobalColor(_T ( "NODTA" ));
}

wxColour ChartCanvas::ShipColor() {
  //      Establish ship color
  //     It changes color based on GPS and Chart accuracy/availability

  if (SHIP_NORMAL != m_ownship_state) return GetGlobalColor(_T ( "GREY1" ));

  if (SHIP_LOWACCURACY == m_ownship_state)
    return GetGlobalColor(_T ( "YELO1" ));

  return GetGlobalColor(_T ( "URED" ));  // default is OK
}

void ChartCanvas::ShipDrawLargeScale(ocpnDC &dc, wxPoint lShipMidPoint) {
  dc.SetPen(wxPen(PredColor(), 2));

  if (SHIP_NORMAL == m_ownship_state)
    dc.SetBrush(wxBrush(ShipColor(), wxBRUSHSTYLE_TRANSPARENT));
  else
    dc.SetBrush(wxBrush(GetGlobalColor(_T ( "YELO1" ))));

  dc.DrawEllipse(lShipMidPoint.x - 10, lShipMidPoint.y - 10, 20, 20);
  dc.DrawEllipse(lShipMidPoint.x - 6, lShipMidPoint.y - 6, 12, 12);

  dc.DrawLine(lShipMidPoint.x - 12, lShipMidPoint.y, lShipMidPoint.x + 12,
              lShipMidPoint.y);
  dc.DrawLine(lShipMidPoint.x, lShipMidPoint.y - 12, lShipMidPoint.x,
              lShipMidPoint.y + 12);
}

void ChartCanvas::ShipIndicatorsDraw(ocpnDC &dc, int img_height,
                                     wxPoint GPSOffsetPixels,
                                     wxPoint lGPSPoint) {
  // Develop a uniform length for course predictor line dash length, based on
  // physical display size Use this reference length to size all other graphics
  // elements
  float ref_dim = m_display_size_mm / 24;
  ref_dim = wxMin(ref_dim, 12);
  ref_dim = wxMax(ref_dim, 6);

  wxColour cPred = PredColor();

  //  Establish some graphic element line widths dependent on the platform
  //  display resolution
  // double nominal_line_width_pix = wxMax(1.0,
  // floor(g_Platform->GetDisplayDPmm() / 2));             //0.5 mm nominal, but
  // not less than 1 pixel
  double nominal_line_width_pix = wxMax(
      1.0,
      floor(m_pix_per_mm / 2));  // 0.5 mm nominal, but not less than 1 pixel

  // If the calculated value is greater than the config file spec value, then
  // use it.
  if (nominal_line_width_pix > g_cog_predictor_width)
    g_cog_predictor_width = nominal_line_width_pix;

  //    Calculate ownship Position Predictor
  wxPoint lPredPoint, lHeadPoint;

  float pCog = std::isnan(gCog) ? 0 : gCog;
  float pSog = std::isnan(gSog) ? 0 : gSog;

  double pred_lat, pred_lon;
  ll_gc_ll(gLat, gLon, pCog, pSog * g_ownship_predictor_minutes / 60.,
           &pred_lat, &pred_lon);
  GetCanvasPointPix(pred_lat, pred_lon, &lPredPoint);

  // test to catch the case where COG/HDG line crosses the screen
  LLBBox box;

  //    Should we draw the Head vector?
  //    Compare the points lHeadPoint and lPredPoint
  //    If they differ by more than n pixels, and the head vector is valid, then
  //    render the head vector

  float ndelta_pix = 10.;
  double hdg_pred_lat, hdg_pred_lon;
  bool b_render_hdt = false;
  if (!std::isnan(gHdt)) {
    //    Calculate ownship Heading pointer as a predictor
    ll_gc_ll(gLat, gLon, gHdt, g_ownship_HDTpredictor_miles, &hdg_pred_lat,
             &hdg_pred_lon);
    GetCanvasPointPix(hdg_pred_lat, hdg_pred_lon, &lHeadPoint);
    float dist = sqrtf(powf((float)(lHeadPoint.x - lPredPoint.x), 2) +
                       powf((float)(lHeadPoint.y - lPredPoint.y), 2));
    if (dist > ndelta_pix /*&& !std::isnan(gSog)*/) {
      box.SetFromSegment(gLat, gLon, hdg_pred_lat, hdg_pred_lon);
      if (!GetVP().GetBBox().IntersectOut(box)) b_render_hdt = true;
    }
  }

  // draw course over ground if they are longer than the ship
  wxPoint lShipMidPoint;
  lShipMidPoint.x = lGPSPoint.x + GPSOffsetPixels.x;
  lShipMidPoint.y = lGPSPoint.y + GPSOffsetPixels.y;
  float lpp = sqrtf(powf((float)(lPredPoint.x - lShipMidPoint.x), 2) +
                    powf((float)(lPredPoint.y - lShipMidPoint.y), 2));

  if (lpp >= img_height / 2) {
    box.SetFromSegment(gLat, gLon, pred_lat, pred_lon);
    if (!GetVP().GetBBox().IntersectOut(box) && !std::isnan(gCog) &&
        !std::isnan(gSog)) {
      //      COG Predictor
      float dash_length = ref_dim;
      wxDash dash_long[2];
      dash_long[0] =
          (int)(floor(g_Platform->GetDisplayDPmm() * dash_length) /
                g_cog_predictor_width);   // Long dash , in mm <---------+
      dash_long[1] = dash_long[0] / 2.0;  // Short gap

      // On ultra-hi-res displays, do not allow the dashes to be greater than
      // 250, since it is defined as (char)
      if (dash_length > 250.) {
        dash_long[0] = 250. / g_cog_predictor_width;
        dash_long[1] = dash_long[0] / 2;
      }

      wxPen ppPen2(cPred, g_cog_predictor_width, wxPENSTYLE_USER_DASH);
      ppPen2.SetDashes(2, dash_long);
      dc.SetPen(ppPen2);
      dc.StrokeLine(
          lGPSPoint.x + GPSOffsetPixels.x, lGPSPoint.y + GPSOffsetPixels.y,
          lPredPoint.x + GPSOffsetPixels.x, lPredPoint.y + GPSOffsetPixels.y);

      if (g_cog_predictor_width > 1) {
        float line_width = g_cog_predictor_width / 3.;

        wxDash dash_long3[2];
        dash_long3[0] = g_cog_predictor_width / line_width * dash_long[0];
        dash_long3[1] = g_cog_predictor_width / line_width * dash_long[1];

        wxPen ppPen3(GetGlobalColor(_T ( "UBLCK" )), wxMax(1, line_width),
                     wxPENSTYLE_USER_DASH);
        ppPen3.SetDashes(2, dash_long3);
        dc.SetPen(ppPen3);
        dc.StrokeLine(
            lGPSPoint.x + GPSOffsetPixels.x, lGPSPoint.y + GPSOffsetPixels.y,
            lPredPoint.x + GPSOffsetPixels.x, lPredPoint.y + GPSOffsetPixels.y);
      }

      // Prepare COG predictor endpoint icon
      double png_pred_icon_scale_factor = .4;
      if (g_ShipScaleFactorExp > 1.0)
        png_pred_icon_scale_factor *= (log(g_ShipScaleFactorExp) + 1.0) * 1.1;
      if (g_scaler)
        png_pred_icon_scale_factor *= 1.0 / g_scaler;

      wxPoint icon[4];

      float cog_rad = atan2f((float)(lPredPoint.y - lShipMidPoint.y),
                             (float)(lPredPoint.x - lShipMidPoint.x));
      cog_rad += (float)PI;

      for (int i = 0; i < 4; i++) {
        int j = i * 2;
        double pxa = (double)(s_png_pred_icon[j]);
        double pya = (double)(s_png_pred_icon[j + 1]);

        pya *= png_pred_icon_scale_factor;
        pxa *= png_pred_icon_scale_factor;

        double px = (pxa * sin(cog_rad)) + (pya * cos(cog_rad));
        double py = (pya * sin(cog_rad)) - (pxa * cos(cog_rad));

        icon[i].x = (int)wxRound(px) + lPredPoint.x + GPSOffsetPixels.x;
        icon[i].y = (int)wxRound(py) + lPredPoint.y + GPSOffsetPixels.y;
      }

      // Render COG endpoint icon
      wxPen ppPen1(GetGlobalColor(_T ( "UBLCK" )), g_cog_predictor_width / 2,
                   wxPENSTYLE_SOLID);
      dc.SetPen(ppPen1);
      dc.SetBrush(wxBrush(cPred));

      dc.StrokePolygon(4, icon);
    }
  }

  //      HDT Predictor
  if (b_render_hdt) {
    float hdt_dash_length = ref_dim * 0.4;

    float hdt_width = g_cog_predictor_width * 0.8;
    wxDash dash_short[2];
    dash_short[0] =
        (int)(floor(g_Platform->GetDisplayDPmm() * hdt_dash_length) /
              hdt_width);  // Short dash , in mm <---------+
    dash_short[1] =
        (int)(floor(g_Platform->GetDisplayDPmm() * hdt_dash_length * 0.9) /
              hdt_width);  // Short gap            |

    wxPen ppPen2(cPred, hdt_width, wxPENSTYLE_USER_DASH);
    ppPen2.SetDashes(2, dash_short);

    dc.SetPen(ppPen2);
    dc.StrokeLine(
        lGPSPoint.x + GPSOffsetPixels.x, lGPSPoint.y + GPSOffsetPixels.y,
        lHeadPoint.x + GPSOffsetPixels.x, lHeadPoint.y + GPSOffsetPixels.y);

    wxPen ppPen1(cPred, g_cog_predictor_width / 3, wxPENSTYLE_SOLID);
    dc.SetPen(ppPen1);
    dc.SetBrush(wxBrush(GetGlobalColor(_T ( "GREY2" ))));

    double nominal_circle_size_pixels = wxMax(
        4.0, floor(m_pix_per_mm * (ref_dim / 5.0)));  // not less than 4 pixel

    // Scale the circle to ChartScaleFactor, slightly softened....
    if (g_ShipScaleFactorExp > 1.0)
      nominal_circle_size_pixels *= (log(g_ShipScaleFactorExp) + 1.0) * 1.1;

    dc.StrokeCircle(lHeadPoint.x + GPSOffsetPixels.x,
                    lHeadPoint.y + GPSOffsetPixels.y,
                    nominal_circle_size_pixels / 2);
  }

  // Draw radar rings if activated
  if (g_iNavAidRadarRingsNumberVisible) {
    double factor = 1.00;
    if (g_pNavAidRadarRingsStepUnits == 1)  // nautical miles
      factor = 1 / 1.852;

    factor *= g_fNavAidRadarRingsStep;

    double tlat, tlon;
    wxPoint r;
    ll_gc_ll(gLat, gLon, 0, factor, &tlat, &tlon);
    GetCanvasPointPix(tlat, tlon, &r);

    double lpp = sqrt(pow((double)(lGPSPoint.x - r.x), 2) +
                      pow((double)(lGPSPoint.y - r.y), 2));
    int pix_radius = (int)lpp;

    extern wxColor GetDimColor(wxColor c);
    wxColor rangeringcolour = GetDimColor(g_colourOwnshipRangeRingsColour);

    wxPen ppPen1(rangeringcolour, g_cog_predictor_width);

    dc.SetPen(ppPen1);
    dc.SetBrush(wxBrush(rangeringcolour, wxBRUSHSTYLE_TRANSPARENT));

    for (int i = 1; i <= g_iNavAidRadarRingsNumberVisible; i++)
      dc.StrokeCircle(lGPSPoint.x, lGPSPoint.y, i * pix_radius);
  }
}

void ChartCanvas::ComputeShipScaleFactor(
    float icon_hdt, int ownShipWidth, int ownShipLength, wxPoint &lShipMidPoint,
    wxPoint &GPSOffsetPixels, wxPoint lGPSPoint, float &scale_factor_x,
    float &scale_factor_y) {
  float screenResolution = m_pix_per_mm;

  //  Calculate the true ship length in exact pixels
  double ship_bow_lat, ship_bow_lon;
  ll_gc_ll(gLat, gLon, icon_hdt, g_n_ownship_length_meters / 1852.,
           &ship_bow_lat, &ship_bow_lon);
  wxPoint lShipBowPoint;
  wxPoint2DDouble b_point =
      GetVP().GetDoublePixFromLL(ship_bow_lat, ship_bow_lon);
  wxPoint2DDouble a_point = GetVP().GetDoublePixFromLL(gLat, gLon);

  float shipLength_px = sqrtf(powf((float)(b_point.m_x - a_point.m_x), 2) +
                              powf((float)(b_point.m_y - a_point.m_y), 2));

  //  And in mm
  float shipLength_mm = shipLength_px / screenResolution;

  //  Set minimum ownship drawing size
  float ownship_min_mm = g_n_ownship_min_mm;
  ownship_min_mm = wxMax(ownship_min_mm, 1.0);

  //  Calculate Nautical Miles distance from midships to gps antenna
  float hdt_ant = icon_hdt + 180.;
  float dy = (g_n_ownship_length_meters / 2 - g_n_gps_antenna_offset_y) / 1852.;
  float dx = g_n_gps_antenna_offset_x / 1852.;
  if (g_n_gps_antenna_offset_y > g_n_ownship_length_meters / 2)  // reverse?
  {
    hdt_ant = icon_hdt;
    dy = -dy;
  }

  //  If the drawn ship size is going to be clamped, adjust the gps antenna
  //  offsets
  if (shipLength_mm < ownship_min_mm) {
    dy /= shipLength_mm / ownship_min_mm;
    dx /= shipLength_mm / ownship_min_mm;
  }

  double ship_mid_lat, ship_mid_lon, ship_mid_lat1, ship_mid_lon1;

  ll_gc_ll(gLat, gLon, hdt_ant, dy, &ship_mid_lat, &ship_mid_lon);
  ll_gc_ll(ship_mid_lat, ship_mid_lon, icon_hdt - 90., dx, &ship_mid_lat1,
           &ship_mid_lon1);

  GetCanvasPointPix(ship_mid_lat1, ship_mid_lon1, &lShipMidPoint);
  GPSOffsetPixels.x = lShipMidPoint.x - lGPSPoint.x;
  GPSOffsetPixels.y = lShipMidPoint.y - lGPSPoint.y;

  float scale_factor = shipLength_px / ownShipLength;

  //  Calculate a scale factor that would produce a reasonably sized icon
  float scale_factor_min = ownship_min_mm / (ownShipLength / screenResolution);

  //  And choose the correct one
  scale_factor = wxMax(scale_factor, scale_factor_min);

  scale_factor_y = scale_factor;
  scale_factor_x = scale_factor_y * ((float)ownShipLength / ownShipWidth) /
                   ((float)g_n_ownship_length_meters / g_n_ownship_beam_meters);
}

/* @ChartCanvas::CalcGridSpacing
 **
 ** Calculate the major and minor spacing between the lat/lon grid
 **
 ** @param [r] WindowDegrees [float] displayed number of lat or lan in the
 *window
 ** @param [w] MajorSpacing [float &] Major distance between grid lines
 ** @param [w] MinorSpacing [float &] Minor distance between grid lines
 ** @return [void]
 */
void CalcGridSpacing(float view_scale_ppm, float &MajorSpacing,
                     float &MinorSpacing) {
  // table for calculating the distance between the grids
  // [0] view_scale ppm
  // [1] spacing between major grid lines in degrees
  // [2] spacing between minor grid lines in degrees
  const float lltab[][3] = {{0.0f, 90.0f, 30.0f},
                            {.000001f, 45.0f, 15.0f},
                            {.0002f, 30.0f, 10.0f},
                            {.0003f, 10.0f, 2.0f},
                            {.0008f, 5.0f, 1.0f},
                            {.001f, 2.0f, 30.0f / 60.0f},
                            {.003f, 1.0f, 20.0f / 60.0f},
                            {.006f, 0.5f, 10.0f / 60.0f},
                            {.03f, 15.0f / 60.0f, 5.0f / 60.0f},
                            {.01f, 10.0f / 60.0f, 2.0f / 60.0f},
                            {.06f, 5.0f / 60.0f, 1.0f / 60.0f},
                            {.1f, 2.0f / 60.0f, 1.0f / 60.0f},
                            {.4f, 1.0f / 60.0f, 0.5f / 60.0f},
                            {.6f, 0.5f / 60.0f, 0.1f / 60.0f},
                            {1.0f, 0.2f / 60.0f, 0.1f / 60.0f},
                            {1e10f, 0.1f / 60.0f, 0.05f / 60.0f}};

  unsigned int tabi;
  for (tabi = 0; tabi < ((sizeof lltab) / (sizeof *lltab)) - 1; tabi++)
    if (view_scale_ppm < lltab[tabi][0]) break;
  MajorSpacing = lltab[tabi][1];  // major latitude distance
  MinorSpacing = lltab[tabi][2];  // minor latitude distance
  return;
}
/* @ChartCanvas::CalcGridText *************************************
 **
 ** Calculates text to display at the major grid lines
 **
 ** @param [r] latlon [float] latitude or longitude of grid line
 ** @param [r] spacing [float] distance between two major grid lines
 ** @param [r] bPostfix [bool] true for latitudes, false for longitudes
 **
 ** @return
 */

wxString CalcGridText(float latlon, float spacing, bool bPostfix) {
  int deg = (int)fabs(latlon);                    // degrees
  float min = fabs((fabs(latlon) - deg) * 60.0);  // Minutes
  char postfix;

  // calculate postfix letter (NSEW)
  if (latlon > 0.0) {
    if (bPostfix) {
      postfix = 'N';
    } else {
      postfix = 'E';
    }
  } else if (latlon < 0.0) {
    if (bPostfix) {
      postfix = 'S';
    } else {
      postfix = 'W';
    }
  } else {
    postfix = ' ';  // no postfix for equator and greenwich
  }
  // calculate text, display minutes only if spacing is smaller than one degree

  wxString ret;
  if (spacing >= 1.0) {
    ret.Printf(_T("%3d%c %c"), deg, 0x00b0, postfix);
  } else if (spacing >= (1.0 / 60.0)) {
    ret.Printf(_T("%3d%c%02.0f %c"), deg, 0x00b0, min, postfix);
  } else {
    ret.Printf(_T("%3d%c%02.2f %c"), deg, 0x00b0, min, postfix);
  }

  return ret;
}

/* @ChartCanvas::GridDraw *****************************************
 **
 ** Draws major and minor Lat/Lon Grid on the chart
 ** - distance between Grid-lm ines are calculated automatic
 ** - major grid lines will be across the whole chart window
 ** - minor grid lines will be 10 pixel at each edge of the chart window.
 **
 ** @param [w] dc [wxDC&] the wx drawing context
 **
 ** @return [void]
 ************************************************************************/
void ChartCanvas::GridDraw(ocpnDC &dc) {
	if (!(m_bDisplayGrid && (fabs(GetVP().rotation) < 1e-5))) return;

	double nlat, elon, slat, wlon;
	float lat, lon;
	float dlon;
	float gridlatMajor, gridlatMinor, gridlonMajor, gridlonMinor;
	wxCoord w, h;
	wxPen GridPen(GetGlobalColor(_T("SNDG1")), 1, wxPENSTYLE_SOLID);
	dc.SetPen(GridPen);
	dc.SetFont(*m_pgridFont);
	dc.SetTextForeground(GetGlobalColor(_T("SNDG1")));

	if (GetVP().IsValid() && GetVP().pix_width > 0) {
		w = GetVP().pix_width;
		h = GetVP().pix_height;
	}
	else {
		w = m_canvas_width;
		h = m_canvas_height;
	}

	GetVP().GetLLFromPix(wxPoint2DDouble(0, 0), &nlat, &wlon);
	GetVP().GetLLFromPix(wxPoint2DDouble(w, h), &slat, &elon);

	dlon = elon - wlon;  // calculate how many degrees of longitude are shown in the window
	if (dlon < 0.0) {  // concider datum border at 180 degrees longitude  
		dlon = dlon + 360.0;
	}
	// calculate distance between latitude grid lines
	CalcGridSpacing(GetVP().view_scale_ppm, gridlatMajor, gridlatMinor);

	// calculate position of first major latitude grid line
	lat = ceil(slat / gridlatMajor) * gridlatMajor;

	// Draw Major latitude grid lines and text
	while (lat < nlat) {
		wxPoint r;
		wxString st = CalcGridText(lat, gridlatMajor, true);  // get text for grid line
		GetCanvasPointPixVP(GetVP(), lat, (elon + wlon) / 2, &r);
		dc.DrawLine(0, r.y, w, r.y, false);  // draw grid line
		dc.DrawText(st, 0, r.y);             // draw text
		lat = lat + gridlatMajor;

		if (fabs(lat - wxRound(lat)) < 1e-5) lat = wxRound(lat);
	}

	// calculate position of first minor latitude grid line
	lat = ceil(slat / gridlatMinor) * gridlatMinor;

	// Draw minor latitude grid lines
	while (lat < nlat) {
		wxPoint r;
		GetCanvasPointPixVP(GetVP(), lat, (elon + wlon) / 2, &r);
		dc.DrawLine(0, r.y, 10, r.y, false);
		dc.DrawLine(w - 10, r.y, w, r.y, false);
		lat = lat + gridlatMinor;
	}

	// calculate distance between grid lines
	CalcGridSpacing(GetVP().view_scale_ppm, gridlonMajor, gridlonMinor);

	// calculate position of first major latitude grid line
	lon = ceil(wlon / gridlonMajor) * gridlonMajor;

	// draw major longitude grid lines
	for (int i = 0, itermax = (int)(dlon / gridlonMajor); i <= itermax; i++) {
		wxPoint r;
		wxString st = CalcGridText(lon, gridlonMajor, false);
		GetCanvasPointPixVP(GetVP(), (nlat + slat) / 2, lon, &r);
		dc.DrawLine(r.x, 0, r.x, h, false);
		dc.DrawText(st, r.x, 0);
		lon = lon + gridlonMajor;
		if (lon > 180.0) {
			lon = lon - 360.0;
		}

		if (fabs(lon - wxRound(lon)) < 1e-5) lon = wxRound(lon);
	}

	// calculate position of first minor longitude grid line
	lon = ceil(wlon / gridlonMinor) * gridlonMinor;
	// draw minor longitude grid lines
	for (int i = 0, itermax = (int)(dlon / gridlonMinor); i <= itermax; i++) {
		wxPoint r;
		GetCanvasPointPixVP(GetVP(), (nlat + slat) / 2, lon, &r);
		dc.DrawLine(r.x, 0, r.x, 10, false);
		dc.DrawLine(r.x, h - 10, r.x, h, false);
		lon = lon + gridlonMinor;
		if (lon > 180.0) {
			lon = lon - 360.0;
		}
	}
}

void ChartCanvas::ScaleBarDraw(ocpnDC &dc) {
  if (0 /*!g_bsimplifiedScalebar*/) {
    double blat, blon, tlat, tlon;
    wxPoint r;

    int x_origin = m_bDisplayGrid ? 60 : 20;
    int y_origin = m_canvas_height - 50;

    float dist;
    int count;
    wxPen pen1, pen2;

    if (GetVP().chart_scale > 80000)  // Draw 10 mile scale as SCALEB11
    {
      dist = 10.0;
      count = 5;
      pen1 = wxPen(GetGlobalColor(_T ( "SNDG2" )), 3, wxPENSTYLE_SOLID);
      pen2 = wxPen(GetGlobalColor(_T ( "SNDG1" )), 3, wxPENSTYLE_SOLID);
    } else  // Draw 1 mile scale as SCALEB10
    {
      dist = 1.0;
      count = 10;
      pen1 = wxPen(GetGlobalColor(_T ( "SCLBR" )), 3, wxPENSTYLE_SOLID);
      pen2 = wxPen(GetGlobalColor(_T ( "CHGRD" )), 3, wxPENSTYLE_SOLID);
    }

    GetCanvasPixPoint(x_origin, y_origin, blat, blon);
    double rotation = -VPoint.rotation;

    ll_gc_ll(blat, blon, rotation * 180 / PI, dist, &tlat, &tlon);
    GetCanvasPointPix(tlat, tlon, &r);
    int l1 = (y_origin - r.y) / count;

    for (int i = 0; i < count; i++) {
      int y = l1 * i;
      if (i & 1)
        dc.SetPen(pen1);
      else
        dc.SetPen(pen2);

      dc.DrawLine(x_origin, y_origin - y, x_origin, y_origin - (y + l1));
    }
  } else {
    double blat, blon, tlat, tlon;

    int x_origin = 5.0 * GetPixPerMM();

    int y_origin = m_canvas_height/* - chartbar_height*/ - 5;
#ifdef __WXOSX__
    if (!g_bopengl)
      y_origin = m_canvas_height/GetContentScaleFactor() - chartbar_height - 5;
#endif

    GetCanvasPixPoint(x_origin, y_origin, blat, blon);
    GetCanvasPixPoint(x_origin + m_canvas_width, y_origin, tlat, tlon);

    double d;
    ll_gc_ll_reverse(blat, blon, tlat, tlon, 0, &d);
    d /= 2;

    int unit = g_iDistanceFormat;
    if (d < .5 &&
        (unit == DISTANCE_KM || unit == DISTANCE_MI || unit == DISTANCE_NMI))
      unit = (unit == DISTANCE_MI) ? DISTANCE_FT : DISTANCE_M;

    // nice number
    float dist = toUsrDistance(d, unit), logdist = log(dist) / log(10.F);
    float places = floor(logdist), rem = logdist - places;
    dist = pow(10, places);

    if (rem < .2)
      dist /= 5;
    else if (rem < .5)
      dist /= 2;

    wxString s = wxString::Format(_T("%g "), dist) + getUsrDistanceUnit(unit);
    wxPen pen1 = wxPen(GetGlobalColor(_T ( "UBLCK" )), 3, wxPENSTYLE_SOLID);
    double rotation = -VPoint.rotation;

    ll_gc_ll(blat, blon, rotation * 180 / PI + 90, fromUsrDistance(dist, unit),
             &tlat, &tlon);
    wxPoint r;
    GetCanvasPointPix(tlat, tlon, &r);
    int l1 = r.x - x_origin;

    m_scaleBarRect = wxRect(x_origin, y_origin - 12, l1,
                            12);  // Store this for later reference

    dc.SetPen(pen1);

    dc.DrawLine(x_origin, y_origin, x_origin, y_origin - 12);
    dc.DrawLine(x_origin, y_origin, x_origin + l1, y_origin);
    dc.DrawLine(x_origin + l1, y_origin, x_origin + l1, y_origin - 12);

    dc.SetFont(*m_pgridFont);
    dc.SetTextForeground(GetGlobalColor(_T ( "UBLCK" )));
    int w, h;
    dc.GetTextExtent(s, &w, &h);
    dc.DrawText(s, x_origin + l1 / 2 - w / 2, y_origin - h - 1);
  }
}

void ChartCanvas::JaggyCircle(ocpnDC &dc, wxPen pen, int x, int y, int radius) {
  //    Constants?
  double da_min = 2.;
  double da_max = 6.;
  double ra_min = 0.;
  double ra_max = 40.;

  wxPen pen_save = dc.GetPen();

  wxDateTime now = wxDateTime::Now();

  dc.SetPen(pen);

  int x0, y0, x1, y1;

  x0 = x1 = x + radius;  // Start point
  y0 = y1 = y;
  double angle = 0.;
  int i = 0;

  while (angle < 360.) {
    double da = da_min + (((double)rand() / RAND_MAX) * (da_max - da_min));
    angle += da;

    if (angle > 360.) angle = 360.;

    double ra = ra_min + (((double)rand() / RAND_MAX) * (ra_max - ra_min));

    double r;
    if (i & 1)
      r = radius + ra;
    else
      r = radius - ra;

    x1 = (int)(x + cos(angle * PI / 180.) * r);
    y1 = (int)(y + sin(angle * PI / 180.) * r);

    dc.DrawLine(x0, y0, x1, y1);

    x0 = x1;
    y0 = y1;

    i++;
  }

  dc.DrawLine(x + radius, y, x1, y1);  // closure

  dc.SetPen(pen_save);
}

static bool bAnchorSoundPlaying = false;

static void onAnchorSoundFinished(void *ptr) {
  g_anchorwatch_sound->UnLoad();
  bAnchorSoundPlaying = false;
}

void ChartCanvas::UpdateAlerts() {
  //  Get the rectangle in the current dc which bounds the detected Alert
  //  targets

  //  Use this dc
  wxClientDC dc(this);

  // Get dc boundary
  int sx, sy;
  dc.GetSize(&sx, &sy);

  //  Need a bitmap
  wxBitmap test_bitmap(sx, sy, -1);

  // Create a memory DC
  wxMemoryDC temp_dc;
  temp_dc.SelectObject(test_bitmap);

  temp_dc.ResetBoundingBox();
  temp_dc.DestroyClippingRegion();
  temp_dc.SetClippingRegion(wxRect(0, 0, sx, sy));

  // Draw the Alert Targets on the temp_dc
  ocpnDC ocpndc = ocpnDC(temp_dc);

  //  Retrieve the drawing extents
  wxRect alert_rect(temp_dc.MinX(), temp_dc.MinY(),
                    temp_dc.MaxX() - temp_dc.MinX(),
                    temp_dc.MaxY() - temp_dc.MinY());

  if (!alert_rect.IsEmpty())
    alert_rect.Inflate(2);  // clear all drawing artifacts

  if (!alert_rect.IsEmpty() || !alert_draw_rect.IsEmpty()) {
    //  The required invalidate rectangle is the union of the last drawn
    //  rectangle and this drawn rectangle
    wxRect alert_update_rect = alert_draw_rect;
    alert_update_rect.Union(alert_rect);

    //  Invalidate the rectangular region
    RefreshRect(alert_update_rect, false);
  }

  //  Save this rectangle for next time
  alert_draw_rect = alert_rect;

  temp_dc.SelectObject(wxNullBitmap);  // clean up
}

void ChartCanvas::ToggleCPAWarn() {
  g_bCPAWarn = !g_bCPAWarn;
  wxString mess = _("ON");
  if (!g_bCPAWarn) {
    g_bTCPA_Max = false;
    mess = _("OFF");
  }
  else { g_bTCPA_Max = true; }
  // Print to status bar if available.
  if (STAT_FIELD_SCALE >= 4 && parent_frame->GetStatusBar()) {
    parent_frame->SetStatusText(_("CPA alarm ") + mess, STAT_FIELD_SCALE);
  }
  else {
	  printf("CPA Alarm is switched %s", (const char *)(mess.MakeLower().mb_str(wxConvUTF8)));
  }
}

void ChartCanvas::OnActivate(wxActivateEvent &event) { ReloadVP(); }

void ChartCanvas::OnSize(wxSizeEvent &event) {
  GetClientSize(&m_canvas_width, &m_canvas_height);

#ifdef __WXOSX__
  // Support scaled HDPI displays.
  m_displayScale = GetContentScaleFactor();
#endif


  m_canvas_width *= m_displayScale;
  m_canvas_height *= m_displayScale;

  //    Resize the current viewport
  VPoint.pix_width = m_canvas_width;
  VPoint.pix_height = m_canvas_height;

  //    Get some canvas metrics

  //          Rescale to current value, in order to rebuild VPoint data
  //          structures for new canvas size
  SetVPScale(GetVPScale());

  m_absolute_min_scale_ppm =
      m_canvas_width /
      (1.2 * WGS84_semimajor_axis_meters * PI);  // something like 180 degrees

  //  Inform the parent Frame that I am being resized...
  gFrame->ProcessCanvasResize();

  //    Set up the scroll margins
  xr_margin = m_canvas_width * 95 / 100;
  xl_margin = m_canvas_width * 5 / 100;
  yt_margin = m_canvas_height * 5 / 100;
  yb_margin = m_canvas_height * 95 / 100;

  if (m_pQuilt)
    m_pQuilt->SetQuiltParameters(m_canvas_scale_factor, m_canvas_width);

  // Resize the scratch BM
  delete pscratch_bm;
  pscratch_bm = new wxBitmap(VPoint.pix_width, VPoint.pix_height, -1);
  m_brepaint_piano = true;

  // Resize the Route Calculation BM
  m_dc_route.SelectObject(wxNullBitmap);
  delete proute_bm;
  proute_bm = new wxBitmap(VPoint.pix_width, VPoint.pix_height, -1);
  m_dc_route.SelectObject(*proute_bm);

  //  Resize the saved Bitmap
  m_cached_chart_bm.Create(VPoint.pix_width, VPoint.pix_height, -1);

  //  Resize the working Bitmap
  m_working_bm.Create(VPoint.pix_width, VPoint.pix_height, -1);

  //  Rescale again, to capture all the changes for new canvas size
  SetVPScale(GetVPScale());

#ifdef ocpnUSE_GL
  if (/*g_bopengl &&*/ m_glcc) {
    //FIXME (dave)  This can go away?
    m_glcc->OnSize(event);
  }
#endif

  FormatPianoKeys();
  //  Invalidate the whole window
  ReloadVP();
}

void ChartCanvas::ProcessNewGUIScale() {  
}

void ChartCanvas::ShowChartInfoWindow(int x, int dbIndex) {
  if (dbIndex >= 0) {
    if (NULL == m_pCIWin) {
      m_pCIWin = new ChInfoWin(this);
      m_pCIWin->Hide();
    }

    if (!m_pCIWin->IsShown() || (m_pCIWin->dbIndex != dbIndex)) {
      wxString s;
      ChartBase *pc = NULL;

      // TOCTOU race but worst case will reload chart.
      // need to lock it or the background spooler may evict charts in
      // OpenChartFromDBAndLock
      if ((ChartData->IsChartInCache(dbIndex)) && ChartData->IsValid())
        pc = ChartData->OpenChartFromDBAndLock(
            dbIndex, FULL_INIT);  // this must come from cache

      int char_width, char_height;
      s = ChartData->GetFullChartInfo(pc, dbIndex, &char_width, &char_height);
      if (pc) ChartData->UnLockCacheChart(dbIndex);

      m_pCIWin->SetString(s);
      m_pCIWin->FitToChars(char_width, char_height);

      wxPoint p;
      p.x = x / GetContentScaleFactor();
      if ((p.x + m_pCIWin->GetWinSize().x) > (m_canvas_width / GetContentScaleFactor()))
        p.x = ((m_canvas_width / GetContentScaleFactor())
                - m_pCIWin->GetWinSize().x) / 2;  // centered

      p.y =
          (m_canvas_height - m_Piano->GetHeight()) / GetContentScaleFactor()
                - 4 - m_pCIWin->GetWinSize().y;

      m_pCIWin->dbIndex = dbIndex;
      m_pCIWin->SetPosition(p);
      m_pCIWin->SetBitmap();
      m_pCIWin->Refresh();
      m_pCIWin->Show();
    }
  } else {
    HideChartInfoWindow();
  }
}

void ChartCanvas::HideChartInfoWindow(void) {
  if (m_pCIWin /*&& m_pCIWin->IsShown()*/) {
    m_pCIWin->Hide();
    m_pCIWin->Destroy();
    m_pCIWin = NULL;

#ifdef __OCPN__ANDROID__
    androidForceFullRepaint();
#endif
  }
}

void ChartCanvas::PanTimerEvent(wxTimerEvent &event) {
  wxMouseEvent ev(wxEVT_MOTION);
  ev.m_x = mouse_x;
  ev.m_y = mouse_y;
  ev.m_leftDown = mouse_leftisdown;

  wxEvtHandler *evthp = GetEventHandler();

  ::wxPostEvent(evthp, ev);
}

void ChartCanvas::MovementTimerEvent(wxTimerEvent &) { DoTimedMovement(); }

void ChartCanvas::MovementStopTimerEvent(wxTimerEvent &) { StopMovement(); }

bool ChartCanvas::CheckEdgePan(int x, int y, bool bdragging, int margin,
                               int delta) {
  if (m_disable_edge_pan) return false;

  bool bft = false;
  int pan_margin = m_canvas_width * margin / 100;
  int pan_timer_set = 200;
  double pan_delta = GetVP().pix_width * delta / 100;
  int pan_x = 0;
  int pan_y = 0;

  if (x > m_canvas_width - pan_margin) {
    bft = true;
    pan_x = pan_delta;
  }

  else if (x < pan_margin) {
    bft = true;
    pan_x = -pan_delta;
  }

  if (y < pan_margin) {
    bft = true;
    pan_y = -pan_delta;
  }

  else if (y > m_canvas_height - pan_margin) {
    bft = true;
    pan_y = pan_delta;
  }

  //    Of course, if dragging, and the mouse left button is not down, we must
  //    stop the event injection
  if (bdragging) {   
  }
  if ((bft) && !pPanTimer->IsRunning()) {
    PanCanvas(pan_x, pan_y);
    pPanTimer->Start(pan_timer_set, wxTIMER_ONE_SHOT);
    return true;
  }

  //    This mouse event must not be due to pan timer event injector
  //    Mouse is out of the pan zone, so prevent any orphan event injection
  if ((!bft) && pPanTimer->IsRunning()) {
    pPanTimer->Stop();
  }

  return (false);
}

// Look for waypoints at the current position.
// Used to determine what a mouse event should act on.

bool leftIsDown;

bool panleftIsDown;

void ChartCanvas::SetCanvasCursor(wxMouseEvent &event) {
  //    Switch to the appropriate cursor on mouse movement

  wxCursor *ptarget_cursor = pCursorArrow;
  if (!pPlugIn_Cursor) {
    ptarget_cursor = pCursorArrow;
    if ((!m_routeState) &&
        (!m_bMeasure_Active) /*&& ( !m_bCM93MeasureOffset_Active )*/) {
      if (cursor_region == MID_RIGHT) {
        ptarget_cursor = pCursorRight;
      } else if (cursor_region == MID_LEFT) {
        ptarget_cursor = pCursorLeft;
      } else if (cursor_region == MID_TOP) {
        ptarget_cursor = pCursorDown;
      } else if (cursor_region == MID_BOT) {
        ptarget_cursor = pCursorUp;
      } else {
        ptarget_cursor = pCursorArrow;
      }
    } else if (m_bMeasure_Active ||
               m_routeState)  // If Measure tool use Pencil Cursor
      ptarget_cursor = pCursorPencil;
  } else {
    ptarget_cursor = pPlugIn_Cursor;
  }

  SetCursor(*ptarget_cursor);
}

void ChartCanvas::LostMouseCapture(wxMouseCaptureLostEvent &event) {
  SetCursor(*pCursorArrow);
}


void ChartCanvas::ShowAISTargetList(void) {
}

void ChartCanvas::RenderAllChartOutlines(ocpnDC &dc, ViewPort &vp) {
  if (!m_bShowOutlines) return;

  if (!ChartData) return;

  int nEntry = ChartData->GetChartTableEntries();

  for (int i = 0; i < nEntry; i++) {
    ChartTableEntry *pt = (ChartTableEntry *)&ChartData->GetChartTableEntry(i);

    //    Check to see if the candidate chart is in the currently active group
    bool b_group_draw = false;
    if (m_groupIndex > 0) {
      for (unsigned int ig = 0; ig < pt->GetGroupArray().size(); ig++) {
        int index = pt->GetGroupArray()[ig];
        if (m_groupIndex == index) {
          b_group_draw = true;
          break;
        }
      }
    } else
      b_group_draw = true;

    if (b_group_draw) RenderChartOutline(dc, i, vp);
  }
}

void ChartCanvas::RenderChartOutline(ocpnDC &dc, int dbIndex, ViewPort &vp) {
#ifdef ocpnUSE_GL
  if (g_bopengl && m_glcc) {
    /* opengl version specially optimized */
    m_glcc->RenderChartOutline(dc, dbIndex, vp);
    return;
  }
#endif

  if (ChartData->GetDBChartType(dbIndex) == CHART_TYPE_PLUGIN) {
    if (!ChartData->IsChartAvailable(dbIndex)) return;
  }

  float plylat, plylon;
  float plylat1, plylon1;

  int pixx, pixy, pixx1, pixy1;

  LLBBox box;
  ChartData->GetDBBoundingBox(dbIndex, box);

  // Don't draw an outline in the case where the chart covers the entire world
  // */
  if (box.GetLonRange() == 360) return;

  double lon_bias = 0;
  // chart is outside of viewport lat/lon bounding box
  if (box.IntersectOutGetBias(vp.GetBBox(), lon_bias)) return;

  int nPly = ChartData->GetDBPlyPoint(dbIndex, 0, &plylat, &plylon);

  if (ChartData->GetDBChartType(dbIndex) == CHART_TYPE_CM93)
    dc.SetPen(wxPen(GetGlobalColor(_T ( "YELO1" )), 1, wxPENSTYLE_SOLID));

  else if (ChartData->GetDBChartFamily(dbIndex) == CHART_FAMILY_VECTOR)
    dc.SetPen(wxPen(GetGlobalColor(_T ( "UINFG" )), 1, wxPENSTYLE_SOLID));

  else
    dc.SetPen(wxPen(GetGlobalColor(_T ( "UINFR" )), 1, wxPENSTYLE_SOLID));

  //        Are there any aux ply entries?
  int nAuxPlyEntries = ChartData->GetnAuxPlyEntries(dbIndex);
  if (0 == nAuxPlyEntries)  // There are no aux Ply Point entries
  {
    wxPoint r, r1;

    ChartData->GetDBPlyPoint(dbIndex, 0, &plylat, &plylon);
    plylon += lon_bias;

    GetCanvasPointPix(plylat, plylon, &r);
    pixx = r.x;
    pixy = r.y;

    for (int i = 0; i < nPly - 1; i++) {
      ChartData->GetDBPlyPoint(dbIndex, i + 1, &plylat1, &plylon1);
      plylon1 += lon_bias;

      GetCanvasPointPix(plylat1, plylon1, &r1);
      pixx1 = r1.x;
      pixy1 = r1.y;

      int pixxs1 = pixx1;
      int pixys1 = pixy1;

      bool b_skip = false;

      if (vp.chart_scale > 5e7) {
        //    calculate projected distance between these two points in meters
        double dist = sqrt(pow((double)(pixx1 - pixx), 2) +
                           pow((double)(pixy1 - pixy), 2)) /
                      vp.view_scale_ppm;

        if (dist > 0.0) {
          //    calculate GC distance between these two points in meters
          double distgc =
              DistGreatCircle(plylat, plylon, plylat1, plylon1) * 1852.;

          //    If the distances are nonsense, it means that the scale is very
          //    small and the segment wrapped the world So skip it....
          //    TODO improve this to draw two segments
          if (fabs(dist - distgc) > 10000. * 1852.)  // lotsa miles
            b_skip = true;
        } else
          b_skip = true;
      }

      ClipResult res = cohen_sutherland_line_clip_i(
          &pixx, &pixy, &pixx1, &pixy1, 0, vp.pix_width, 0, vp.pix_height);
      if (res != Invisible && !b_skip)
        dc.DrawLine(pixx, pixy, pixx1, pixy1, false);

      plylat = plylat1;
      plylon = plylon1;
      pixx = pixxs1;
      pixy = pixys1;
    }

    ChartData->GetDBPlyPoint(dbIndex, 0, &plylat1, &plylon1);
    plylon1 += lon_bias;

    GetCanvasPointPix(plylat1, plylon1, &r1);
    pixx1 = r1.x;
    pixy1 = r1.y;

    ClipResult res = cohen_sutherland_line_clip_i(
        &pixx, &pixy, &pixx1, &pixy1, 0, vp.pix_width, 0, vp.pix_height);
    if (res != Invisible) dc.DrawLine(pixx, pixy, pixx1, pixy1, false);
  }

  else  // Use Aux PlyPoints
  {
    wxPoint r, r1;

    int nAuxPlyEntries = ChartData->GetnAuxPlyEntries(dbIndex);
    for (int j = 0; j < nAuxPlyEntries; j++) {
      int nAuxPly =
          ChartData->GetDBAuxPlyPoint(dbIndex, 0, j, &plylat, &plylon);
      GetCanvasPointPix(plylat, plylon, &r);
      pixx = r.x;
      pixy = r.y;

      for (int i = 0; i < nAuxPly - 1; i++) {
        ChartData->GetDBAuxPlyPoint(dbIndex, i + 1, j, &plylat1, &plylon1);

        GetCanvasPointPix(plylat1, plylon1, &r1);
        pixx1 = r1.x;
        pixy1 = r1.y;

        int pixxs1 = pixx1;
        int pixys1 = pixy1;

        bool b_skip = false;

        if (vp.chart_scale > 5e7) {
          //    calculate projected distance between these two points in meters
          double dist = sqrt((double)((pixx1 - pixx) * (pixx1 - pixx)) +
                             ((pixy1 - pixy) * (pixy1 - pixy))) /
                        vp.view_scale_ppm;
          if (dist > 0.0) {
            //    calculate GC distance between these two points in meters
            double distgc =
                DistGreatCircle(plylat, plylon, plylat1, plylon1) * 1852.;

            //    If the distances are nonsense, it means that the scale is very
            //    small and the segment wrapped the world So skip it....
            //    TODO improve this to draw two segments
            if (fabs(dist - distgc) > 10000. * 1852.)  // lotsa miles
              b_skip = true;
          } else
            b_skip = true;
        }

        ClipResult res = cohen_sutherland_line_clip_i(
            &pixx, &pixy, &pixx1, &pixy1, 0, vp.pix_width, 0, vp.pix_height);
        if (res != Invisible && !b_skip) dc.DrawLine(pixx, pixy, pixx1, pixy1);

        plylat = plylat1;
        plylon = plylon1;
        pixx = pixxs1;
        pixy = pixys1;
      }

      ChartData->GetDBAuxPlyPoint(dbIndex, 0, j, &plylat1, &plylon1);
      GetCanvasPointPix(plylat1, plylon1, &r1);
      pixx1 = r1.x;
      pixy1 = r1.y;

      ClipResult res = cohen_sutherland_line_clip_i(
          &pixx, &pixy, &pixx1, &pixy1, 0, vp.pix_width, 0, vp.pix_height);
      if (res != Invisible) dc.DrawLine(pixx, pixy, pixx1, pixy1, false);
    }
  }
}

void ChartCanvas::RenderVisibleSectorLights(ocpnDC &dc) {
  if (!m_bShowVisibleSectors) return;

  if (g_bDeferredInitDone) {
    // need to re-evaluate sectors?
    double rhumbBearing, rhumbDist;
    DistanceBearingMercator(gLat, gLon, m_sector_glat, m_sector_glon,
                                      &rhumbBearing, &rhumbDist);

    if (rhumbDist > 0.05)   // miles
    {
      m_sector_glat = gLat;
      m_sector_glon = gLon;
    }
    s57_DrawExtendedLightSectors(dc, VPoint, m_sectorlegsVisible);
  }
}

void ChartCanvas::WarpPointerDeferred(int x, int y) {
  warp_x = x;
  warp_y = y;
  warp_flag = true;
}

int s_msg;

void ChartCanvas::UpdateCanvasS52PLIBConfig() {
  if (!ps52plib) return;

  if (VPoint.b_quilt) {                                // quilted
    if (!m_pQuilt || !m_pQuilt->IsComposed()) return;  // not ready

    if (m_pQuilt->IsQuiltVector()) {
      if (ps52plib->GetStateHash() != m_s52StateHash) {
        UpdateS52State();
        m_s52StateHash = ps52plib->GetStateHash();
      }
    }
  } else {
    if (ps52plib->GetStateHash() != m_s52StateHash) {
      UpdateS52State();
      m_s52StateHash = ps52plib->GetStateHash();
    }
  }
}

int spaint;
int s_in_update;

void ChartCanvas::DrawCanvasData(LLBBox &llbBox, int nWidth, int nHeight, std::vector<int>& vnLayers, std::string& sIMGFilePath, bool bPNGFlag) {
	SetViewPointByCorners(llbBox.GetMinLat(), llbBox.GetMinLon(), llbBox.GetMaxLat(), llbBox.GetMaxLon());	

	SetShowENCText(HasLayer(vnLayers, LAYER_TEXT));
	SetShowENCLightDesc(HasLayer(vnLayers, LAYER_LDESCR));
	SetShowENCAnchor(HasLayer(vnLayers, LAYER_AINFO));
	SetShowENCDepth(HasLayer(vnLayers, LAYER_DEPTHS));
	SetShowENCBuoyLabels(HasLayer(vnLayers, LAYER_BLLABELS));
	SetShowENCLights(HasLayer(vnLayers, LAYER_LIGHTS));
	SetShowVisibleSectors(HasLayer(vnLayers, LAYER_SLVIS));
	SetShowGrid(false);

	UpdateCanvasS52PLIBConfig();

	pscratch_bm->Create(VPoint.pix_width, VPoint.pix_height, -1);
	m_working_bm.Create(VPoint.pix_width, VPoint.pix_height, -1);

	printf("cc : Draw Canvas manually 1\n");
	if (!m_b_paint_enable) {
		return;
	}
	printf("cc : Draw Canvas manually 2\n");
	m_b_paint_enable = false;
	//  If necessary, reconfigure the S52 PLIB
	UpdateCanvasS52PLIBConfig();

#ifdef ocpnUSE_GL
	if (!g_bdisable_opengl && m_glcc) m_glcc->Show(g_bopengl);

	if (m_glcc && g_bopengl) {
		if (!s_in_update) {  // no recursion allowed, seen on lo-spec Mac
			s_in_update++;
			m_glcc->Update();
			s_in_update--;
		}

		return;
	}
#endif

	if ((GetVP().pix_width == 0) || (GetVP().pix_height == 0)) return;
	
#ifdef ocpnUSE_DIBSECTION
	ocpnMemDC temp_dc;
#else
	wxMemoryDC temp_dc;
#endif

	long height = GetVP().pix_height;

#ifdef __WXMAC__
	// On OS X we have to explicitly extend the region for the piano area
	ocpnStyle::Style * style = g_StyleManager->GetCurrentStyle();
	if (!style->chartStatusWindowTransparent && g_bShowChartBar)
		height += m_Piano->GetHeight();
#endif  // __WXMAC__	
	wxRegion rgn_blit(0, 0, GetVP().pix_width, height);
	//  If the ViewPort is skewed or rotated, we may be able to use the cached
	//  rotated bitmap.
	bool b_rcache_ok = false;
	if (fabs(VPoint.skew) > 0.01 || fabs(VPoint.rotation) > 0.01) b_rcache_ok = true;

	//  Make a special VP
	if (VPoint.b_MercatorProjectionOverride) VPoint.SetProjectionType(PROJECTION_MERCATOR);
	ViewPort svp = VPoint;

	svp.pix_width = svp.rv_rect.width;
	svp.pix_height = svp.rv_rect.height;

	printf("Onpaint pix %d %d\n", VPoint.pix_width, VPoint.pix_height);
	printf("OnPaint rv_rect %d %d\n", VPoint.rv_rect.width, VPoint.rv_rect.height);

	OCPNRegion chart_get_region(wxRect(0, 0, svp.pix_width, svp.pix_height));
	
	//  Blit pan acceleration
	if (VPoint.b_quilt)  // quilted
	{
		if (!m_pQuilt || !m_pQuilt->IsComposed()) return;  // not ready

		bool bvectorQuilt = m_pQuilt->IsQuiltVector();

		if ((m_working_bm.GetWidth() != svp.pix_width) || (m_working_bm.GetHeight() != svp.pix_height)) {
			m_working_bm.Create(svp.pix_width, svp.pix_height, -1);  // make sure the target is big enoug
		}

		if (fabs(VPoint.rotation) < 0.01) {
			bool b_save = true;

			if (g_SencThreadManager) {
				if (g_SencThreadManager->GetJobCount()) {
					b_save = false;
					m_cache_vp.Invalidate();
				}
			}

			wxPoint c_old = VPoint.GetPixFromLL(VPoint.clat, VPoint.clon);
			wxPoint c_new = m_bm_cache_vp.GetPixFromLL(VPoint.clat, VPoint.clon);

			int dy = c_new.y - c_old.y;
			int dx = c_new.x - c_old.x;

			if (m_pQuilt->IsVPBlittable(VPoint, dx, dy, true)) {
				if (dx || dy) {
					//  Blit the reuseable portion of the cached wxBitmap to a working
					//  bitmap
					temp_dc.SelectObject(m_working_bm);

					wxMemoryDC cache_dc;
					cache_dc.SelectObject(m_cached_chart_bm);

					if (dy > 0) {
						if (dx > 0) {
							temp_dc.Blit(0, 0, VPoint.pix_width - dx,
								VPoint.pix_height - dy, &cache_dc, dx, dy);
						}
						else {
							temp_dc.Blit(-dx, 0, VPoint.pix_width + dx,
								VPoint.pix_height - dy, &cache_dc, 0, dy);
						}
					}
					else {
						if (dx > 0) {
							temp_dc.Blit(0, -dy, VPoint.pix_width - dx,
								VPoint.pix_height + dy, &cache_dc, dx, 0);
						}
						else {
							temp_dc.Blit(-dx, -dy, VPoint.pix_width + dx,
								VPoint.pix_height + dy, &cache_dc, 0, 0);
						}
					}

					OCPNRegion update_region;
					if (dy) {
						if (dy > 0)
							update_region.Union(
								wxRect(0, VPoint.pix_height - dy, VPoint.pix_width, dy));
						else
							update_region.Union(wxRect(0, 0, VPoint.pix_width, -dy));
					}

					if (dx) {
						if (dx > 0)
							update_region.Union(
								wxRect(VPoint.pix_width - dx, 0, dx, VPoint.pix_height));
						else
							update_region.Union(wxRect(0, 0, -dx, VPoint.pix_height));
					}

					//  Render the new region
					m_pQuilt->RenderQuiltRegionViewOnDCNoText(temp_dc, svp, update_region, HasLayer(vnLayers, LAYER_DEPTHS), HasLayer(vnLayers, LAYER_LIGHTS), HasLayer(vnLayers, LAYER_BLLABELS), HasLayer(vnLayers, LAYER_LDESCR), HasLayer(vnLayers, LAYER_AINFO), HasLayer(vnLayers, LAYER_SLVIS));
					cache_dc.SelectObject(wxNullBitmap);
				}
				else {
					//    No sensible (dx, dy) change in the view, so use the cached
					//    member bitmap
					temp_dc.SelectObject(m_cached_chart_bm);
					b_save = false;
				}
				m_pQuilt->ComputeRenderRegion(svp, chart_get_region);

			} else {  // not blitable
				temp_dc.SelectObject(m_working_bm);
				m_pQuilt->RenderQuiltRegionViewOnDCNoText(temp_dc, svp, chart_get_region, HasLayer(vnLayers, LAYER_DEPTHS), HasLayer(vnLayers, LAYER_LIGHTS), HasLayer(vnLayers, LAYER_BLLABELS), HasLayer(vnLayers, LAYER_LDESCR), HasLayer(vnLayers, LAYER_AINFO), HasLayer(vnLayers, LAYER_SLVIS));
			}	
		} else  // quilted, rotated
		{
			temp_dc.SelectObject(m_working_bm);
			OCPNRegion chart_get_all_region(wxRect(0, 0, svp.pix_width, svp.pix_height));
			m_pQuilt->RenderQuiltRegionViewOnDCNoText(temp_dc, svp, chart_get_all_region, HasLayer(vnLayers, LAYER_DEPTHS), HasLayer(vnLayers, LAYER_LIGHTS), HasLayer(vnLayers, LAYER_BLLABELS), HasLayer(vnLayers, LAYER_LDESCR), HasLayer(vnLayers, LAYER_AINFO), HasLayer(vnLayers, LAYER_SLVIS));
		}
	} else {
		if (!m_singleChart) {
			return;
		}

		if (!chart_get_region.IsEmpty()) {
			m_singleChart->RenderRegionViewOnDC(temp_dc, svp, chart_get_region);
		}
	}

	if (temp_dc.IsOk()) {  //process background except the ENC data's region (draw water and rotate)
		OCPNRegion chartValidRegion;
		if (!VPoint.b_quilt) {
			if (m_singleChart->GetChartFamily() == CHART_FAMILY_VECTOR)
				m_singleChart->GetValidCanvasRegion(svp, &chartValidRegion);
			else {
				m_singleChart->GetValidCanvasRegion(VPoint, &chartValidRegion);
				chartValidRegion.Offset(-VPoint.rv_rect.x, -VPoint.rv_rect.y);
			}
		}
		else
			chartValidRegion = m_pQuilt->GetFullQuiltRenderedRegion();

		temp_dc.DestroyClippingRegion();

		//    Copy current chart region
		OCPNRegion backgroundRegion(wxRect(0, 0, svp.pix_width, svp.pix_height));

		if (chartValidRegion.IsOk()) backgroundRegion.Subtract(chartValidRegion);

		if (!backgroundRegion.IsEmpty()) {
			wxColour water = pWorldBackgroundChart->water;
			if (water.IsOk()) {
				temp_dc.SetPen(*wxTRANSPARENT_PEN);
				temp_dc.SetBrush(wxBrush(water));
				OCPNRegionIterator upd(backgroundRegion);  // get the update rect list
				while (upd.HaveRects()) {
					wxRect rect = upd.GetRect();
					temp_dc.DrawRectangle(rect);
					upd.NextRect();
				}
			}
			//    Associate with temp_dc
			wxRegion* clip_region = backgroundRegion.GetNew_wxRegion();
			temp_dc.SetDeviceClippingRegion(*clip_region);
			delete clip_region;

			ocpnDC bgdc(temp_dc);
			double r = VPoint.rotation;
			SetVPRotation(VPoint.skew);

			pWorldBackgroundChart->RenderViewOnDC(bgdc, VPoint);
			SetVPRotation(r);
		}
	}

	wxMemoryDC* pChartDC = &temp_dc;
	wxMemoryDC rotd_dc;

  if (((fabs(GetVP().rotation) > 0.01)) || (fabs(GetVP().skew) > 0.01)) {
		//  Can we use the current rotated image cache?
		if (!b_rcache_ok) {
#ifdef __WXMSW__
			wxMemoryDC tbase_dc;
			wxBitmap bm_base(svp.pix_width, svp.pix_height, -1);
			tbase_dc.SelectObject(bm_base);
			tbase_dc.Blit(0, 0, svp.pix_width, svp.pix_height, &temp_dc, 0, 0);
			tbase_dc.SelectObject(wxNullBitmap);
#else
			const wxBitmap & bm_base = temp_dc.GetSelectedBitmap();
#endif

			wxImage base_image;
			if (bm_base.IsOk()) base_image = bm_base.ConvertToImage();
			double angle = GetVP().skew - GetVP().rotation;
			wxImage ri;
			bool b_rot_ok = false;
			if (base_image.IsOk()) {
				ViewPort rot_vp = GetVP();

				m_b_rot_hidef = false;

				ri = Image_Rotate(
					base_image, angle,
					wxPoint(GetVP().rv_rect.width / 2, GetVP().rv_rect.height / 2),
					m_b_rot_hidef, &m_roffset);

				if ((rot_vp.view_scale_ppm == VPoint.view_scale_ppm) &&
					(rot_vp.rotation == VPoint.rotation) &&
					(rot_vp.clat == VPoint.clat) && (rot_vp.clon == VPoint.clon) &&
					rot_vp.IsValid() && (ri.IsOk())) {
					b_rot_ok = true;
				}
			}

			if (b_rot_ok) {
				delete m_prot_bm;
				m_prot_bm = new wxBitmap(ri);
			}

			m_roffset.x += VPoint.rv_rect.x;
			m_roffset.y += VPoint.rv_rect.y;
		}

		if (m_prot_bm && m_prot_bm->IsOk()) {
			rotd_dc.SelectObject(*m_prot_bm);
			pChartDC = &rotd_dc;
		}
		else {
			pChartDC = &temp_dc;
			m_roffset = wxPoint(0, 0);
		}
	}
	else {  // unrotated
		pChartDC = &temp_dc;

		m_roffset = wxPoint(0, 0);
	}

	wxPoint offset = m_roffset;

	//        Save the PixelCache viewpoint for next time
	m_cache_vp = VPoint;

	//    Set up a scratch DC for overlay objects
	wxMemoryDC mscratch_dc;
	mscratch_dc.SetBackground(*wxWHITE_BRUSH);
	mscratch_dc.SetTextForeground(*wxBLACK);

	mscratch_dc.SelectObject(*pscratch_bm);

	mscratch_dc.ResetBoundingBox();
	mscratch_dc.DestroyClippingRegion();
	mscratch_dc.SetDeviceClippingRegion(rgn_blit);

	wxRegionIterator upd(rgn_blit);  // get the update rect list
	while (upd) {
		wxRect rect = upd.GetRect();

		mscratch_dc.Blit(rect.x, rect.y, rect.width, rect.height, pChartDC,
			rect.x - offset.x, rect.y - offset.y);
		upd++;
	}
  
	// If multi-canvas, indicate which canvas has keyboard focus
	// by drawing a simple blue bar at the top.
	if (g_canvasConfig != 0) {  // multi-canvas?
		if (this == wxWindow::FindFocus()) {
			g_focusCanvas = this;

			wxColour colour = GetGlobalColor(_T("BLUE4"));
			mscratch_dc.SetPen(wxPen(colour));
			mscratch_dc.SetBrush(wxBrush(colour));

			wxRect activeRect(0, 0, GetClientSize().x, m_focus_indicator_pix);
			mscratch_dc.DrawRectangle(activeRect);
		}
	}

	// Any MBtiles?
	std::vector<int> stackIndexArray = m_pQuilt->GetExtendedStackIndexArray();
	unsigned int im = stackIndexArray.size();
	if (VPoint.b_quilt && im > 0) {
		std::vector<int> tiles_to_show;
		for (unsigned int is = 0; is < im; is++) {
			const ChartTableEntry& cte = ChartData->GetChartTableEntry(stackIndexArray[is]);
			if (IsTileOverlayIndexInNoShow(stackIndexArray[is])) {
				continue;
			}			
		}

		if (tiles_to_show.size())
			SetAlertString(_("MBTile requires OpenGL to be enabled"));
	}

	ocpnDC scratch_dc(mscratch_dc);
	DrawOverlayObjects(scratch_dc, rgn_blit);

	if (m_bShowTide) {
		RebuildTideSelectList(GetVP().GetBBox());
		DrawAllTidesInBBox(scratch_dc, GetVP().GetBBox());
	}

	if (m_bShowCurrent) {
		RebuildCurrentSelectList(GetVP().GetBBox());
		DrawAllCurrentsInBBox(scratch_dc, GetVP().GetBBox());
	}
	
	RenderAlertMessage(mscratch_dc, GetVP());

	// quiting?
	if (g_bquiting) {
#ifdef ocpnUSE_DIBSECTION
		ocpnMemDC q_dc;
#else
		wxMemoryDC q_dc;
#endif
		wxBitmap qbm(GetVP().pix_width, GetVP().pix_height);
		q_dc.SelectObject(qbm);

		// Get a copy of the screen
		q_dc.Blit(0, 0, GetVP().pix_width, GetVP().pix_height, &mscratch_dc, 0, 0);

		//  Draw a rectangle over the screen with a stipple brush
		wxBrush qbr(*wxBLACK, wxBRUSHSTYLE_FDIAGONAL_HATCH);
		q_dc.SetBrush(qbr);
		q_dc.DrawRectangle(0, 0, GetVP().pix_width, GetVP().pix_height);

		// Blit back into source
		mscratch_dc.Blit(0, 0, GetVP().pix_width, GetVP().pix_height, &q_dc, 0, 0, wxCOPY);

		q_dc.SelectObject(wxNullBitmap);
	}
#if 0
	//  It is possible that this two-step method may be reuired for some platforms.
	//  So, retain in the code base to aid recovery if necessary

	// Create and Render the Vector quilt decluttered text overlay, omitting CM93 composite
	if (VPoint.b_quilt) {
		if (m_pQuilt->IsQuiltVector() && ps52plib && ps52plib->GetShowS57Text()) {
			ChartBase* chart = m_pQuilt->GetRefChart();
			if (chart && chart->GetChartType() != CHART_TYPE_CM93COMP) {

				if (ps52plib) {
					s52plib->ClearTextList();
				}

				wxMemoryDC t_dc;
				wxBitmap qbm(GetVP().pix_width, GetVP().pix_height);

				wxColor maskBackground = wxColour(1, 0, 0);
				t_dc.SelectObject(qbm);
				t_dc.SetBackground(wxBrush(maskBackground));
				t_dc.Clear();

				//  Copy the scratch DC into the new bitmap
				t_dc.Blit(0, 0, GetVP().pix_width, GetVP().pix_height, scratch_dc.GetDC(), 0, 0, wxCOPY);

				//  Render the text to the new bitmap
				OCPNRegion chart_all_text_region(wxRect(0, 0, GetVP().pix_width, GetVP().pix_height));
				m_pQuilt->RenderQuiltRegionViewOnDCTextOnly(t_dc, svp, chart_all_text_region);

				//  Copy the new bitmap back to the scratch dc
				wxRegionIterator upd_final(ru);
				while (upd_final) {
					wxRect rect = upd_final.GetRect();
					scratch_dc.GetDC()->Blit(rect.x, rect.y, rect.width, rect.height, &t_dc, rect.x, rect.y, wxCOPY, true);
					upd_final++;
				}

				t_dc.SelectObject(wxNullBitmap);
			}
		}
	}
#endif
	if (VPoint.b_quilt) {
		if (m_pQuilt->IsQuiltVector() && ps52plib && ps52plib->GetShowS57Text()) {
			ChartBase* chart = m_pQuilt->GetRefChart();
			if (chart && chart->GetChartType() != CHART_TYPE_CM93COMP) {
				//        Clear the text Global declutter list
				if (ps52plib) ps52plib->ClearTextList();				

				//  Render the text directly to the scratch bitmap
				OCPNRegion chart_all_text_region(
					wxRect(0, 0, GetVP().pix_width, GetVP().pix_height));

				mscratch_dc.DestroyClippingRegion();
				m_pQuilt->RenderQuiltRegionViewOnDCTextOnly(mscratch_dc, svp,
					chart_all_text_region);
			}
		}
	}
	
	LLBBox llbViewPortBox = VPoint.GetBBox();
	double dVPMinLat = llbViewPortBox.GetMinLat();
	double dVPMaxLat = llbViewPortBox.GetMaxLat();
	double dVPMinLon = llbViewPortBox.GetMinLon();
	double dVPMaxLon = llbViewPortBox.GetMaxLon();

	double dMinLat = llbBox.GetMinLat();
	double dMaxLat = llbBox.GetMaxLat();
	double dMinLon = llbBox.GetMinLon();
	double dMaxLon = llbBox.GetMaxLon();

	int nLeft = 0, nTop = 0, nRight = mscratch_dc.GetSize().x, nBottom = mscratch_dc.GetSize().y;

	double dPixelPerDegreeForLat = (nBottom - nTop) / abs(dVPMaxLat - dVPMinLat);
	double dPixelPerDegreeForLon = (nRight - nLeft) / abs(dVPMaxLon - dVPMinLon);
	
	wxPoint xPoint;

	int x1, y1, x2, y2;
	if (dMinLat > dVPMinLat) {
		GetCanvasPointPix(dMinLat, dVPMinLon, &xPoint);
	} else GetCanvasPointPix(dVPMinLat, dVPMinLon, &xPoint);

	y1 = xPoint.y;

	if (dMinLon > dVPMinLon) {
		GetCanvasPointPix(dVPMinLat, dMinLon, &xPoint);
	}
	else GetCanvasPointPix(dVPMinLat, dVPMinLon, &xPoint);
	
	x1 = xPoint.x;

	if (dMaxLat < dVPMaxLat) {
		GetCanvasPointPix(dMaxLat, dVPMaxLon, &xPoint);			
	}
	else GetCanvasPointPix(dVPMaxLat, dVPMaxLon, &xPoint);

	y2 = xPoint.y;

	if (dMaxLon < dVPMaxLon) {
		GetCanvasPointPix(dVPMaxLat, dMaxLon, &xPoint);
	}else GetCanvasPointPix(dVPMaxLat, dVPMaxLon, &xPoint);

	x2 = xPoint.x;

	if (x1 < x2) {
		nLeft = x1;
		nRight = x2;
	}
	else {
		nLeft = x2;
		nRight = x1;
	}

	if (y1 < y2) {
		nTop = y1;
		nBottom = y2;
	}
	else {
		nTop = y2;
		nBottom = y1;
	}

	if (HasLayer(vnLayers, LAYER_GRID)) {
		DrawGridInDC(&mscratch_dc, nLeft, nTop, nRight, nBottom, llbBox);
	}

	GenerateImageFile(&mscratch_dc, nLeft, nTop, nRight, nBottom, nWidth, nHeight, sIMGFilePath, bPNGFlag);
	
	temp_dc.SelectObject(wxNullBitmap);
	mscratch_dc.SelectObject(wxNullBitmap);
	m_b_paint_enable = true;

	printf("cc : Draw Canvas manually 3\n");
}

void ChartCanvas::DrawGridInDC(wxMemoryDC *mscratch_dc, int nLeft, int nTop, int nRight, int nBottom, LLBBox& llbBox) {
	double dMinLat = llbBox.GetMinLat();
	double dMaxLat = llbBox.GetMaxLat();
	double dMinLon = llbBox.GetMinLon();
	double dMaxLon = llbBox.GetMaxLon();

	wxPoint xPointBegin, xPointEnd, xDegreeShowPos;
	double dLat = dMinLat;
	while (dLat < dMaxLat) {
		GetCanvasPointPix(dLat, dMinLon, &xPointBegin);
		GetCanvasPointPix(dLat, dMaxLon, &xPointEnd);

		wxColour colour = GetGlobalColor(_T("BLACK"));
		mscratch_dc->SetPen(wxPen(colour, 2));
		mscratch_dc->SetBrush(wxBrush(colour));
		mscratch_dc->DrawLine(xPointBegin, xPointEnd);

		xDegreeShowPos = xPointBegin;
		if (xPointBegin.x > xPointEnd.x) xDegreeShowPos = xPointEnd;
		wxString sDegree = wxString::Format(wxT("%.2f"), dLat);
		mscratch_dc->DrawText(sDegree, xDegreeShowPos);

		dLat += 0.5;
	}

	double dLon = dMinLon;
	while (dLon < dMaxLon) {
		GetCanvasPointPix(dMinLat, dLon, &xPointBegin);
		GetCanvasPointPix(dMaxLat, dLon, &xPointEnd);

		wxColour colour = GetGlobalColor(_T("BLACK"));
		mscratch_dc->SetPen(wxPen(colour, 2));
		mscratch_dc->SetBrush(wxBrush(colour));
		mscratch_dc->DrawLine(xPointBegin, xPointEnd);

		xDegreeShowPos = xPointBegin;
		if (xPointBegin.y > xPointEnd.y) xDegreeShowPos = xPointEnd;
		wxString sDegree = wxString::Format(wxT("%.2f"), dLon);
		mscratch_dc->DrawText(sDegree, xDegreeShowPos);

		dLon += 0.5;
	}
}

void ChartCanvas::GenerateImageFile(wxImage &xImage, std::string& sIMGFilePath, bool bPNGFlag) {
	if (xImage.IsOk()) {
		wxFileOutputStream xFileOutput(sIMGFilePath);
		if (xFileOutput.IsOk()) {
			if (bPNGFlag) xImage.SaveFile(xFileOutput, wxBITMAP_TYPE_PNG);
      else xImage.SaveFile(xFileOutput, wxBITMAP_TYPE_JPEG);
		}
	}
}

void ChartCanvas::GenerateImageFile(wxBitmap &xBitmap, int nWidth, int nHeight, std::string& sIMGFilePath, bool bPNGFlag) {
	wxImage xImage;
	if (xBitmap.IsOk()) {
		xImage = xBitmap.ConvertToImage();
		if (xBitmap.GetSize().GetWidth() != nWidth || xBitmap.GetSize().GetHeight() != nHeight) {
			xImage = xImage.Scale(nWidth, nHeight, wxIMAGE_QUALITY_HIGH);
		}
		
		wxFileOutputStream xFileOutput(sIMGFilePath);
		if (xFileOutput.IsOk()) {
			if (bPNGFlag) xImage.SaveFile(xFileOutput, wxBITMAP_TYPE_PNG);
			else xImage.SaveFile(xFileOutput, wxBITMAP_TYPE_JPEG);
		}
	}
}

void ChartCanvas::GenerateImageFile(wxMemoryDC *pMemDC, int nLeft, int nTop, int nRight, int nBottom, int nTargetWidth, int nTargetHeight, std::string& sIMGFilePath, bool bPNGFlag) {
	wxBitmap xbTargetBitmap(nTargetWidth, nTargetHeight, -1);
	wxMemoryDC xMemDC;
	xMemDC.SelectObject(xbTargetBitmap);
	
	if (nTargetHeight > 1) {
		xMemDC.StretchBlit(0, 0, nTargetWidth, nTargetHeight, pMemDC, nLeft, nTop, nRight - nLeft, nBottom - nTop);	
	}
	xMemDC.SelectObject(wxNullBitmap);

	wxImage xImage;
	if (xbTargetBitmap.IsOk()) {
		xImage = xbTargetBitmap.ConvertToImage();

		wxFileOutputStream xFileOutput(sIMGFilePath);
		if (xFileOutput.IsOk()) {
			if (bPNGFlag) xImage.SaveFile(xFileOutput, wxBITMAP_TYPE_PNG);
			else xImage.SaveFile(xFileOutput, wxBITMAP_TYPE_JPEG);
		}
	}
}

void ChartCanvas::GenerateImageFile(wxMemoryDC* pMemDC, int nTargetWidth, int nTargetHeight, std::string& sIMGFilePath, bool bPNGFlag) {
	wxBitmap xbTargetBitmap(nTargetWidth, nTargetHeight, -1);
	wxMemoryDC xMemDC;
	xMemDC.SelectObject(xbTargetBitmap);

	if (nTargetHeight > 1) {
		xMemDC.StretchBlit(0, 0, nTargetWidth, nTargetHeight, pMemDC, 0, 0, pMemDC->GetSize().GetWidth(), pMemDC->GetSize().GetHeight());
	}
	xMemDC.SelectObject(wxNullBitmap);

	wxImage xImage;
	if (xbTargetBitmap.IsOk()) {
		xImage = xbTargetBitmap.ConvertToImage();

		wxFileOutputStream xFileOutput(sIMGFilePath);
		if (xFileOutput.IsOk()) {
			if (bPNGFlag) xImage.SaveFile(xFileOutput, wxBITMAP_TYPE_PNG);
			else xImage.SaveFile(xFileOutput, wxBITMAP_TYPE_JPEG);
		}
	}
}

void ChartCanvas::PaintCleanup() {
  //    And set flags for next time
  m_bTCupdate = false;

  //    Handle deferred WarpPointer
  if (warp_flag) {
    WarpPointer(warp_x, warp_y);
    warp_flag = false;
  }

  // Start movement timer, this runs nearly immediately.
  // the reason we cannot simply call it directly is the
  // refresh events it emits may be blocked from this paint event
  pMovementTimer->Start(1, wxTIMER_ONE_SHOT);
}

#if 0
wxColour GetErrorGraphicColor(double val)
{
    /*
     double valm = wxMin(val_max, val);

     unsigned char green = (unsigned char)(255 * (1 - (valm/val_max)));
     unsigned char red   = (unsigned char)(255 * (valm/val_max));

     wxImage::HSVValue hv = wxImage::RGBtoHSV(wxImage::RGBValue(red, green, 0));

     hv.saturation = 1.0;
     hv.value = 1.0;

     wxImage::RGBValue rv = wxImage::HSVtoRGB(hv);
     return wxColour(rv.red, rv.green, rv.blue);
     */

    //    HTML colors taken from NOAA WW3 Web representation
    wxColour c;
    if((val > 0) && (val < 1)) c.Set(_T("#002ad9"));
    else if((val >= 1) && (val < 2)) c.Set(_T("#006ed9"));
    else if((val >= 2) && (val < 3)) c.Set(_T("#00b2d9"));
    else if((val >= 3) && (val < 4)) c.Set(_T("#00d4d4"));
    else if((val >= 4) && (val < 5)) c.Set(_T("#00d9a6"));
    else if((val >= 5) && (val < 7)) c.Set(_T("#00d900"));
    else if((val >= 7) && (val < 9)) c.Set(_T("#95d900"));
    else if((val >= 9) && (val < 12)) c.Set(_T("#d9d900"));
    else if((val >= 12) && (val < 15)) c.Set(_T("#d9ae00"));
    else if((val >= 15) && (val < 18)) c.Set(_T("#d98300"));
    else if((val >= 18) && (val < 21)) c.Set(_T("#d95700"));
    else if((val >= 21) && (val < 24)) c.Set(_T("#d90000"));
    else if((val >= 24) && (val < 27)) c.Set(_T("#ae0000"));
    else if((val >= 27) && (val < 30)) c.Set(_T("#8c0000"));
    else if((val >= 30) && (val < 36)) c.Set(_T("#870000"));
    else if((val >= 36) && (val < 42)) c.Set(_T("#690000"));
    else if((val >= 42) && (val < 48)) c.Set(_T("#550000"));
    else if( val >= 48) c.Set(_T("#410000"));

    return c;
}

void ChartCanvas::RenderGeorefErrorMap( wxMemoryDC *pmdc, ViewPort *vp)
{
    wxImage gr_image(vp->pix_width, vp->pix_height);
    gr_image.InitAlpha();

    double maxval = -10000;
    double minval = 10000;

    double rlat, rlon;
    double glat, glon;

    GetCanvasPixPoint(0, 0, rlat, rlon);

    for(int i=1; i < vp->pix_height-1; i++)
    {
        for(int j=0; j < vp->pix_width; j++)
        {
            // Reference mercator value
//                  vp->GetMercatorLLFromPix(wxPoint(j, i), &rlat, &rlon);

            // Georef value
            GetCanvasPixPoint(j, i, glat, glon);

            maxval = wxMax(maxval, (glat - rlat));
            minval = wxMin(minval, (glat - rlat));

        }
        rlat = glat;
    }

    GetCanvasPixPoint(0, 0, rlat, rlon);
    for(int i=1; i < vp->pix_height-1; i++)
    {
        for(int j=0; j < vp->pix_width; j++)
        {
            // Reference mercator value
//                  vp->GetMercatorLLFromPix(wxPoint(j, i), &rlat, &rlon);

            // Georef value
            GetCanvasPixPoint(j, i, glat, glon);

            double f = ((glat - rlat)-minval)/(maxval - minval);

            double dy = (f * 40);

            wxColour c = GetErrorGraphicColor(dy);
            unsigned char r = c.Red();
            unsigned char g = c.Green();
            unsigned char b = c.Blue();

            gr_image.SetRGB(j, i, r,g,b);
            if((glat - rlat )!= 0)
                gr_image.SetAlpha(j, i, 128);
            else
                gr_image.SetAlpha(j, i, 255);

        }
        rlat = glat;
    }

    //    Create a Bitmap
    wxBitmap *pbm = new wxBitmap(gr_image);
    wxMask *gr_mask = new wxMask(*pbm, wxColour(0,0,0));
    pbm->SetMask(gr_mask);

    pmdc->DrawBitmap(*pbm, 0,0);

    delete pbm;

}

#endif
bool ChartCanvas::SetCursor(const wxCursor &c) {
#ifdef ocpnUSE_GL
  if (g_bopengl && m_glcc)
    return m_glcc->SetCursor(c);
  else
#endif
    return wxWindow::SetCursor(c);
}

void ChartCanvas::Refresh(bool eraseBackground, const wxRect *rect) {
  if (g_bquiting) return;
  //  Keep the mouse position members up to date
  GetCanvasPixPoint(mouse_x, mouse_y, m_cursor_lat, m_cursor_lon);

  //      Retrigger the route leg popup timer
  //      This handles the case when the chart is moving in auto-follow mode,
  //      but no user mouse input is made. The timer handler may Hide() the
  //      popup if the chart moved enough n.b.  We use slightly longer oneshot
  //      value to allow this method's Refresh() to complete before potentially
  //      getting another Refresh() in the popup timer handler.  

#ifdef ocpnUSE_GL
  if (m_glcc && g_bopengl) {
    //      We need to invalidate the FBO cache to ensure repaint of "grounded"
    //      overlay objects.
    if (eraseBackground && m_glcc->UsingFBO()) m_glcc->Invalidate();

    m_glcc->Refresh(eraseBackground,
                    NULL);  // We always are going to render the entire screen
                            // anyway, so make
    // sure that the window managers understand the invalid area
    // is actually the entire client area.

    //  We need to selectively Refresh some child windows, if they are visible.
    //  Note that some children are refreshed elsewhere on timer ticks, so don't
    //  need attention here.

    //      Thumbnail chart
    if (pthumbwin && pthumbwin->IsShown()) {
      pthumbwin->Raise();
      pthumbwin->Refresh(false);
    }

    //      ChartInfo window
    if (m_pCIWin && m_pCIWin->IsShown()) {
      m_pCIWin->Raise();
      m_pCIWin->Refresh(false);
    }
  } else
#endif
    wxWindow::Refresh(eraseBackground, rect);
}

void ChartCanvas::Update() {
  if (m_glcc && g_bopengl) {
#ifdef ocpnUSE_GL
    m_glcc->Update();
#endif
  } else
    wxWindow::Update();
}

void ChartCanvas::DrawEmboss(ocpnDC &dc, emboss_data *pemboss) {
  if (!pemboss) return;
  int x = pemboss->x, y = pemboss->y;
  const double factor = 200;

  wxASSERT_MSG(dc.GetDC(), wxT("DrawEmboss has no dc (opengl?)"));
  wxMemoryDC *pmdc = dynamic_cast<wxMemoryDC *>(dc.GetDC());
  wxASSERT_MSG(pmdc, wxT("dc to EmbossCanvas not a memory dc"));

  // Grab a snipped image out of the chart
  wxMemoryDC snip_dc;
  wxBitmap snip_bmp(pemboss->width, pemboss->height, -1);
  snip_dc.SelectObject(snip_bmp);

  snip_dc.Blit(0, 0, pemboss->width, pemboss->height, pmdc, x, y);
  snip_dc.SelectObject(wxNullBitmap);

  wxImage snip_img = snip_bmp.ConvertToImage();

  //  Apply Emboss map to the snip image
  unsigned char *pdata = snip_img.GetData();
  if (pdata) {
    for (int y = 0; y < pemboss->height; y++) {
      int map_index = (y * pemboss->width);
      for (int x = 0; x < pemboss->width; x++) {
        double val = (pemboss->pmap[map_index] * factor) / 256.;

        int nred = (int)((*pdata) + val);
        nred = nred > 255 ? 255 : (nred < 0 ? 0 : nred);
        *pdata++ = (unsigned char)nred;

        int ngreen = (int)((*pdata) + val);
        ngreen = ngreen > 255 ? 255 : (ngreen < 0 ? 0 : ngreen);
        *pdata++ = (unsigned char)ngreen;

        int nblue = (int)((*pdata) + val);
        nblue = nblue > 255 ? 255 : (nblue < 0 ? 0 : nblue);
        *pdata++ = (unsigned char)nblue;

        map_index++;
      }
    }
  }

  //  Convert embossed snip to a bitmap
  wxBitmap emb_bmp(snip_img);

  //  Map to another memoryDC
  wxMemoryDC result_dc;
  result_dc.SelectObject(emb_bmp);

  //  Blit to target
  pmdc->Blit(x, y, pemboss->width, pemboss->height, &result_dc, 0, 0);

  result_dc.SelectObject(wxNullBitmap);
}

emboss_data *ChartCanvas::EmbossOverzoomIndicator(ocpnDC &dc) {
  double zoom_factor = GetVP().ref_scale / GetVP().chart_scale;

  if (GetQuiltMode()) {
    // disable Overzoom indicator for MBTiles
    int refIndex = GetQuiltRefChartdbIndex();
    if (refIndex >= 0) {
      const ChartTableEntry &cte = ChartData->GetChartTableEntry(refIndex);
      ChartTypeEnum current_type = (ChartTypeEnum)cte.GetChartType();
      if (current_type == CHART_TYPE_MBTILES) {
        ChartBase *pChart = m_pQuilt->GetRefChart();
      }
    }

    if (zoom_factor <= 3.9) return NULL;
  } else {
    if (m_singleChart) {
      if (zoom_factor <= 3.9) return NULL;
    } else
      return NULL;
  }

  if (m_pEM_OverZoom) {
    m_pEM_OverZoom->x = 4;
    m_pEM_OverZoom->y = 0;
  }
  return m_pEM_OverZoom;
}

void ChartCanvas::DrawOverlayObjects(ocpnDC &dc, const wxRegion &ru) {
  GridDraw(dc);

  //     bool pluginOverlayRender = true;
  //
  //     if(g_canvasConfig > 0){     // Multi canvas
  //         if(IsPrimaryCanvas())
  //             pluginOverlayRender = false;
  //     }

  g_overlayCanvas = this;

  AISDrawAreaNotices(dc, GetVP(), this);

  wxDC *pdc = dc.GetDC();
  if (pdc) {
    pdc->DestroyClippingRegion();
    wxDCClipper(*pdc, ru);
  }

  AISDraw(dc, GetVP(), this);

  RenderVisibleSectorLights(dc);

  RenderAllChartOutlines(dc, GetVP());
  
  ScaleBarDraw(dc);
  s57_DrawExtendedLightSectors(dc, VPoint, extendedSectorLegs);

  DrawEmboss(dc, EmbossDepthScale());
  DrawEmboss(dc, EmbossOverzoomIndicator(dc));  
}

emboss_data *ChartCanvas::EmbossDepthScale() {
  if (!m_bShowDepthUnits) return NULL;

  int depth_unit_type = DEPTH_UNIT_UNKNOWN;

  if (GetQuiltMode()) {
    wxString s = m_pQuilt->GetQuiltDepthUnit();
    s.MakeUpper();
    if (s == _T("FEET"))
      depth_unit_type = DEPTH_UNIT_FEET;
    else if (s.StartsWith(_T("FATHOMS")))
      depth_unit_type = DEPTH_UNIT_FATHOMS;
    else if (s.StartsWith(_T("METERS")))
      depth_unit_type = DEPTH_UNIT_METERS;
    else if (s.StartsWith(_T("METRES")))
      depth_unit_type = DEPTH_UNIT_METERS;
    else if (s.StartsWith(_T("METRIC")))
      depth_unit_type = DEPTH_UNIT_METERS;
    else if (s.StartsWith(_T("METER")))
      depth_unit_type = DEPTH_UNIT_METERS;

  } else {
    if (m_singleChart) {
      depth_unit_type = m_singleChart->GetDepthUnitType();
      if (m_singleChart->GetChartFamily() == CHART_FAMILY_VECTOR)
        depth_unit_type = ps52plib->m_nDepthUnitDisplay + 1;
    }
  }

  emboss_data *ped = NULL;
  switch (depth_unit_type) {
    case DEPTH_UNIT_FEET:
      ped = m_pEM_Feet;
      break;
    case DEPTH_UNIT_METERS:
      ped = m_pEM_Meters;
      break;
    case DEPTH_UNIT_FATHOMS:
      ped = m_pEM_Fathoms;
      break;
    default:
      return NULL;
  }

  ped->x = (GetVP().pix_width - ped->width);

  ped->y = 40;
  
  return ped;
}

void ChartCanvas::CreateDepthUnitEmbossMaps(ColorScheme cs) {
  ocpnStyle::Style *style = g_StyleManager->GetCurrentStyle();
  wxFont font;
  if (style->embossFont == wxEmptyString) {
    wxFont *dFont = FontMgr::Get().GetFont(_("Dialog"), 0);
    font = *dFont;
    font.SetPointSize(60);
    font.SetWeight(wxFONTWEIGHT_BOLD);
  } else
    font = wxFont(style->embossHeight, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
                  wxFONTWEIGHT_BOLD, false, style->embossFont);

  int emboss_width = 500;
  int emboss_height = 200;

  // Free any existing emboss maps
  delete m_pEM_Feet;
  delete m_pEM_Meters;
  delete m_pEM_Fathoms;

  // Create the 3 DepthUnit emboss map structures
  m_pEM_Feet =
      CreateEmbossMapData(font, emboss_width, emboss_height, _("Feet"), cs);
  m_pEM_Meters =
      CreateEmbossMapData(font, emboss_width, emboss_height, _("Meters"), cs);
  m_pEM_Fathoms =
      CreateEmbossMapData(font, emboss_width, emboss_height, _("Fathoms"), cs);
}

#define OVERZOOM_TEXT _("OverZoom")

void ChartCanvas::SetOverzoomFont() {
  ocpnStyle::Style *style = g_StyleManager->GetCurrentStyle();
  int w, h;

  wxFont font;
  if (style->embossFont == wxEmptyString) {
    wxFont *dFont = FontMgr::Get().GetFont(_("Dialog"), 0);
    font = *dFont;
    font.SetPointSize(40);
    font.SetWeight(wxFONTWEIGHT_BOLD);
  } else
    font = wxFont(style->embossHeight, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL,
                  wxFONTWEIGHT_BOLD, false, style->embossFont);

  wxClientDC dc(this);
  dc.SetFont(font);
  dc.GetTextExtent(OVERZOOM_TEXT, &w, &h);

  while (font.GetPointSize() > 10 && (w > 500 || h > 100)) {
    font.SetPointSize(font.GetPointSize() - 1);
    dc.SetFont(font);
    dc.GetTextExtent(OVERZOOM_TEXT, &w, &h);
  }
  m_overzoomFont = font;
  m_overzoomTextWidth = w;
  m_overzoomTextHeight = h;
}

void ChartCanvas::CreateOZEmbossMapData(ColorScheme cs) {
  delete m_pEM_OverZoom;

  if (m_overzoomTextWidth > 0 && m_overzoomTextHeight > 0)
    m_pEM_OverZoom =
        CreateEmbossMapData(m_overzoomFont, m_overzoomTextWidth + 10,
                            m_overzoomTextHeight + 10, OVERZOOM_TEXT, cs);
}

emboss_data *ChartCanvas::CreateEmbossMapData(wxFont &font, int width,
                                              int height, const wxString &str,
                                              ColorScheme cs) {
  int *pmap;

  //  Create a temporary bitmap
  wxBitmap bmp(width, height, -1);

  // Create a memory DC
  wxMemoryDC temp_dc;
  temp_dc.SelectObject(bmp);

  //  Paint on it
  temp_dc.SetBackground(*wxWHITE_BRUSH);
  temp_dc.SetTextBackground(*wxWHITE);
  temp_dc.SetTextForeground(*wxBLACK);

  temp_dc.Clear();

  temp_dc.SetFont(font);

  int str_w, str_h;
  temp_dc.GetTextExtent(str, &str_w, &str_h);
  //    temp_dc.DrawText( str, width - str_w - 10, 10 );
  temp_dc.DrawText(str, 1, 1);

  //  Deselect the bitmap
  temp_dc.SelectObject(wxNullBitmap);

  //  Convert bitmap the wxImage for manipulation
  wxImage img = bmp.ConvertToImage();

  int image_width = str_w * 105 / 100;
  int image_height = str_h * 105 / 100;
  wxRect r(0, 0, wxMin(image_width, img.GetWidth()),
           wxMin(image_height, img.GetHeight()));
  wxImage imgs = img.GetSubImage(r);

  double val_factor;
  switch (cs) {
    case GLOBAL_COLOR_SCHEME_DAY:
    default:
      val_factor = 1;
      break;
    case GLOBAL_COLOR_SCHEME_DUSK:
      val_factor = .5;
      break;
    case GLOBAL_COLOR_SCHEME_NIGHT:
      val_factor = .25;
      break;
  }

  int val;
  int index;
  const int w = imgs.GetWidth();
  const int h = imgs.GetHeight();
  pmap = (int *)calloc(w * h * sizeof(int), 1);
  //  Create emboss map by differentiating the emboss image
  //  and storing integer results in pmap
  //  n.b. since the image is B/W, it is sufficient to check
  //  one channel (i.e. red) only
  for (int y = 1; y < h - 1; y++) {
    for (int x = 1; x < w - 1; x++) {
      val =
          img.GetRed(x + 1, y + 1) - img.GetRed(x - 1, y - 1);  // range +/- 256
      val = (int)(val * val_factor);
      index = (y * w) + x;
      pmap[index] = val;
    }
  }

  emboss_data *pret = new emboss_data;
  pret->pmap = pmap;
  pret->width = w;
  pret->height = h;

  return pret;
}

void ChartCanvas::DrawBlinkObjects(void) {
  //  All RoutePoints
  wxRect update_rect;
  if (!update_rect.IsEmpty()) RefreshRect(update_rect);
}
//------------------------------------------------------------------------------------------
//    Tides Support
//------------------------------------------------------------------------------------------
void ChartCanvas::RebuildTideSelectList(LLBBox &BBox) {
  if (!ptcmgr) return;

  for (int i = 1; i < ptcmgr->Get_max_IDX() + 1; i++) {
    const IDX_entry *pIDX = ptcmgr->GetIDX_entry(i);
    double lon = pIDX->IDX_lon;
    double lat = pIDX->IDX_lat;

    char type = pIDX->IDX_type;  // Entry "TCtcIUu" identifier  
  }
}

extern wxDateTime gTimeSource;

void ChartCanvas::DrawAllTidesInBBox(ocpnDC &dc, LLBBox &BBox) {
  if (!ptcmgr) return;

  wxDateTime this_now = gTimeSource;
  bool cur_time = !gTimeSource.IsValid();
  if (cur_time) this_now = wxDateTime::Now();
  time_t t_this_now = this_now.GetTicks();

  wxPen *pblack_pen = wxThePenList->FindOrCreatePen(
      GetGlobalColor(_T ( "UINFD" )), 1, wxPENSTYLE_SOLID);
  wxPen *pyelo_pen = wxThePenList->FindOrCreatePen(
      GetGlobalColor(cur_time ? _T ( "YELO1" ) : _T ( "YELO2" )), 1,
      wxPENSTYLE_SOLID);
  wxPen *pblue_pen = wxThePenList->FindOrCreatePen(
      GetGlobalColor(cur_time ? _T ( "BLUE2" ) : _T ( "BLUE3" )), 1,
      wxPENSTYLE_SOLID);

  wxBrush *pgreen_brush = wxTheBrushList->FindOrCreateBrush(
      GetGlobalColor(_T ( "GREEN1" )), wxBRUSHSTYLE_SOLID);
  //        wxBrush *pblack_brush = wxTheBrushList->FindOrCreateBrush (
  //        GetGlobalColor ( _T ( "UINFD" ) ), wxSOLID );
  wxBrush *pblue_brush = wxTheBrushList->FindOrCreateBrush(
      GetGlobalColor(cur_time ? _T ( "BLUE2" ) : _T ( "BLUE3" )),
      wxBRUSHSTYLE_SOLID);
  wxBrush *pyelo_brush = wxTheBrushList->FindOrCreateBrush(
      GetGlobalColor(cur_time ? _T ( "YELO1" ) : _T ( "YELO2" )),
      wxBRUSHSTYLE_SOLID);

  wxFont *dFont = FontMgr::Get().GetFont(_("ExtendedTideIcon"));
  dc.SetTextForeground(FontMgr::Get().GetFontColor(_("ExtendedTideIcon")));
  int font_size = wxMax(10, dFont->GetPointSize());
  font_size /= g_Platform->GetDisplayDIPMult(this);
  wxFont *plabelFont =
          FontMgr::Get().FindOrCreateFont(font_size,
                                  dFont->GetFamily(), dFont->GetStyle(),
                                  dFont->GetWeight(), false,
                                  dFont->GetFaceName());

  dc.SetPen(*pblack_pen);
  dc.SetBrush(*pgreen_brush);

  wxBitmap bm;
  switch (m_cs) {
    case GLOBAL_COLOR_SCHEME_DAY:
      bm = m_bmTideDay;
      break;
    case GLOBAL_COLOR_SCHEME_DUSK:
      bm = m_bmTideDusk;
      break;
    case GLOBAL_COLOR_SCHEME_NIGHT:
      bm = m_bmTideNight;
      break;
    default:
      bm = m_bmTideDay;
      break;
  }

  int bmw = bm.GetWidth();
  int bmh = bm.GetHeight();

  float scale_factor = 1.0;

  //  Set the onscreen size of the symbol
  //  Compensate for various display resolutions
  float icon_pixelRefDim = 45;

#if 0
    float nominal_icon_size_mm = g_Platform->GetDisplaySizeMM() *25 / 1000; // Intended physical rendered size onscreen
    nominal_icon_size_mm = wxMax(nominal_icon_size_mm, 8);
    nominal_icon_size_mm = wxMin(nominal_icon_size_mm, 15);
    float nominal_icon_size_pixels = wxMax(4.0, floor(g_Platform->GetDisplayDPmm() * nominal_icon_size_mm));  // nominal size, but not less than 4 pixel
#endif

#ifndef __OCPN__ANDROID__
  // another method is simply to declare that the icon shall be x times the size
  // of a raster symbol (e.g.BOYLAT)
  //  This is a bit of a hack that will suffice until until we get fully
  //  scalable ENC symbol sets
  //   float nominal_icon_size_pixels = 48;  // 3 x 16
  //   float pix_factor = nominal_icon_size_pixels / icon_pixelRefDim;

  // or, x times size of text font
  wxScreenDC sdc;
  int height;
  sdc.GetTextExtent("M", NULL, &height, NULL, NULL, plabelFont);
  height *= g_Platform->GetDisplayDIPMult(this);
  float nominal_icon_size_pixels = 48;  // 3 x 16
  float pix_factor = (2 * height) / nominal_icon_size_pixels;


#else
  //  Yet another method goes like this:
  //  Set the onscreen size of the symbol
  //  Compensate for various display resolutions
  //  Develop empirically, making a symbol about 16 mm tall
  double symHeight =
      icon_pixelRefDim /
      GetPixPerMM();  // from draw instructions, symbol is xx pix high
  double targetHeight0 = 16.0;

  // But we want to scale the size down for smaller displays
  double displaySize = m_display_size_mm;
  displaySize = wxMax(displaySize, 100);

  float targetHeight = wxMin(targetHeight0, displaySize / 15);

  double pix_factor = targetHeight / symHeight;
#endif

  scale_factor *= pix_factor;

  float user_scale_factor = g_ChartScaleFactorExp;
  if (g_ChartScaleFactorExp > 1.0)
    user_scale_factor = (log(g_ChartScaleFactorExp) + 1.0) *
                        1.2;  // soften the scale factor a bit

  scale_factor *= user_scale_factor;
  scale_factor *= GetContentScaleFactor();


  {
    double marge = 0.05;
    std::vector<LLBBox> drawn_boxes;
    for (int i = 1; i < ptcmgr->Get_max_IDX() + 1; i++) {
      const IDX_entry *pIDX = ptcmgr->GetIDX_entry(i);

      char type = pIDX->IDX_type;          // Entry "TCtcIUu" identifier
      if ((type == 't') || (type == 'T'))  // only Tides
      {
        double lon = pIDX->IDX_lon;
        double lat = pIDX->IDX_lat;

        if (BBox.ContainsMarge(lat, lon, marge)) {

          // Avoid drawing detailed graphic for duplicate tide stations
          if (GetVP().chart_scale < 500000){
            bool bdrawn = false;
            for (size_t i = 0; i < drawn_boxes.size(); i++){
              if (drawn_boxes[i].Contains(lat, lon)){
                bdrawn = true;
                break;
              }
            }
            if (bdrawn)
              continue;   // the station loop

            LLBBox this_box;
            this_box.Set(lat, lon, lat, lon);
            this_box.EnLarge(.005);
            drawn_boxes.push_back(this_box);
          }

          wxPoint r;
          GetCanvasPointPix(lat, lon, &r);
          // draw standard icons
          if (GetVP().chart_scale > 500000) {
            dc.DrawBitmap(bm, r.x - bmw / 2, r.y - bmh / 2, true);
          }
          // draw "extended" icons
          else {
            dc.SetFont(*plabelFont);
            {
              {
                float val, nowlev;
                float ltleve = 0.;
                float htleve = 0.;
                time_t tctime;
                time_t lttime = 0;
                time_t httime = 0;
                bool wt;
                // define if flood or ebb in the last ten minutes and verify if
                // data are useable
                if (ptcmgr->GetTideFlowSens(
                        t_this_now, BACKWARD_TEN_MINUTES_STEP,
                        pIDX->IDX_rec_num, nowlev, val, wt)) {
                  // search forward the first HW or LW near "now" ( starting at
                  // "now" - ten minutes )
                  ptcmgr->GetHightOrLowTide(
                      t_this_now + BACKWARD_TEN_MINUTES_STEP,
                      FORWARD_TEN_MINUTES_STEP, FORWARD_ONE_MINUTES_STEP, val,
                      wt, pIDX->IDX_rec_num, val, tctime);
                  if (wt) {
                    httime = tctime;
                    htleve = val;
                  } else {
                    lttime = tctime;
                    ltleve = val;
                  }
                  wt = !wt;

                  // then search opposite tide near "now"
                  if (tctime > t_this_now)  // search backward
                    ptcmgr->GetHightOrLowTide(
                        t_this_now, BACKWARD_TEN_MINUTES_STEP,
                        BACKWARD_ONE_MINUTES_STEP, nowlev, wt,
                        pIDX->IDX_rec_num, val, tctime);
                  else
                    // or search forward
                    ptcmgr->GetHightOrLowTide(
                        t_this_now, FORWARD_TEN_MINUTES_STEP,
                        FORWARD_ONE_MINUTES_STEP, nowlev, wt, pIDX->IDX_rec_num,
                        val, tctime);
                  if (wt) {
                    httime = tctime;
                    htleve = val;
                  } else {
                    lttime = tctime;
                    ltleve = val;
                  }

                  // draw the tide rectangle:

                  // tide icon rectangle has default pre-scaled width = 12 ,
                  // height = 45
                  int width = (int)(12 * scale_factor + 0.5);
                  int height = (int)(45 * scale_factor + 0.5);
                  int linew = wxMax(1, (int)(scale_factor));
                  int xDraw = r.x - (width / 2);
                  int yDraw = r.y - (height / 2);

                  // process tide state  ( %height and flow sens )
                  float ts = 1 - ((nowlev - ltleve) / (htleve - ltleve));
                  int hs = (httime > lttime) ? -4 : 4;
                  hs *= (int)(scale_factor + 0.5);
                  if (ts > 0.995 || ts < 0.005) hs = 0;
                  int ht_y = (int)(height * ts);

                  // draw yellow tide rectangle outlined in black
                  pblack_pen->SetWidth(linew);
                  dc.SetPen(*pblack_pen);
                  dc.SetBrush(*pyelo_brush);
                  dc.DrawRectangle(xDraw, yDraw, width, height);

                  // draw blue rectangle as water height, smaller in width than
                  // yellow rectangle
                  dc.SetPen(*pblue_pen);
                  dc.SetBrush(*pblue_brush);
                  dc.DrawRectangle((xDraw + 2 * linew), yDraw + ht_y,
                                   (width - (4 * linew)), height - ht_y);

                  // draw sens arrows (ensure they are not "under-drawn" by top
                  // line of blue rectangle )
                  int hl;
                  wxPoint arrow[3];
                  arrow[0].x = xDraw + 2 * linew;
                  arrow[1].x = xDraw + width / 2;
                  arrow[2].x = xDraw + width - 2 * linew;
                  pyelo_pen->SetWidth(linew);
                  pblue_pen->SetWidth(linew);
                  if (ts > 0.35 || ts < 0.15)  // one arrow at 3/4 hight tide
                  {
                    hl = (int)(height * 0.25) + yDraw;
                    arrow[0].y = hl;
                    arrow[1].y = hl + hs;
                    arrow[2].y = hl;
                    if (ts < 0.15)
                      dc.SetPen(*pyelo_pen);
                    else
                      dc.SetPen(*pblue_pen);
                    dc.DrawLines(3, arrow);
                  }
                  if (ts > 0.60 || ts < 0.40)  // one arrow at 1/2 hight tide
                  {
                    hl = (int)(height * 0.5) + yDraw;
                    arrow[0].y = hl;
                    arrow[1].y = hl + hs;
                    arrow[2].y = hl;
                    if (ts < 0.40)
                      dc.SetPen(*pyelo_pen);
                    else
                      dc.SetPen(*pblue_pen);
                    dc.DrawLines(3, arrow);
                  }
                  if (ts < 0.65 || ts > 0.85)  // one arrow at 1/4 Hight tide
                  {
                    hl = (int)(height * 0.75) + yDraw;
                    arrow[0].y = hl;
                    arrow[1].y = hl + hs;
                    arrow[2].y = hl;
                    if (ts < 0.65)
                      dc.SetPen(*pyelo_pen);
                    else
                      dc.SetPen(*pblue_pen);
                    dc.DrawLines(3, arrow);
                  }
                  // draw tide level text
                  wxString s;
                  s.Printf(_T("%3.1f"), nowlev);
                  Station_Data *pmsd = pIDX->pref_sta_data;  // write unit
                  if (pmsd) s.Append(wxString(pmsd->units_abbrv, wxConvUTF8));
                  int wx1;
                  dc.GetTextExtent(s, &wx1, NULL);
                  wx1 *= g_Platform->GetDisplayDIPMult(this);
                  dc.DrawText(s, r.x - (wx1 / 2), yDraw + height);
                }
              }
            }
          }
        }
      }
    }
  }
}

//------------------------------------------------------------------------------------------
//    Currents Support
//------------------------------------------------------------------------------------------

void ChartCanvas::RebuildCurrentSelectList(LLBBox &BBox) {
  if (!ptcmgr) return;

  for (int i = 1; i < ptcmgr->Get_max_IDX() + 1; i++) {
    const IDX_entry *pIDX = ptcmgr->GetIDX_entry(i);
    double lon = pIDX->IDX_lon;
    double lat = pIDX->IDX_lat;
  }
}

void ChartCanvas::DrawAllCurrentsInBBox(ocpnDC &dc, LLBBox &BBox) {
  if (!ptcmgr) return;

  float tcvalue, dir;
  bool bnew_val;
  char sbuf[20];
  wxFont *pTCFont;
  double lon_last = 0.;
  double lat_last = 0.;
  // arrow size for Raz Blanchard : 12 knots north
  double marge = 0.2;
  bool cur_time = !gTimeSource.IsValid();

  double true_scale_display = floor(VPoint.chart_scale / 100.) * 100.;
  bDrawCurrentValues = true_scale_display < g_Show_Target_Name_Scale;

  wxPen *pblack_pen = wxThePenList->FindOrCreatePen(
      GetGlobalColor(_T ( "UINFD" )), 1, wxPENSTYLE_SOLID);
  wxPen *porange_pen = wxThePenList->FindOrCreatePen(
      GetGlobalColor(cur_time ? _T ( "UINFO" ) : _T ( "UINFB" )), 1,
      wxPENSTYLE_SOLID);
  wxBrush *porange_brush = wxTheBrushList->FindOrCreateBrush(
      GetGlobalColor(cur_time ? _T ( "UINFO" ) : _T ( "UINFB" )),
      wxBRUSHSTYLE_SOLID);
  wxBrush *pgray_brush = wxTheBrushList->FindOrCreateBrush(
      GetGlobalColor(_T ( "UIBDR" )), wxBRUSHSTYLE_SOLID);
  wxBrush *pblack_brush = wxTheBrushList->FindOrCreateBrush(
      GetGlobalColor(_T ( "UINFD" )), wxBRUSHSTYLE_SOLID);

  double skew_angle = GetVPRotation();

  wxFont *dFont = FontMgr::Get().GetFont(_("CurrentValue"));
  int font_size = wxMax(10, dFont->GetPointSize());
  font_size /= g_Platform->GetDisplayDIPMult(this);
  pTCFont =
          FontMgr::Get().FindOrCreateFont(font_size,
                                  dFont->GetFamily(), dFont->GetStyle(),
                                  dFont->GetWeight(), false,
                                  dFont->GetFaceName());


  float scale_factor = 1.0;

  //  Set the onscreen size of the symbol
  //  Compensate for various display resolutions

#if 0
    float nominal_icon_size_mm = g_Platform->GetDisplaySizeMM() *3 / 1000; // Intended physical rendered size onscreen
    nominal_icon_size_mm = wxMax(nominal_icon_size_mm, 2);
    nominal_icon_size_mm = wxMin(nominal_icon_size_mm, 4);
    float nominal_icon_size_pixels = wxMax(4.0, floor(g_Platform->GetDisplayDPmm() * nominal_icon_size_mm));  // nominal size, but not less than 4 pixel
#endif

#if 0
    // another method is simply to declare that the icon shall be x times the size of a raster symbol (e.g.BOYLAT)
    //  This is a bit of a hack that will suffice until until we get fully scalable ENC symbol sets
    float nominal_icon_size_pixels = 6;  // 16 / 3
    float pix_factor = nominal_icon_size_pixels / icon_pixelRefDim;
#endif

#ifndef __OCPN__ANDROID__
  // or, x times size of text font
  wxScreenDC sdc;
  int height;
  sdc.GetTextExtent("M", NULL, &height, NULL, NULL, pTCFont);
  height *= g_Platform->GetDisplayDIPMult(this);
  float nominal_icon_size_pixels = 15;
  float pix_factor = (1 * height) / nominal_icon_size_pixels;

#else
  //  Yet another method goes like this:
  //  Set the onscreen size of the symbol
  //  Compensate for various display resolutions
  //  Develop empirically....
  float icon_pixelRefDim = 5;

  double symHeight =
      icon_pixelRefDim /
      GetPixPerMM();  // from draw instructions, symbol is xx pix high
  double targetHeight0 = 2.0;

  // But we want to scale the size down for smaller displays
  double displaySize = m_display_size_mm;
  displaySize = wxMax(displaySize, 100);

  float targetHeight = wxMin(targetHeight0, displaySize / 50);
  double pix_factor = targetHeight / symHeight;
#endif

  scale_factor *= pix_factor;

  float user_scale_factor = g_ChartScaleFactorExp;
  if (g_ChartScaleFactorExp > 1.0)
    user_scale_factor = (log(g_ChartScaleFactorExp) + 1.0) *
                        1.2;  // soften the scale factor a bit

  scale_factor *= user_scale_factor;

  scale_factor *= GetContentScaleFactor();

  {
    for (int i = 1; i < ptcmgr->Get_max_IDX() + 1; i++) {
      const IDX_entry *pIDX = ptcmgr->GetIDX_entry(i);
      double lon = pIDX->IDX_lon;
      double lat = pIDX->IDX_lat;

      char type = pIDX->IDX_type;  // Entry "TCtcIUu" identifier
      if (((type == 'c') || (type == 'C')) && (1 /*pIDX->IDX_Useable*/)) {
        if (!pIDX->b_skipTooDeep && (BBox.ContainsMarge(lat, lon, marge))) {
          wxPoint r;
          GetCanvasPointPix(lat, lon, &r);

          wxPoint d[4];  // points of a diamond at the current station location
          int dd = (int)(5.0 * scale_factor + 0.5);
          d[0].x = r.x;
          d[0].y = r.y + dd;
          d[1].x = r.x + dd;
          d[1].y = r.y;
          d[2].x = r.x;
          d[2].y = r.y - dd;
          d[3].x = r.x - dd;
          d[3].y = r.y;

          if (1) {
            pblack_pen->SetWidth(wxMax(2, (int)(scale_factor + 0.5)));
            dc.SetPen(*pblack_pen);
            dc.SetBrush(*porange_brush);
            dc.DrawPolygon(4, d);

            if (type == 'C') {
              dc.SetBrush(*pblack_brush);
              dc.DrawCircle(r.x, r.y, (int)(2 * scale_factor));
            }

            if (GetVP().chart_scale < 1000000) {
              if (!ptcmgr->GetTideOrCurrent15(0, i, tcvalue, dir, bnew_val))
                continue;
            } else
              continue;

            if (1 /*type == 'c'*/) {
              {
                //    Get the display pixel location of the current station
                int pixxc, pixyc;
                pixxc = r.x;
                pixyc = r.y;

                //    Adjust drawing size using logarithmic scale. tcvalue is
                //    current in knots
                double a1 = fabs(tcvalue) * 10.;
                // Current values <= 0.1 knot will have no arrow
                a1 = wxMax(1.0, a1);
                double a2 = log10(a1);

                float cscale = scale_factor * a2 * 0.4;

                porange_pen->SetWidth(wxMax(2, (int)(scale_factor + 0.5)));
                dc.SetPen(*porange_pen);
                DrawArrow(dc, pixxc, pixyc, dir - 90 + (skew_angle * 180. / PI),
                          cscale);
                // Draw text, if enabled

                if (bDrawCurrentValues) {
                  dc.SetFont(*pTCFont);
                  snprintf(sbuf, 19, "%3.1f", fabs(tcvalue));
                  dc.DrawText(wxString(sbuf, wxConvUTF8), pixxc, pixyc);
                }
              }
            }  // scale
          }
          /*          This is useful for debugging the TC database
           else
           {
           dc.SetPen ( *porange_pen );
           dc.SetBrush ( *pgray_brush );
           dc.DrawPolygon ( 4, d );
           }
           */
        }
        lon_last = lon;
        lat_last = lat;
      }
    }
  }
}

#define NUM_CURRENT_ARROW_POINTS 9
static wxPoint CurrentArrowArray[NUM_CURRENT_ARROW_POINTS] = {
    wxPoint(0, 0),    wxPoint(0, -10), wxPoint(55, -10),
    wxPoint(55, -25), wxPoint(100, 0), wxPoint(55, 25),
    wxPoint(55, 10),  wxPoint(0, 10),  wxPoint(0, 0)};

void ChartCanvas::DrawArrow(ocpnDC &dc, int x, int y, double rot_angle,
                            double scale) {
  if (scale > 1e-2) {
    float sin_rot = sin(rot_angle * PI / 180.);
    float cos_rot = cos(rot_angle * PI / 180.);

    // Move to the first point

    float xt = CurrentArrowArray[0].x;
    float yt = CurrentArrowArray[0].y;

    float xp = (xt * cos_rot) - (yt * sin_rot);
    float yp = (xt * sin_rot) + (yt * cos_rot);
    int x1 = (int)(xp * scale);
    int y1 = (int)(yp * scale);

    // Walk thru the point list
    for (int ip = 1; ip < NUM_CURRENT_ARROW_POINTS; ip++) {
      xt = CurrentArrowArray[ip].x;
      yt = CurrentArrowArray[ip].y;

      float xp = (xt * cos_rot) - (yt * sin_rot);
      float yp = (xt * sin_rot) + (yt * cos_rot);
      int x2 = (int)(xp * scale);
      int y2 = (int)(yp * scale);

      dc.DrawLine(x1 + x, y1 + y, x2 + x, y2 + y);

      x1 = x2;
      y1 = y2;
    }
  }
}

void ChartCanvas::ToggleCanvasQuiltMode(void) {
  bool cur_mode = GetQuiltMode();

  if (!GetQuiltMode())
    SetQuiltMode(true);
  else if (GetQuiltMode()) {
    SetQuiltMode(false);
    g_sticky_chart = GetQuiltReferenceChartIndex();
  }

  if (cur_mode != GetQuiltMode()) {
    SetupCanvasQuiltMode();
    DoCanvasUpdate();
    InvalidateGL();
    Refresh();
  }
  //  TODO What to do about this?
  // g_bQuiltEnable = GetQuiltMode();

  // Recycle the S52 PLIB so that vector charts will flush caches and re-render
  if (ps52plib) ps52plib->GenerateStateHash();
}

void ChartCanvas::DoCanvasStackDelta(int direction) {
  if (!GetQuiltMode()) {
    int current_stack_index = GetpCurrentStack()->CurrentStackEntry;
    if ((current_stack_index + direction) >= GetpCurrentStack()->nEntry) return;
    if ((current_stack_index + direction) < 0) return;

    if (m_bpersistent_quilt /*&& g_bQuiltEnable*/) {
      int new_dbIndex =
          GetpCurrentStack()->GetDBIndex(current_stack_index + direction);

      if (IsChartQuiltableRef(new_dbIndex)) {
        ToggleCanvasQuiltMode();
        SelectQuiltRefdbChart(new_dbIndex);
        m_bpersistent_quilt = false;
      }
    } else {
      SelectChartFromStack(current_stack_index + direction);
    }
  } else {
    std::vector<int> piano_chart_index_array =
        GetQuiltExtendedStackdbIndexArray();
    int refdb = GetQuiltRefChartdbIndex();

    //      Find the ref chart in the stack
    int current_index = -1;
    for (unsigned int i = 0; i < piano_chart_index_array.size(); i++) {
      if (refdb == piano_chart_index_array[i]) {
        current_index = i;
        break;
      }
    }
    if (current_index == -1) return;

    const ChartTableEntry &ctet = ChartData->GetChartTableEntry(refdb);
    int target_family = ctet.GetChartFamily();

    int new_index = -1;
    int check_index = current_index + direction;
    bool found = false;
    int check_dbIndex = -1;
    int new_dbIndex = -1;

    //      When quilted. switch within the same chart family
    while (!found &&
           (unsigned int)check_index < piano_chart_index_array.size() &&
           (check_index >= 0)) {
      check_dbIndex = piano_chart_index_array[check_index];
      const ChartTableEntry &cte = ChartData->GetChartTableEntry(check_dbIndex);
      if (target_family == cte.GetChartFamily()) {
        found = true;
        new_index = check_index;
        new_dbIndex = check_dbIndex;
        break;
      }

      check_index += direction;
    }

    if (!found) return;

    if (!IsChartQuiltableRef(new_dbIndex)) {
      ToggleCanvasQuiltMode();
      SelectdbChart(new_dbIndex);
      m_bpersistent_quilt = true;
    } else {
      SelectQuiltRefChart(new_index);
    }
  }

  gFrame->UpdateGlobalMenuItems();  // update the state of the menu items
                                    // (checkmarks etc)
  SetQuiltChartHiLiteIndex(-1);

  ReloadVP();
}

//--------------------------------------------------------------------------------------------------------
//
//      Toolbar support
//
//--------------------------------------------------------------------------------------------------------

void ChartCanvas::SetToolbarPosition(wxPoint position) {
  m_toolbarPosition = position;
}

extern bool g_bAllowShowScaled;

void ChartCanvas::SetShowAIS(bool show) {
  m_bShowAIS = show;
}

void ChartCanvas::SetAttenAIS(bool show) {
  m_bShowAISScaled = show;
}

void ChartCanvas::SetAISCanvasDisplayStyle(int StyleIndx) {
  // make some arrays to hold the dfferences between cycle steps
  // show all, scaled, hide all
  bool bShowAIS_Array[3] = {true, true, false};
  bool bShowScaled_Array[3] = {false, true, true};
  wxString ToolShortHelp_Array[3] = {_("Show all AIS Targets"),
                                     _("Attenuate less critical AIS targets"),
                                     _("Hide AIS Targets")};
  wxString iconName_Array[3] = {_T("AIS"), _T("AIS_Suppressed"),
                                _T("AIS_Disabled")};
  int ArraySize = 3;
  int AIS_Toolbar_Switch = 0;
  if (StyleIndx == -1) {  // -1 means coming from toolbar button
    // find current state of switch
    for (int i = 1; i < ArraySize; i++) {
      if ((bShowAIS_Array[i] == m_bShowAIS) &&
          (bShowScaled_Array[i] == m_bShowAISScaled))
        AIS_Toolbar_Switch = i;
    }
    AIS_Toolbar_Switch++;  // we did click so continu with next item
    if ((!g_bAllowShowScaled) && (AIS_Toolbar_Switch == 1))
      AIS_Toolbar_Switch++;

  } else {  // coming from menu bar.
    AIS_Toolbar_Switch = StyleIndx;
  }
  // make sure we are not above array
  if (AIS_Toolbar_Switch >= ArraySize) AIS_Toolbar_Switch = 0;

  int AIS_Toolbar_Switch_Next =
      AIS_Toolbar_Switch + 1;  // Find out what will happen at next click
  if ((!g_bAllowShowScaled) && (AIS_Toolbar_Switch_Next == 1))
    AIS_Toolbar_Switch_Next++;
  if (AIS_Toolbar_Switch_Next >= ArraySize)
    AIS_Toolbar_Switch_Next = 0;  // If at end of cycle start at 0

  // Set found values to global and member variables
  m_bShowAIS = bShowAIS_Array[AIS_Toolbar_Switch];
  m_bShowAISScaled = bShowScaled_Array[AIS_Toolbar_Switch];  
}

//---------------------------------------------------------------------------------
//
//      Compass/GPS status icon support
//
//---------------------------------------------------------------------------------

void ChartCanvas::SelectChartFromStack(int index, bool bDir,
                                       ChartTypeEnum New_Type,
                                       ChartFamilyEnum New_Family) {
  if (!GetpCurrentStack()) return;
  if (!ChartData) return;

  if (index < GetpCurrentStack()->nEntry) {
    //      Open the new chart
    ChartBase *pTentative_Chart;
    pTentative_Chart = ChartData->OpenStackChartConditional(
        GetpCurrentStack(), index, bDir, New_Type, New_Family);

    if (pTentative_Chart) {
      if (m_singleChart) m_singleChart->Deactivate();

      m_singleChart = pTentative_Chart;
      m_singleChart->Activate();

      GetpCurrentStack()->CurrentStackEntry = ChartData->GetStackEntry(
          GetpCurrentStack(), m_singleChart->GetFullPath());
    }

    //      Setup the view
    double zLat, zLon;
    if (m_bFollow) {
      zLat = gLat;
      zLon = gLon;
    } else {
      zLat = m_vLat;
      zLon = m_vLon;
    }

    double best_scale_ppm = GetBestVPScale(m_singleChart);
    double rotation = GetVPRotation();
    double oldskew = GetVPSkew();
    double newskew = m_singleChart->GetChartSkew() * PI / 180.0;

    if (!g_bskew_comp && (GetUpMode() == NORTH_UP_MODE)) {
      if (fabs(oldskew) > 0.0001) rotation = 0.0;
      if (fabs(newskew) > 0.0001) rotation = newskew;
    }

    SetViewPoint(zLat, zLon, best_scale_ppm, newskew, rotation);
  }

  //  refresh Piano
  int idx = GetpCurrentStack()->GetCurrentEntrydbIndex();
  if (idx < 0) return;

  std::vector<int> piano_active_chart_index_array;
  piano_active_chart_index_array.push_back(
      GetpCurrentStack()->GetCurrentEntrydbIndex());
  m_Piano->SetActiveKeyArray(piano_active_chart_index_array);
}

void ChartCanvas::SelectdbChart(int dbindex) {
  if (!GetpCurrentStack()) return;
  if (!ChartData) return;

  if (dbindex >= 0) {
    //      Open the new chart
    ChartBase *pTentative_Chart;
    pTentative_Chart = ChartData->OpenChartFromDB(dbindex, FULL_INIT);

    if (pTentative_Chart) {
      if (m_singleChart) m_singleChart->Deactivate();

      m_singleChart = pTentative_Chart;
      m_singleChart->Activate();

      GetpCurrentStack()->CurrentStackEntry = ChartData->GetStackEntry(
          GetpCurrentStack(), m_singleChart->GetFullPath());
    }

    //      Setup the view
    double zLat, zLon;
    if (m_bFollow) {
      zLat = gLat;
      zLon = gLon;
    } else {
      zLat = m_vLat;
      zLon = m_vLon;
    }

    double best_scale_ppm = GetBestVPScale(m_singleChart);

    if (m_singleChart)
      SetViewPoint(zLat, zLon, best_scale_ppm,
                   m_singleChart->GetChartSkew() * PI / 180., GetVPRotation());

    // SetChartUpdatePeriod( );

    // UpdateGPSCompassStatusBox();           // Pick up the rotation
  }

  // TODO refresh_Piano();
}

void ChartCanvas::selectCanvasChartDisplay(int type, int family) {
  double target_scale = GetVP().view_scale_ppm;

  if (!GetQuiltMode()) {
    if (GetpCurrentStack()) {
      int stack_index = -1;
      for (int i = 0; i < GetpCurrentStack()->nEntry; i++) {
        int check_dbIndex = GetpCurrentStack()->GetDBIndex(i);
        if (check_dbIndex < 0) continue;
        const ChartTableEntry &cte =
            ChartData->GetChartTableEntry(check_dbIndex);
        if (type == cte.GetChartType()) {
          stack_index = i;
          break;
        } else if (family == cte.GetChartFamily()) {
          stack_index = i;
          break;
        }
      }

      if (stack_index >= 0) {
        SelectChartFromStack(stack_index);
      }
    }
  } else {
    int sel_dbIndex = -1;
    std::vector<int> piano_chart_index_array =
        GetQuiltExtendedStackdbIndexArray();
    for (unsigned int i = 0; i < piano_chart_index_array.size(); i++) {
      int check_dbIndex = piano_chart_index_array[i];
      const ChartTableEntry &cte = ChartData->GetChartTableEntry(check_dbIndex);
      if (type == cte.GetChartType()) {
        if (IsChartQuiltableRef(check_dbIndex)) {
          sel_dbIndex = check_dbIndex;
          break;
        }
      } else if (family == cte.GetChartFamily()) {
        if (IsChartQuiltableRef(check_dbIndex)) {
          sel_dbIndex = check_dbIndex;
          break;
        }
      }
    }

    if (sel_dbIndex >= 0) {
      SelectQuiltRefdbChart(sel_dbIndex, false);  // no autoscale
      //  Re-qualify the quilt reference chart selection
      AdjustQuiltRefChart();
    }

    //  Now reset the scale to the target...
    SetVPScale(target_scale);
  }

  SetQuiltChartHiLiteIndex(-1);

  ReloadVP();
}

bool ChartCanvas::IsTileOverlayIndexInYesShow(int index) {
  return std::find(m_tile_yesshow_index_array.begin(),
                   m_tile_yesshow_index_array.end(),
                   index) != m_tile_yesshow_index_array.end();
}

bool ChartCanvas::IsTileOverlayIndexInNoShow(int index) {
  return std::find(m_tile_noshow_index_array.begin(),
                   m_tile_noshow_index_array.end(),
                   index) != m_tile_noshow_index_array.end();
}

void ChartCanvas::AddTileOverlayIndexToNoShow(int index) {
  if (std::find(m_tile_noshow_index_array.begin(),
                m_tile_noshow_index_array.end(),
                index) == m_tile_noshow_index_array.end()) {
    m_tile_noshow_index_array.push_back(index);
  }
}

//-------------------------------------------------------------------------------------------------------
//
//      Piano support
//
//-------------------------------------------------------------------------------------------------------

void ChartCanvas::HandlePianoClick(int selected_index, int selected_dbIndex) {
  if (g_boptionsactive)
    return;  // Piano might be invalid due to chartset updates.
  if (!m_pCurrentStack) return;
  if (!ChartData) return;

  // stop movement or on slow computer we may get something like :
  // zoom out with the wheel (timer is set)
  // quickly click and display a chart, which may zoom in
  // but the delayed timer fires first and it zooms out again!
  //StopMovement();

  if (!GetQuiltMode()) {
    if (m_bpersistent_quilt /* && g_bQuiltEnable*/) {
      if (IsChartQuiltableRef(selected_dbIndex)) {
        ToggleCanvasQuiltMode();
        SelectQuiltRefdbChart(selected_dbIndex);
        m_bpersistent_quilt = false;
      } else {
        SelectChartFromStack(selected_index);
      }
    } else {
      SelectChartFromStack(selected_index);
      g_sticky_chart = selected_dbIndex;
    }

    if (m_singleChart)
      GetVP().SetProjectionType(m_singleChart->GetChartProjectionType());

  } else {
    // Handle MBTiles overlays first
    // Left click simply toggles the noshow array index entry
    if (CHART_TYPE_MBTILES == ChartData->GetDBChartType(selected_dbIndex)) {
      bool bfound = false;
      for (unsigned int i = 0; i < m_tile_noshow_index_array.size(); i++) {
        if (m_tile_noshow_index_array[i] ==
            selected_dbIndex) {  // chart is in the noshow list
          m_tile_noshow_index_array.erase(m_tile_noshow_index_array.begin() +
                                          i);  // erase it
          bfound = true;
          break;
        }
      }
      if (!bfound) {
        m_tile_noshow_index_array.push_back(selected_dbIndex);
      }

      // If not already present, add this tileset to the "yes_show" array.
      if (!IsTileOverlayIndexInYesShow(selected_dbIndex))
        m_tile_yesshow_index_array.push_back(selected_dbIndex);
    }

    else {
      if (IsChartQuiltableRef(selected_dbIndex)) {
        //            if( ChartData ) ChartData->PurgeCache();

        //  If the chart is a vector chart, and of very large scale,
        //  then we had better set the new scale directly to avoid excessive
        //  underzoom on, eg, Inland ENCs
        bool set_scale = false;
        if (CHART_TYPE_S57 == ChartData->GetDBChartType(selected_dbIndex)) {
          if (ChartData->GetDBChartScale(selected_dbIndex) < 5000) {
            set_scale = true;
          }
        }

        if (!set_scale) {
          SelectQuiltRefdbChart(selected_dbIndex, true);  // autoscale
        } else {
          SelectQuiltRefdbChart(selected_dbIndex, false);  // no autoscale

          //  Adjust scale so that the selected chart is underzoomed/overzoomed
          //  by a controlled amount
          ChartBase *pc =
              ChartData->OpenChartFromDB(selected_dbIndex, FULL_INIT);
          if (pc) {
            double proposed_scale_onscreen =
                GetCanvasScaleFactor() / GetVPScale();

            if (g_bPreserveScaleOnX) {
              proposed_scale_onscreen =
                  wxMin(proposed_scale_onscreen,
                        100 * pc->GetNormalScaleMax(GetCanvasScaleFactor(),
                                                    GetCanvasWidth()));
            } else {
              proposed_scale_onscreen =
                  wxMin(proposed_scale_onscreen,
                        20 * pc->GetNormalScaleMax(GetCanvasScaleFactor(),
                                                   GetCanvasWidth()));

              proposed_scale_onscreen =
                  wxMax(proposed_scale_onscreen,
                        pc->GetNormalScaleMin(GetCanvasScaleFactor(),
                                              g_b_overzoom_x));
            }

            SetVPScale(GetCanvasScaleFactor() / proposed_scale_onscreen);
          }
        }
      } else {
        ToggleCanvasQuiltMode();
        SelectdbChart(selected_dbIndex);
        m_bpersistent_quilt = true;
      }
    }
  }

  SetQuiltChartHiLiteIndex(-1);
  gFrame->UpdateGlobalMenuItems();  // update the state of the menu items
                                    // (checkmarks etc)
  HideChartInfoWindow();
  DoCanvasUpdate();
  ReloadVP();  // Pick up the new selections
}

void ChartCanvas::HandlePianoRClick(int x, int y, int selected_index,
                                    int selected_dbIndex) {
  if (g_boptionsactive)
    return;  // Piano might be invalid due to chartset updates.
  if (!GetpCurrentStack()) return;

  PianoPopupMenu(x, y, selected_index, selected_dbIndex);
  UpdateCanvasControlBar();

  SetQuiltChartHiLiteIndex(-1);
}

void ChartCanvas::HandlePianoRollover(int selected_index,
                                      int selected_dbIndex) {
  if (g_boptionsactive)
    return;  // Piano might be invalid due to chartset updates.
  if (!GetpCurrentStack()) return;
  if (!ChartData) return;

  if (ChartData->IsBusy()) return;

  wxPoint key_location = m_Piano->GetKeyOrigin(selected_index);

  if (!GetQuiltMode()) {
    ShowChartInfoWindow(key_location.x, selected_dbIndex);
  } else {
    std::vector<int> piano_chart_index_array =
        GetQuiltExtendedStackdbIndexArray();

    if ((GetpCurrentStack()->nEntry > 1) ||
        (piano_chart_index_array.size() >= 1)) {
      ShowChartInfoWindow(key_location.x, selected_dbIndex);
      SetQuiltChartHiLiteIndex(selected_dbIndex);

      ReloadVP(false);  // no VP adjustment allowed
    } else if (GetpCurrentStack()->nEntry == 1) {
      const ChartTableEntry &cte =
          ChartData->GetChartTableEntry(GetpCurrentStack()->GetDBIndex(0));
      if (CHART_TYPE_CM93COMP != cte.GetChartType()) {
        ShowChartInfoWindow(key_location.x, selected_dbIndex);
        ReloadVP(false);
      } else if ((-1 == selected_index) && (-1 == selected_dbIndex)) {
        ShowChartInfoWindow(key_location.x, selected_dbIndex);
      }
    }
  }
}

void ChartCanvas::UpdateCanvasControlBar(void) {
  if (m_pianoFrozen) return;

  if (!GetpCurrentStack()) return;
  if (!ChartData) return;
  if (!g_bShowChartBar) return;

  int sel_type = -1;
  int sel_family = -1;

  std::vector<int> piano_chart_index_array;
  std::vector<int> empty_piano_chart_index_array;

  wxString old_hash = m_Piano->GetStoredHash();

  if (GetQuiltMode()) {
    piano_chart_index_array = GetQuiltExtendedStackdbIndexArray();
    m_Piano->SetKeyArray(piano_chart_index_array);

    std::vector<int> piano_active_chart_index_array =
        GetQuiltCandidatedbIndexArray();
    m_Piano->SetActiveKeyArray(piano_active_chart_index_array);

    std::vector<int> piano_eclipsed_chart_index_array =
        GetQuiltEclipsedStackdbIndexArray();
    m_Piano->SetEclipsedIndexArray(piano_eclipsed_chart_index_array);

    m_Piano->SetNoshowIndexArray(m_quilt_noshow_index_array);
    m_Piano->AddNoshowIndexArray(m_tile_noshow_index_array);

    sel_type = ChartData->GetDBChartType(GetQuiltReferenceChartIndex());
    sel_family = ChartData->GetDBChartFamily(GetQuiltReferenceChartIndex());
  } else {
    piano_chart_index_array = ChartData->GetCSArray(GetpCurrentStack());
    m_Piano->SetKeyArray(piano_chart_index_array);
    // TODO refresh_Piano();

    if (m_singleChart) {
      sel_type = m_singleChart->GetChartType();
      sel_family = m_singleChart->GetChartFamily();
    }
  }

  //    Set up the TMerc and Skew arrays
  std::vector<int> piano_skew_chart_index_array;
  std::vector<int> piano_tmerc_chart_index_array;
  std::vector<int> piano_poly_chart_index_array;

  for (unsigned int ino = 0; ino < piano_chart_index_array.size(); ino++) {
    const ChartTableEntry &ctei =
        ChartData->GetChartTableEntry(piano_chart_index_array[ino]);
    double skew_norm = ctei.GetChartSkew();
    if (skew_norm > 180.) skew_norm -= 360.;

    if (ctei.GetChartProjectionType() == PROJECTION_TRANSVERSE_MERCATOR)
      piano_tmerc_chart_index_array.push_back(piano_chart_index_array[ino]);

    //    Polyconic skewed charts should show as skewed
    else if (ctei.GetChartProjectionType() == PROJECTION_POLYCONIC) {
      if (fabs(skew_norm) > 1.)
        piano_skew_chart_index_array.push_back(piano_chart_index_array[ino]);
      else
        piano_poly_chart_index_array.push_back(piano_chart_index_array[ino]);
    } else if (fabs(skew_norm) > 1.)
      piano_skew_chart_index_array.push_back(piano_chart_index_array[ino]);
  }
  m_Piano->SetSkewIndexArray(piano_skew_chart_index_array);
  m_Piano->SetTmercIndexArray(piano_tmerc_chart_index_array);
  m_Piano->SetPolyIndexArray(piano_poly_chart_index_array);

  wxString new_hash = m_Piano->GenerateAndStoreNewHash();
  if (new_hash != old_hash) {
    m_Piano->FormatKeys();
    HideChartInfoWindow();
    m_Piano->ResetRollover();
    SetQuiltChartHiLiteIndex(-1);
    m_brepaint_piano = true;
  }

  // Create a bitmask int that describes what Family/Type of charts are shown in
  // the bar, and notify the platform.
  int mask = 0;
  for (unsigned int ino = 0; ino < piano_chart_index_array.size(); ino++) {
    const ChartTableEntry &ctei =
        ChartData->GetChartTableEntry(piano_chart_index_array[ino]);
    ChartFamilyEnum e = (ChartFamilyEnum)ctei.GetChartFamily();
    ChartTypeEnum t = (ChartTypeEnum)ctei.GetChartType();
    if (e == CHART_FAMILY_RASTER) mask |= 1;
    if (e == CHART_FAMILY_VECTOR) {
      if (t == CHART_TYPE_CM93COMP)
        mask |= 4;
      else
        mask |= 2;
    }
  }

  wxString s_indicated;
  if (sel_type == CHART_TYPE_CM93COMP)
    s_indicated = _T("cm93");
  else {
    if (sel_family == CHART_FAMILY_RASTER)
      s_indicated = _T("raster");
    else if (sel_family == CHART_FAMILY_VECTOR)
      s_indicated = _T("vector");
  }

  g_Platform->setChartTypeMaskSel(mask, s_indicated);
}

void ChartCanvas::FormatPianoKeys(void) { m_Piano->FormatKeys(); }

void ChartCanvas::PianoPopupMenu(int x, int y, int selected_index,
                                 int selected_dbIndex) {
  if (!GetpCurrentStack()) return;

  //    No context menu if quilting is disabled
  if (!GetQuiltMode()) return;

  menu_selected_dbIndex = selected_dbIndex;
  menu_selected_index = selected_index;

  m_piano_ctx_menu = new wxMenu();

  //    Search the no-show array
  bool b_is_in_noshow = false;
  for (unsigned int i = 0; i < m_quilt_noshow_index_array.size(); i++) {
    if (m_quilt_noshow_index_array[i] ==
        selected_dbIndex)  // chart is in the noshow list
    {
      b_is_in_noshow = true;
      break;
    }
  }

  if (b_is_in_noshow) {
    m_piano_ctx_menu->Append(ID_PIANO_ENABLE_QUILT_CHART, _("Show This Chart"));
    Connect(ID_PIANO_ENABLE_QUILT_CHART, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ChartCanvas::OnPianoMenuEnableChart));
  } else if (GetpCurrentStack()->nEntry > 1) {
    m_piano_ctx_menu->Append(ID_PIANO_DISABLE_QUILT_CHART,
                             _("Hide This Chart"));
    Connect(ID_PIANO_DISABLE_QUILT_CHART, wxEVT_COMMAND_MENU_SELECTED,
            wxCommandEventHandler(ChartCanvas::OnPianoMenuDisableChart));
  }

  wxPoint pos = wxPoint(x, y - 30);

  //        Invoke the drop-down menu
  if (m_piano_ctx_menu->GetMenuItems().GetCount())
    PopupMenu(m_piano_ctx_menu, pos);

  delete m_piano_ctx_menu;
  m_piano_ctx_menu = NULL;

  HideChartInfoWindow();
  m_Piano->ResetRollover();

  SetQuiltChartHiLiteIndex(-1);

  ReloadVP();
}

void ChartCanvas::OnPianoMenuEnableChart(wxCommandEvent &event) {
  for (unsigned int i = 0; i < m_quilt_noshow_index_array.size(); i++) {
    if (m_quilt_noshow_index_array[i] ==
        menu_selected_dbIndex)  // chart is in the noshow list
    {
      m_quilt_noshow_index_array.erase(m_quilt_noshow_index_array.begin() + i);
      break;
    }
  }
}

void ChartCanvas::OnPianoMenuDisableChart(wxCommandEvent &event) {
  if (!GetpCurrentStack()) return;
  if (!ChartData) return;

  RemoveChartFromQuilt(menu_selected_dbIndex);

  //      It could happen that the chart being disabled is the reference
  //      chart....
  if (menu_selected_dbIndex == GetQuiltRefChartdbIndex()) {
    int type = ChartData->GetDBChartType(menu_selected_dbIndex);

    int i = menu_selected_index + 1;  // select next smaller scale chart
    bool b_success = false;
    while (i < GetpCurrentStack()->nEntry - 1) {
      int dbIndex = GetpCurrentStack()->GetDBIndex(i);
      if (type == ChartData->GetDBChartType(dbIndex)) {
        SelectQuiltRefChart(i);
        b_success = true;
        break;
      }
      i++;
    }

    //    If that did not work, try to select the next larger scale compatible
    //    chart
    if (!b_success) {
      i = menu_selected_index - 1;
      while (i > 0) {
        int dbIndex = GetpCurrentStack()->GetDBIndex(i);
        if (type == ChartData->GetDBChartType(dbIndex)) {
          SelectQuiltRefChart(i);
          b_success = true;
          break;
        }
        i--;
      }
    }
  }
}

void ChartCanvas::RemoveChartFromQuilt(int dbIndex) {
  //    Remove the item from the list (if it appears) to avoid multiple addition
  for (unsigned int i = 0; i < m_quilt_noshow_index_array.size(); i++) {
    if (m_quilt_noshow_index_array[i] ==
        dbIndex)  // chart is already in the noshow list
    {
      m_quilt_noshow_index_array.erase(m_quilt_noshow_index_array.begin() + i);
      break;
    }
  }

  m_quilt_noshow_index_array.push_back(dbIndex);
}

bool ChartCanvas::UpdateS52State() {
  bool retval = false;
  //    printf("    update %d\n", IsPrimaryCanvas());

  if (ps52plib) {
    ps52plib->SetShowS57Text(m_encShowText);
    ps52plib->SetDisplayCategory((DisCat)m_encDisplayCategory);
    ps52plib->m_bShowSoundg = m_encShowDepth;
    ps52plib->m_bShowAtonText = m_encShowBuoyLabels;
    ps52plib->m_bShowLdisText = m_encShowLightDesc;

    // Lights
    if (!m_encShowLights)  // On, going off
      ps52plib->AddObjNoshow("LIGHTS");
    else  // Off, going on
      ps52plib->RemoveObjNoshow("LIGHTS");
    ps52plib->SetLightsOff(!m_encShowLights);
    ps52plib->m_bExtendLightSectors = true;

    // TODO ps52plib->m_bShowAtons = m_encShowBuoys;
    ps52plib->SetAnchorOn(m_encShowAnchor);
    ps52plib->SetQualityOfData(m_encShowDataQual);
  }

  return retval;
}

void ChartCanvas::SetShowENCDataQual(bool show) {
  m_encShowDataQual = show;
  m_s52StateHash = 0;  // Force a S52 PLIB re-configure
}

void ChartCanvas::SetShowENCText(bool show) {
  m_encShowText = show;
  m_s52StateHash = 0;  // Force a S52 PLIB re-configure
}

void ChartCanvas::SetENCDisplayCategory(int category) {
  m_encDisplayCategory = category;
  m_s52StateHash = 0;  // Force a S52 PLIB re-configure
}

void ChartCanvas::SetShowENCDepth(bool show) {
  m_encShowDepth = show;
  m_s52StateHash = 0;  // Force a S52 PLIB re-configure
}

void ChartCanvas::SetShowENCLightDesc(bool show) {
  m_encShowLightDesc = show;
  m_s52StateHash = 0;  // Force a S52 PLIB re-configure
}

void ChartCanvas::SetShowENCBuoyLabels(bool show) {
  m_encShowBuoyLabels = show;
  m_s52StateHash = 0;  // Force a S52 PLIB re-configure
}

void ChartCanvas::SetShowENCLights(bool show) {
  m_encShowLights = show;
  m_s52StateHash = 0;  // Force a S52 PLIB re-configure
}

void ChartCanvas::SetShowENCAnchor(bool show) {
  m_encShowAnchor = show;
  m_s52StateHash = 0;  // Force a S52 PLIB re-configure
}

void ChartCanvas::RenderAlertMessage(wxDC &dc, const ViewPort &vp) {
  if (!GetAlertString().IsEmpty()) {
    wxFont *pfont = wxTheFontList->FindOrCreateFont(
        10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    dc.SetFont(*pfont);
    dc.SetPen(*wxTRANSPARENT_PEN);

    dc.SetBrush(wxColour(243, 229, 47));
    int w, h;
    dc.GetMultiLineTextExtent(GetAlertString(), &w, &h);
    h += 2;
    // int yp = vp.pix_height - 20 - h;

    wxRect sbr = GetScaleBarRect();
    int xp = sbr.x + sbr.width + 10;
    int yp = (sbr.y + sbr.height) - h;

    int wdraw = w + 10;
    dc.DrawRectangle(xp, yp, wdraw, h);
    dc.DrawLabel(GetAlertString(), wxRect(xp, yp, wdraw, h),
                 wxALIGN_CENTRE_HORIZONTAL | wxALIGN_CENTRE_VERTICAL);
  }
}

bool ChartCanvas::HasLayer(std::vector<int>& vnLayers, int nLayerIndex) {
	int nLayerCnt = vnLayers.size();

	bool bFoundFlag = false;
	for (int i = 0; i < nLayerCnt; i++) {
		if (vnLayers[i] == nLayerIndex) {
			bFoundFlag = true;
			break;
		}
	}

	return bFoundFlag;
}

//--------------------------------------------------------------------------------------------------------
//    Screen Brightness Control Support Routines
//
//--------------------------------------------------------------------------------------------------------

#ifdef __UNIX__
#define BRIGHT_XCALIB
#define __OPCPN_USEICC__
#endif

#ifdef __OPCPN_USEICC__
int CreateSimpleICCProfileFile(const char *file_name, double co_red,
                               double co_green, double co_blue);

wxString temp_file_name;
#endif

#if 0
class ocpnCurtain: public wxDialog
{
    DECLARE_CLASS( ocpnCurtain )
    DECLARE_EVENT_TABLE()

public:
    ocpnCurtain( wxWindow *parent, wxPoint position, wxSize size, long wstyle );
    ~ocpnCurtain( );
    bool ProcessEvent(wxEvent& event);

};

IMPLEMENT_CLASS ( ocpnCurtain, wxDialog )

BEGIN_EVENT_TABLE(ocpnCurtain, wxDialog)
END_EVENT_TABLE()

ocpnCurtain::ocpnCurtain( wxWindow *parent, wxPoint position, wxSize size, long wstyle )
{
    wxDialog::Create( parent, -1, _T("ocpnCurtain"), position, size, wxNO_BORDER | wxSTAY_ON_TOP );
}

ocpnCurtain::~ocpnCurtain()
{
}

bool ocpnCurtain::ProcessEvent(wxEvent& event)
{
    GetParent()->GetEventHandler()->SetEvtHandlerEnabled(true);
    return GetParent()->GetEventHandler()->ProcessEvent(event);
}
#endif

#ifdef _WIN32
#include <windows.h>

HMODULE hGDI32DLL;
typedef BOOL(WINAPI *SetDeviceGammaRamp_ptr_type)(HDC hDC, LPVOID lpRampTable);
typedef BOOL(WINAPI *GetDeviceGammaRamp_ptr_type)(HDC hDC, LPVOID lpRampTable);
SetDeviceGammaRamp_ptr_type
    g_pSetDeviceGammaRamp;  // the API entry points in the dll
GetDeviceGammaRamp_ptr_type g_pGetDeviceGammaRamp;

WORD *g_pSavedGammaMap;

#endif

int InitScreenBrightness(void) {
#ifdef _WIN32
  if (gFrame->GetPrimaryCanvas()->GetglCanvas() && g_bopengl) {
    HDC hDC;
    BOOL bbr;

    if (NULL == hGDI32DLL) {
      hGDI32DLL = LoadLibrary(TEXT("gdi32.dll"));

      if (NULL != hGDI32DLL) {
        // Get the entry points of the required functions
        g_pSetDeviceGammaRamp = (SetDeviceGammaRamp_ptr_type)GetProcAddress(
            hGDI32DLL, "SetDeviceGammaRamp");
        g_pGetDeviceGammaRamp = (GetDeviceGammaRamp_ptr_type)GetProcAddress(
            hGDI32DLL, "GetDeviceGammaRamp");

        //    If the functions are not found, unload the DLL and return false
        if ((NULL == g_pSetDeviceGammaRamp) ||
            (NULL == g_pGetDeviceGammaRamp)) {
          FreeLibrary(hGDI32DLL);
          hGDI32DLL = NULL;
          return 0;
        }
      }
    }

    //    Interface is ready, so....
    //    Get some storage
    if (!g_pSavedGammaMap) {
      g_pSavedGammaMap = (WORD *)malloc(3 * 256 * sizeof(WORD));

      hDC = GetDC(NULL);  // Get the full screen DC
      bbr = g_pGetDeviceGammaRamp(
          hDC, g_pSavedGammaMap);  // Get the existing ramp table
      ReleaseDC(NULL, hDC);        // Release the DC
    }

    //    On Windows hosts, try to adjust the registry to allow full range
    //    setting of Gamma table This is an undocumented Windows hack.....
    wxRegKey *pRegKey = new wxRegKey(
        _T("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows ")
        _T("NT\\CurrentVersion\\ICM"));
    if (!pRegKey->Exists()) pRegKey->Create();
    pRegKey->SetValue(_T("GdiIcmGammaRange"), 256);

    g_brightness_init = true;
    return 1;
  }

  else {
    if (NULL == g_pcurtain) {
      if (gFrame->CanSetTransparent()) {
        //    Build the curtain window
        g_pcurtain = new wxDialog(gFrame->GetPrimaryCanvas(), -1, _T(""),
                                  wxPoint(0, 0), ::wxGetDisplaySize(),
                                  wxNO_BORDER | wxTRANSPARENT_WINDOW |
                                      wxSTAY_ON_TOP | wxDIALOG_NO_PARENT);

        //                  g_pcurtain = new ocpnCurtain(gFrame,
        //                  wxPoint(0,0),::wxGetDisplaySize(),
        //                      wxNO_BORDER | wxTRANSPARENT_WINDOW
        //                      |wxSTAY_ON_TOP | wxDIALOG_NO_PARENT);

        g_pcurtain->Hide();

        HWND hWnd = GetHwndOf(g_pcurtain);
        SetWindowLong(hWnd, GWL_EXSTYLE,
                      GetWindowLong(hWnd, GWL_EXSTYLE) | ~WS_EX_APPWINDOW);
        g_pcurtain->SetBackgroundColour(wxColour(0, 0, 0));
        g_pcurtain->SetTransparent(0);

        g_pcurtain->Maximize();
        g_pcurtain->Show();

        //    All of this is obtuse, but necessary for Windows...
        g_pcurtain->Enable();
        g_pcurtain->Disable();

        gFrame->Disable();
        gFrame->Enable();
        // SetFocus();
      }
    }
    g_brightness_init = true;

    return 1;
  }
#else
  //    Look for "xcalib" application
  wxString cmd(_T ( "xcalib -version" ));

  wxArrayString output;
  long r = wxExecute(cmd, output);
  if (0 != r)
    wxLogMessage(
        _T("   External application \"xcalib\" not found. Screen brightness ")
        _T("not changed."));

  g_brightness_init = true;
  return 0;
#endif
}

int RestoreScreenBrightness(void) {
#ifdef _WIN32

  if (g_pSavedGammaMap) {
    HDC hDC = GetDC(NULL);  // Get the full screen DC
    g_pSetDeviceGammaRamp(hDC,
                          g_pSavedGammaMap);  // Restore the saved ramp table
    ReleaseDC(NULL, hDC);                     // Release the DC

    free(g_pSavedGammaMap);
    g_pSavedGammaMap = NULL;
  }

  if (g_pcurtain) {
    g_pcurtain->Close();
    g_pcurtain->Destroy();
    g_pcurtain = NULL;
  }

  g_brightness_init = false;
  return 1;

#endif

#ifdef BRIGHT_XCALIB
  if (g_brightness_init) {
    wxString cmd;
    cmd = _T("xcalib -clear");
    wxExecute(cmd, wxEXEC_ASYNC);
    g_brightness_init = false;
  }

  return 1;
#endif

  return 0;
}

//    Set brightness. [0..100]
int SetScreenBrightness(int brightness) {
#ifdef _WIN32

  //    Under Windows, we use the SetDeviceGammaRamp function which exists in
  //    some (most modern?) versions of gdi32.dll Load the required library dll,
  //    if not already in place
  if (gFrame->GetPrimaryCanvas()->GetglCanvas() && g_bopengl) {
    if (g_pcurtain) {
      g_pcurtain->Close();
      g_pcurtain->Destroy();
      g_pcurtain = NULL;
    }

    InitScreenBrightness();

    if (NULL == hGDI32DLL) {
      // Unicode stuff.....
      wchar_t wdll_name[80];
      MultiByteToWideChar(0, 0, "gdi32.dll", -1, wdll_name, 80);
      LPCWSTR cstr = wdll_name;

      hGDI32DLL = LoadLibrary(cstr);

      if (NULL != hGDI32DLL) {
        // Get the entry points of the required functions
        g_pSetDeviceGammaRamp = (SetDeviceGammaRamp_ptr_type)GetProcAddress(
            hGDI32DLL, "SetDeviceGammaRamp");
        g_pGetDeviceGammaRamp = (GetDeviceGammaRamp_ptr_type)GetProcAddress(
            hGDI32DLL, "GetDeviceGammaRamp");

        //    If the functions are not found, unload the DLL and return false
        if ((NULL == g_pSetDeviceGammaRamp) ||
            (NULL == g_pGetDeviceGammaRamp)) {
          FreeLibrary(hGDI32DLL);
          hGDI32DLL = NULL;
          return 0;
        }
      }
    }

    HDC hDC = GetDC(NULL);  // Get the full screen DC

    /*
     int cmcap = GetDeviceCaps(hDC, COLORMGMTCAPS);
     if (cmcap != CM_GAMMA_RAMP)
     {
     wxLogMessage(_T("    Video hardware does not support brightness control by
     gamma ramp adjustment.")); return false;
     }
     */

    int increment = brightness * 256 / 100;

    // Build the Gamma Ramp table
    WORD GammaTable[3][256];

    int table_val = 0;
    for (int i = 0; i < 256; i++) {
      GammaTable[0][i] = r_gamma_mult * (WORD)table_val;
      GammaTable[1][i] = g_gamma_mult * (WORD)table_val;
      GammaTable[2][i] = b_gamma_mult * (WORD)table_val;

      table_val += increment;

      if (table_val > 65535) table_val = 65535;
    }

    g_pSetDeviceGammaRamp(hDC, GammaTable);  // Set the ramp table
    ReleaseDC(NULL, hDC);                    // Release the DC

    return 1;
  } else {
    if (g_pSavedGammaMap) {
      HDC hDC = GetDC(NULL);  // Get the full screen DC
      g_pSetDeviceGammaRamp(hDC,
                            g_pSavedGammaMap);  // Restore the saved ramp table
      ReleaseDC(NULL, hDC);                     // Release the DC
    }

    if (brightness < 100) {
      if (NULL == g_pcurtain) InitScreenBrightness();

      if (g_pcurtain) {
        int sbrite = wxMax(1, brightness);
        sbrite = wxMin(100, sbrite);

        g_pcurtain->SetTransparent((100 - sbrite) * 256 / 100);
      }
    } else {
      if (g_pcurtain) {
        g_pcurtain->Close();
        g_pcurtain->Destroy();
        g_pcurtain = NULL;
      }
    }

    return 1;
  }

#endif

#ifdef BRIGHT_XCALIB

  if (!g_brightness_init) {
    last_brightness = 100;
    g_brightness_init = true;
    temp_file_name = wxFileName::CreateTempFileName(_T(""));
    InitScreenBrightness();
  }

#ifdef __OPCPN_USEICC__
  //  Create a dead simple temporary ICC profile file, with gamma ramps set as
  //  desired, and then activate this temporary profile using xcalib <filename>
  if (!CreateSimpleICCProfileFile(
          (const char *)temp_file_name.fn_str(), brightness * r_gamma_mult,
          brightness * g_gamma_mult, brightness * b_gamma_mult)) {
    wxString cmd(_T ( "xcalib " ));
    cmd += temp_file_name;

    wxExecute(cmd, wxEXEC_ASYNC);
  }

#else
  //    Or, use "xcalib -co" to set overall contrast value
  //    This is not as nice, since the -co parameter wants to be a fraction of
  //    the current contrast, and values greater than 100 are not allowed.  As a
  //    result, increases of contrast must do a "-clear" step first, which
  //    produces objectionable flashing.
  if (brightness > last_brightness) {
    wxString cmd;
    cmd = _T("xcalib -clear");
    wxExecute(cmd, wxEXEC_ASYNC);

    ::wxMilliSleep(10);

    int brite_adj = wxMax(1, brightness);
    cmd.Printf(_T("xcalib -co %2d -a"), brite_adj);
    wxExecute(cmd, wxEXEC_ASYNC);
  } else {
    int brite_adj = wxMax(1, brightness);
    int factor = (brite_adj * 100) / last_brightness;
    factor = wxMax(1, factor);
    wxString cmd;
    cmd.Printf(_T("xcalib -co %2d -a"), factor);
    wxExecute(cmd, wxEXEC_ASYNC);
  }

#endif

  last_brightness = brightness;

#endif

  return 0;
}

#ifdef __OPCPN_USEICC__

#define MLUT_TAG 0x6d4c5554L
#define VCGT_TAG 0x76636774L

int GetIntEndian(unsigned char *s) {
  int ret;
  unsigned char *p;
  int i;

  p = (unsigned char *)&ret;

  if (1)
    for (i = sizeof(int) - 1; i > -1; --i) *p++ = s[i];
  else
    for (i = 0; i < (int)sizeof(int); ++i) *p++ = s[i];

  return ret;
}

unsigned short GetShortEndian(unsigned char *s) {
  unsigned short ret;
  unsigned char *p;
  int i;

  p = (unsigned char *)&ret;

  if (1)
    for (i = sizeof(unsigned short) - 1; i > -1; --i) *p++ = s[i];
  else
    for (i = 0; i < (int)sizeof(unsigned short); ++i) *p++ = s[i];

  return ret;
}

//    Create a very simple Gamma correction file readable by xcalib
int CreateSimpleICCProfileFile(const char *file_name, double co_red,
                               double co_green, double co_blue) {
  FILE *fp;

  if (file_name) {
    fp = fopen(file_name, "wb");
    if (!fp) return -1; /* file can not be created */
  } else
    return -1; /* filename char pointer not valid */

  //    Write header
  char header[128];
  for (int i = 0; i < 128; i++) header[i] = 0;

  fwrite(header, 128, 1, fp);

  //    Num tags
  int numTags0 = 1;
  int numTags = GetIntEndian((unsigned char *)&numTags0);
  fwrite(&numTags, 1, 4, fp);

  int tagName0 = VCGT_TAG;
  int tagName = GetIntEndian((unsigned char *)&tagName0);
  fwrite(&tagName, 1, 4, fp);

  int tagOffset0 = 128 + 4 * sizeof(int);
  int tagOffset = GetIntEndian((unsigned char *)&tagOffset0);
  fwrite(&tagOffset, 1, 4, fp);

  int tagSize0 = 1;
  int tagSize = GetIntEndian((unsigned char *)&tagSize0);
  fwrite(&tagSize, 1, 4, fp);

  fwrite(&tagName, 1, 4, fp);  // another copy of tag

  fwrite(&tagName, 1, 4, fp);  // dummy

  //  Table type

  /* VideoCardGammaTable (The simplest type) */
  int gammatype0 = 0;
  int gammatype = GetIntEndian((unsigned char *)&gammatype0);
  fwrite(&gammatype, 1, 4, fp);

  int numChannels0 = 3;
  unsigned short numChannels = GetShortEndian((unsigned char *)&numChannels0);
  fwrite(&numChannels, 1, 2, fp);

  int numEntries0 = 256;
  unsigned short numEntries = GetShortEndian((unsigned char *)&numEntries0);
  fwrite(&numEntries, 1, 2, fp);

  int entrySize0 = 1;
  unsigned short entrySize = GetShortEndian((unsigned char *)&entrySize0);
  fwrite(&entrySize, 1, 2, fp);

  unsigned char ramp[256];

  //    Red ramp
  for (int i = 0; i < 256; i++) ramp[i] = i * co_red / 100.;
  fwrite(ramp, 256, 1, fp);

  //    Green ramp
  for (int i = 0; i < 256; i++) ramp[i] = i * co_green / 100.;
  fwrite(ramp, 256, 1, fp);

  //    Blue ramp
  for (int i = 0; i < 256; i++) ramp[i] = i * co_blue / 100.;
  fwrite(ramp, 256, 1, fp);

  fclose(fp);

  return 0;
}
#endif  // __OPCPN_USEICC__
