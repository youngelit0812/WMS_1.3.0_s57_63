#include <wx/wxprec.h>

#ifdef __MINGW32__
#undef IPV6STRICT  // mingw FTBS fix:  missing struct ip_mreq
#include <windows.h>
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif  // precompiled headers

#include <wx/app.h>
#include <wx/apptrait.h>
#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/tokenzr.h>
#include <wx/textfile.h>

#include "config.h"

#include "base_platform.h"
#include "OCPNPlatform.h"
#include "gui_lib.h"
#include "cutil.h"
#include "config_vars.h"
#include "logger.h"
#include "styles.h"
#include "navutil.h"
#include "ocpn_utils.h"
#include "FontMgr.h"
#include "s52s57.h"
#include "options.h"
#include "AboutFrameImpl.h"
#include "about.h"
#include "plugin_paths.h"
#include "ocpn_frame.h"
#include <string>
#include <vector>

#ifdef ocpnUSE_GL
#include "glChartCanvas.h"
#endif

// Include CrashRpt Header
#ifdef OCPN_USE_CRASHREPORT
#include "CrashRpt.h"
#endif
#ifdef __MSVC__
#include <new.h>
#endif

#ifdef LINUX_CRASHRPT
#include "crashprint.h"
#endif

#ifndef __WXMSW__
#include <signal.h>
#include <setjmp.h>
#endif

#ifdef __WXMSW__
#include <windows.h>
#include <winioctl.h>
#include <initguid.h>
#include "setupapi.h"  // presently stored in opencpn/src
#endif

#ifdef __WXOSX__
#include "macutils.h"
#endif

#ifdef __WXGTK__
//#include <gdk/gdk.h>
#endif

#include <cstdlib>

void appendOSDirSlash(wxString *pString);

#ifndef __WXMSW__
struct sigaction sa_all;
struct sigaction sa_all_old;
extern sigjmp_buf env;  // the context saved by sigsetjmp();
#endif

extern OCPNPlatform *g_Platform;
extern wxString g_winPluginDir;
extern bool g_bFirstRun;
extern bool g_bUpgradeInProcess;

extern int quitflag;
extern MyFrame *gFrame;
extern bool g_bportable;

extern MyConfig *pConfig;

extern ocpnStyle::StyleManager *g_StyleManager;

extern bool g_bshowToolbar;
extern bool g_bexpert;
extern bool g_bBasicMenus;
extern bool g_bUIexpert;
extern int g_ChartScaleFactor;

extern bool g_bshowToolbar;
extern bool g_bBasicMenus;

extern bool g_bShowOutlines;
extern int g_nAWDefault;
extern int g_nAWMax;
extern bool g_bPermanentMOBIcon;
extern float g_toolbar_scalefactor;

extern options *g_options;
extern bool g_boptionsactive;

// AIS Global configuration
extern double g_CPAMax_NM;
extern double g_CPAWarn_NM;
extern double g_TCPA_Max;
extern bool g_bMarkLost;
extern double g_MarkLost_Mins;
extern bool g_bRemoveLost;
extern double g_RemoveLost_Mins;
extern bool g_bShowCOG;
extern bool g_bSyncCogPredictors;
extern double g_ShowCOG_Mins;
extern bool g_bHideMoored;
extern double g_ShowMoored_Kts;
extern bool g_bShowAreaNotices;
extern bool g_bDrawAISSize;
extern bool g_bDrawAISRealtime;
extern double g_AIS_RealtPred_Kts;
extern bool g_bShowAISName;
extern bool g_bAIS_GCPA_Alert_Audio;
extern bool g_bAIS_SART_Alert_Audio;
extern bool g_bAIS_DSC_Alert_Audio;
extern bool g_bAIS_CPA_Alert_Audio;
extern bool g_bCPAWarn;
extern bool g_bAIS_CPA_Alert;

extern wxString *pInit_Chart_Dir;

extern double g_config_display_size_mm;
extern bool g_config_display_size_manual;

extern float g_selection_radius_mm;
extern float g_selection_radius_touch_mm;

extern double g_PlanSpeed;
extern bool g_bFullScreenQuilt;
extern bool g_bQuiltEnable;
extern bool g_bskew_comp;

extern bool g_bopengl;
extern bool g_bresponsive;
extern int g_cm93_zoom_factor;
extern int g_GUIScaleFactor;
extern bool g_fog_overzoom;
extern bool g_oz_vector_scale;
extern wxString g_toolbarConfig;
extern bool g_bPreserveScaleOnX;
extern bool g_running;

#ifdef ocpnUSE_GL
extern ocpnGLOptions g_GLOptions;
#endif
extern int g_default_font_size;
extern wxString g_default_font_facename;

wxLog *g_logger;
bool g_bEmailCrashReport;
extern int g_ais_alert_dialog_x, g_ais_alert_dialog_y;
extern int g_ais_alert_dialog_sx, g_ais_alert_dialog_sy;

extern double g_ChartNotRenderScaleFactor;
extern bool g_bRollover;

#if wxUSE_XLOCALE || !wxCHECK_VERSION(3, 0, 0)
extern wxLocale *plocale_def_lang;

extern wxString g_locale;
extern wxString g_localeOverride;
extern wxArrayString g_locale_catalog_array;

#endif
extern int options_lastPage;
extern AboutFrameImpl *g_pAboutDlg;
extern about *g_pAboutDlgLegacy;
extern wxColour g_colourTrackLineColour;
extern int g_n_ownship_min_mm;

extern int g_AndroidVersionCode;
extern bool g_bShowMuiZoomButtons;
extern int g_FlushNavobjChangesTimeout;
extern wxString g_CmdSoundString;
extern int g_maintoolbar_x;
extern int g_maintoolbar_y;
extern std::vector<std::string> TideCurrentDataSet;
extern int g_Android_SDK_Version;
extern wxString g_androidDownloadDirectory;
extern wxString g_gpx_path;
extern BasePlatform *g_BasePlatform;
extern bool g_bdisable_opengl;

OCPN_GLCaps *GL_Caps;

static const char *const DEFAULT_XDG_DATA_DIRS =
    "~/.local/share:/usr/local/share:/usr/share";

#ifdef __WXMSW__
static const char PATH_SEP = ';';
#else
static const char PATH_SEP = ':';
#endif

