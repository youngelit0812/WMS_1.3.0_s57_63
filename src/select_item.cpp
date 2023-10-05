
#include "select_item.h"

#include <wx/listimpl.cpp>
WX_DEFINE_LIST(SelectableItemList);

SelectItem::SelectItem() {}

SelectItem::~SelectItem() {}

int SelectItem::GetUserData(void) { return m_Data4; }

void SelectItem::SetUserData(int data) { m_Data4 = data; }
