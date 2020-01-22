#include "structs.hpp"
#include "blockUtils.hpp"
#include <glm/glm.hpp>

bool getLookingAt(glm::vec3 pos, glm::vec3 dir, intvec3 * out, intvec3 * prevBlock);
bool getLookingAt(glm::vec3 pos, glm::vec3 dir, intvec3 * out);
