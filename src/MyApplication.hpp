/**
 * MyApplication.hpp skeleton
 * Contributors:
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#ifndef OPENGL_CMAKE_SKELETON_MYAPPLICATION
#define OPENGL_CMAKE_SKELETON_MYAPPLICATION

#include "Application.hpp"
#include "Shader.hpp"

class MyApplication : public Application {
 public:
  MyApplication();

 protected:
  virtual void loop();

 private:
  const int size = 200;
  float last_refresh_time = 0.0;
  double x_mouse_pos, y_mouse_pos;
  glm::vec3 point_position = glm::vec3(0.0, 0.0, 0.0);
  glm::vec3 camera_position = glm::vec3(0.0, 0.0, 0.0);
  bool mousePressed = false;
  void moveView();
  void rotateView();
  glm::vec3 getCameraDirection();
  void createGraph();

  // shader
  Shader vertexShader;
  Shader fragmentShader;
  ShaderProgram shaderProgram;

  // shader matrix uniform
  glm::mat4 projection = glm::mat4(1.0);
  glm::mat4 view = glm::mat4(1.0);

  // VBO/VAO/ibo
  GLuint vao, vbo, ibo;
};

#endif  // OPENGL_CMAKE_SKELETON_MYAPPLICATION
