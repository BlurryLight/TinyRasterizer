#ifndef CAMERA_H
#define CAMERA_H
#include <glm/glm.hpp>

namespace pd {

class Camera
{
public:
  Camera(glm::vec3 origin, glm::vec3 up, glm::vec3 lookat, float vfov,
         float aspect);
  glm::mat4 look_at();
  glm::mat4 project();

private:
  glm::vec3 origin_;
  glm::vec3 up_;
  glm::vec3 lookat_;
  float vfov_;
  float aspect_;
};

} // namespace pd
#endif // CAMERA_H
