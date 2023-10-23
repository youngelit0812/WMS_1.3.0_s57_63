/***************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  CanvasMenuHandler
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

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#include <wx/aui/aui.h>
#include <wx/clipbrd.h>
#include <wx/dynarray.h>
#include <wx/event.h>
#include <wx/font.h>
#include <wx/gdicmn.h>
#include <wx/graphics.h>
#include <wx/image.h>
#include <wx/listbook.h>
#include <wx/listimpl.cpp>
#include <wx/menu.h>

#include "FontMgr.h"
#include "Quilt.h"
#include "TCWin.h"
#include "canvasMenu.h"
#include "chartdb.h"
#include "chcanv.h"
#include "cm93.h"      // for chart outline draw
#include "config.h"
#include "config_vars.h"
#include "cutil.h"
#include "georef.h"
#include "kml.h"
#include "navutil.h"
#include "ocpn_frame.h"
#include "own_ship.h"
#include "pluginmanager.h"
#include "s52plib.h"
#include "s57chart.h"  // for ArrayOfS57Obj
#include "styles.h"
#include "tcmgr.h"
#include "tide_time.h"
#include "mDNS_query.h"
#include "OCPNPlatform.h"

// ----------------------------------------------------------------------------
// Useful Prototypes
// ----------------------------------------------------------------------------
extern bool g_bCPAWarn;
extern bool g_bShowAreaNotices;
extern bool bGPSValid;
extern bool g_bskew_comp;
extern double vLat, vLon;
extern MyFrame *gFrame;
extern ChartGroupArray *g_pGroupArray;
extern PlugInManager *g_pi_manager;
extern int g_nAWMax;
extern int g_nAWDefault;
extern wxString g_AW1GUID;
extern wxString g_AW2GUID;
extern int g_click_stop;
extern bool g_bConfirmObjectDelete;
extern MyConfig *pConfig;
extern OCPNPlatform* g_Platform;

extern CM93OffsetDialog *g_pCM93OffsetDialog;

extern wxString g_default_wp_icon;
extern bool g_bBasicMenus;
extern double gHdt;
extern bool g_FlushNavobjChanges;
extern ColorScheme global_color_scheme;
extern std::vector<std::shared_ptr<ocpn_DNS_record_t>> g_DNS_cache;
extern wxDateTime g_DNS_cache_time;

//    Constants for right click menus
enum {
  ID_DEF_MENU_MAX_DETAIL = 1,
  ID_DEF_MENU_SCALE_IN,
  ID_DEF_MENU_SCALE_OUT,
  ID_DEF_MENU_DROP_WP,
  ID_DEF_MENU_NEW_RT,
  ID_DEF_MENU_QUERY,
  ID_DEF_MENU_MOVE_BOAT_HERE,
  ID_DEF_MENU_GOTO_HERE,
  ID_DEF_MENU_GOTOPOSITION,

  ID_WP_MENU_GOTO,
  ID_WP_MENU_DELPOINT,
  ID_WP_MENU_PROPERTIES,
  ID_RT_MENU_ACTIVATE,
  ID_RT_MENU_DEACTIVATE,
  ID_RT_MENU_INSERT,
  ID_RT_MENU_APPEND,
  ID_RT_MENU_COPY,
  ID_RT_MENU_SPLIT_LEG,
  ID_RT_MENU_SPLIT_WPT,
  ID_TK_MENU_COPY,
  ID_WPT_MENU_COPY,
  ID_WPT_MENU_SENDTOGPS,
  ID_WPT_MENU_SENDTONEWGPS,
  ID_WPT_MENU_SENDTOPEER,
  ID_PASTE_WAYPOINT,
  ID_PASTE_ROUTE,
  ID_PASTE_TRACK,
  ID_RT_MENU_DELETE,
  ID_RT_MENU_REVERSE,
  ID_RT_MENU_DELPOINT,
  ID_RT_MENU_ACTPOINT,
  ID_RT_MENU_DEACTPOINT,
  ID_RT_MENU_ACTNXTPOINT,
  ID_RT_MENU_REMPOINT,
  ID_RT_MENU_PROPERTIES,
  ID_RT_MENU_SENDTOGPS,
  ID_RT_MENU_SENDTONEWGPS,
  ID_RT_MENU_SHOWNAMES,
  ID_RT_MENU_RESEQUENCE,
  ID_RT_MENU_SENDTOPEER,
  ID_WP_MENU_SET_ANCHORWATCH,
  ID_WP_MENU_CLEAR_ANCHORWATCH,
  ID_DEF_MENU_AISTARGETLIST,
  ID_DEF_MENU_AIS_CPAWARNING,

  ID_RC_MENU_SCALE_IN,
  ID_RC_MENU_SCALE_OUT,
  ID_RC_MENU_ZOOM_IN,
  ID_RC_MENU_ZOOM_OUT,
  ID_RC_MENU_FINISH,
  ID_DEF_MENU_AIS_QUERY,
  ID_DEF_MENU_AIS_CPA,
  ID_DEF_MENU_AISSHOWTRACK,
  ID_DEF_MENU_ACTIVATE_MEASURE,
  ID_DEF_MENU_DEACTIVATE_MEASURE,
  ID_DEF_MENU_COPY_MMSI,

  ID_UNDO,
  ID_REDO,

  ID_DEF_MENU_CM93OFFSET_DIALOG,

  ID_TK_MENU_PROPERTIES,
  ID_TK_MENU_DELETE,
  ID_TK_MENU_SENDTOPEER,
  ID_WP_MENU_ADDITIONAL_INFO,

  ID_DEF_MENU_QUILTREMOVE,
  ID_DEF_MENU_COGUP,
  ID_DEF_MENU_NORTHUP,
  ID_DEF_MENU_HEADUP,
  ID_DEF_MENU_TOGGLE_FULL,
  ID_DEF_MENU_TIDEINFO,
  ID_DEF_MENU_CURRENTINFO,
  ID_DEF_ZERO_XTE,

  ID_DEF_MENU_GROUPBASE,  // Must be last entry, as chart group identifiers are
                          // created dynamically

  ID_DEF_MENU_LAST
};

//------------------------------------------------------------------------------
//    CanvasMenuHandler Implementation
//------------------------------------------------------------------------------
int CanvasMenuHandler::GetNextContextMenuId() {
  return ID_DEF_MENU_LAST +
         100;  // Allowing for 100 dynamic menu item identifiers
}

wxFont CanvasMenuHandler::m_scaledFont;

// Define a constructor for my canvas
CanvasMenuHandler::CanvasMenuHandler(ChartCanvas *parentCanvas,
                                     int selectedAIS_MMSI,
                                     void *selectedTCIndex)

{
  parent = parentCanvas;  
  m_FoundAIS_MMSI = selectedAIS_MMSI;
  m_pIDXCandidate = selectedTCIndex;
  if (!m_scaledFont.IsOk()){
    wxFont *qFont = GetOCPNScaledFont(_("Menu"));
    m_scaledFont = *qFont;
  }

  m_DIPFactor =g_Platform->GetDisplayDIPMult(gFrame);

}

CanvasMenuHandler::~CanvasMenuHandler() {}

//-------------------------------------------------------------------------------
//          Popup Menu Handling
//-------------------------------------------------------------------------------

void CanvasMenuHandler::PrepareMenuItem( wxMenuItem *item ){
#if defined(__WXMSW__)
  wxColour ctrl_back_color = GetGlobalColor(_T("DILG1"));    // Control Background
  item->SetBackgroundColour(ctrl_back_color);
  wxColour menu_text_color = GetGlobalColor(_T ( "UITX1" ));
  item->SetTextColour(menu_text_color);
#endif
}

void CanvasMenuHandler::MenuPrepend1(wxMenu *menu, int id, wxString label) {
  wxMenuItem *item = new wxMenuItem(menu, id, label);
#if defined(__WXMSW__)
  item->SetFont(m_scaledFont);
#endif

#ifdef __OCPN__ANDROID__
  wxFont sFont = GetOCPNGUIScaledFont(_("Menu"));
  item->SetFont(sFont);
#endif

  PrepareMenuItem( item );

  menu->Prepend(item);
}

void CanvasMenuHandler::MenuAppend1(wxMenu *menu, int id, wxString label) {
  wxMenuItem *item = new wxMenuItem(menu, id, label);
#if defined(__WXMSW__)
  item->SetFont(m_scaledFont);
#endif

#ifdef __OCPN__ANDROID__
  wxFont sFont = GetOCPNGUIScaledFont(_T("Menu"));
  item->SetFont(sFont);
#endif

  PrepareMenuItem( item );

  menu->Append(item);
}

void CanvasMenuHandler::SetMenuItemFont1(wxMenuItem *item) {
#if defined(__WXMSW__)
  item->SetFont(m_scaledFont);
#endif

#if defined(__OCPN__ANDROID__)
  wxFont *qFont = GetOCPNScaledFont(_("Menu"));
  item->SetFont(*qFont);
#endif

  PrepareMenuItem( item );
}

void CanvasMenuHandler::CanvasPopupMenu(int x, int y, int seltype) {
  wxMenu *contextMenu = new wxMenu;
  wxMenu *menuWaypoint = NULL;
  wxMenu *menuRoute = NULL;
  wxMenu *menuTrack = NULL;
  wxMenu *menuAIS = NULL;

  wxMenu *subMenuChart = new wxMenu;
  wxMenu *subMenuUndo = new wxMenu("Undo...Ctrl-Z");

#ifdef __WXOSX__
  wxMenu *subMenuRedo = new wxMenu("Redo...Shift-Ctrl-Z");
#else
  wxMenu *subMenuRedo = new wxMenu("Redo...Ctrl-Y");
#endif

  wxMenu *menuFocus = contextMenu;  // This is the one that will be shown

  popx = x;
  popy = y;
   
  int nChartStack = 0;
  if (parent->GetpCurrentStack())
    nChartStack = parent->GetpCurrentStack()->nEntry;

  if (!parent->GetVP().b_quilt) {
    if (nChartStack > 1) {
      MenuAppend1(contextMenu, ID_DEF_MENU_MAX_DETAIL, _("Max Detail Here"));
      MenuAppend1(contextMenu, ID_DEF_MENU_SCALE_IN,
                  _menuText(_("Scale In"), _T("Ctrl-Left")));
      MenuAppend1(contextMenu, ID_DEF_MENU_SCALE_OUT,
                  _menuText(_("Scale Out"), _T("Ctrl-Right")));
    }

    if ((parent->m_singleChart &&
         (parent->m_singleChart->GetChartFamily() == CHART_FAMILY_VECTOR))) {
      MenuAppend1(contextMenu, ID_DEF_MENU_QUERY,
                  _("Object Query") + _T( "..." ));
    }

  } else {
    ChartBase *pChartTest =
        parent->m_pQuilt->GetChartAtPix(parent->GetVP(), wxPoint(x, y));
    if ((pChartTest && (pChartTest->GetChartFamily() == CHART_FAMILY_VECTOR))) {
      MenuAppend1(contextMenu, ID_DEF_MENU_QUERY,
                  _("Object Query") + _T( "..." ));
    } else {
#ifndef __OCPN__ANDROID__
      if (!g_bBasicMenus && (nChartStack > 1)) {
        MenuAppend1(contextMenu, ID_DEF_MENU_SCALE_IN,
                    _menuText(_("Scale In"), _T("Ctrl-Left")));
        MenuAppend1(contextMenu, ID_DEF_MENU_SCALE_OUT,
                    _menuText(_("Scale Out"), _T("Ctrl-Right")));
      }
#endif
    }
  }

  if (!g_bBasicMenus)
    MenuAppend1(contextMenu, ID_DEF_MENU_GOTOPOSITION,
                _("Center view") + _T("..."));

  if (!g_bBasicMenus) {
    if (parent->GetVP().b_quilt) {
      if (parent->GetUpMode() == NORTH_UP_MODE) {
        MenuAppend1(contextMenu, ID_DEF_MENU_COGUP, _("Course Up Mode"));
        if (!std::isnan(gHdt))
          MenuAppend1(contextMenu, ID_DEF_MENU_HEADUP, _("Heading Up Mode"));
      } else {
        MenuAppend1(contextMenu, ID_DEF_MENU_NORTHUP, _("North Up Mode"));
      }
    } else {
      if (parent->m_singleChart &&
          (fabs(parent->m_singleChart->GetChartSkew()) > .01) && !g_bskew_comp)
        MenuAppend1(contextMenu, ID_DEF_MENU_NORTHUP, _("Chart Up Mode"));
      else
        MenuAppend1(contextMenu, ID_DEF_MENU_NORTHUP, _("North Up Mode"));
    }
  }

  if (!g_bBasicMenus) {
    bool full_toggle_added = false;

    if (!full_toggle_added) {
      // if(gFrame->IsFullScreen())
      MenuAppend1(contextMenu, ID_DEF_MENU_TOGGLE_FULL,
                  _("Toggle Full Screen"));
    }

    Kml *kml = new Kml;
    int pasteBuffer = kml->ParsePasteBuffer();
    if (pasteBuffer != KML_PASTE_INVALID) {
      switch (pasteBuffer) {
        case KML_PASTE_WAYPOINT: {
          MenuAppend1(contextMenu, ID_PASTE_WAYPOINT, _("Paste Waypoint"));
          break;
        }
        case KML_PASTE_ROUTE: {
          MenuAppend1(contextMenu, ID_PASTE_ROUTE, _("Paste Route"));
          break;
        }
        case KML_PASTE_TRACK: {
          MenuAppend1(contextMenu, ID_PASTE_TRACK, _("Paste Track"));
          break;
        }
        case KML_PASTE_ROUTE_TRACK: {
          MenuAppend1(contextMenu, ID_PASTE_ROUTE, _("Paste Route"));
          MenuAppend1(contextMenu, ID_PASTE_TRACK, _("Paste Track"));
          break;
        }
      }
    }
    delete kml;

    if (!parent->GetVP().b_quilt && parent->m_singleChart &&
        (parent->m_singleChart->GetChartType() == CHART_TYPE_CM93COMP)) {
      MenuAppend1(contextMenu, ID_DEF_MENU_CM93OFFSET_DIALOG,
                  _("CM93 Offset Dialog..."));
    }

  }  // if( !g_bBasicMenus){

#ifndef __OCPN__ANDROID__
// TODO stack
//     if( ( parent->GetVP().b_quilt ) && ( pCurrentStack &&
//     pCurrentStack->b_valid ) ) {
//         int dbIndex = parent->m_pQuilt->GetChartdbIndexAtPix(
//         parent->GetVP(), wxPoint( popx, popy ) ); if( dbIndex != -1 )
//             MenuAppend1( contextMenu, ID_DEF_MENU_QUILTREMOVE, _( "Hide This
//             Chart" ) );
//     }
#endif

#ifdef __WXMSW__
  //  If we dismiss the context menu without action, we need to discard some
  //  mouse events.... Eat the next 2 button events, which happen as down-up on
  //  MSW XP
  g_click_stop = 2;
#endif

  //  ChartGroup SubMenu
  wxMenuItem *subItemChart = contextMenu->AppendSubMenu(subMenuChart, _("Chart Groups"));

  SetMenuItemFont1(subItemChart);

  if (g_pGroupArray->GetCount()) {
#ifdef __WXMSW__
    MenuAppend1(subMenuChart, wxID_CANCEL, _("temporary"));
#endif
    wxMenuItem *subItem0 = subMenuChart->AppendRadioItem(
        ID_DEF_MENU_GROUPBASE, _("All Active Charts"));

    SetMenuItemFont1(subItem0);

    for (unsigned int i = 0; i < g_pGroupArray->GetCount(); i++) {
      subItem0 = subMenuChart->AppendRadioItem(
          ID_DEF_MENU_GROUPBASE + i + 1, g_pGroupArray->Item(i)->m_group_name);
      SetMenuItemFont1(subItem0);
    }

#ifdef __WXMSW__
    subMenuChart->Remove(wxID_CANCEL);
#endif
    subMenuChart->Check(ID_DEF_MENU_GROUPBASE + parent->m_groupIndex, true);
  }

  //  This is the default context menu
  menuFocus = contextMenu;

  wxString name;  

  enum { WPMENU = 1, TKMENU = 2, RTMENU = 4, MMMENU = 8 };
  int sub_menu = 0;
  if (!g_bBasicMenus && menuFocus != contextMenu) {
    if(global_color_scheme != GLOBAL_COLOR_SCHEME_DUSK &&
                   global_color_scheme != GLOBAL_COLOR_SCHEME_NIGHT ){
      menuFocus->AppendSeparator();
    }
    wxMenuItem *subMenu1;
    if (menuWaypoint && menuFocus != menuWaypoint) {
      subMenu1 =
          menuFocus->AppendSubMenu(menuWaypoint, menuWaypoint->GetTitle());
      SetMenuItemFont1(subMenu1);
      sub_menu |= WPMENU;
#ifdef __WXMSW__
      menuWaypoint->SetTitle(wxEmptyString);
#endif
    }
    if (menuTrack && menuFocus != menuTrack) {
      subMenu1 = menuFocus->AppendSubMenu(menuTrack, menuTrack->GetTitle());
      SetMenuItemFont1(subMenu1);
      sub_menu |= TKMENU;
#ifdef __WXMSW__
      menuTrack->SetTitle(wxEmptyString);
#endif
    }
    if (menuRoute && menuFocus != menuRoute) {
      subMenu1 = menuFocus->AppendSubMenu(menuRoute, menuRoute->GetTitle());
      SetMenuItemFont1(subMenu1);
      sub_menu |= RTMENU;
#ifdef __WXMSW__
      menuRoute->SetTitle(wxEmptyString);
#endif
    }
    subMenu1 = menuFocus->AppendSubMenu(contextMenu, _("Main Menu"));
    SetMenuItemFont1(subMenu1);
    sub_menu |= MMMENU;
  }

  if (!subMenuChart->GetMenuItemCount()) contextMenu->Destroy(subItemChart);

  //  Add the Tide/Current selections if the item was not activated by shortcut
  //  in right-click handlers
  bool bsep = false;
  // Give the plugins a chance to update their menu items
  g_pi_manager->PrepareAllPluginContextMenus();

  //  Add PlugIn Context Menu items
  ArrayOfPlugInMenuItems item_array =
      g_pi_manager->GetPluginContextMenuItemArray();

  for (unsigned int i = 0; i < item_array.GetCount(); i++) {
    PlugInMenuItemContainer *pimis = item_array[i];
    if (!pimis->b_viz) continue;

    wxMenu *submenu = NULL;
    if (pimis->pmenu_item->GetSubMenu()) {
      submenu = new wxMenu();
      const wxMenuItemList &items =
          pimis->pmenu_item->GetSubMenu()->GetMenuItems();
      for (wxMenuItemList::const_iterator it = items.begin(); it != items.end();
           ++it) {
        int id = -1;
        for (unsigned int j = 0; j < item_array.GetCount(); j++) {
          PlugInMenuItemContainer *pimis = item_array[j];
          if (pimis->pmenu_item == *it) id = pimis->id;
        }

        wxMenuItem *pmi = new wxMenuItem(submenu, id,
#if wxCHECK_VERSION(3, 0, 0)
                                         (*it)->GetItemLabelText(),
#else
                                         (*it)->GetLabel(),
#endif
                                         (*it)->GetHelp(), (*it)->GetKind());

#ifdef __WXMSW__
        pmi->SetFont(m_scaledFont);
#endif

#ifdef __OCPN__ANDROID__
        wxFont sFont = GetOCPNGUIScaledFont(_("Menu"));
        pmi->SetFont(sFont);
#endif

        PrepareMenuItem( pmi );
        submenu->Append(pmi);
        pmi->Check((*it)->IsChecked());
      }
    }

    wxMenuItem *pmi = new wxMenuItem(contextMenu, pimis->id,
#if wxCHECK_VERSION(3, 0, 0)
                                     pimis->pmenu_item->GetItemLabelText(),
#else
                                     pimis->pmenu_item->GetLabel(),
#endif
                                     pimis->pmenu_item->GetHelp(),
                                     pimis->pmenu_item->GetKind(), submenu);
#ifdef __WXMSW__
    pmi->SetFont(m_scaledFont);
#endif

#ifdef __OCPN__ANDROID__
    wxFont sFont = GetOCPNGUIScaledFont(_("Menu"));
    pmi->SetFont(sFont);
#endif

    PrepareMenuItem( pmi );

    wxMenu *dst = contextMenu;
    if (pimis->m_in_menu == "Waypoint")
      dst = menuWaypoint;
    else if (pimis->m_in_menu == "Route")
      dst = menuRoute;
    else if (pimis->m_in_menu == "Track")
      dst = menuTrack;
    else if (pimis->m_in_menu == "AIS")
      dst = menuAIS;

    if (dst != NULL) {
      dst->Append(pmi);
      dst->Enable(pimis->id, !pimis->b_grey);
    }
  }

  //        Invoke the correct focused drop-down menu

#ifdef __OCPN__ANDROID__
  androidEnableBackButton(false);
  androidEnableOptionsMenu(false);

  setMenuStyleSheet(menuRoute, GetOCPNGUIScaledFont(_T("Menu")));
  setMenuStyleSheet(menuWaypoint, GetOCPNGUIScaledFont(_T("Menu")));
  setMenuStyleSheet(menuTrack, GetOCPNGUIScaledFont(_T("Menu")));
  setMenuStyleSheet(menuAIS, GetOCPNGUIScaledFont(_T("Menu")));
#endif

  parent->PopupMenu(menuFocus, x, y);

#ifdef __OCPN__ANDROID__
  androidEnableBackButton(true);
  androidEnableOptionsMenu(true);
#endif

  /* Cleanup if necessary.
  Do not delete menus witch are submenu as they will be deleted by their parent
  menu. This could create a crash*/
  delete menuAIS;
  if (!(sub_menu & MMMENU)) delete contextMenu;
  if (!(sub_menu & RTMENU)) delete menuRoute;
  if (!(sub_menu & TKMENU)) delete menuTrack;
  if (!(sub_menu & WPMENU)) delete menuWaypoint;
}

