#include <wx/wxprec.h>

#include <cstdlib>
#include <string>
#include <vector>

#ifdef __MINGW32__
#undef IPV6STRICT  // mingw FTBS fix:  missing struct ip_mreq
#include <windows.h>
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif  // precompiled headers

#ifdef __WXMSW__
#include <windows.h>
#include <winioctl.h>
#include <initguid.h>
#include "setupapi.h"  // presently stored in opencpn/src
#endif

#include <wx/apptrait.h>
#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/tokenzr.h>
#include <wx/platinfo.h>

#include "config.h"

#include "base_platform.h"
#include "logger.h"
#include "ocpn_utils.h"

#ifdef __WXOSX__
#include "macutils.h"
#endif

#ifdef __WXMSW__
static const char PATH_SEP = ';';
#else
static const char PATH_SEP = ':';
#endif

static const char* const DEFAULT_XDG_DATA_DIRS = "~/.local/share:/usr/local/share:/usr/share";
void appendOSDirSlash(wxString* pString);

extern wxString g_winPluginDir;

extern bool g_bportable;
extern float g_selection_radius_mm;
extern float g_selection_radius_touch_mm;

extern wxLog* g_logger;

extern BasePlatform* g_BasePlatform;
#ifdef _MSC_VER
extern bool m_bdisableWindowsDisplayEnum;
#endif

static bool checkIfFlatpacked() {
  wxString id;
  if (!wxGetEnv("FLATPAK_ID", &id)) {
    return false;
  }
  return id == "org.opencpn.OpenCPN";
}

static wxString ExpandPaths(wxString paths, BasePlatform* platform);

static wxString GetLinuxDataPath() {
  wxString dirs;
  if (wxGetEnv("XDG_DATA_DIRS", &dirs)) {
    dirs = wxString("~/.local/share:") + dirs;
  } else {
    dirs = DEFAULT_XDG_DATA_DIRS;
  }
  wxString s;
  wxStringTokenizer tokens(dirs, ':');
  while (tokens.HasMoreTokens()) {
    wxString dir = tokens.GetNextToken();
    if (dir.EndsWith("/")) {
      dir = dir.SubString(0, dir.length() - 1);
    }
    if (!dir.EndsWith("/opencpn/plugins")) {
      dir += "/opencpn/plugins";
    }
    s += dir + (tokens.HasMoreTokens() ? ";" : "");
  }
  return s;
}

static wxString ExpandPaths(wxString paths, BasePlatform* platform) {
  wxStringTokenizer tokens(paths, ';');
  wxString s = "";
  while (tokens.HasMoreTokens()) {
    wxFileName filename(tokens.GetNextToken());
    filename.Normalize();
    s += platform->NormalizePath(filename.GetFullPath());
    if (tokens.HasMoreTokens()) {
      s += ';';
    }
  }
  return s;
}

//  OCPN Platform implementation
BasePlatform::BasePlatform() {
  m_isFlatpacked = checkIfFlatpacked();
  m_osDetail = new OCPN_OSDetail;
  DetectOSDetail(m_osDetail);

  InitializeLogFile();
}

//--------------------------------------------------------------------------
//      Per-Platform file/directory support
//--------------------------------------------------------------------------

wxStandardPaths& BasePlatform::GetStdPaths() {
  return wxStandardPaths::Get();
}

wxString BasePlatform::NormalizePath(const wxString& full_path) {
  if (!g_bportable) {
    return full_path;
  } else {
    wxString path(full_path);
    wxFileName f(path);
    // If not on another voulme etc. make the portable relative path
    if (f.MakeRelativeTo(GetPrivateDataDir())) {
      path = f.GetFullPath();
    }
    return path;
  }
}