static bool checkIfFlatpacked() {
  wxString id;
  if (!wxGetEnv("FLATPAK_ID", &id)) {
    return false;
  }
  return id == "org.opencpn.OpenCPN";
}


OCPNPlatform::OCPNPlatform() {
  m_pt_per_pixel = 0;  // cached value
  m_bdisableWindowsDisplayEnum = false;
  m_displaySize = wxSize(0, 0);
  m_displaySizeMM = wxSize(0, 0);
  m_monitorWidth = m_monitorHeight = 0;
  m_displaySizeMMOverride = 0;
  m_pluginDataPath = "";
}

OCPNPlatform::~OCPNPlatform() {}

//--------------------------------------------------------------------------
//      Per-Platform Initialization support
//--------------------------------------------------------------------------
#ifdef __WXMSW__
int MyNewHandler(size_t size) {
  //  Pass to wxWidgets Main Loop handler
  throw std::bad_alloc();

  return 0;
}
#endif

//-----------------------------------------------------------------------
//      Signal Handlers
//-----------------------------------------------------------------------
#ifndef __WXMSW__

// These are the signals possibly expected
//      SIGUSR1
//      Raised externally to cause orderly termination of application
//      Intended to act just like pushing the "EXIT" button

//      SIGSEGV
//      Some undefined segfault......

int s_inhup;

void catch_signals(int signo) {
  switch (signo) {
    case SIGUSR1:
      quitflag++;  // signal to the timer loop
      break;

    case SIGSEGV:
      siglongjmp(env, 1);  // jump back to the setjmp() point
      break;

    case SIGHUP:
      if (!s_inhup) {
        s_inhup++;  // incase SIGHUP is closely followed by SIGTERM
        gFrame->FastClose();
      }
      break;

    case SIGTERM:
      if (!s_inhup) {
        s_inhup++;  // incase SIGHUP is closely followed by SIGTERM
        gFrame->FastClose();
      }

      break;

    default:
      break;
  }
}
#endif

#ifdef OCPN_USE_CRASHREPORT
// Define the crash callback
int CALLBACK CrashCallback(CR_CRASH_CALLBACK_INFO *pInfo) {
  //  Flush log file
  if (g_logger) g_logger->Flush();

  return CR_CB_DODEFAULT;
}
#endif

void OCPNPlatform::Initialize_1(void) {
#ifdef OCPN_USE_CRASHREPORT
#ifndef _DEBUG
  // Install Windows crash reporting

  CR_INSTALL_INFO info;
  memset(&info, 0, sizeof(CR_INSTALL_INFO));
  info.cb = sizeof(CR_INSTALL_INFO);
  info.pszAppName = _T("OpenCPN");

  info.pszAppVersion = wxString(VERSION_FULL).c_str();

  int type = MiniDumpNormal;

  // This results in the inclusion of global variables
  type |= MiniDumpWithDataSegs;

  // If this flag is specified, the contents of every readable and writeable
  // private memory page will be included into the minidump. type |=
  // MiniDumpWithPrivateReadWriteMemory;

  // If this flag is specified, MiniDumpWriteDump function will scan the stack
  // memory of every thread looking for pointers that point to other readable
  // memory pages in the process' address space. type |=
  // MiniDumpWithIndirectlyReferencedMemory;

  info.uMiniDumpType = (MINIDUMP_TYPE)type;

  // Install all available exception handlers....
  info.dwFlags = CR_INST_ALL_POSSIBLE_HANDLERS;

  //  Except memory allocation failures
  info.dwFlags &= ~CR_INST_NEW_OPERATOR_ERROR_HANDLER;

  //  Allow user to attach files
  info.dwFlags |= CR_INST_ALLOW_ATTACH_MORE_FILES;

  //  Allow user to add more info
  info.dwFlags |= CR_INST_SHOW_ADDITIONAL_INFO_FIELDS;

  // URL for sending error reports over HTTP.

  if (g_bEmailCrashReport) {
    info.pszUrl = _T("https://bigdumboat.com/crashrpt/ocpn_crashrpt.php");
    info.uPriorities[CR_HTTP] = 3;  // First try send report over HTTP
  } else {
    info.dwFlags |= CR_INST_DONT_SEND_REPORT;
    info.uPriorities[CR_HTTP] = CR_NEGATIVE_PRIORITY;  // don't send at all
  }

  info.uPriorities[CR_SMTP] =
      CR_NEGATIVE_PRIORITY;  // Second try send report over SMTP
  info.uPriorities[CR_SMAPI] =
      CR_NEGATIVE_PRIORITY;  // Third try send report over Simple MAPI

  wxStandardPaths &crash_std_path = g_Platform->GetStdPaths();

  wxString crash_rpt_save_locn = crash_std_path.GetConfigDir();
  if (g_bportable) {
    wxFileName exec_path_crash(crash_std_path.GetExecutablePath());
    crash_rpt_save_locn =
        exec_path_crash.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
  }

  wxString locn = crash_rpt_save_locn + _T("\\CrashReports");

  if (!wxDirExists(locn)) wxMkdir(locn);

  if (wxDirExists(locn)) {
    wxCharBuffer buf = locn.ToUTF8();
    wchar_t wlocn[256];
    if (buf && (locn.Length() < sizeof(wlocn))) {
      MultiByteToWideChar(0, 0, buf.data(), -1, wlocn, sizeof(wlocn) - 1);
      info.pszErrorReportSaveDir = (LPCWSTR)wlocn;
    }
  }

  // Provide privacy policy URL
  wxFileName exec_path_crash(crash_std_path.GetExecutablePath());
  wxString policy_file =
      exec_path_crash.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
  policy_file += _T("PrivacyPolicy.txt");
  policy_file.Prepend(_T("file:"));

  info.pszPrivacyPolicyURL = policy_file.c_str();
  ;

  int nResult = crInstall(&info);
  if (nResult != 0) {
    TCHAR buff[256];
    crGetLastErrorMsg(buff, 256);
    // MessageBox(NULL, buff, _T("crInstall error, Crash Reporting disabled."),
    // MB_OK);
  }

  if (nResult == 0) {  // Complete the installation
    // Establish the crash callback function
    crSetCrashCallback(CrashCallback, NULL);

    // Take screenshot of the app window at the moment of crash
    crAddScreenshot2(CR_AS_PROCESS_WINDOWS | CR_AS_USE_JPEG_FORMAT, 95);

    //  Mark some files to add to the crash report
    wxString home_data_crash = crash_std_path.GetConfigDir();
    if (g_bportable) {
      wxFileName f(crash_std_path.GetExecutablePath());
      home_data_crash = f.GetPath();
    }
    appendOSDirSlash(&home_data_crash);

    wxString config_crash = _T("wmsserver.conf");
    config_crash.Prepend(home_data_crash);
    crAddFile2(config_crash.c_str(), NULL, NULL,
               CR_AF_MISSING_FILE_OK | CR_AF_ALLOW_DELETE);

    wxString log_crash = _T("wmsserver.log");
    log_crash.Prepend(home_data_crash);
    crAddFile2(log_crash.c_str(), NULL, NULL,
               CR_AF_MISSING_FILE_OK | CR_AF_ALLOW_DELETE);
  }
#endif
#endif

#ifdef LINUX_CRASHRPT
#if wxUSE_ON_FATAL_EXCEPTION
  // fatal exceptions handling
  wxHandleFatalExceptions(true);
#endif
#endif

#ifdef __MSVC__
  //  Invoke my own handler for failures of malloc/new
  _set_new_handler(MyNewHandler);
  //  configure malloc to call the New failure handler on failure
  _set_new_mode(1);
#endif

#if 0
#ifdef __WXMSW__
    //    On MSW, force the entire process to run on one CPU core only
    //    This resolves some difficulty with wxThread syncronization
    //Gets the current process handle
    HANDLE hProc = GetCurrentProcess();
    DWORD procMask;
    DWORD sysMask;
    HANDLE hDup;
    DuplicateHandle( hProc, hProc, hProc, &hDup, 0, FALSE, DUPLICATE_SAME_ACCESS );

    //Gets the current process affinity mask
    GetProcessAffinityMask( hDup, &procMask, &sysMask );

    // Take a simple approach, and assume up to 4 processors
    DWORD newMask;
    if( ( procMask & 1 ) == 1 ) newMask = 1;
    else
        if( ( procMask & 2 ) == 2 ) newMask = 2;
        else
            if( ( procMask & 4 ) == 4 ) newMask = 4;
            else
                if( ( procMask & 8 ) == 8 ) newMask = 8;

                //Set te affinity mask for the process
                BOOL res = SetProcessAffinityMask( hDup, (DWORD_PTR) newMask );

            if( res == 0 ) {
                //Error setting affinity mask!!
            }
#endif
#endif

#ifdef __MSVC__

  //    Handle any Floating Point Exceptions which may leak thru from other
  //    processes.  The exception filter is in cutil.c
  //    Seems to only happen for W98

  wxPlatformInfo Platform;
  if (Platform.GetOperatingSystemId() == wxOS_WINDOWS_9X)
    SetUnhandledExceptionFilter(&MyUnhandledExceptionFilter);
#endif

#ifdef __WXMSW__
    //     _CrtSetBreakAlloc(25503);
#endif

#ifndef __WXMSW__
  //      Setup Linux SIGNAL handling, for external program control

  //      Build the sigaction structure
  sa_all.sa_handler = catch_signals;  // point to my handler
  sigemptyset(&sa_all.sa_mask);       // make the blocking set
  // empty, so that all
  // other signals will be
  // unblocked during my handler
  sa_all.sa_flags = 0;

  sigaction(SIGUSR1, NULL,
            &sa_all_old);  // save existing action for this signal

  //      Register my request for some signals
  sigaction(SIGUSR1, &sa_all, NULL);
  sigaction(SIGUSR1, NULL,
            &sa_all_old);  // inspect existing action for this signal

  sigaction(SIGTERM, &sa_all, NULL);
  sigaction(SIGTERM, NULL, &sa_all_old);

  sigaction(SIGHUP, &sa_all, NULL);
  sigaction(SIGHUP, NULL, &sa_all_old);

#endif
}

