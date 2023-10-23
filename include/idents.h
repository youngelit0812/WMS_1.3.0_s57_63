/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:
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

#ifndef _IDENTS_H__
#define _IDENTS_H__

#include <wx/event.h>
#include <wx/toolbar.h>

#include "gui_lib.h"
#include "ocpn_print.h"

#define TIMER_GFRAME_1 999

#define ID_QUIT 101
#define ID_CM93ZOOMG 102

//    ToolBar Constants
const int ID_TOOLBAR = 500;

enum {
  // The following constants represent the toolbar items (some are also used in
  // menus). They MUST be in the SAME ORDER as on the toolbar and new items MUST
  // NOT be added amongst them, due to the way the toolbar button visibility is
  // saved and calculated.
  ID_ZOOMIN = 1550,
  ID_ZOOMOUT,
  ID_STKUP,
  ID_STKDN,
  ID_ROUTE,
  ID_FOLLOW,
  ID_SETTINGS,
  ID_AIS,
  ID_ENC_TEXT,
  ID_CURRENT,
  ID_TIDE,
  ID_PRINT,
  ID_ROUTEMANAGER,
  ID_TRACK,
  ID_COLSCHEME,
  ID_ABOUT,
  ID_MOB,
  ID_TBEXIT,
  ID_TBSTAT,
  ID_TBSTATBOX,
  ID_MASTERTOGGLE,

  ID_PLUGIN_BASE  // This MUST be the last item in the enum
};

static const long TOOLBAR_STYLE = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT;

enum {
  IDM_TOOLBAR_TOGGLETOOLBARSIZE = 200,
  IDM_TOOLBAR_TOGGLETOOLBARORIENT,
  IDM_TOOLBAR_TOGGLETOOLBARROWS,
  IDM_TOOLBAR_ENABLEPRINT,
  IDM_TOOLBAR_DELETEPRINT,
  IDM_TOOLBAR_INSERTPRINT,
  IDM_TOOLBAR_TOGGLEHELP,
  IDM_TOOLBAR_TOGGLE_TOOLBAR,
  IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR,
  IDM_TOOLBAR_CHANGE_TOOLTIP,
  IDM_TOOLBAR_SHOW_TEXT,
  IDM_TOOLBAR_SHOW_ICONS,
  IDM_TOOLBAR_SHOW_BOTH,

  ID_COMBO = 1000
};

// Menu item IDs for the main menu bar
enum {
  ID_MENU_ZOOM_IN = 2000,
  ID_MENU_ZOOM_OUT,
  ID_MENU_SCALE_IN,
  ID_MENU_SCALE_OUT,

  ID_MENU_NAV_FOLLOW,
  ID_MENU_NAV_TRACK,

  ID_MENU_CHART_NORTHUP,
  ID_MENU_CHART_COGUP,
  ID_MENU_CHART_HEADUP,
  ID_MENU_CHART_QUILTING,
  ID_MENU_CHART_OUTLINES,

  ID_MENU_UI_CHARTBAR,
  ID_MENU_UI_COLSCHEME,
  ID_MENU_UI_FULLSCREEN,

  ID_MENU_ENC_TEXT,
  ID_MENU_ENC_LIGHTS,
  ID_MENU_ENC_SOUNDINGS,
  ID_MENU_ENC_ANCHOR,
  ID_MENU_ENC_DATA_QUALITY,

  ID_MENU_SHOW_TIDES,
  ID_MENU_SHOW_CURRENTS,

  ID_MENU_TOOL_MEASURE,
  ID_MENU_ROUTE_MANAGER,
  ID_MENU_ROUTE_NEW,
  ID_MENU_MARK_BOAT,
  ID_MENU_MARK_CURSOR,
  ID_MENU_MARK_MOB,

  ID_MENU_AIS_TARGETS,
  ID_MENU_AIS_MOORED_TARGETS,
  ID_MENU_AIS_SCALED_TARGETS,
  ID_MENU_AIS_TRACKS,
  ID_MENU_AIS_CPADIALOG,
  ID_MENU_AIS_CPASOUND,
  ID_MENU_AIS_TARGETLIST,
  ID_MENU_AIS_CPAWARNING,

  ID_MENU_SETTINGS_BASIC,

  ID_MENU_OQUIT,

  ID_CMD_SELECT_CHART_TYPE,
  ID_CMD_SELECT_CHART_FAMILY,
  ID_CMD_INVALIDATE,
  ID_CMD_CLOSE_ALL_DIALOGS,

  ID_MENU_SHOW_NAVOBJECTS,
};

