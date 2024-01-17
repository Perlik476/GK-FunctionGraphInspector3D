/**
 * MyApplication.cpp
 * Contributors:
 *      * Arthur Sonzogni (author)
 * Licence:
 *      * MIT
 */
#include "MyApplication.hpp"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_operation.hpp>
#include <iostream>
#include <vector>

#include "asset.hpp"
#include "glError.hpp"

struct VertexType {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec4 color;
};

float heightMap(const glm::vec2 position) {
  return 2.0 * sin(position.x * position.y) * exp(-0.05 * (position.x * position.x + position.y * position.y)) + 0.5 * sin(position.x * position.x);
}

float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}

VertexType getHeightMap(const glm::vec2 position) {
  const glm::vec2 dx(1.0, 0.0);
  const glm::vec2 dy(0.0, 1.0);

  VertexType v;
  float h = heightMap(position);
  float hx = 100.f * (heightMap(position + 0.01f * dx) - h);
  float hy = 100.f * (heightMap(position + 0.01f * dy) - h);

  v.position = glm::vec3(position, h);
  v.normal = glm::normalize(glm::vec3(-hx, -hy, 1.0));

  float c = sigmoid(h);
  v.color = glm::vec4(c, 1.0 - c, 1.0, 1.0);
  return v;
}

MyApplication::MyApplication()
    : Application(),
      vertexShader(SHADER_DIR "/shader.vert.glsl", GL_VERTEX_SHADER),
      fragmentShader(SHADER_DIR "/shader.frag.glsl", GL_FRAGMENT_SHADER),
      shaderProgram({vertexShader, fragmentShader}) {
  glCheckError(__FILE__, __LINE__);

  // creation of the mesh ------------------------------------------------------
  std::vector<VertexType> vertices;
  std::vector<GLuint> index;

  for (int y = 0; y <= size; ++y)
    for (int x = 0; x <= size; ++x) {
      float xx = (x - size / 2) * 0.01f;
      float yy = (y - size / 2) * 0.01f;
      vertices.push_back(getHeightMap({xx, yy}));
    }

  for (int y = 0; y < size; ++y)
    for (int x = 0; x < size; ++x) {
      index.push_back((x + 0) + (size + 1) * (y + 0));
      index.push_back((x + 1) + (size + 1) * (y + 0));
      index.push_back((x + 1) + (size + 1) * (y + 1));

      index.push_back((x + 1) + (size + 1) * (y + 1));
      index.push_back((x + 0) + (size + 1) * (y + 1));
      index.push_back((x + 0) + (size + 1) * (y + 0));
    }

  std::cout << "vertices=" << vertices.size() << std::endl;
  std::cout << "index=" << index.size() << std::endl;

  // creation of the vertex array buffer----------------------------------------

  // vbo
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType),
               vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // ibo
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(GLuint),
               index.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // vao
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // bind vbo
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // map vbo to shader attributes
  shaderProgram.setAttribute("position", 3, sizeof(VertexType),
                             offsetof(VertexType, position));
  shaderProgram.setAttribute("normal", 3, sizeof(VertexType),
                             offsetof(VertexType, normal));
  shaderProgram.setAttribute("color", 4, sizeof(VertexType),
                             offsetof(VertexType, color));

  // bind the ibo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // vao end
  glBindVertexArray(0);

  // camera position
  camera_position = glm::vec3(15.0, 15.0, 15.0);

  view = glm::lookAt(camera_position, point_position, glm::vec3(0, 0, 1));
}

glm::vec3 MyApplication::getCameraDirection() {
  // camera is at (camera_x_pos, camera_y_pos, camera_z_pos) and look at (x_pos, y_pos, 0)
  return glm::normalize(point_position - camera_position);
}