void OCPNPlatform::Initialize_2(void) {
  //  Set a global toolbar scale factor
  g_toolbar_scalefactor = GetToolbarScaleFactor(g_GUIScaleFactor);
  auto configdir = wxFileName(GetPrivateDataDir());
  if (!configdir.DirExists()) {
    if (!configdir.Mkdir()) {
      auto msg = std::string("Cannot create config directory: ");
      wxLogWarning(msg + configdir.GetFullPath());
    }
  }
}

void OCPNPlatform::Initialize_3(void) {
  bool bcapable = IsGLCapable();    

  if(!bcapable)
    g_bopengl = false;
  else {
    g_bopengl = true;
    g_bdisable_opengl = false;
    pConfig->UpdateSettings();
  }


  // Try to automatically switch to guaranteed usable GL mode on an OCPN upgrade
  // or fresh install

  if ((g_bFirstRun || g_bUpgradeInProcess) && bcapable) {
    g_bopengl = true;

    // Set up visually nice options
    g_GLOptions.m_bUseAcceleratedPanning = true;
    g_GLOptions.m_bTextureCompression = true;
    g_GLOptions.m_bTextureCompressionCaching = true;

    g_GLOptions.m_iTextureDimension = 512;
    g_GLOptions.m_iTextureMemorySize = 64;

    g_GLOptions.m_GLPolygonSmoothing = true;
    g_GLOptions.m_GLLineSmoothing = true;
  }
    
  g_FlushNavobjChangesTimeout = 300;  // Seconds, so 5 minutes
}

//  Called from MyApp() just before end of MyApp::OnInit()
void OCPNPlatform::Initialize_4(void) {
#ifdef __WXMAC__
  // A bit of a hack for Mojave MacOS 10.14.
  // Force the user to actively select "Display" tab to ensure initial rendering
  // of canvas layout select button.
  options_lastPage = 1;
#endif
}

void OCPNPlatform::OnExit_1(void) {}

void OCPNPlatform::OnExit_2(void) {
#ifdef OCPN_USE_CRASHREPORT
#ifndef _DEBUG
  // Uninstall Windows crash reporting
//    crUninstall();
#endif
#endif
}