//      A global definition for window, timer and other ID's as needed.
enum {
  ID_NMEA_WINDOW = wxID_HIGHEST,
  ID_AIS_WINDOW,
  INIT_TIMER,
  FRAME_TIMER_1,
  FRAME_TIMER_2,
  TIMER_AIS1,
  TIMER_DSC,
  TIMER_AISAUDIO,
  AIS_SOCKET_Isa,
  FRAME_TIMER_DOG,
  FRAME_TC_TIMER,
  FRAME_COG_TIMER,
  MEMORY_FOOTPRINT_TIMER,
  BELLS_TIMER,
  ID_NMEA_THREADMSG,
  RESIZE_TIMER,
  TOOLBAR_ANIMATE_TIMER,
  RECAPTURE_TIMER,
  WATCHDOG_TIMER

};


enum { TIME_TYPE_UTC = 1, TIME_TYPE_LMT, TIME_TYPE_COMPUTER };

#define DS_SOCKET_ID             5001
#define DS_SERVERSOCKET_ID       5002
#define DS_ACTIVESERVERSOCKET_ID 5003


#if 0
//      Command identifiers for wxCommandEvents coming from the outside world.
//      Removed from enum to facilitate constant definition
#define ID_CMD_APPLY_SETTINGS 300
#define ID_CMD_NULL_REFRESH 301
#define ID_CMD_TRIGGER_RESIZE 302
#define ID_CMD_SETVP 303
#define ID_CMD_POST_JSON_TO_PLUGINS 304
#define ID_CMD_SET_LOCALE 305
#define ID_CMD_SOUND_FINISHED 306

#define N_STATUS_BAR_FIELDS_MAX 20

#define STAT_FIELD_COUNT 5
#define STAT_FIELD_TICK 0
#define STAT_FIELD_SOGCOG 1
#define STAT_FIELD_CURSOR_LL 2
#define STAT_FIELD_CURSOR_BRGRNG 3
#define STAT_FIELD_SCALE 4

//      Define a constant GPS signal watchdog timeout value
#define GPS_TIMEOUT_SECONDS 6

//    Define a timer value for Tide/Current updates
//    Note that the underlying data algorithms produce fresh data only every 15
//    minutes So maybe 5 minute updates should provide sufficient oversampling
#define TIMER_TC_VALUE_SECONDS 300

#define MAX_COG_AVERAGE_SECONDS 60
#define MAX_COGSOG_FILTER_SECONDS 60
//----------------------------------------------------------------------------
// fwd class declarations
//----------------------------------------------------------------------------
class ChartBase;
class wxSocketEvent;
class ocpnToolBarSimple;
class OCPN_DataStreamEvent;
class OCPN_SignalKEvent;
class DataStream;
class AisTargetData;

bool isSingleChart(ChartBase *chart);

#if 0
//      A class to contain NMEA messages, their receipt time, and their source
//      priority
class NMEA_Msg_Container {
public:
  wxDateTime receipt_time;
  int current_priority;
  wxString stream_name;
};
#endif

class OCPN_ThreadMessageEvent : public wxEvent {
public:
  OCPN_ThreadMessageEvent(wxEventType commandType = wxEVT_NULL, int id = 0);
  ~OCPN_ThreadMessageEvent();

  // accessors
  void SetSString(std::string string) { m_string = string; }
  std::string GetSString() { return m_string; }

  // required for sending with wxPostEvent()
  wxEvent *Clone() const;

private:
  std::string m_string;
};

class MyApp : public wxApp {
public:
  bool OnInit();
  int OnExit();
  void OnInitCmdLine(wxCmdLineParser &parser);
  bool OnCmdLineParsed(wxCmdLineParser &parser);
  void OnActivateApp(wxActivateEvent &event);

#ifdef LINUX_CRASHRPT
  //! fatal exeption handling
  void OnFatalException();
#endif

#ifdef __WXMSW__
  //  Catch malloc/new fail exceptions
  //  All the rest will be caught be CrashRpt
  bool OnExceptionInMainLoop();
#endif

  wxSingleInstanceChecker *m_checker;

  DECLARE_EVENT_TABLE()
};

#if 0
class MyFrame : public wxFrame {
  friend class SignalKEventHandler;

public:
  MyFrame(wxFrame *frame, const wxString &title, const wxPoint &pos,
          const wxSize &size, long style);

  ~MyFrame();

  int GetApplicationMemoryUse(void);