wxString& BasePlatform::GetHomeDir() {
  if (m_homeDir.IsEmpty()) {
    //      Establish a "home" location
    wxStandardPaths& std_path = GetStdPaths();
    // TODO  Why is the following preferred?  Will not compile with gcc...
    //    wxStandardPaths& std_path = wxApp::GetTraits()->GetStandardPaths();

#ifdef __unix__
    std_path.SetInstallPrefix(wxString(PREFIX, wxConvUTF8));
#endif

#ifdef __WXMSW__
    m_homeDir =
        std_path
            .GetConfigDir();  // on w98, produces "/windows/Application Data"
#else
    m_homeDir = std_path.GetUserConfigDir();
#endif
    if (g_bportable) {
      wxFileName path(GetExePath());
      m_homeDir = path.GetPath();
    }

#ifdef __WXOSX__
    appendOSDirSlash(&m_homeDir);
    m_homeDir.Append(_T("opencpn"));
#endif

    appendOSDirSlash(&m_homeDir);
  }

  return m_homeDir;
}

wxString& BasePlatform::GetExePath() {
  if (m_exePath.IsEmpty()) {
    wxStandardPaths& std_path = GetStdPaths();
    m_exePath = std_path.GetExecutablePath();
  }

  return m_exePath;
}

wxString* BasePlatform::GetSharedDataDirPtr() {
  if (m_SData_Dir.IsEmpty()) GetSharedDataDir();
  return &m_SData_Dir;
}

wxString* BasePlatform::GetPrivateDataDirPtr() {
  if (m_PrivateDataDir.IsEmpty()) GetPrivateDataDir();
  return &m_PrivateDataDir;
}

wxString& BasePlatform::GetSharedDataDir() {	
  if (m_SData_Dir.IsEmpty()) {
    //      Establish a "shared data" location
    /*  From the wxWidgets documentation...
     *
     *     wxStandardPaths::GetDataDir
     *     wxString GetDataDir() const
     *     Return the location of the applications global, i.e. not
     * user-specific, data files. Unix: prefix/share/appname Windows: the
     * directory where the executable file is located Mac:
     * appname.app/Contents/SharedSupport bundle subdirectory
     */
    wxStandardPaths& std_path = GetStdPaths();
    m_SData_Dir = std_path.GetDataDir();
    appendOSDirSlash(&m_SData_Dir);

    if (g_bportable) m_SData_Dir = GetHomeDir();
  }

  return m_SData_Dir;
}

wxString GetPluginDataDir(const char* plugin_name) {
  static const wxString sep = wxFileName::GetPathSeparator();

  wxString datadirs = g_BasePlatform->GetPluginDataPath();  
  wxStringTokenizer dirs(datadirs, ";");
  while (dirs.HasMoreTokens()) {
    wxString dir = dirs.GetNextToken();
    wxFileName tryDirName(dir);
    wxDir tryDir;
    if (!tryDir.Open(tryDirName.GetFullPath())) continue;
    wxString next;
    bool more = tryDir.GetFirst(&next);
    while (more) {
      if (next == plugin_name) {
        next = next.Prepend(tryDirName.GetFullPath() + sep);
        return next;
      }
      more = tryDir.GetNext(&next);
    }
    tryDir.Close();
  }
  printf("Warning: no data directory found, using \"\"");
  return "";
}

wxString& BasePlatform::GetPrivateDataDir() {
  if (m_PrivateDataDir.IsEmpty()) {
    //      Establish the prefix of the location of user specific data files
    wxStandardPaths& std_path = GetStdPaths();

#ifdef __WXMSW__
    m_PrivateDataDir = GetHomeDir();  // should be {Documents and Settings}\......
#elif defined FLATPAK
    std::string config_home;
    if (getenv("XDG_CONFIG_HOME")) {
      config_home = getenv("XDG_CONFIG_HOME");
    } else {
      config_home = getenv("HOME");
      config_home += "/.var/app/org.opencpn.OpenCPN/config";
    }
    m_PrivateDataDir = config_home + "/opencpn";

#elif defined __WXOSX__
    m_PrivateDataDir =
        std_path.GetUserConfigDir();  // should be ~/Library/Preferences
    appendOSDirSlash(&m_PrivateDataDir);
    m_PrivateDataDir.Append(_T("opencpn"));
#else
    m_PrivateDataDir = std_path.GetUserDataDir();    // should be ~/.opencpn
#endif

    if (g_bportable) {
      m_PrivateDataDir = GetHomeDir();
      if (m_PrivateDataDir.Last() == wxFileName::GetPathSeparator())
        m_PrivateDataDir.RemoveLast();
    }
  }
  return m_PrivateDataDir;
}

