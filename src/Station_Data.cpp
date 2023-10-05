
#include "Station_Data.h"
#include <stdlib.h>

#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfStationData);

Station_Data::Station_Data() {
  station_name = NULL;
  amplitude = NULL;
  epoch = NULL;
}

Station_Data::~Station_Data() {
  free(station_name);
  free(amplitude);
  free(epoch);
}