  void OnMaximize(wxMaximizeEvent &event);
  void OnCloseWindow(wxCloseEvent &event);
  void OnExit(wxCommandEvent &event);
  void OnSize(wxSizeEvent &event);
  void OnMove(wxMoveEvent &event);  
  void OnFrameTimer1(wxTimerEvent &event);
  bool DoChartUpdate(void);
  void OnEvtTHREADMSG(OCPN_ThreadMessageEvent &event);
  void OnEvtOCPN_NMEA(OCPN_DataStreamEvent &event);
  void OnEvtOCPN_SignalK(OCPN_SignalKEvent &event);
  void OnEvtOCPN_SIGNALK_Test(OCPN_SignalKEvent &event);
  void OnEvtPlugInMessage(OCPN_MsgEvent &event);
  void OnMemFootTimer(wxTimerEvent &event);  
  void OnSENCEvtThread(OCPN_BUILDSENC_ThreadEvent &event);  
  void OnBellsFinished(wxCommandEvent &event);

#ifdef wxHAS_POWER_EVENTS
  void OnSuspending(wxPowerEvent &event);
  void OnSuspended(wxPowerEvent &event);
  void OnSuspendCancel(wxPowerEvent &event);
  void OnResume(wxPowerEvent &event);
#endif  // wxHAS_POWER_EVENTS

  void RefreshCanvasOther(ChartCanvas *ccThis);
  void UpdateAllFonts(void);
  void PositionConsole(void);  
  void DoStackUp(ChartCanvas *cc);
  void DoStackDown(ChartCanvas *cc);
  void selectChartDisplay(int type, int family);
  void applySettingsString(wxString settings);
  void setStringVP(wxString VPS);
  void InvalidateAllGL();
  void RefreshAllCanvas(bool bErase = true);
  void InvalidateAllQuilts();

  void SetUpMode(ChartCanvas *cc, int mode);

  ChartCanvas *GetPrimaryCanvas();
  ChartCanvas *GetFocusCanvas();

  void DoStackDelta(ChartCanvas *cc, int direction);  
  void SwitchKBFocus(ChartCanvas *pCanvas);
  ChartCanvas *GetCanvasUnderMouse();
  int GetCanvasIndexUnderMouse();

  bool DropMarker(bool atOwnShip = true);

  bool SetGlobalToolbarViz(bool viz);

  void MouseEvent(wxMouseEvent &event);
  //     void SelectChartFromStack(int index,  bool bDir = false,  ChartTypeEnum
  //     New_Type = CHART_TYPE_DONTCARE, ChartFamilyEnum New_Family =
  //     CHART_FAMILY_DONTCARE); void SelectdbChart(int dbindex); void
  //     SelectQuiltRefChart(int selected_index); void SelectQuiltRefdbChart(int
  //     db_index, bool b_autoscale = true);
  void JumpToPosition(ChartCanvas *cc, double lat, double lon, double scale);

  void ProcessCanvasResize(void);

  void ApplyGlobalSettings(bool bnewtoolbar);
  int DoOptionsDialog();
  bool ProcessOptionsDialog(int resultFlags, ArrayOfCDI *pNewDirArray);
  void DoPrint(void);
  void StopSockets(void);
  void ResumeSockets(void);
  void ToggleDataQuality(ChartCanvas *cc);
  void TogglebFollow(ChartCanvas *cc);
  void ToggleFullScreen();
  void ToggleChartBar(ChartCanvas *cc);
  void SetbFollow(ChartCanvas *cc);
  void ClearbFollow(ChartCanvas *cc);
  void ToggleChartOutlines(ChartCanvas *cc);
  void ToggleENCText(ChartCanvas *cc);
  void ToggleSoundings(ChartCanvas *cc);
  void ToggleRocks(void);
  bool ToggleLights(ChartCanvas *cc);
  void ToggleAnchor(ChartCanvas *cc);
  void ToggleAISDisplay(ChartCanvas *cc);
  void ToggleAISMinimizeTargets(ChartCanvas *cc);

  void ToggleTestPause(void);
  void SetENCDisplayCategory(ChartCanvas *cc, enum _DisCat nset);
  void ToggleNavobjects(ChartCanvas *cc);

  bool ShouldRestartTrack();
  void ToggleColorScheme();
  void SetMasterToolbarItemState(int tool_id, bool state);

  void SetToolbarItemBitmaps(int tool_id, wxBitmap *bitmap,
                             wxBitmap *bmpDisabled);
  void SetToolbarItemSVG(int tool_id, wxString normalSVGfile,
                         wxString rolloverSVGfile, wxString toggledSVGfile);
  void ToggleQuiltMode(ChartCanvas *cc);
  void UpdateControlBar(ChartCanvas *cc);
  
  void RefreshGroupIndices(void);

  double GetBestVPScale(ChartBase *pchart);