wxString BasePlatform::GetWinPluginBaseDir() {
  if (g_winPluginDir != "") {
    wxFileName fn(g_winPluginDir);
    if (!fn.DirExists()) {
      printf("Plugin dir %s does not exist\n", (const char *)(fn.GetFullPath().mb_str(wxConvUTF8)));
    }
    fn.Normalize();
    return fn.GetFullPath();
  }
  wxString winPluginDir;
  // Portable case: plugins directory is in the .exe folder
  if (g_bportable) {
    winPluginDir = (GetHomeDir() + _T("plugins"));
    if (ocpn::exists(winPluginDir.ToStdString())) {
      return winPluginDir;
    }
  }
  // Standard case: c:\Users\%USERPROFILE%\AppData\Local
  bool ok = wxGetEnv(_T("LOCALAPPDATA"), &winPluginDir);
  if (!ok) {
    // Without %LOCALAPPDATA%: Use default location if it exists.
    std::string path(wxGetHomeDir().ToStdString());
    path += "\\AppData\\Local";
    if (ocpn::exists(path)) {
      winPluginDir = wxString(path.c_str());
      ok = true;
    }
  }
  if (!ok) {
    // Usually: c:\Users\%USERPROFILE%\AppData\Roaming
    ok = wxGetEnv(_T("APPDATA"), &winPluginDir);
  }
  if (!ok) {
    // Without %APPDATA%: Use default location if it exists.
    std::string path(wxGetHomeDir().ToStdString());
    path += "\\AppData\\Roaming";
    if (ocpn::exists(path)) {
      winPluginDir = wxString(path.c_str());
      ok = true;
    }
  }
  if (!ok) {
    // {Documents and Settings}\.. on W7, else \ProgramData
    winPluginDir = GetHomeDir();
  }
  wxFileName path(winPluginDir);
  path.Normalize();
  winPluginDir = path.GetFullPath() + "\\opencpn\\plugins";
  return winPluginDir;
}

wxString& BasePlatform::GetPluginDir() {
  if (m_PluginsDir.IsEmpty()) {
    wxStandardPaths& std_path = GetStdPaths();

    //  Get the PlugIns directory location
    m_PluginsDir = std_path.GetPluginsDir();  // linux:   {prefix}/lib/opencpn
    // Mac:     appname.app/Contents/PlugIns
#ifdef __WXMSW__
    m_PluginsDir += _T("\\plugins");  // Windows: {exe dir}/plugins
#endif
    if (g_bportable) {
      m_PluginsDir = GetHomeDir();
      m_PluginsDir += _T("plugins");
    }
  }
  return m_PluginsDir;
}

wxString* BasePlatform::GetPluginDirPtr() {
  if (m_PluginsDir.IsEmpty()) GetPluginDir();
  return &m_PluginsDir;
}

bool BasePlatform::isPlatformCapable(int flag) {
  return true;
}

void appendOSDirSlash(wxString* pString) {
  wxChar sep = wxFileName::GetPathSeparator();
  if (pString->Last() != sep) pString->Append(sep);
}

wxString BasePlatform::GetWritableDocumentsDir() {
  wxString dir;

  wxStandardPaths& std_path = GetStdPaths();
  dir = std_path.GetDocumentsDir();

  return dir;
}

