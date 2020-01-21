#include "ray.hpp"

bool getLookingAt(glm::vec3 pos, glm::vec3 dir, intvec3 * out, intvec3 * prevBlock){
  for(float i = 0;i<8.0f;i+=0.01f){
    glm::vec3 p = pos+dir*i;
    intvec3 blockPos(
      round(p.x),
      round(p.y),
      round(p.z)
    );
    if(isBlock(blockPos)) {
      if(out != NULL)*out = blockPos;
      return true;
    }
    if(prevBlock!=NULL)*prevBlock = blockPos;
  }
  return false;
}

bool getLookingAt(glm::vec3 pos, glm::vec3 dir, intvec3 * out){
  return getLookingAt(pos,dir,out,NULL);
}