  ColorScheme GetColorScheme();
  void SetAndApplyColorScheme(ColorScheme cs);

  void OnFrameTCTimer(wxTimerEvent &event);
  void OnFrameCOGTimer(wxTimerEvent &event);

  void ChartsRefresh();

  bool CheckGroup(int igroup);
  double GetMag(double a);
  double GetMag(double a, double lat, double lon);
  bool SendJSON_WMM_Var_Request(double lat, double lon, wxDateTime date);
  
  int nBlinkerTick;
  bool m_bTimeIsSet;
  bool m_bDateIsSet;

  wxTimer InitTimer;
  int m_iInitCount;
  bool m_initializing;

  wxTimer FrameTCTimer;
  wxTimer FrameTimer1;
  wxTimer FrameCOGTimer;
  wxTimer MemFootTimer;
  wxTimer m_resizeTimer;

  int m_BellsToPlay;
  wxTimer BellsTimer;

  //      PlugIn support
  int GetNextToolbarToolId() { return m_next_available_plugin_tool_id; }
  void RequestNewToolbarArgEvent(wxCommandEvent &WXUNUSED(event)) {
    return RequestNewMasterToolbar();
  }
  void RequestNewToolbars(bool bforcenew = false);
  void UpdateRotationState(double rotation);
  
  bool m_bdefer_resize;
  wxSize m_defer_size;
  wxSize m_newsize;
  double COGTable[MAX_COG_AVERAGE_SECONDS];

  void FastClose();
  void SetChartUpdatePeriod();
  void CreateCanvasLayout(bool b_useStoredSize = false);
  void LoadHarmonics();
  void ReloadAllVP();
  void SetCanvasSizes(wxSize frameSize);

  ocpnToolBarSimple *CreateMasterToolbar();
  void RequestNewMasterToolbar(bool bforcenew = true);
  bool CheckAndAddPlugInTool();
  bool AddDefaultPositionPlugInTools();

  void NotifyChildrenResize(void);
  void UpdateCanvasConfigDescriptors();
  void ScheduleSettingsDialog();
  static void RebuildChartDatabase();

private:
  void CheckToolbarPosition();
  void ODoSetSize(void);
  void DoCOGSet(void);

  void FilterCogSog(void);

  bool ScrubGroupArray();
  wxString GetGroupName(int igroup);

  void SetAISDisplayStyle(ChartCanvas *cc, int StyleIndx);

  bool GetMasterToolItemShow(int toolid);
  void OnToolbarAnimateTimer(wxTimerEvent &event);
  bool CollapseGlobalToolbar();

  NMEA0183 m_NMEA0183;  // Used to parse messages from NMEA threads

  wxDateTime m_MMEAeventTime;
  unsigned long m_ulLastNMEATicktime;

  wxMutex m_mutexNMEAEvent;  // Mutex to handle static data from NMEA threads

  wxString m_last_reported_chart_name;
  wxString m_last_reported_chart_pubdate;

  wxString m_lastAISiconName;

  //      Plugin Support
  int m_next_available_plugin_tool_id;

  double COGFilterTable[MAX_COGSOG_FILTER_SECONDS];
  double SOGFilterTable[MAX_COGSOG_FILTER_SECONDS];

  bool m_bpersistent_quilt;
  int m_ChartUpdatePeriod;
  bool m_last_bGPSValid;

  wxString prev_locale;
  bool bPrevQuilt;
  bool bPrevFullScreenQuilt;
  bool bPrevOGL;

  MsgPriorityHash NMEA_Msg_Hash;
  wxString m_VDO_accumulator;

  time_t m_fixtime;
  wxMenu *piano_ctx_menu;
  bool b_autofind;

  time_t m_last_track_rotation_ts;
  wxRect m_mainlast_tb_rect;
  wxTimer ToolbarAnimateTimer;
  int m_nMasterToolCountShown;

  SignalKEventHandler m_signalKHandler;

  //  comm event listeners
  ObservableListener listener_N2K_129029;
  ObservableListener listener_N2K_129026;

  ObservableListener listener_N0183_RMC;
  ObservableListener listener_N0183_HDT;
  ObservableListener listener_N0183_HDG;
  ObservableListener listener_N0183_HDM;
  ObservableListener listener_N0183_VTG;
  ObservableListener listener_N0183_GSV;
  ObservableListener listener_N0183_GGA;
  ObservableListener listener_N0183_GLL;
  ObservableListener listener_N0183_AIVDO;

  DECLARE_EVENT_TABLE()
};

#endif   // 0

#endif

#endif  // _IDENTS_H__
