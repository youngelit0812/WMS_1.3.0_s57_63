/******************************************************************************
 *
 * Project:  OpenCPN
 * Purpose:  Read and write KML Format
 *(http://en.wikipedia.org/wiki/Keyhole_Markup_Language) Author:   Jesper
 *Weissglas
 *
 ***************************************************************************
 *   Copyright (C) 2012 by David S. Register                               *
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
 ***************************************************************************
 *
 *
 */

#include "config.h"

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <vector>

#include <wx/file.h>
#include <wx/datetime.h>
#include <wx/clipbrd.h>

#include "ocpn_types.h"
#include "navutil.h"
#include "tinyxml.h"
#include "kml.h"
#include "ocpn_frame.h"
#include "own_ship.h"

extern MyFrame* gFrame;

int Kml::seqCounter = 0;
bool Kml::insertQtVlmExtendedData = false;

int Kml::ParseCoordinates(TiXmlNode* node, dPointList& points) {
  TiXmlElement* e = node->FirstChildElement("coordinates");
  if (!e) {    
    return 0;
  }

  // Parse "long,lat,z" format.

  dPoint point;

  std::stringstream ss(e->GetText());
  std::string txtCoord;

  while (1) {
    if (!std::getline(ss, txtCoord, ',')) break;
    ;
    if (txtCoord.length() == 0) break;

    point.x = atof(txtCoord.c_str());
    std::getline(ss, txtCoord, ',');
    point.y = atof(txtCoord.c_str());
    std::getline(ss, txtCoord, ' ');
    point.z = atof(txtCoord.c_str());

    points.push_back(point);
  }
  return points.size();
}

KmlPastebufferType Kml::ParseTrack(TiXmlNode* node, wxString& name) {
  if (0 == strncmp(node->ToElement()->Value(), "LineString", 10)) {
    dPointList coordinates;
    if (ParseCoordinates(node, coordinates) > 2) {
    }
    return KML_PASTE_TRACK;
  }

  if (0 == strncmp(node->ToElement()->Value(), "gx:Track", 8)) {
    
    TiXmlElement* point = node->FirstChildElement("gx:coord");
    int pointCounter = 0;

    for (; point; point = point->NextSiblingElement("gx:coord")) {
      double lat, lon;
      std::stringstream ss(point->GetText());
      std::string txtCoord;
      std::getline(ss, txtCoord, ' ');
      lon = atof(txtCoord.c_str());
      std::getline(ss, txtCoord, ' ');
      lat = atof(txtCoord.c_str());

      pointCounter++;
    }

    TiXmlElement* when = node->FirstChildElement("when");

    wxDateTime whenTime;

    int i = 0;
    for (; when; when = when->NextSiblingElement("when")) {    
      whenTime.ParseFormat(wxString(when->GetText(), wxConvUTF8),
                           _T("%Y-%m-%dT%H:%M:%SZ"));
      i++;
    }

    return KML_PASTE_TRACK;
  }
  return KML_PASTE_INVALID;
}

KmlPastebufferType Kml::ParseOnePlacemarkPoint(TiXmlNode* node,
                                               wxString& name) {
  double newLat = 0., newLon = 0.;
  dPointList coordinates;

  if (ParseCoordinates(node->ToElement(), coordinates)) {
    newLat = coordinates[0].y;
    newLon = coordinates[0].x;
  }

  if (newLat == 0.0 && newLon == 0.0) {    
    return KML_PASTE_INVALID;
  }
  wxString pointName = wxEmptyString;
  TiXmlElement* e = node->Parent()->FirstChild("name")->ToElement();
  if (e) pointName = wxString(e->GetText(), wxConvUTF8);

  wxString pointDescr = wxEmptyString;
  e = node->Parent()->FirstChildElement("description");

  // If the <description> is an XML element we must convert it to text,
  // otherwise it gets lost.
  if (e) {
    TiXmlNode* n = e->FirstChild();
    if (n) switch (n->Type()) {
        case TiXmlNode::TINYXML_TEXT:
          pointDescr = wxString(e->GetText(), wxConvUTF8);
          break;
        case TiXmlNode::TINYXML_ELEMENT:
          TiXmlPrinter printer;
          printer.SetIndent("\t");
          n->Accept(&printer);
          pointDescr = wxString(printer.CStr(), wxConvUTF8);
          break;
      }
  }

  // Extended data will override description.
  TiXmlNode* n = node->Parent()->FirstChild("ExtendedData");
  if (n) {
    TiXmlPrinter printer;
    printer.SetIndent("\t");
    n->Accept(&printer);
    pointDescr = wxString(printer.CStr(), wxConvUTF8);
  }

  // XXX leak ?
  return KML_PASTE_WAYPOINT;
}

