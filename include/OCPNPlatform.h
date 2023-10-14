/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN Platform specific support utilities
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2015 by David S. Register                               *
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

#ifndef OCPNPLATFORM_H
#define OCPNPLATFORM_H

#include <cstdio>
#include <string>
#include <vector>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif  // precompiled headers

#include <wx/bmpbuttn.h>
#include <wx/clrpicker.h>
#include <wx/colordlg.h>
#include <wx/colour.h>
#include <wx/colourdata.h>
#include <wx/dirdlg.h>
#include <wx/filedlg.h>
#include <wx/gdicmn.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/validate.h>
#include <wx/window.h>

#include "base_platform.h"

class MyConfig;
class ArrayOfCDI;


//--------------------------------------------------------------------------
//      Per-Platform Utility support
//--------------------------------------------------------------------------

// #ifdef __WXQT__
// extern bool LoadQtStyleSheet(wxString &sheet_file);
// extern QString getQtStyleSheet( void );
// #endif

class OCPNPlatform : public BasePlatform {
public:
  OCPNPlatform();
  virtual ~OCPNPlatform();

  static bool hasInternalBT(wxString profile = _T(""));  // Bluetooth
  bool startBluetoothScan();
  wxArrayString getBluetoothScanResults();
  bool stopBluetoothScan();

  //  Per-Platform initialization support

  //  Called from MyApp() immediately upon entry to MyApp::OnInit()
  static void Initialize_1(void);

  void Initialize_2(void);
  void Initialize_3();

  //  Called from MyApp() just before end of MyApp::OnInit()
  static void Initialize_4(void);

  static void OnExit_1(void);
  static void OnExit_2(void);

  void SetDefaultOptions(void);
  void SetUpgradeOptions(wxString vString, wxString vStringConfig);

  //--------------------------------------------------------------------------
  //      Platform Display Support
  //--------------------------------------------------------------------------
  static void ShowBusySpinner(void);
  static void HideBusySpinner(void);
  double getFontPointsperPixel(void);
  wxSize getDisplaySize();
  double GetDisplaySizeMM();
  double GetDisplayAreaCM2();
  virtual double GetDisplayDPmm();

  void SetDisplaySizeMM(double size);
  unsigned int GetSelectRadiusPix();
  double GetToolbarScaleFactor(int GUIScaleFactor);

  void PositionAISAlert(wxWindow *alert_window);
  float GetChartScaleFactorExp(float scale_linear);
  float GetMarkScaleFactorExp(float scale_linear);
  //float GetDIPScaleFactor();
  int GetStatusBarFieldCount();
  bool GetFullscreen();
  bool SetFullscreen(bool bFull);
  bool AllowAlertDialog(const wxString &class_name);
  double GetDisplayDensityFactor();
  double m_pt_per_pixel;
  long GetDefaultToolbarOrientation();

  //--------------------------------------------------------------------------
  //      Per-Platform file/directory support
  //--------------------------------------------------------------------------

    MyConfig *GetConfigObject();
  int DoFileSelectorDialog(wxWindow *parent, wxString *file_spec,
                           wxString Title, wxString initDir,
                           wxString suggestedName, wxString wildcard);  

  //--------------------------------------------------------------------------
  //      Per-Platform Utility support
  //--------------------------------------------------------------------------
  void setChartTypeMaskSel(int mask, wxString &indicator);
  bool isPlatformCapable(int flag);
  void LaunchLocalHelp();

  int platformApplyPrivateSettingsString(wxString settings,
                                         ArrayOfCDI *pDirArray);
  void platformLaunchDefaultBrowser(wxString URL);

  void SetLocaleSearchPrefixes(void);
  wxString GetDefaultSystemLocale();
  float GetChartScaleFactorExp();

#if wxUSE_XLOCALE
  wxString GetAdjustedAppLocale();
  wxString ChangeLocale(wxString &newLocaleID, wxLocale *presentLocale,
                        wxLocale **newLocale);
#endif
private:
  wxString m_SData_Dir;


};

//      Private colourPicker control
//--------------------------------------------------------------------------

class OCPNColourPickerCtrl : public wxBitmapButton {
public:
  OCPNColourPickerCtrl();
  OCPNColourPickerCtrl(wxWindow *parent, wxWindowID id,
                       const wxColour &initial = *wxBLACK,
                       const wxPoint &pos = wxDefaultPosition,
                       const wxSize &size = wxDefaultSize, long style = 0,
                       const wxValidator &validator = wxDefaultValidator,
                       const wxString &name = _T(""));

  bool Create(wxWindow *parent, wxWindowID id,
              const wxColour &initial = *wxBLACK,
              const wxPoint &pos = wxDefaultPosition,
              const wxSize &size = wxDefaultSize, long style = 0,
              const wxValidator &validator = wxDefaultValidator,
              const wxString &name = _T(""));

  void OnButtonClick(wxCommandEvent &WXUNUSED(ev));
  void InitColourData();
  void SetColour(wxColour &c);
  wxColour GetColour(void);

protected:
  virtual void UpdateColour();
  wxSize DoGetBestSize() const;

  void OnPaint(wxPaintEvent &event);

  DECLARE_EVENT_TABLE();

private:
  wxBitmap m_bitmap;
  wxColour m_colour;
  wxColourData ms_data;
};

#endif  // guard
