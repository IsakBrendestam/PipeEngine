#include "stdafx.h"
#include "UniqueID.h"

ID UniqueID::m_id = 1;

ID UniqueID::GenerateID()
{
    return m_id++;
}
