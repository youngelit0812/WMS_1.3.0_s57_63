/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  OpenCPN private types and ENUMs
 * Author:   David Register
 *
 ***************************************************************************
 *   Copyright (C) 2010 by David S. Register   *
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
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,  USA.             *
 ***************************************************************************
 *
 *
 *
 */

#ifndef _OCPNTYPES_H__
#define _OCPNTYPES_H__

#include <wx/colour.h>




//    ChartType constants
typedef enum ChartTypeEnum {
  CHART_TYPE_UNKNOWN = 0,
  CHART_TYPE_DUMMY,
  CHART_TYPE_DONTCARE,
  CHART_TYPE_KAP,
  CHART_TYPE_GEO,
  CHART_TYPE_S57,
  CHART_TYPE_CM93,
  CHART_TYPE_CM93COMP,
  CHART_TYPE_PLUGIN,
  CHART_TYPE_MBTILES
} _ChartTypeEnum;

//    ChartFamily constants
typedef enum ChartFamilyEnum {
  CHART_FAMILY_UNKNOWN = 0,
  CHART_FAMILY_RASTER,
  CHART_FAMILY_VECTOR,
  CHART_FAMILY_DONTCARE
} _ChartFamilyEnum;


//----------------------------------------------------------------------------
// ocpn Toolbar stuff
//----------------------------------------------------------------------------
class ChartBase;
class wxSocketEvent;
class ocpnToolBarSimple;

//    A generic Position Data structure
typedef struct {
  double kLat;
  double kLon;
  double kCog;
  double kSog;
  double kVar;  // Variation, typically from RMC message
  double kHdm;  // Magnetic heading
  double kHdt;  // true heading
  time_t FixTime;
  int nSats;
} GenericPosDatEx;

//    A collection of active leg Data structure
typedef struct {
  double Xte;  // Left side of the track -> negative XTE
  double Btw;
  double Dtw;
  wxString wp_name;  // Name of destination waypoint for active leg;
  bool arrival;
} ActiveLegDat;

#endif    //  _OCPNTYPES_H__