bool OCPNPlatform::BuildGLCaps(void *pbuf) {
  // Investigate OpenGL capabilities
  gFrame->Show();
  glTestCanvas *tcanvas = new glTestCanvas(gFrame);
  tcanvas->Show();
  wxYield();
  wxGLContext *pctx = new wxGLContext(tcanvas);
  tcanvas->SetCurrent(*pctx);

  OCPN_GLCaps *pcaps = (OCPN_GLCaps *)pbuf;

  char *str = (char *)glGetString(GL_RENDERER);
  if (str == NULL) {    //No GL at all...
    wxLogMessage("GL_RENDERER not found.");
    delete tcanvas;
    delete pctx;
    return false;
  }
  pcaps->Renderer = std::string(str);

  char *stv = (char *)glGetString(GL_VERSION);
  if (stv == NULL) {    //No GL Version...
    wxLogMessage("GL_VERSION not found");
    delete tcanvas;
    delete pctx;
    return false;
  }
  pcaps->Version = std::string(stv);

  char *stsv = (char *)glGetString(GL_SHADING_LANGUAGE_VERSION);
  if (stsv == NULL) {    //No GLSL...
    wxLogMessage("GL_SHADING_LANGUAGE_VERSION not found");
    delete tcanvas;
    delete pctx;
    return false;
  }
  pcaps->GLSL_Version = std::string(stsv);

  pcaps->dGLSL_Version = 0;
  pcaps->dGLSL_Version = ::atof(pcaps->GLSL_Version.c_str());

  if (pcaps->dGLSL_Version < 1.2){
    wxString msg;
    msg.Printf(_T("GLCaps Probe: OpenGL-> GLSL Version reported:  "));
    msg += wxString(pcaps->GLSL_Version.c_str());
    msg += "\n OpenGL disabled due to insufficient OpenGL capabilities";
    wxLogMessage(msg);
    pcaps->bCanDoGLSL = false;
    return false;
  }

  pcaps->bCanDoGLSL = true;

  if (QueryExtension("GL_ARB_texture_non_power_of_two"))
    pcaps->TextureRectangleFormat = GL_TEXTURE_2D;
  else if (QueryExtension("GL_OES_texture_npot"))
    pcaps->TextureRectangleFormat = GL_TEXTURE_2D;
  else if (QueryExtension("GL_ARB_texture_rectangle"))
    pcaps->TextureRectangleFormat = GL_TEXTURE_RECTANGLE_ARB;

  pcaps->bOldIntel = false;

  // Can we use VBO?
  pcaps->bCanDoVBO = true;

#if defined(__WXMSW__) || defined(__WXOSX__)
  if (pcaps->bOldIntel) pcaps->bCanDoVBO = false;
#endif


  // Can we use FBO?
  pcaps->bCanDoFBO = true;

  //  We need NPOT to support FBO rendering
  if (!pcaps->TextureRectangleFormat) pcaps->bCanDoFBO = false;

  //      We require certain extensions to support FBO rendering
  if (!QueryExtension("GL_EXT_framebuffer_object")) pcaps->bCanDoFBO = false;

  delete tcanvas;
  delete pctx;

  return true;
}


bool OCPNPlatform::IsGLCapable() {
#if defined(CLI)
  return false;
#else

  if(g_bdisable_opengl)
    return false;

  // Protect against fault in OpenGL caps test
  // If this method crashes due to bad GL drivers,
  // next startup will disable OpenGL
  g_bdisable_opengl = true;

  // Update and flush the config file
  pConfig->UpdateSettings();

  wxLogMessage("Starting OpenGL test...");
  wxLog::FlushActive();

  OCPN_GLCaps GL_Caps;
  bool bcaps = BuildGLCaps(&GL_Caps);

  wxLogMessage("OpenGL test complete.");
  if (!bcaps){
    wxLogMessage("BuildGLCaps fails.");
    wxLog::FlushActive();
    return false;
  }

  // and so we decide....

  // Require a modern GLSL implementation
  if (!GL_Caps.bCanDoGLSL) {
    return false;
  }

  // We insist on FBO support, since otherwise DC mode is always faster on
  // canvas panning..
  if (!GL_Caps.bCanDoFBO)  {
    return false;
  }

  // OpenGL is OK for OCPN
  wxLogMessage("OpenGL determined CAPABLE.");
  wxLog::FlushActive();

  g_bdisable_opengl = false;
  g_bopengl = true;

  // Update and flush the config file
  pConfig->UpdateSettings();

  return true;
#endif
}
void OCPNPlatform::SetLocaleSearchPrefixes(void) {
#if wxUSE_XLOCALE
// Add a new prefixes for search order.
#if defined(__WINDOWS__)

  // Legacy and system plugin location
  wxString locale_location = GetSharedDataDir();
  locale_location += _T("share\\locale");
  wxLocale::AddCatalogLookupPathPrefix(locale_location);


  // Managed plugin location
  wxFileName usrShare(GetWinPluginBaseDir() + wxFileName::GetPathSeparator());
  usrShare.RemoveLastDir();
  locale_location = usrShare.GetFullPath() + ("share\\locale");
  wxLocale::AddCatalogLookupPathPrefix(locale_location);

#elif defined(__UNIX__) && !defined(__WINE__)

  // On Unix, wxWidgets defaults to installation prefix of its own, usually
  // "/usr". On the other hand, canonical installation prefix for OpenCPN is
  // "/usr/local".
  wxString locale_location;
  if (!wxGetEnv(_T("OPENCPN_PREFIX"), &locale_location)) {
    locale_location = _T("/usr/local");
  }
  wxFileName location;
  location.AssignDir(locale_location);
  location.AppendDir(_T("share"));
  location.SetName(_T("locale"));
  locale_location = location.GetFullPath();
  wxLocale::AddCatalogLookupPathPrefix(locale_location);

  // And then for managed plugins
  std::string dir = PluginPaths::getInstance()->UserDatadir();
  wxString managed_locale_location(dir + "/locale");
  wxLocale::AddCatalogLookupPathPrefix(managed_locale_location);
#endif

#ifdef __WXOSX__
  std::string macDir =
      PluginPaths::getInstance()->Homedir() +
      "/Library/Application Support/OpenCPN/Contents/Resources";
  wxString Mac_managed_locale_location(macDir);
  wxLocale::AddCatalogLookupPathPrefix(Mac_managed_locale_location);
#endif

#endif
}

