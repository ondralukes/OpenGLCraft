#include "structs.hpp"

bool operator==(const intvec3& lhs, const intvec3& rhs)
{
  return lhs.x == rhs.x&&lhs.y==rhs.y&&lhs.z==rhs.z;
}
bool operator!=(const intvec3& lhs, const intvec3& rhs)
{
  return lhs.x != rhs.x||lhs.y!=rhs.y||lhs.z!=rhs.z;
}
