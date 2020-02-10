#include "structs.hpp"

bool operator==(const intvec3& lhs, const intvec3& rhs)
{
  return lhs.x == rhs.x&&lhs.y==rhs.y&&lhs.z==rhs.z;
}
bool operator!=(const intvec3& lhs, const intvec3& rhs)
{
  return lhs.x != rhs.x||lhs.y!=rhs.y||lhs.z!=rhs.z;
}
intvec3 operator+(const intvec3& lhs, const intvec3& rhs){
  return intvec3(
    lhs.x+rhs.x,
    lhs.y+rhs.y,
    lhs.z+rhs.z
  );
}

intvec3 operator*(const intvec3& lhs, const int& rhs){
  return intvec3(
    lhs.x*rhs,
    lhs.y*rhs,
    lhs.z*rhs
  );
}