wxString OCPNPlatform::GetDefaultSystemLocale() {
  wxString retval = _T("en_US");

#if wxUSE_XLOCALE

  const wxLanguageInfo *languageInfo =
      wxLocale::GetLanguageInfo(wxLANGUAGE_DEFAULT);
  if (languageInfo) retval = languageInfo->CanonicalName;

#if defined(__WXMSW__)
  LANGID lang_id = GetUserDefaultUILanguage();

  wchar_t lngcp[101];
  const wxLanguageInfo *languageInfoW = 0;
  if (0 != GetLocaleInfo(MAKELCID(lang_id, SORT_DEFAULT), LOCALE_SENGLANGUAGE,
                         lngcp, 100)) {
    wxString lstring = wxString(lngcp);

    languageInfoW = wxLocale::FindLanguageInfo(lngcp);
  } else {
    languageInfoW = wxLocale::GetLanguageInfo(wxLANGUAGE_DEFAULT);
  }

  if (languageInfoW) retval = languageInfoW->CanonicalName;
#endif
#endif

  return retval;
}

#if wxUSE_XLOCALE || !wxCHECK_VERSION(3, 0, 0)
wxString OCPNPlatform::GetAdjustedAppLocale() {
  wxString adjLocale = g_locale;

//  For windows, installer may have left information in the registry defining
//  the user's selected install language. If so, override the config file value
//  and use this selection for opencpn...
#if defined(__WXMSW__)
  if (g_bFirstRun || wxIsEmpty(adjLocale)) {
    wxRegKey RegKey(wxString(_T("HKEY_LOCAL_MACHINE\\SOFTWARE\\OpenCPN")));
    if (RegKey.Exists()) {
      RegKey.QueryValue(wxString(_T("InstallerLanguage")), adjLocale);
    }
  }
  if (wxIsEmpty(adjLocale)) {
    if (g_localeOverride.Length())
      adjLocale = g_localeOverride;
    else
      adjLocale = GetDefaultSystemLocale();
  }
#endif

  return adjLocale;
}

wxString OCPNPlatform::ChangeLocale(wxString &newLocaleID,
                                    wxLocale *presentLocale,
                                    wxLocale **newLocale) {
  wxString return_val;
  //  Old locale is done.
  delete (wxLocale *)presentLocale;

  wxLocale *locale = new wxLocale;
  if (isFlatpacked()) {
    std::string path(getenv("HOME"));
    path += "/.var/app/org.opencpn.OpenCPN/data/locale";
    locale->AddCatalogLookupPathPrefix(path);
  }
  wxString loc_lang_canonical;

  const wxLanguageInfo *pli = wxLocale::FindLanguageInfo(newLocaleID);
  bool b_initok = false;

  if (pli) {
    locale->Init(pli->Language, 1);
    // If the locale was not initialized OK, it may be that the wxstd.mo
    // translations of the wxWidgets strings is not present. So try again,
    // without attempting to load defaults wxstd.mo.
    if (!locale->IsOk()) {
      delete locale;
      locale = new wxLocale;
      locale->Init(pli->Language, 0);
    }
    loc_lang_canonical = pli->CanonicalName;

    b_initok = locale->IsOk();
  }

  if (!b_initok) {
    delete locale;
    locale = new wxLocale;
    locale->Init(wxLANGUAGE_ENGLISH_US, 0);
    loc_lang_canonical = wxLocale::GetLanguageInfo(wxLANGUAGE_ENGLISH_US)->CanonicalName;
  }

  if (b_initok) {
    //  wxWidgets assigneds precedence to message catalogs in reverse order of
    //  loading. That is, the last catalog containing a certain translatable
    //  item takes precedence.

    //  So, Load the catalogs saved in a global string array which is populated
    //  as PlugIns request a catalog load. We want to load the PlugIn catalogs
    //  first, so that core opencpn translations loaded later will become
    //  precedent.

    for (unsigned int i = 0; i < g_locale_catalog_array.GetCount(); i++) {
		locale->AddCatalog(g_locale_catalog_array[i]);        
    }

    // Get core opencpn catalog translation (.mo) file
    locale->AddCatalog(_T("opencpn"));

    return_val = locale->GetCanonicalName();

    // We may want to override the default system locale, so set a flag.
    if (return_val != GetDefaultSystemLocale())
      g_localeOverride = return_val;
    else
      g_localeOverride = _T("");
  }

  *newLocale = locale;  // return the new locale

  //    Always use dot as decimal
  setlocale(LC_NUMERIC, "C");

  return return_val;
}
#endif

//      Setup default global options when config file is unavailable,
//      as on initial startup after new install
//      The global config object (pConfig) is available, so direct updates are
//      also allowed

