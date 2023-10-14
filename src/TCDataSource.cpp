
#include <wx/log.h>
#include <wx/filename.h>

#include "TCDataSource.h"
#include "TCDS_Ascii_Harmonic.h"
#include "TCDS_Binary_Harmonic.h"

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfTCDSources);

TCDataSource::TCDataSource() {
  m_pfactory = NULL;
  pTCDS_Ascii_Harmonic = NULL;
  pTCDS_Binary_Harmonic = NULL;
}

TCDataSource::~TCDataSource() {
  delete pTCDS_Ascii_Harmonic;
  delete pTCDS_Binary_Harmonic;
}

TC_Error_Code TCDataSource::LoadData(const wxString &data_file_path) {
  m_data_source_path = data_file_path;

  wxFileName fname(data_file_path);

  if (!fname.FileExists()) return TC_FILE_NOT_FOUND;

  if (fname.GetExt() == _T("IDX") || fname.GetExt() == _T("idx")) {
    TCDS_Ascii_Harmonic *pdata = new TCDS_Ascii_Harmonic;
    m_pfactory = dynamic_cast<TCDataFactory *>(pdata);
    pTCDS_Ascii_Harmonic = pdata;
  } else if (fname.GetExt() == _T("tcd") || fname.GetExt() == _T("TCD")) {
    TCDS_Binary_Harmonic *pdata = new TCDS_Binary_Harmonic;
    m_pfactory = dynamic_cast<TCDataFactory *>(pdata);
    pTCDS_Binary_Harmonic = pdata;
  }

  TC_Error_Code err_code;
  if (m_pfactory) {
    err_code = m_pfactory->LoadData(data_file_path);

    //  Mark the index entries individually with owner
    unsigned int max_index = GetMaxIndex();
    for (unsigned int i = 0; i < max_index; i++) {
      IDX_entry *pIDX = GetIndexEntry(i);
      if (pIDX) {
        pIDX->pDataSource = this;
        strncpy(pIDX->source_ident, m_data_source_path.mb_str(),
                MAXNAMELEN - 1);
        pIDX->source_ident[MAXNAMELEN - 1] = '\0';
      }
    }
  } else
    err_code = TC_FILE_NOT_FOUND;

  return err_code;
}

int TCDataSource::GetMaxIndex(void) {
  if (m_pfactory)
    return m_pfactory->GetMaxIndex();
  else
    return 0;
}

IDX_entry *TCDataSource::GetIndexEntry(int n_index) {
  if (m_pfactory) {
    if (n_index < m_pfactory->GetMaxIndex())
      return m_pfactory->GetIndexEntry(n_index);
    else
      return NULL;
  } else
    return NULL;
}

TC_Error_Code TCDataSource::LoadHarmonicData(IDX_entry *pIDX) {
  switch (pIDX->source_data_type) {
    case SOURCE_TYPE_ASCII_HARMONIC:
      return pTCDS_Ascii_Harmonic->LoadHarmonicData(pIDX);
      break;

    case SOURCE_TYPE_BINARY_HARMONIC:
      return pTCDS_Binary_Harmonic->LoadHarmonicData(pIDX);
      break;

    default:
      return TC_GENERIC_ERROR;
  }
}
