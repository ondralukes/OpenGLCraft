#ifndef STRUCTS_H
#define STRUCTS_H 545
typedef struct intvec3{
  intvec3(int _x = 0, int _y = 0, int _z = 0):x(_x),y(_y),z(_z){};
  int x;
  int y;
  int z;

};

bool operator==(const intvec3& lhs, const intvec3& rhs);
bool operator!=(const intvec3& lhs, const intvec3& rhs);

struct chunk_render_side{
  chunk_render_side(intvec3 _pos, intvec3 _dir) : pos(_pos),dir(_dir){};
  intvec3 pos;
  intvec3 dir;
};
#endif