void OCPNPlatform::SetDefaultOptions(void) {
  //  General options, applied to all platforms
  g_bShowOutlines = true;

  g_CPAMax_NM = 20.;
  g_CPAWarn_NM = 2.;
  g_TCPA_Max = 30.;
  g_bMarkLost = true;
  g_MarkLost_Mins = 8;
  g_bRemoveLost = true;
  g_RemoveLost_Mins = 10;
  g_bShowCOG = true;
  g_ShowCOG_Mins = 6;
  g_bSyncCogPredictors = false;
  g_bHideMoored = false;
  g_ShowMoored_Kts = 0.2;
  g_PlanSpeed = 6.;
  g_bFullScreenQuilt = true;
  g_bQuiltEnable = true;
  g_bskew_comp = false;
  g_bShowAreaNotices = false;
  g_bDrawAISSize = false;
  g_bDrawAISRealtime = false;
  g_AIS_RealtPred_Kts = 0.7;
  g_bShowAISName = false;  
  g_bPreserveScaleOnX = true;
  g_nAWDefault = 50;
  g_nAWMax = 1852;
  gps_watchdog_timeout_ticks = GPS_TIMEOUT_SECONDS;
  g_n_ownship_min_mm = 8;
  g_bShowMuiZoomButtons = true;
  g_bresponsive = false;

  // Initial S52/S57 options
  if (pConfig) {
    pConfig->SetPath(_T ( "/Settings/GlobalState" ));
    pConfig->Write(_T ( "bShowS57Text" ), true);
    pConfig->Write(_T ( "bShowS57ImportantTextOnly" ), false);
    pConfig->Write(_T ( "nDisplayCategory" ), (int)(_DisCat)STANDARD);
    pConfig->Write(_T ( "nSymbolStyle" ), (int)(_LUPname)PAPER_CHART);
    pConfig->Write(_T ( "nBoundaryStyle" ), (int)(_LUPname)PLAIN_BOUNDARIES);

    pConfig->Write(_T ( "bShowSoundg" ), true);
    pConfig->Write(_T ( "bShowMeta" ), false);
    pConfig->Write(_T ( "bUseSCAMIN" ), true);
    pConfig->Write(_T ( "bShowAtonText" ), false);
    pConfig->Write(_T ( "bShowLightDescription" ), false);
    pConfig->Write(_T ( "bExtendLightSectors" ), true);
    pConfig->Write(_T ( "bDeClutterText" ), true);
    pConfig->Write(_T ( "bShowNationalText" ), true);

    pConfig->Write(_T ( "S52_MAR_SAFETY_CONTOUR" ), 3);
    pConfig->Write(_T ( "S52_MAR_SHALLOW_CONTOUR" ), 2);
    pConfig->Write(_T ( "S52_MAR_DEEP_CONTOUR" ), 6);
    pConfig->Write(_T ( "S52_MAR_TWO_SHADES" ), 0);
    pConfig->Write(_T ( "S52_DEPTH_UNIT_SHOW" ), 1);

    pConfig->Write(_T ( "ZoomDetailFactorVector" ), 3);

    pConfig->Write(_T ( "nColorScheme" ), 1);  // higher contrast on NOAA RNCs
  }

#ifdef __WXMSW__
  //  Enable some default PlugIns, and their default options
  if (pConfig) {
    pConfig->SetPath(_T ( "/PlugIns/chartdldr_pi.dll" ));
    pConfig->Write(_T ( "bEnabled" ), true);

    pConfig->SetPath(_T ( "/PlugIns/wmm_pi.dll" ));
    pConfig->Write(_T ( "bEnabled" ), true);

    pConfig->SetPath(_T ( "/Settings/WMM" ));
    pConfig->Write(_T ( "ShowIcon" ), true);
    pConfig->Write(_T ( "ShowLiveIcon" ), true);
  }
#endif

#ifdef __WXOSX__
  //  Enable some default PlugIns, and their default options
  if (pConfig) {
    pConfig->SetPath(_T ( "/PlugIns/libchartdldr_pi.dylib" ));
    pConfig->Write(_T ( "bEnabled" ), true);

    pConfig->SetPath(_T ( "/PlugIns/libwmm_pi.dylib" ));
    pConfig->Write(_T ( "bEnabled" ), true);

    pConfig->SetPath(_T ( "/Settings/WMM" ));
    pConfig->Write(_T ( "ShowIcon" ), true);
    pConfig->Write(_T ( "ShowLiveIcon" ), true);
  }
#endif

#ifdef __linux__
  //  Enable some default PlugIns, and their default options
  if (pConfig) {
    pConfig->SetPath(_T ( "/PlugIns/libchartdldr_pi.so" ));
    pConfig->Write(_T ( "bEnabled" ), true);

    pConfig->SetPath(_T ( "/PlugIns/libwmm_pi.so" ));
    pConfig->Write(_T ( "bEnabled" ), true);

    pConfig->SetPath(_T ( "/Settings/WMM" ));
    pConfig->Write(_T ( "ShowIcon" ), true);
    pConfig->Write(_T ( "ShowLiveIcon" ), true);
  }
#endif
}

//      Setup global options on upgrade detected
//      The global config object (pConfig) has already been loaded, so updates
//      here override values set by config Direct updates to config for next
//      boot are also allowed

void OCPNPlatform::SetUpgradeOptions(wxString vNew, wxString vOld) {
  // Check for upgrade....
  if (!vOld.IsSameAs(vNew)) {  // upgrade

    // Verify some default directories, create if necessary

    // UserIcons
    wxString UserIconPath = GetPrivateDataDir();
    wxChar sep = wxFileName::GetPathSeparator();
    if (UserIconPath.Last() != sep) UserIconPath.Append(sep);
    UserIconPath.Append(_T("UserIcons"));

    if (!::wxDirExists(UserIconPath)) {
      ::wxMkdir(UserIconPath);
    }

    // layers
    wxString LayersPath = GetPrivateDataDir();
    if (LayersPath.Last() != sep) LayersPath.Append(sep);
    LayersPath.Append(_T("layers"));

    if (!::wxDirExists(LayersPath)) {
      ::wxMkdir(LayersPath);
    }

    // Force a generally useable sound command, overriding any previous user's
    // selection
    //  that may not be available on new build.
    g_CmdSoundString = wxString(OCPN_SOUND_CMD);
    pConfig->SetPath(_T ( "/Settings" ));
    pConfig->Write(_T( "CmdSoundString" ), g_CmdSoundString);

    // Force AIS specific sound effects ON, leaving the master control
    // (g_bAIS_CPA_Alert_Audio) as configured
    g_bAIS_GCPA_Alert_Audio = true;
    g_bAIS_SART_Alert_Audio = true;
    g_bAIS_DSC_Alert_Audio = true;

    // Force a recalculation of default main toolbar location
    g_maintoolbar_x = -1;

    // Check the tide/current databases for readability,
    //  remove any not readable
    std::vector<std::string> TCDS_temp;
    for (unsigned int i=0; i < TideCurrentDataSet.size() ; i++)
      TCDS_temp.push_back(TideCurrentDataSet[i]);

    TideCurrentDataSet.clear();
    for (unsigned int i=0; i < TCDS_temp.size() ; i++){
      wxString tide = TCDS_temp[i];
      wxFileName ft(tide);
      if (ft.FileExists())
        TideCurrentDataSet.push_back(TCDS_temp[i]);
    }
  }
}

int OCPNPlatform::platformApplyPrivateSettingsString(wxString settings,
                                                     ArrayOfCDI *pDirArray) {
  int ret_val = 0;

  return ret_val;
}

//--------------------------------------------------------------------------
//      Per-Platform file/directory support
//--------------------------------------------------------------------------


static wxString ExpandPaths(wxString paths, OCPNPlatform *platform);




MyConfig *OCPNPlatform::GetConfigObject() {
  MyConfig *result = NULL;

  result = new MyConfig(GetConfigFileName());

  return result;
}
//--------------------------------------------------------------------------
//      Internal GPS Support
//--------------------------------------------------------------------------