bool BasePlatform::DetectOSDetail(OCPN_OSDetail* detail) {
  if (!detail) return false;

  // We take some defaults from build-time definitions
  detail->osd_name = std::string(PKG_TARGET);
  detail->osd_version = std::string(PKG_TARGET_VERSION);

  // Now parse by basic platform
#ifdef __linux__
  if (wxFileExists(_T("/etc/os-release"))) {
    wxTextFile release_file(_T("/etc/os-release"));
    if (release_file.Open()) {
      wxString val;
      for (wxString str = release_file.GetFirstLine(); !release_file.Eof();
           str = release_file.GetNextLine()) {
        if (str.StartsWith(_T("NAME"))) {
          val = str.AfterFirst('=').Mid(1);
          val = val.Mid(0, val.Length() - 1);
          if (val.Length()) detail->osd_name = std::string(val.mb_str());
        } else if (str.StartsWith(_T("VERSION_ID"))) {
          val = str.AfterFirst('=').Mid(1);
          val = val.Mid(0, val.Length() - 1);
          if (val.Length()) detail->osd_version = std::string(val.mb_str());
        } else if (str.StartsWith(_T("ID="))) {
          val = str.AfterFirst('=');
          if (val.Length()) detail->osd_ID = ocpn::split(val.mb_str(), " ")[0];
        } else if (str.StartsWith(_T("ID_LIKE"))) {
          if (val.StartsWith('"')) {
            val = str.AfterFirst('=').Mid(1);
            val = val.Mid(0, val.Length() - 1);
          } else {
            val = str.AfterFirst('=');
          }

          if (val.Length()) {
            detail->osd_names_like = ocpn::split(val.mb_str(), " ");
          }
        }
      }

      release_file.Close();
    }
    if (detail->osd_name == _T("Linux Mint")) {
      if (wxFileExists(_T("/etc/upstream-release/lsb-release"))) {
        wxTextFile upstream_release_file(
            _T("/etc/upstream-release/lsb-release"));
        if (upstream_release_file.Open()) {
          wxString val;
          for (wxString str = upstream_release_file.GetFirstLine();
               !upstream_release_file.Eof();
               str = upstream_release_file.GetNextLine()) {
            if (str.StartsWith(_T("DISTRIB_RELEASE"))) {
              val = str.AfterFirst('=').Mid(0);
              val = val.Mid(0, val.Length());
              if (val.Length()) detail->osd_version = std::string(val.mb_str());
            }
          }
          upstream_release_file.Close();
        }
      }
    }
  }
#endif

  //  Set the default processor architecture
  detail->osd_arch = std::string("x86_64");

  // then see what is actually running.
  wxPlatformInfo platformInfo = wxPlatformInfo::Get();
  wxArchitecture arch = platformInfo.GetArchitecture();
  if (arch == wxARCH_32) detail->osd_arch = std::string("i386");

#ifdef ocpnARM
  //  arm supports a multiarch runtime environment
  //  That is, the OS may be 64 bit, but OCPN may be built as a 32 bit binary
  //  So, we cannot trust the wxPlatformInfo architecture determination.
  detail->osd_arch = std::string("arm64");
#ifdef ocpnARMHF
  detail->osd_arch = std::string("armhf");
#endif
#endif

#ifdef __ANDROID__
  detail->osd_arch = std::string("arm64");
  if (arch == wxARCH_32) detail->osd_arch = std::string("armhf");
#endif

  return true;
}

wxString& BasePlatform::GetConfigFileName() {
  if (m_config_file_name.IsEmpty()) {
    //      Establish the location of the config file
    wxStandardPaths& std_path = GetStdPaths();

#ifdef __WXMSW__
    m_config_file_name = "wmsserver.conf";
    m_config_file_name.Prepend(GetHomeDir());

#elif defined __WXOSX__
    m_config_file_name =
        std_path.GetUserConfigDir();  // should be ~/Library/Preferences
    appendOSDirSlash(&m_config_file_name);
    m_config_file_name.Append("wmsserver");
    appendOSDirSlash(&m_config_file_name);
    m_config_file_name.Append("wmsserver.conf");
#elif defined FLATPAK
    m_config_file_name = GetPrivateDataDir();
    m_config_file_name.Append("/wmsserver.conf");
    // Usually ~/.var/app/org.opencpn.OpenCPN/config/opencpn.conf
#else
    m_config_file_name = std_path.GetUserDataDir();  // should be ~/.opencpn
    appendOSDirSlash(&m_config_file_name);
    m_config_file_name.Append("wmsserver.conf");
#endif

    if (g_bportable) {
      m_config_file_name = GetHomeDir();
      m_config_file_name += "wmsserver.conf";
    }
  }
  return m_config_file_name;
}

