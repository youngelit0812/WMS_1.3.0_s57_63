#include "Layer.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(LayerList);

extern bool g_bShowLayers;
extern LayerList *pLayerList;

Layer::Layer(void) {
  m_bIsVisibleOnChart = g_bShowLayers;
  m_bIsVisibleOnListing = false;
  m_bHasVisibleNames = wxCHK_UNDETERMINED;
  m_NoOfItems = 0;
  m_LayerType = _T("");
  m_LayerName = _T("");
  m_LayerFileName = _T("");
  m_LayerDescription = _T("");
  m_CreateTime = wxDateTime::Now();
}

Layer::~Layer(void) {
  //  Remove this layer from the global layer list
  if (NULL != pLayerList) pLayerList->DeleteObject(this);
}