bool OCPNPlatform::hasInternalGPS(wxString profile) {
#ifdef __OCPN__ANDROID__
  bool t = androidDeviceHasGPS();
  //    qDebug() << "androidDeviceHasGPS" << t;
  return t;
#else

  return false;

#endif
}

double OCPNPlatform::GetDisplayDensityFactor() {
  return 1.0;
}

long OCPNPlatform::GetDefaultToolbarOrientation() {
  return wxTB_VERTICAL;
}

int OCPNPlatform::GetStatusBarFieldCount() {
  return STAT_FIELD_COUNT;  // default
}

double OCPNPlatform::getFontPointsperPixel(void) {
  double pt_per_pixel = 1.0;

  if (m_pt_per_pixel == 0) {
    //  Make a measurement...
    wxScreenDC dc;

    wxFont *f = FontMgr::Get().FindOrCreateFont(
        12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, FALSE,
        wxString(_T ( "" )), wxFONTENCODING_SYSTEM);
    dc.SetFont(*f);

    int width, height;
    dc.GetTextExtent(_T("H"), &width, &height, NULL, NULL, f);

    if (height > 0) m_pt_per_pixel = 12.0 / (double)height;
  }
  if (m_pt_per_pixel > 0) pt_per_pixel = m_pt_per_pixel;

  return pt_per_pixel;
}
wxSize OCPNPlatform::getDisplaySize() {
  if (m_displaySize.x < 10)
    m_displaySize = ::wxGetDisplaySize();  // default, for most platforms
  return m_displaySize;
}

double OCPNPlatform::GetDisplaySizeMM() {
  if (m_displaySizeMMOverride > 0) return m_displaySizeMMOverride;

  if (m_displaySizeMM.x < 1) m_displaySizeMM = wxGetDisplaySizeMM();

  double ret = m_displaySizeMM.GetWidth();
#if 0
#ifdef __WXGTK__
    printf("Try to get display size with gdk");
    GdkScreen *screen = gdk_screen_get_default();
    wxSize resolution = getDisplaySize();
    double gdk_monitor_mm;
    double ratio = (double)resolution.GetWidth() / (double)resolution.GetHeight();
    if( std::abs(ratio - 32.0/10.0) < std::abs(ratio - 16.0/10.0) ) {
        // We suspect that when the resolution aspect ratio is closer to 32:10 than 16:10, there are likely 2 monitors side by side. This works nicely when they are landscape, but what if both are rotated 90 degrees...
        gdk_monitor_mm = gdk_screen_get_width_mm(screen);
    } else {
        gdk_monitor_mm = gdk_screen_get_monitor_width_mm(screen, 0);
    }
    if(gdk_monitor_mm > 0) // if gdk detects a valid screen width (returns -1 on raspberry pi)
        ret = gdk_monitor_mm;
#endif
#endif

#ifdef __WXMSW__
  int w, h;

  if (!m_bdisableWindowsDisplayEnum) {
    if (GetWindowsMonitorSize(&w, &h) && (w > 100)) {  // sanity check
      m_displaySizeMM == wxSize(w, h);
      ret = w;
    } else
      m_bdisableWindowsDisplayEnum = true;  // disable permanently
  }

#endif

#ifdef __WXOSX__
  ret = GetMacMonitorSize();
#endif
  return ret;
}

double OCPNPlatform::GetDisplayAreaCM2() {
  double size1 = GetDisplaySizeMM();
  wxSize sz = getDisplaySize();
  double ratio = 1.;
  if (sz.x < sz.y)
    ratio = (double)sz.x / (double)sz.y;  // <1
  else
    ratio = (double)sz.y / (double)sz.x;  // <1

  double area = size1 * (size1 * ratio) / 100.;
  // qDebug() << "cm2" << size1 << ratio << sz.x << sz.y;
  return area;
}

void OCPNPlatform::SetDisplaySizeMM(double sizeMM) {
  m_displaySizeMMOverride = sizeMM;
}

double OCPNPlatform::GetDisplayDPmm() {
  double r = getDisplaySize().x;  // dots
  return r / GetDisplaySizeMM();
}

unsigned int OCPNPlatform::GetSelectRadiusPix() {
  return GetDisplayDPmm() * g_selection_radius_mm;
}

bool OCPNPlatform::GetFullscreen() {
  bool bret = false;
  return bret;
}

bool OCPNPlatform::SetFullscreen(bool bFull) {
  bool bret = false;
  return bret;
}

void OCPNPlatform::PositionAISAlert(wxWindow *alert_window) {
  if (alert_window) {
    alert_window->SetSize(g_ais_alert_dialog_x, g_ais_alert_dialog_y,
                          g_ais_alert_dialog_sx, g_ais_alert_dialog_sy);
  }
}

double OCPNPlatform::GetToolbarScaleFactor(int GUIScaleFactor) {
  double rv = 1.0;

  double premult = 1.0;

  if (g_bresponsive) {
    //  Get the basic size of a tool icon
    ocpnStyle::Style *style = g_StyleManager->GetCurrentStyle();
    wxSize style_tool_size = style->GetToolSize();
    double tool_size = style_tool_size.x;

    // unless overridden by user, we declare the "best" tool size
    // to be roughly 9 mm square.
    double target_size = 9.0;  // mm

    double basic_tool_size_mm = tool_size / GetDisplayDPmm();
    premult = target_size / basic_tool_size_mm;
  }

  // Adjust the scale factor using the global GUI scale parameter
  double postmult = exp(GUIScaleFactor * (0.693 / 5.0));  //  exp(2)

  rv = premult * postmult;
  rv = wxMin(rv, 3.0);  //  Clamp at 3.0
  rv = wxMax(rv, 0.5);  //  and at 0.5

  rv /= g_BasePlatform->GetDisplayDIPMult(gFrame);

  return rv;
}

float OCPNPlatform::GetChartScaleFactorExp() {
  double factor = 1.0;
  factor = exp(g_ChartScaleFactor * (log(3.0) / 5.0));

  factor = wxMax(factor, .5);
  factor = wxMin(factor, 6.);

  return factor;
}

float OCPNPlatform::GetChartScaleFactorExp(float scale_linear) {
  double factor = 1.0;
  factor = exp(scale_linear * (log(3.0) / 5.0));

  factor = wxMax(factor, .5);
  factor = wxMin(factor, 6.);

  return factor;
}