KmlPastebufferType Kml::ParsePasteBuffer() {
  if (!wxTheClipboard->IsOpened())
    if (!wxTheClipboard->Open()) return KML_PASTE_INVALID;

  wxTextDataObject data;
  wxTheClipboard->GetData(data);
  kmlText = data.GetText();
  wxTheClipboard->Close();

  if (kmlText.Find(_T("<kml")) == wxNOT_FOUND) return KML_PASTE_INVALID;

  TiXmlDocument doc;
  if (!doc.Parse(kmlText.mb_str(wxConvUTF8), 0, TIXML_ENCODING_UTF8)) {
    wxLogError(wxString(doc.ErrorDesc(), wxConvUTF8));
    return KML_PASTE_INVALID;
  }
  if (0 != strncmp(doc.RootElement()->Value(), "kml", 3))
    return KML_PASTE_INVALID;

  TiXmlHandle docHandle(doc.RootElement());

  // We may or may not have a <document> depending on what the user copied.
  TiXmlElement* placemark =
      docHandle.FirstChild("Document").FirstChild("Placemark").ToElement();
  if (!placemark) {
    placemark = docHandle.FirstChild("Placemark").ToElement();
  }
  if (!placemark) {    
    return KML_PASTE_INVALID;
  }

  int pointCounter = 0;
  wxString name;
  for (; placemark; placemark = placemark->NextSiblingElement()) {
    TiXmlElement* e = placemark->FirstChildElement("name");
    if (e) name = wxString(e->GetText(), wxConvUTF8);
    pointCounter++;
  }

  if (pointCounter == 1) {
    // Is it a single waypoint?
    TiXmlNode* element = docHandle.FirstChild("Document")
                             .FirstChild("Placemark")
                             .FirstChild("Point")
                             .ToNode();
    if (!element)
      element = docHandle.FirstChild("Placemark").FirstChild("Point").ToNode();
    if (element) return ParseOnePlacemarkPoint(element, name);

    // Is it a dumb <LineString> track?
    element = docHandle.FirstChild("Document")
                  .FirstChild("Placemark")
                  .FirstChild("LineString")
                  .ToNode();
    if (!element)
      element =
          docHandle.FirstChild("Placemark").FirstChild("LineString").ToNode();
    if (element) return ParseTrack(element, name);

    // Is it a smart extended <gx:track> track?
    element = docHandle.FirstChild("Document")
                  .FirstChild("Placemark")
                  .FirstChild("gx:Track")
                  .ToNode();
    if (!element)
      element =
          docHandle.FirstChild("Placemark").FirstChild("gx:Track").ToNode();
    if (element) return ParseTrack(element, name);

    return KML_PASTE_INVALID;
  }

  // Here we go with a full route.

  bool foundPoints = false;
  bool foundTrack = false;
  TiXmlElement* element = docHandle.FirstChild("Document").FirstChild("name").ToElement();

  placemark =
      docHandle.FirstChild("Document").FirstChild("Placemark").ToElement();
  for (; placemark; placemark = placemark->NextSiblingElement()) {
    TiXmlNode* n = placemark->FirstChild("Point");
    if (n) {
      if (ParseOnePlacemarkPoint(n->ToElement(), name) == KML_PASTE_WAYPOINT) {        
        foundPoints = true;
      }
    }

    n = placemark->FirstChild("LineString");
    if (n) {
      ParseTrack(n->ToElement(), name);
      foundTrack = true;
    }
    n = placemark->FirstChild("gx:Track");
    if (n) {
      ParseTrack(n->ToElement(), name);
      foundTrack = true;
    }
  }

  if (foundPoints) {
    foundPoints = false;
  }

  if (foundPoints && !foundTrack) return KML_PASTE_ROUTE;
  if (foundPoints && foundTrack) return KML_PASTE_ROUTE_TRACK;
  if (!foundPoints && foundTrack) return KML_PASTE_TRACK;
  return KML_PASTE_INVALID;
}

TiXmlElement* Kml::StandardHead(TiXmlDocument& xmlDoc, wxString name) {
  TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "UTF-8", "");
  xmlDoc.LinkEndChild(decl);

  TiXmlElement* kml = new TiXmlElement("kml");
  kml->SetAttribute("xmlns:atom", "http://www.w3.org/2005/Atom");
  kml->SetAttribute("xmlns", "http://www.opengis.net/kml/2.2");
  kml->SetAttribute("xmlns:gx", "http://www.google.com/kml/ext/2.2");
  kml->SetAttribute("xmlns:kml", "http://www.opengis.net/kml/2.2");

  if (insertQtVlmExtendedData)
    kml->SetAttribute("xmlns:vlm", "http://virtual-loup-de-mer.org");

  xmlDoc.LinkEndChild(kml);

  TiXmlElement* document = new TiXmlElement("Document");
  kml->LinkEndChild(document);
  TiXmlElement* docName = new TiXmlElement("name");
  document->LinkEndChild(docName);
  TiXmlText* docNameVal = new TiXmlText(name.mb_str(wxConvUTF8));
  docName->LinkEndChild(docNameVal);
  return document;
}

Kml::Kml() {    
}

Kml::~Kml() {  
}
