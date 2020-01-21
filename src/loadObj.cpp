#include "loadObj.hpp"

void loadObj(const char * filename, std::vector<glm::vec3> * vertices, std::vector<glm::vec2> * uvs){
  std::vector<glm::vec3> verticesList;
  std::vector<glm::vec2> uvsList;
  printf("[Objects Loader] Loading %s\n",filename);
  FILE * fp = fopen(filename,"r");
  char type[64];
  while(!feof(fp)){
  fscanf(fp,"%s",type);
  if(strcmp(type,"v") == 0){
    glm::vec3 v;
    fscanf(fp,"%f %f %f\n",&v.x,&v.y,&v.z);
    verticesList.push_back(v);
  }
  else if(strcmp(type,"vt") == 0){
    glm::vec2 uv;
    fscanf(fp,"%f %f\n",&uv.x,&uv.y);
    uvsList.push_back(uv);
  }
  else if(strcmp(type,"f") == 0){
    int vIndexes[3];
    int uvIndexes[3];
    fscanf(fp,"%d/%d/%*d %d/%d/%*d %d/%d/%*d\n",&vIndexes[0],&uvIndexes[0],&vIndexes[1],&uvIndexes[1],&vIndexes[2],&uvIndexes[2]);
    glm::vec3 fV[3];
    glm::vec2 fUv[3];
    fV[0] = verticesList[vIndexes[0]-1];
    fV[1] = verticesList[vIndexes[1]-1];
    fV[2] = verticesList[vIndexes[2]-1];
    fUv[0] = uvsList[uvIndexes[0]-1];
    fUv[1] = uvsList[uvIndexes[1]-1];
    fUv[2] = uvsList[uvIndexes[2]-1];
    vertices->push_back(fV[0]);
    vertices->push_back(fV[1]);
    vertices->push_back(fV[2]);
    uvs->push_back(fUv[0]);
    uvs->push_back(fUv[1]);
    uvs->push_back(fUv[2]);
  }
}
fclose(fp);
}