bool BasePlatform::InitializeLogFile(void) {
  //      Establish Log File location
  mlog_file = GetPrivateDataDir();
  appendOSDirSlash(&mlog_file);

#ifdef __WXOSX__

  wxFileName LibPref(mlog_file);  // starts like "~/Library/Preferences/opencpn"
  LibPref.RemoveLastDir();        // takes off "opencpn"
  LibPref.RemoveLastDir();        // takes off "Preferences"

  mlog_file = LibPref.GetFullPath();
  appendOSDirSlash(&mlog_file);

  mlog_file.Append(_T("Logs/"));  // so, on OS X, opencpn.log ends up in
                                  // ~/Library/Logs which makes it accessible to
                                  // Applications/Utilities/Console....
#endif

  // create the opencpn "home" directory if we need to
  wxFileName wxHomeFiledir(GetHomeDir());
  if (true != wxHomeFiledir.DirExists(wxHomeFiledir.GetPath()))
    if (!wxHomeFiledir.Mkdir(wxHomeFiledir.GetPath())) {
      wxASSERT_MSG(false, _T("Cannot create opencpn home directory"));
      return false;
    }

  // create the opencpn "log" directory if we need to
  wxFileName wxLogFiledir(mlog_file);
  if (true != wxLogFiledir.DirExists(wxLogFiledir.GetPath())) {
    if (!wxLogFiledir.Mkdir(wxLogFiledir.GetPath())) {
      wxASSERT_MSG(false, _T("Cannot create opencpn log directory"));
      return false;
    }
  }

  mlog_file.Append(_T("wmsserver.log"));
  wxString logit = mlog_file;
  
  //  Constrain the size of the log file
  if (::wxFileExists(mlog_file)) {
    if (wxFileName::GetSize(mlog_file) > 1000000) {
      wxString oldlog = mlog_file;
      oldlog.Append(_T(".log"));
      //  Defer the showing of this messagebox until the system locale is
      //  established.
      large_log_message = (_T("Old log will be moved to opencpn.log.log"));
      ::wxRenameFile(mlog_file, oldlog);
    }
  }

  if (wxLog::GetLogLevel() > wxLOG_User) wxLog::SetLogLevel(wxLOG_Info);

#if CLIAPP
  wxLog::SetActiveTarget(new wxLogStderr);
  wxLog::SetTimestamp("");
  wxLog::SetLogLevel(wxLOG_Warning);
#else
  g_logger = new OcpnLog(mlog_file.mb_str());
  m_Oldlogger = wxLog::SetActiveTarget(g_logger);
#endif

  return true;
}

void BasePlatform::CloseLogFile(void) {
  if (g_logger) {
    wxLog::SetActiveTarget(m_Oldlogger);
    delete g_logger;
  }
}

wxString BasePlatform::GetPluginDataPath() {
  if (g_bportable) {
    wxString sep = wxFileName::GetPathSeparator();
    wxString ret = GetPrivateDataDir() + sep + _T("plugins");
    return ret;
  }

  if (m_pluginDataPath != "") {
    return m_pluginDataPath;
  }
  wxString dirs("");

  auto const osSystemId = wxPlatformInfo::Get().GetOperatingSystemId();
  if (isFlatpacked()) {
    dirs = "~/.var/app/org.opencpn.OpenCPN/data/opencpn/plugins";
  } else if (osSystemId & wxOS_UNIX_LINUX) {
    dirs = GetLinuxDataPath();
  } else if (osSystemId & wxOS_WINDOWS) {
    dirs = GetWinPluginBaseDir();
  } else if (osSystemId & wxOS_MAC) {
    dirs = "/Applications/OpenCPN.app/Contents/SharedSupport/plugins;";
    dirs +=
        "~/Library/Application Support/OpenCPN/Contents/SharedSupport/plugins";
  }

  m_pluginDataPath = ExpandPaths(dirs, this);
  if (m_pluginDataPath != "") {
    m_pluginDataPath += ";";
  }
  m_pluginDataPath += GetPluginDir();
  if (m_pluginDataPath.EndsWith(wxFileName::GetPathSeparator())) {
    m_pluginDataPath.RemoveLast();
  }
  return m_pluginDataPath;
}


