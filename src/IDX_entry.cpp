#include "IDX_entry.h"

#include <string.h>
#include <stdlib.h>
#include <wx/arrimpl.cpp>
WX_DEFINE_OBJARRAY(ArrayOfIDXEntry);

IDX_entry::IDX_entry() { memset(this, 0, sizeof(IDX_entry)); }

IDX_entry::~IDX_entry() { free(IDX_tzname); }
