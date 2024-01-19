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
#include <ft2build.h>
#include FT_FREETYPE_H
#include <utils.hpp>
#include <Optimizers.hpp>
#include <optional>
#include <memory>


class MyApplication : public Application {
public:
  MyApplication(func_t function, std::optional<grad_t> gradient, std::optional<hess_t> hessian);

protected:
  virtual void loop();

private:
  // function
  func_t function;
  std::optional<grad_t> gradient;
  std::optional<hess_t> hessian;

  // optimizer
  std::shared_ptr<Optimizer> optimizer;
  void changeOptimizer();
  bool button_pressed = false;
  std::vector<glm::vec2> points;

  // graphics variables
  const int size = 200;
  float last_refresh_time = 0.0;
  double x_mouse_pos, y_mouse_pos;
  bool mouse_pressed = false;
  double y_mouse_pos_right;
  bool mouse_pressed_right = false;
  glm::vec3 point_position = glm::vec3(0.0, 0.0, 0.0);
  glm::vec3 camera_position = glm::vec3(0.0, 0.0, 0.0);

  void moveView();
  void rotateView();
  void zoomView();
  glm::vec3 getCameraDirection();
  float getCameraDistance();
  void createGraph();

  FT_Library ft;
  FT_Face face;
  void renderText(std::string text, float x, float y, float sx, float sy);

  // shader
  Shader vertexShader;
  Shader fragmentShader;
  ShaderProgram shaderProgram;

  Shader vertexShaderText;
  Shader fragmentShaderText;
  ShaderProgram shaderProgramText;

  // shader matrix uniform
  glm::mat4 projection = glm::mat4(1.0);
  glm::mat4 view = glm::mat4(1.0);

  // VBO/VAO/ibo
  GLuint vao, vbo, ibo, vbotext, vaotext, ibotext;
};

#endif  // OPENGL_CMAKE_SKELETON_MYAPPLICATION
