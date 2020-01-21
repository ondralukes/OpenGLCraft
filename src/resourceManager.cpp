#include "resourceManager.hpp"

std::vector<texture_t> ResourceManager::textures;
std::vector<obj_t> ResourceManager::objs;

GLuint
ResourceManager::getTexture(const char * path){
  //printf("[Resource Manager] Requested to load texture %s\n",path);
  for(int i =0;i<ResourceManager::textures.size();i++){
    if(strcmp(ResourceManager::textures[i].name,path)==0){
      //printf("[Resource Manager] Texture found at index %d\n",i);
      return ResourceManager::textures[i].id;
    }
  }
  //printf("[Resource Manager] Texture not found. Loading...\n");
  texture_t tex;
  tex.name = path;
  tex.id = loadBmp(path);
  ResourceManager::textures.push_back(tex);
  return tex.id;
}

std::vector<glm::vec3> *
ResourceManager::getObjVertices(const char * path){
  //printf("[Resource Manager] Requested to load vertices of obj %s\n",path);
  for(int i =0;i<ResourceManager::objs.size();i++){
    if(strcmp(ResourceManager::objs[i].name,path)==0){
      //printf("[Resource Manager] Obj found at index %d\n",i);
      return ResourceManager::objs[i].vertices;
    }
  }
  //printf("[Resource Manager] Obj not found. Loading...\n");
  std::vector<glm::vec3> * vertices = new std::vector<glm::vec3>();
  std::vector<glm::vec2> * uvs = new std::vector<glm::vec2>();
  loadObj(path,vertices,uvs);
  obj_t obj;
  obj.name = path;
  obj.vertices = vertices;
  obj.uvs = uvs;
  ResourceManager::objs.push_back(obj);
  return obj.vertices;
}

std::vector<glm::vec2> *
ResourceManager::getObjUVs(const char * path){
  //printf("[Resource Manager] Requested to load UVs of obj %s\n",path);
  for(int i =0;i<ResourceManager::objs.size();i++){
    if(strcmp(ResourceManager::objs[i].name,path)==0){
      //printf("[Resource Manager] Obj found at index %d\n",i);
      return ResourceManager::objs[i].uvs;
    }
  }
  //printf("[Resource Manager] Obj not found. Loading...\n");
  std::vector<glm::vec3> * vertices = new std::vector<glm::vec3>();
  std::vector<glm::vec2> * uvs = new std::vector<glm::vec2>();
  loadObj(path,vertices,uvs);
  obj_t obj;
  obj.name = path;
  obj.vertices = vertices;
  obj.uvs = uvs;
  ResourceManager::objs.push_back(obj);
  return obj.uvs;
}