float OCPNPlatform::GetMarkScaleFactorExp(float scale_linear) {
  if(scale_linear <= 0)
    return GetChartScaleFactorExp(scale_linear);
  else
    return GetChartScaleFactorExp(scale_linear-1);
}
//--------------------------------------------------------------------------
//      Internal Bluetooth Support
//--------------------------------------------------------------------------

bool OCPNPlatform::hasInternalBT(wxString profile) {
  return false;
}

bool OCPNPlatform::startBluetoothScan() {
  return false;
}

bool OCPNPlatform::stopBluetoothScan() {
  return false;
}

wxArrayString OCPNPlatform::getBluetoothScanResults() {
  wxArrayString ret_val;

  ret_val.Add(_T("line 1"));
  ret_val.Add(_T("line 2"));
  ret_val.Add(_T("line 3"));
  return ret_val;
}

//--------------------------------------------------------------------------
//      Per-Platform Utility support
//--------------------------------------------------------------------------

bool OCPNPlatform::AllowAlertDialog(const wxString &class_name) {
  return true;
}

void OCPNPlatform::setChartTypeMaskSel(int mask, wxString &indicator) {
}

bool OCPNPlatform::isPlatformCapable(int flag) {
  return true;
}
void OCPNPlatform::LaunchLocalHelp(void) {
  wxString def_lang_canonical = _T("en_US");

#if wxUSE_XLOCALE
  if (plocale_def_lang)
    def_lang_canonical = plocale_def_lang->GetCanonicalName();
#endif

  wxString help_locn = g_Platform->GetSharedDataDir() + _T("doc/help_");

  wxString help_try = help_locn + def_lang_canonical + _T(".html");

  if (!::wxFileExists(help_try)) {
    help_try = help_locn + _T("en_US") + _T(".html");

    if (!::wxFileExists(help_try)) {
      help_try = help_locn + _T("web") + _T(".html");
    }

    if (!::wxFileExists(help_try)) return;
  }

  wxLaunchDefaultBrowser(wxString(_T("file:///")) + help_try);
}

void OCPNPlatform::platformLaunchDefaultBrowser(wxString URL) {
  ::wxLaunchDefaultBrowser(URL);
}

// ============================================================================
// OCPNColourPickerCtrl implementation
// ============================================================================

BEGIN_EVENT_TABLE(OCPNColourPickerCtrl, wxButton)
#ifdef __WXMSW__
EVT_PAINT(OCPNColourPickerCtrl::OnPaint)
#endif
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// OCPNColourPickerCtrl
// ----------------------------------------------------------------------------

OCPNColourPickerCtrl::OCPNColourPickerCtrl(wxWindow *parent, wxWindowID id,
                                           const wxColour &initial,
                                           const wxPoint &pos,
                                           const wxSize &size, long style,
                                           const wxValidator &validator,
                                           const wxString &name) {
  Create(parent, id, initial, pos, size, style, validator, name);
}

bool OCPNColourPickerCtrl::Create(wxWindow *parent, wxWindowID id,
                                  const wxColour &col, const wxPoint &pos,
                                  const wxSize &size, long style,
                                  const wxValidator &validator,
                                  const wxString &name) {
  m_bitmap = wxBitmap(60, 13);

  // create this button
  if (!wxBitmapButton::Create(parent, id, m_bitmap, pos, size,
                              style | wxBU_AUTODRAW, validator, name)) {
    wxFAIL_MSG(wxT("OCPNColourPickerCtrl creation failed"));
    return false;
  }

  // and handle user clicks on it
  Connect(GetId(), wxEVT_BUTTON,
          wxCommandEventHandler(OCPNColourPickerCtrl::OnButtonClick), NULL,
          this);

  m_colour = col;
  UpdateColour();
  InitColourData();

  return true;
}

void OCPNColourPickerCtrl::InitColourData() {
#if 0
    ms_data.SetChooseFull(true);
    unsigned char grey = 0;
    for (int i = 0; i < 16; i++, grey += 16)
    {
        // fill with grey tones the custom colors palette
        wxColour colour(grey, grey, grey);
        ms_data.SetCustomColour(i, colour);
    }
#endif
}

void OCPNColourPickerCtrl::OnButtonClick(wxCommandEvent &WXUNUSED(ev)) {
  // update the wxColouData to be shown in the dialog
  ms_data.SetColour(m_colour);

  // create the colour dialog and display it
  wxColourDialog dlg(this, &ms_data);
  if (dlg.ShowModal() == wxID_OK) {
    ms_data = dlg.GetColourData();
    SetColour(ms_data.GetColour());
  }
}

void OCPNColourPickerCtrl::UpdateColour() {
  SetBitmapLabel(wxBitmap());

  wxMemoryDC dc(m_bitmap);
  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(wxBrush(m_colour));
  dc.DrawRectangle(0, 0, m_bitmap.GetWidth(), m_bitmap.GetHeight());

  dc.SelectObject(wxNullBitmap);
  SetBitmapLabel(m_bitmap);
}

void OCPNColourPickerCtrl::SetColour(wxColour &c) {
  m_colour = c;
  m_bitmap = wxBitmap(GetSize().x - 20, GetSize().y - 20);
  UpdateColour();
}

wxColour OCPNColourPickerCtrl::GetColour(void) { return m_colour; }

wxSize OCPNColourPickerCtrl::DoGetBestSize() const {
  wxSize sz(wxBitmapButton::DoGetBestSize());
#ifdef __WXMAC__
  sz.y += 6;
#else
  sz.y += 2;
#endif
  sz.x += 30;
  if (HasFlag(wxCLRP_SHOW_LABEL)) return sz;

  // if we have no label, then make this button a square
  // (like e.g. native GTK version of this control) ???
  // sz.SetWidth(sz.GetHeight());
  return sz;
}

void OCPNColourPickerCtrl::OnPaint(wxPaintEvent &event) {
  wxPaintDC dc(this);

  int offset_x = (GetSize().x - m_bitmap.GetWidth()) / 2;
  int offset_y = (GetSize().y - m_bitmap.GetHeight()) / 2;

  dc.SetPen(*wxTRANSPARENT_PEN);
  dc.SetBrush(wxBrush(m_colour));
  dc.DrawRectangle(offset_x, offset_y, m_bitmap.GetWidth(),
                   m_bitmap.GetHeight());

  event.Skip();
}