void MyApplication::moveView() {
  // get arrow keys state
  int left = glfwGetKey(getWindow(), GLFW_KEY_LEFT);
  int right = glfwGetKey(getWindow(), GLFW_KEY_RIGHT);
  int up = glfwGetKey(getWindow(), GLFW_KEY_UP);
  int down = glfwGetKey(getWindow(), GLFW_KEY_DOWN);

  float speed = 0.1;

  // compute new view matrix
  glm::vec3 translation(0, 0, 0);
  if (left == GLFW_PRESS)
    translation.x -= speed;
  if (right == GLFW_PRESS)
    translation.x += speed;
  if (up == GLFW_PRESS)
    translation.y -= speed;
  if (down == GLFW_PRESS)
    translation.y += speed;

  glm::vec3 camera_direction = getCameraDirection();
  camera_direction.z = 0;
  camera_direction = camera_direction;
  glm::vec3 camera_orthogonal = glm::vec3(-camera_direction.y, camera_direction.x, 0);
  translation = translation.y * camera_direction + translation.x * camera_orthogonal;

  camera_position.x += translation.x;
  camera_position.y += translation.y;

  point_position.x += translation.x;
  point_position.y += translation.y;

  view = glm::translate(view, translation);
}

void MyApplication::rotateView() {
  if (glfwGetMouseButton(getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    // get mouse position
    double x_mouse_pos_current, y_mouse_pos_current;
    glfwGetCursorPos(getWindow(), &x_mouse_pos_current, &y_mouse_pos_current);

    // update mouse state

    if (!mousePressed) {
      mousePressed = true;
      x_mouse_pos = x_mouse_pos_current;
      y_mouse_pos = y_mouse_pos_current;
    }

    float delta_xi = (x_mouse_pos_current - x_mouse_pos) / getWidth();
    float delta_eta = (y_mouse_pos_current - y_mouse_pos) / getHeight();
    x_mouse_pos = x_mouse_pos_current;
    y_mouse_pos = y_mouse_pos_current;
    // if (delta_xi == 0 && delta_eta == 0)
    //   return;

    glm::vec3 camera_direction = getCameraDirection();
    glm::vec3 camera_orthogonal_direction_up_down = 
      glm::normalize(glm::vec3(-camera_direction.y, camera_direction.x, 0));
    glm::vec3 camera_orthogonal_direction_left_right = 
      glm::normalize(glm::vec3(camera_orthogonal_direction_up_down.y, -camera_orthogonal_direction_up_down.x, 0));
    glm::mat4x4 transformation =
        glm::translate(glm::mat4(1.0), -camera_position) *
        // glm::rotate(glm::mat4(1.0), -0.01f, camera_orthogonal_direction_up_down) *
        // glm::rotate(glm::mat4(1.0), -0.01f, camera_orthogonal_direction_left_right) *
        glm::rotate(glm::mat4(1.0), delta_xi, camera_orthogonal_direction_left_right) *
        glm::rotate(glm::mat4(1.0), delta_eta, camera_orthogonal_direction_up_down) *
        glm::translate(glm::mat4(1.0), camera_position);
    camera_position = glm::normalize(glm::vec3(transformation * glm::vec4(camera_position, 1.0))) * glm::length(camera_position);
    // camera_position = glm::vec3(transformation * glm::vec4(camera_position, 1.0));
    std::cout << "camera_position norm: " << glm::length(camera_position) << std::endl;
    view = glm::lookAt(camera_position, point_position, glm::vec3(0, 0, 1));
  }
  else {
    mousePressed = false;
  }
}

void MyApplication::loop() {
  // exit on window close button pressed
  if (glfwWindowShouldClose(getWindow()))
    exit();

  float t = getTime();
  // set matrix : projection + view
  projection = glm::perspective(float(2.0 * atan(getHeight() / 1920.f)),
                                getWindowRatio(), 0.1f, 100.f);
  moveView();
  rotateView();
  printf("cx=%f cy=%f cz=%f\n", camera_position.x, camera_position.y, camera_position.z);
  printf("px=%f py=%f pz=%f\n", point_position.x, point_position.y, point_position.z);

  // clear
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shaderProgram.use();

  // send uniforms
  shaderProgram.setUniform("projection", projection);
  shaderProgram.setUniform("view", view);

  glCheckError(__FILE__, __LINE__);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  glCheckError(__FILE__, __LINE__);
  glDrawElements(GL_TRIANGLES,         // mode
                 size * size * 2 * 3,  // count
                 GL_UNSIGNED_INT,      // type
                 NULL                  // element array buffer offset
  );

  glBindVertexArray(0);

  shaderProgram.unuse();
}