#ifdef __ANDROID__
void BasePlatform::ShowBusySpinner() { androidShowBusyIcon(); }
#elif defined(CLIAPP)
void BasePlatform::ShowBusySpinner() { }
#else
void BasePlatform::ShowBusySpinner() { ::wxBeginBusyCursor(); }
#endif

#ifdef __ANDROID__
void BasePlatform::HideBusySpinner() { androidHideBusyIcon(); }
#elif defined(CLIAPP)
void BasePlatform::HideBusySpinner() { }
#else
void BasePlatform::HideBusySpinner() { ::wxEndBusyCursor(); }
#endif

// getDisplaySize

#ifdef CLIAPP
wxSize BasePlatform::getDisplaySize() { return wxSize(); }

#elif defined(__ANDROID__)
wxSize BasePlatform::getDisplaySize() { return getAndroidDisplayDimensions(); }

#else
wxSize BasePlatform::getDisplaySize() {
  if (m_displaySize.x < 10)
    m_displaySize = ::wxGetDisplaySize();  // default, for most platforms
  return m_displaySize;
}
#endif

// GetDisplaySizeMM

#ifdef CLIAPP
double BasePlatform::GetDisplaySizeMM() { return 1.0; }

#else
double BasePlatform::GetDisplaySizeMM() {

  if (m_displaySizeMMOverride > 0) return m_displaySizeMMOverride;

  if (m_displaySizeMM.x < 1) m_displaySizeMM = wxGetDisplaySizeMM();

  double ret = m_displaySizeMM.GetWidth();

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

  wxLogDebug("Detected display size (horizontal): %d mm", (int)ret);
  return ret;
}
#endif   // CLIAPP


#ifdef CLIAPP
double BasePlatform::GetDisplayDPmm() { return 1.0; }
#else
double BasePlatform::GetDisplayDPmm() {
  double r = getDisplaySize().x;  // dots
  return r / GetDisplaySizeMM();
}
#endif


double BasePlatform::GetDisplayDIPMult(wxWindow *win) {
  double rv = 1.0;
#ifdef __WXMSW__
  if (win)
    rv = (double)(win->ToDIP(100))/100.;
#endif
  return rv;
}

unsigned int BasePlatform::GetSelectRadiusPix() {
  return GetDisplayDPmm() * g_selection_radius_mm;
}

#ifdef __WXMSW__

#define NAME_SIZE 128

const GUID GUID_CLASS_MONITOR = {0x4d36e96e, 0xe325, 0x11ce, 0xbf, 0xc1, 0x08,
                                 0x00,       0x2b,   0xe1,   0x03, 0x18};

// Assumes hDevRegKey is valid
bool GetMonitorSizeFromEDID(const HKEY hDevRegKey, int *WidthMm,
                            int *HeightMm) {
  DWORD dwType, AcutalValueNameLength = NAME_SIZE;
  TCHAR valueName[NAME_SIZE];

  BYTE EDIDdata[1024];
  DWORD edidsize = sizeof(EDIDdata);

  for (LONG i = 0, retValue = ERROR_SUCCESS; retValue != ERROR_NO_MORE_ITEMS;
       ++i) {
    retValue = RegEnumValue(hDevRegKey, i, &valueName[0],
                            &AcutalValueNameLength, NULL, &dwType,
                            EDIDdata,    // buffer
                            &edidsize);  // buffer size

    if (retValue != ERROR_SUCCESS || 0 != _tcscmp(valueName, _T("EDID")))
      continue;

    *WidthMm = ((EDIDdata[68] & 0xF0) << 4) + EDIDdata[66];
    *HeightMm = ((EDIDdata[68] & 0x0F) << 8) + EDIDdata[67];

    return true;  // valid EDID found
  }

  return false;  // EDID not found
}

