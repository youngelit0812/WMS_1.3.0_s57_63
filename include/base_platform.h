#ifndef BASEPLATFORM_H
#define BASEPLATFORM_H

#include <stdio.h>
#include <vector>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif  // precompiled headers

#include <wx/gdicmn.h>
#include <wx/log.h>
#include <wx/stdpaths.h>
#include <wx/clrpicker.h>
#include <wx/colourdata.h>
#include <wx/colordlg.h>


#define PLATFORM_CAP_PLUGINS 1
#define PLATFORM_CAP_FASTPAN 2

typedef struct {
  char tsdk[20];
  char hn[20];
  char msdk[20];
} PlatSpec;

void appendOSDirSlash(wxString* path);

struct OCPN_OSDetail {
  OCPN_OSDetail(){};
  ~OCPN_OSDetail(){};

  std::string osd_name;
  std::string osd_version;
  std::vector<std::string> osd_names_like;
  std::string osd_arch;
  std::string osd_ID;
};

class BasePlatform {
public:
  BasePlatform();
  virtual ~BasePlatform() {}

  wxString& GetPrivateDataDir();
  wxString* GetPluginDirPtr();
  wxString* GetSharedDataDirPtr();
  wxString* GetPrivateDataDirPtr();

  /** The original in-tree plugin directory, sometimes not user-writable.*/
  wxString& GetPluginDir();

  wxStandardPaths& GetStdPaths();

  /**
   * Base directory for user writable windows plugins, reflects
   * winPluginDir option, defaults to %LOCALAPPDATA%/opencpn.
   **/
  wxString GetWinPluginBaseDir();

  wxString& GetSharedDataDir();
  wxString& GetExePath();
  wxString& GetHomeDir();
  wxString GetWritableDocumentsDir();

  /**
   * Return ';'-separated list of base directories for plugin data. The
   * list always includes the main installation directory. Some platforms
   * prepends this with user-writable path(s), each of which ending in
   * "opencpn". All paths are guaranteed to exist.
   *
   * For Linux, return paths ending in .../share.
   */
  wxString GetPluginDataPath();

  wxString& GetConfigFileName();
  wxString& GetLogFileName() { return mlog_file; }

  bool isFlatpacked() { return m_isFlatpacked; }

  bool isPlatformCapable(int flag);
  OCPN_OSDetail* GetOSDetail() { return m_osDetail; }

  void CloseLogFile(void);
  bool InitializeLogFile(void);
  wxString& GetLargeLogMessage(void) { return large_log_message; }
  FILE* GetLogFilePtr() { return flog; }

  wxString NormalizePath(const wxString& full_path);

  virtual wxSize getDisplaySize();
  virtual double GetDisplaySizeMM();
  virtual double GetDisplayDPmm();
  virtual unsigned int GetSelectRadiusPix();
  double GetDisplayDIPMult(wxWindow *win);

protected:
  bool DetectOSDetail(OCPN_OSDetail* detail);

  wxString m_PrivateDataDir;
  wxString m_PluginsDir;
  bool m_isFlatpacked;
  wxString m_homeDir;
  wxString m_exePath;
  wxString m_SData_Dir;
  wxString m_config_file_name;
  wxString m_pluginDataPath;
  wxString mlog_file;

  OCPN_OSDetail* m_osDetail;

  FILE* flog;
  wxLog* m_Oldlogger;
  wxString large_log_message;

  wxSize m_displaySize;
  wxSize m_displaySizeMM;
  int m_displaySizeMMOverride;


#ifdef _MSC_VER
  bool GetWindowsMonitorSize(int *width, int *height);
#endif
  int m_monitorWidth, m_monitorHeight;
  bool m_bdisableWindowsDisplayEnum;
};

#endif  //  BASEPLATFORM_H
