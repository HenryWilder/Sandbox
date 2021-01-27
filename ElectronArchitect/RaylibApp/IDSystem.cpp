#include "IDSystem.h"

bool IsIDValid(ID_t id)
{
    return !(id & (ID_t)(IDFlags::NULLID));
}