bool GetSizeForDevID(wxString &TargetDevID, int *WidthMm, int *HeightMm) {
  HDEVINFO devInfo =
      SetupDiGetClassDevsEx(&GUID_CLASS_MONITOR,  // class GUID
                            NULL,                 // enumerator
                            NULL,                 // HWND
                            DIGCF_PRESENT,        // Flags //DIGCF_ALLCLASSES|
                            NULL,   // device info, create a new one.
                            NULL,   // machine name, local machine
                            NULL);  // reserved

  if (NULL == devInfo) return false;

  bool bRes = false;

  for (ULONG i = 0; ERROR_NO_MORE_ITEMS != GetLastError(); ++i) {
    SP_DEVINFO_DATA devInfoData;
    memset(&devInfoData, 0, sizeof(devInfoData));
    devInfoData.cbSize = sizeof(devInfoData);

    if (SetupDiEnumDeviceInfo(devInfo, i, &devInfoData)) {
      wchar_t Instance[80];
      SetupDiGetDeviceInstanceId(devInfo, &devInfoData, Instance, MAX_PATH,
                                 NULL);
      wxString instance(Instance);
      if (instance.Upper().Find(TargetDevID.Upper()) == wxNOT_FOUND) continue;

      HKEY hDevRegKey = SetupDiOpenDevRegKey(
          devInfo, &devInfoData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);

      if (!hDevRegKey || (hDevRegKey == INVALID_HANDLE_VALUE)) continue;

      bRes = GetMonitorSizeFromEDID(hDevRegKey, WidthMm, HeightMm);

      RegCloseKey(hDevRegKey);
    }
  }
  SetupDiDestroyDeviceInfoList(devInfo);
  return bRes;
}

bool BasePlatform::GetWindowsMonitorSize(int *width, int *height) {
  bool bFoundDevice = true;

  if (m_monitorWidth < 10) {
    int WidthMm = 0;
    int HeightMm = 0;

    DISPLAY_DEVICE dd;
    dd.cb = sizeof(dd);
    DWORD dev = 0;  // device index
    int id = 1;     // monitor number, as used by Display Properties > Settings

    wxString DeviceID;
    bFoundDevice = false;
    while (EnumDisplayDevices(0, dev, &dd, 0) && !bFoundDevice) {
      DISPLAY_DEVICE ddMon;
      ZeroMemory(&ddMon, sizeof(ddMon));
      ddMon.cb = sizeof(ddMon);
      DWORD devMon = 0;

      while (EnumDisplayDevices(dd.DeviceName, devMon, &ddMon, 0) &&
             !bFoundDevice) {
        if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE &&
            !(ddMon.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER)) {
          DeviceID = wxString(ddMon.DeviceID, wxConvUTF8);
          DeviceID = DeviceID.Mid(8);
          DeviceID = DeviceID.Mid(0, DeviceID.Find('\\'));

          bFoundDevice = GetSizeForDevID(DeviceID, &WidthMm, &HeightMm);
        }
        devMon++;

        ZeroMemory(&ddMon, sizeof(ddMon));
        ddMon.cb = sizeof(ddMon);
      }

      ZeroMemory(&dd, sizeof(dd));
      dd.cb = sizeof(dd);
      dev++;
    }
    m_monitorWidth = WidthMm;
    m_monitorHeight = HeightMm;
  }

  if (width) *width = m_monitorWidth;
  if (height) *height = m_monitorHeight;

  return bFoundDevice;
}

#endif