void CanvasMenuHandler::PopupMenuHandler(wxCommandEvent &event) {
  wxPoint r;
  double zlat, zlon;

  int splitMode = 0;  // variables for split
  bool dupFirstWpt = true, showRPD;

  parent->GetCanvasPixPoint(popx * parent->GetDisplayScale(),
                            popy* parent->GetDisplayScale(),
                            zlat, zlon);

  switch (event.GetId()) {
    case ID_DEF_MENU_MAX_DETAIL:
      vLat = zlat;
      vLon = zlon;
      parent->ClearbFollow();

      parent->parent_frame->DoChartUpdate();

      parent->SelectChartFromStack(0, false, CHART_TYPE_DONTCARE,
                                   CHART_FAMILY_RASTER);
      break;

    case ID_DEF_MENU_SCALE_IN:
      parent->DoCanvasStackDelta(-1);
      break;

    case ID_DEF_MENU_SCALE_OUT:
      parent->DoCanvasStackDelta(1);
      break;

    case ID_UNDO:
      parent->InvalidateGL();
      parent->Refresh(false);
      break;

    case ID_REDO:
      parent->InvalidateGL();
      parent->Refresh(false);
      break;

    case ID_DEF_MENU_MOVE_BOAT_HERE:
      gLat = zlat;
      gLon = zlon;
      break;

    case ID_DEF_MENU_GOTO_HERE: {     
      break;
    }

    case ID_DEF_MENU_DROP_WP: {      
      gFrame->RefreshAllCanvas(false);
      gFrame->InvalidateAllGL();
      g_FlushNavobjChanges = true;
      break;
    }

    case ID_DEF_MENU_NEW_RT: {
      break;
    }

    case ID_DEF_MENU_AISTARGETLIST:
      parent->ShowAISTargetList();
      break;

    case ID_DEF_MENU_AIS_CPAWARNING:
      parent->ToggleCPAWarn();
      break;

    case ID_WP_MENU_GOTO: {      
      break;
    }

    case ID_DEF_MENU_COGUP:
      parent->SetUpMode(COURSE_UP_MODE);
      break;

    case ID_DEF_MENU_HEADUP:
      parent->SetUpMode(HEAD_UP_MODE);
      break;

    case ID_DEF_MENU_NORTHUP:
      parent->SetUpMode(NORTH_UP_MODE);
      break;

    case ID_DEF_MENU_TOGGLE_FULL:
      gFrame->ToggleFullScreen();
      break;

    case ID_DEF_MENU_GOTOPOSITION:      
      break;

    case ID_WP_MENU_DELPOINT: {      
      break;
    }
    case ID_WP_MENU_PROPERTIES:
      break;

    case ID_WP_MENU_CLEAR_ANCHORWATCH:
    {
      wxString guid = wxEmptyString;      
      if(!guid.IsEmpty()) {
        wxJSONValue v;
        v[_T("GUID")] = guid;
        wxString msg_id(_T("OCPN_ANCHOR_WATCH_CLEARED"));
        g_pi_manager->SendJSONMessageToAllPlugins(msg_id, v);
      }
      break;
    }

    case ID_WP_MENU_SET_ANCHORWATCH:
    {
      wxString guid = wxEmptyString;
      
      if(!guid.IsEmpty()) {
        wxJSONValue v;
        v[_T("GUID")] = guid;
        wxString msg_id(_T("OCPN_ANCHOR_WATCH_SET"));
        g_pi_manager->SendJSONMessageToAllPlugins(msg_id, v);
      }
      break;
    }

    case ID_DEF_MENU_ACTIVATE_MEASURE:
      break;

    case ID_DEF_MENU_DEACTIVATE_MEASURE:
      parent->InvalidateGL();
      parent->Refresh(false);
      break;

    case ID_DEF_MENU_CM93OFFSET_DIALOG: {
      if (NULL == g_pCM93OffsetDialog) {
        g_pCM93OffsetDialog = new CM93OffsetDialog(parent->parent_frame);
      }

      cm93compchart *pch = NULL;
      if (!parent->GetVP().b_quilt && parent->m_singleChart &&
          (parent->m_singleChart->GetChartType() == CHART_TYPE_CM93COMP)) {
        pch = (cm93compchart *)parent->m_singleChart;
      }

      if (g_pCM93OffsetDialog) {
        g_pCM93OffsetDialog->SetCM93Chart(pch);
        g_pCM93OffsetDialog->Show();
        g_pCM93OffsetDialog->UpdateMCOVRList(parent->GetVP());
      }

      break;
    }
    case ID_DEF_MENU_QUERY: {
      break;
    }
    case ID_DEF_MENU_AIS_QUERY: {      
      break;
    }

    case ID_DEF_MENU_AIS_CPA: {      
      break;
    }

    case ID_DEF_MENU_AISSHOWTRACK: {      
      break;
    }

    case ID_DEF_MENU_COPY_MMSI: {
      // Write MMSI # as text to the clipboard
      if (wxTheClipboard->Open()) {
        wxTheClipboard->SetData(new wxTextDataObject(
            wxString::Format(wxT("%09d"), m_FoundAIS_MMSI)));
        wxTheClipboard->Close();
      }
      break;
    }

    case ID_DEF_MENU_QUILTREMOVE: {
      if (parent->GetVP().b_quilt) {
        int dbIndex = parent->m_pQuilt->GetChartdbIndexAtPix(
            parent->GetVP(), wxPoint(popx, popy));
        parent->RemoveChartFromQuilt(dbIndex);

        parent->ReloadVP();
      }

      break;
    }

    case ID_DEF_MENU_CURRENTINFO: {
      break;
    }

    case ID_DEF_MENU_TIDEINFO: {
      break;
    }
    case ID_RT_MENU_REVERSE: {      
      break;
    }

    case ID_RT_MENU_SHOWNAMES: {
      
      break;
    }

    case ID_RT_MENU_RESEQUENCE: {
      
      break;
    }

    case ID_RT_MENU_DELETE: {
      int dlg_return = wxID_YES;
      if (g_bConfirmObjectDelete) {
        dlg_return = OCPNMessageBox(
            parent, _("Are you sure you want to delete this route?"),
            _("OpenCPN Route Delete"),
            (long)wxYES_NO | wxCANCEL | wxYES_DEFAULT);
      }

      if (dlg_return == wxID_YES) {        
        gFrame->InvalidateAllGL();
        gFrame->RefreshAllCanvas();
      }
      break;
    }

    case ID_RT_MENU_ACTIVATE: {
      
      break;
    }

    case ID_RT_MENU_DEACTIVATE:
      
      break;

    case ID_RT_MENU_INSERT: {
      
      break;
    }

    case ID_RT_MENU_APPEND:
      break;

    case ID_RT_MENU_SPLIT_LEG:  // split route around a leg
      splitMode++;
      dupFirstWpt = false;
    case ID_RT_MENU_SPLIT_WPT:  // split route at a wpt
      break;

    case ID_RT_MENU_COPY:      
      break;

    case ID_TK_MENU_COPY:
      break;

    case ID_WPT_MENU_COPY:
      break;

    case ID_WPT_MENU_SENDTOGPS:
      break;

    case ID_WPT_MENU_SENDTONEWGPS:      
      break;

    case ID_WPT_MENU_SENDTOPEER:       
      break;
    case ID_RT_MENU_SENDTOGPS:      
      break;

    case ID_RT_MENU_SENDTONEWGPS:      
      break;

     case ID_RT_MENU_SENDTOPEER:      
      break;

    case ID_PASTE_WAYPOINT:      
      break;

    case ID_PASTE_ROUTE:      
      break;

    case ID_PASTE_TRACK:      
      break;

    case ID_RT_MENU_DELPOINT:      

      break;

    case ID_RT_MENU_REMPOINT:

      break;

    case ID_RT_MENU_ACTPOINT:      

      break;

    case ID_RT_MENU_DEACTPOINT:
      break;

    case ID_RT_MENU_ACTNXTPOINT:
      break;

    case ID_RT_MENU_PROPERTIES: {
      break;
    }

    case ID_TK_MENU_PROPERTIES: {
      break;
    }

    case ID_TK_MENU_DELETE: {
      int dlg_return = wxID_YES;
      if (g_bConfirmObjectDelete) {
        dlg_return = OCPNMessageBox(
            parent, _("Are you sure you want to delete this track?"),
            _("OpenCPN Track Delete"),
            (long)wxYES_NO | wxCANCEL | wxYES_DEFAULT);
      }

      if (dlg_return == wxID_YES) {        
        gFrame->InvalidateAllGL();
        gFrame->RefreshAllCanvas();
      }
      break;
    }

    case ID_TK_MENU_SENDTOPEER:      
      break;

    case ID_RC_MENU_SCALE_IN:
      parent->parent_frame->DoStackDown(parent);
      parent->GetCanvasPointPix(zlat, zlon, &r);
      parent->WarpPointer(r.x, r.y);
      break;

    case ID_RC_MENU_SCALE_OUT:
      parent->parent_frame->DoStackUp(parent);
      parent->GetCanvasPointPix(zlat, zlon, &r);
      parent->WarpPointer(r.x, r.y);
      break;

    case ID_RC_MENU_ZOOM_IN:
      parent->SetVPScale(parent->GetVPScale() * 2);
      parent->GetCanvasPointPix(zlat, zlon, &r);
      parent->WarpPointer(r.x, r.y);
      break;

    case ID_RC_MENU_ZOOM_OUT:
      parent->SetVPScale(parent->GetVPScale() / 2);
      parent->GetCanvasPointPix(zlat, zlon, &r);
      parent->WarpPointer(r.x, r.y);
      break;

    case ID_RC_MENU_FINISH:
      parent->Refresh(false);
      g_FlushNavobjChanges = true;
      break;

    case ID_DEF_ZERO_XTE:      
      break;

    default: {
      //  Look for PlugIn Context Menu selections
      //  If found, make the callback
      ArrayOfPlugInMenuItems item_array =
          g_pi_manager->GetPluginContextMenuItemArray();

      for (unsigned int i = 0; i < item_array.GetCount(); i++) {
        PlugInMenuItemContainer *pimis = item_array[i];
        {
          if (pimis->id == event.GetId()) {
            if (pimis->m_pplugin)
              pimis->m_pplugin->OnContextMenuItemCallback(pimis->id);
          }
        }
      }

      break;
    }
  }  // switch

  //  Chart Groups....
  if ((event.GetId() >= ID_DEF_MENU_GROUPBASE) &&
      (event.GetId() <=
       ID_DEF_MENU_GROUPBASE + (int)g_pGroupArray->GetCount())) {
    parent->SetGroupIndex(event.GetId() - ID_DEF_MENU_GROUPBASE);
  }

  parent->InvalidateGL();

  g_click_stop = 0;  // Context menu was processed, all is well
}
