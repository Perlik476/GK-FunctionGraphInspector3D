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

float sigmoid(float x) {
  return 1.0 / (1.0 + exp(-x));
}

VertexType getHeightMap(const glm::vec2 position, float diff, std::function<float(glm::vec2)> func) {
  const glm::vec2 dx(1.0, 0.0);
  const glm::vec2 dy(0.0, 1.0);

  VertexType v;
  float h = func(position);
  float hx = 100.f * (func(position + diff * dx) - h);
  float hy = 100.f * (func(position + diff * dy) - h);

  v.position = glm::vec3(position, h);
  v.normal = glm::normalize(glm::vec3(-hx, -hy, 1.0));

  float c = sigmoid(0.1f * h);
  float c2 = sigmoid(h);
  v.color = glm::vec4(c2, 1.0 - c2, c, 1.0);
  return v;
}

void MyApplication::createGraph() {
  // creation of the mesh ------------------------------------------------------
  std::vector<VertexType> vertices;
  std::vector<GLuint> index;

  float diff = glm::round(getCameraDistance()) * 0.004f;

  for (int y = 0; y <= size; ++y)
    for (int x = 0; x <= size; ++x) {
      float xx = (x - size / 2) * diff + glm::round(point_position.x / diff) * diff; // round to the nearest multiple of diff
      float yy = (y - size / 2) * diff + glm::round(point_position.y / diff) * diff; // round to the nearest multiple of diff
      vertices.push_back(getHeightMap({xx, yy}, diff, function));
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

  // std::cout << "vertices=" << vertices.size() << std::endl;
  // std::cout << "index=" << index.size() << std::endl;

  // Add the axes lines
  const float axis_length = 100.0;
  vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec4(1, 0, 0, 1)});
  vertices.push_back({glm::vec3(axis_length, 0, 0), glm::vec3(0, 0, 1), glm::vec4(1, 0, 0, 1)});
  vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec4(0, 1, 0, 1)});
  vertices.push_back({glm::vec3(0, axis_length, 0), glm::vec3(0, 0, 1), glm::vec4(0, 1, 0, 1)});
  vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec4(0, 0, 1, 1)});
  vertices.push_back({glm::vec3(0, 0, axis_length), glm::vec3(0, 0, 1), glm::vec4(0, 0, 1, 1)});
  for (int i = 0; i < 6; ++i)
    index.push_back(vertices.size() - 6 + i);

  // Add axes lines at the point position
  vertices.push_back({glm::vec3(0, 0, -axis_length), glm::vec3(0, 0, 1), glm::vec4(1, 1, 1, 1)});
  vertices.push_back({glm::vec3(0, 0, axis_length), glm::vec3(0, 0, 1), glm::vec4(1, 1, 1, 1)});
  vertices.push_back({glm::vec3(0, -axis_length, 0), glm::vec3(0, 0, 1), glm::vec4(1, 1, 1, 1)});
  vertices.push_back({glm::vec3(0, axis_length, 0), glm::vec3(0, 0, 1), glm::vec4(1, 1, 1, 1)});
  vertices.push_back({glm::vec3(-axis_length, 0, 0), glm::vec3(0, 0, 1), glm::vec4(1, 1, 1, 1)});
  vertices.push_back({glm::vec3(axis_length, 0, 0), glm::vec3(0, 0, 1), glm::vec4(1, 1, 1, 1)});
  for (int i = 0; i < 6; ++i)
    index.push_back(vertices.size() - 6 + i);

  // Add a sphere at 0
  int current_index = vertices.size();
  const float sphere_radius = 1.0;
  const int sphere_resolution = 20;
  glm::vec4 sphere_color(0.0, 1.0, 1.0, 1.0);
  for (int i = 0; i < sphere_resolution; ++i) {
    float theta = 2.0 * M_PI * i / sphere_resolution;
    for (int j = 0; j < sphere_resolution; ++j) {
      float phi = M_PI * j / sphere_resolution;
      vertices.push_back({
        glm::vec3(
          sphere_radius * sin(phi) * cos(theta), 
          sphere_radius * sin(phi) * sin(theta), 
          sphere_radius * cos(phi)
        ),
        glm::vec3(sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi)), 
        sphere_color
      });
    }
  }
  for (int i = 0; i < sphere_resolution; ++i) {
    for (int j = 0; j < sphere_resolution; ++j) {
      index.push_back(current_index + i * sphere_resolution + j);
      index.push_back(current_index + ((i + 1) % sphere_resolution) * sphere_resolution + j);
      index.push_back(current_index + ((i + 1) % sphere_resolution) * sphere_resolution + (j + 1) % sphere_resolution);

      index.push_back(current_index + ((i + 1) % sphere_resolution) * sphere_resolution + (j + 1) % sphere_resolution);
      index.push_back(current_index + i * sphere_resolution + (j + 1) % sphere_resolution);
      index.push_back(current_index + i * sphere_resolution + j);
    }
  }

  // add line from (0, 0, 0) to (1, 0, 0)
  vertices.push_back({glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), sphere_color});
  vertices.push_back({glm::vec3(1, 0, 0), glm::vec3(0, 0, 1), sphere_color});
  for (int i = 0; i < 2; ++i)
    index.push_back(vertices.size() - 2 + i);

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

  glGenBuffers(1, &vbotext);
  glGenVertexArrays(1, &vaotext);
  glBindVertexArray(vaotext);
  glGenBuffers(1, &ibotext);

  // bind the ibo
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

  // vao end
  glBindVertexArray(0);
}

MyApplication::MyApplication(func_t func, std::optional<grad_t> grad, std::optional<hess_t> hess)
    : Application(),
      function(func),
      gradient(grad),
      hessian(hess),
      vertexShader(SHADER_DIR "/shader.vert.glsl", GL_VERTEX_SHADER),
      fragmentShader(SHADER_DIR "/shader.frag.glsl", GL_FRAGMENT_SHADER),
      shaderProgram({vertexShader, fragmentShader}),
      vertexShaderText(SHADER_DIR "/text.vert.glsl", GL_VERTEX_SHADER),
      fragmentShaderText(SHADER_DIR "/text.frag.glsl", GL_FRAGMENT_SHADER),
      shaderProgramText({vertexShaderText, fragmentShaderText}) {
  glCheckError(__FILE__, __LINE__);

  if(FT_Init_FreeType(&ft)) {
    fprintf(stderr, "Could not init freetype library\n");
    return;
  }
  if(FT_New_Face(ft, SHADER_DIR "/liberation-sans.ttf", 0, &face)) {
    fprintf(stderr, "Could not open font\n");
    return;
  }
  FT_Set_Pixel_Sizes(face, 0, 16);

  createGraph();

  // camera position
  camera_position = glm::vec3(15.0, 15.0, 15.0);

  view = glm::lookAt(camera_position, point_position, glm::vec3(0, 0, 1));
}

glm::vec3 MyApplication::getCameraDirection() {
  return camera_position - point_position;
}

float MyApplication::getCameraDistance() {
  return glm::length(getCameraDirection());
}

void MyApplication::moveView() {
  // get arrow keys state
  int left = glfwGetKey(getWindow(), GLFW_KEY_LEFT);
  int right = glfwGetKey(getWindow(), GLFW_KEY_RIGHT);
  int forward = glfwGetKey(getWindow(), GLFW_KEY_UP);
  int backward = glfwGetKey(getWindow(), GLFW_KEY_DOWN);
  int up = glfwGetKey(getWindow(), GLFW_KEY_PAGE_UP);
  int down = glfwGetKey(getWindow(), GLFW_KEY_PAGE_DOWN);

  float speed = glm::round(getCameraDistance()) * 0.002f;

  // compute new view matrix
  glm::vec3 translation(0, 0, 0);
  if (left == GLFW_PRESS)
    translation.x += speed;
  if (right == GLFW_PRESS)
    translation.x -= speed;
  if (forward == GLFW_PRESS)
    translation.y += speed;
  if (backward == GLFW_PRESS)
    translation.y -= speed;
  if (up == GLFW_PRESS)
    translation.z -= speed;
  if (down == GLFW_PRESS)
    translation.z += speed;

  glm::vec3 camera_direction = glm::normalize(getCameraDirection());
  camera_direction.z = 0;
  glm::vec3 camera_orthogonal = glm::vec3(-camera_direction.y, camera_direction.x, 0);
  translation = translation.y * camera_direction + translation.x * camera_orthogonal + translation.z * glm::vec3(0, 0, 1);

  camera_position.x -= translation.x;
  camera_position.y -= translation.y;
  camera_position.z -= translation.z;

  point_position.x -= translation.x;
  point_position.y -= translation.y;
  point_position.z -= translation.z;

  view = glm::translate(view, translation);
}

void MyApplication::rotateView() {
  if (glfwGetMouseButton(getWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
    // get mouse position
    double x_mouse_pos_current, y_mouse_pos_current;
    glfwGetCursorPos(getWindow(), &x_mouse_pos_current, &y_mouse_pos_current);

    // update mouse state

    if (!mouse_pressed) {
      mouse_pressed = true;
      x_mouse_pos = x_mouse_pos_current;
      y_mouse_pos = y_mouse_pos_current;
    }

    float delta_xi = (x_mouse_pos_current - x_mouse_pos) / getWidth();
    float delta_eta = (y_mouse_pos_current - y_mouse_pos) / getHeight();
    x_mouse_pos = x_mouse_pos_current;
    y_mouse_pos = y_mouse_pos_current;
    if (abs(delta_xi) < 0.001) {
      delta_xi = 0;
    }
    if (abs(delta_eta) < 0.001) {
      delta_eta = 0;
    }
    if (delta_xi == 0 && delta_eta == 0)
      return;

    glm::vec3 camera_direction = getCameraDirection();
    glm::vec3 camera_direction_normalized = glm::normalize(camera_direction);
    glm::vec3 global_up = glm::vec3(0, 0, 1);
    glm::vec3 camera_up_down = 
      glm::cross(camera_direction_normalized, global_up);
    glm::vec3 camera_left_right = 
      glm::cross(camera_direction_normalized, camera_up_down);
    glm::mat4x4 transformation =
      glm::translate(
        glm::rotate(
          glm::rotate(
            glm::translate(glm::mat4(1.0), -camera_direction_normalized), 
            delta_eta, camera_up_down
          ), delta_xi, camera_left_right
        ), camera_direction_normalized);
    glm::vec3 new_camera_direction = glm::normalize(glm::vec3(transformation * glm::vec4(camera_direction_normalized, 1.0))) * glm::length(camera_direction);
    glm::vec3 new_camera_position = point_position + new_camera_direction;
    camera_position = new_camera_position;
    view = glm::lookAt(camera_position, point_position, glm::vec3(0, 0, 1));
  }
  else {
    mouse_pressed = false;
  }
}

void MyApplication::zoomView() {
  if (glfwGetMouseButton(getWindow(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    // get mouse position
    double x_mouse_pos_current, y_mouse_pos_current;
    glfwGetCursorPos(getWindow(), &x_mouse_pos_current, &y_mouse_pos_current);

    // update mouse state

    if (!mouse_pressed_right) {
      mouse_pressed_right = true;
      y_mouse_pos_right = y_mouse_pos_current;
    }

    float delta_eta = (y_mouse_pos_current - y_mouse_pos_right) / getHeight();
    y_mouse_pos_right = y_mouse_pos_current;
    if (abs(delta_eta) < 0.001) {
      delta_eta = 0;
    }
    if (delta_eta == 0)
      return;

    glm::vec3 new_camera_position = point_position + (camera_position - point_position) * (1.0f + delta_eta);
    camera_position = new_camera_position;
    view = glm::lookAt(camera_position, point_position, glm::vec3(0, 0, 1));
  }
  else {
    mouse_pressed_right = false;
  }
}

void MyApplication::renderText(std::string text, float x, float y, float sx, float sy) {
  const char *p;
  FT_GlyphSlot &g = face->glyph;

  GLuint tex;
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
  shaderProgramText.setUniform("tex", 0);

  /* We require 1 byte alignment when uploading texture data */
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	/* Clamping to edges is important to prevent artifacts when scaling */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	/* Linear filtering usually looks best for text */
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  shaderProgramText.setAttribute("coord", 4, 4 * sizeof(GLfloat), 0);

    // Enable alpha blending
  glEnable(GL_BLEND);

  // Set the blend function
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for(p = text.c_str(); *p; p++) {
    if(FT_Load_Char(face, *p, FT_LOAD_RENDER))
        continue;
 
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      g->bitmap.width,
      g->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      g->bitmap.buffer
    );
 
    float x2 = x + g->bitmap_left * sx;
    float y2 = -y - g->bitmap_top * sy;
    float w = g->bitmap.width * sx;
    float h = g->bitmap.rows * sy;
 
    GLfloat box[4][4] = {
        {x2,     -y2    , 0, 0},
        {x2 + w, -y2    , 1, 0},
        {x2,     -y2 - h, 0, 1},
        {x2 + w, -y2 - h, 1, 1},
    };
 
    glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
 
    x += (g->advance.x/64) * sx;
    y += (g->advance.y/64) * sy;
  }

  glDisable(GL_BLEND);
	glDeleteTextures(1, &tex);
}

void MyApplication::changeOptimizer() {
  if (glfwGetKey(getWindow(), GLFW_KEY_1) == GLFW_PRESS) {
    if (button_pressed)
      return;
    button_pressed = true;
    optimizer = nullptr;
    points.clear();
  }
  else if (glfwGetKey(getWindow(), GLFW_KEY_2) == GLFW_PRESS) {
    if (button_pressed)
      return;
    button_pressed = true;
    if (!gradient || !hessian) {
      std::cout << "Gradient or Hessian are not defined" << std::endl;
      return;
    }
    optimizer = std::make_shared<Newton>(function, gradient.value(), hessian.value());
    optimizer->reset(point_position);
    points.clear();
    points.push_back(glm::vec3(point_position.x, point_position.y, function(glm::vec2(point_position.x, point_position.y))));
  }
  else if (glfwGetKey(getWindow(), GLFW_KEY_3) == GLFW_PRESS) {
    if (button_pressed)
      return;
    button_pressed = true;
    if (!gradient) {
      std::cout << "Gradient is not defined" << std::endl;
      return;
    }
    optimizer = std::make_shared<GradientDescent>(function, gradient.value(), 0.1f);
    optimizer->reset(point_position);
    points.clear();
    points.push_back(glm::vec3(point_position.x, point_position.y, function(glm::vec2(point_position.x, point_position.y))));
  }
  else if (glfwGetKey(getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
    if (button_pressed)
      return;
    button_pressed = true;
    if (optimizer) {
      glm::vec2 new_point = optimizer->step();
      if (new_point.x != new_point.x || new_point.y != new_point.y) {
        std::cout << "Iteration failed" << std::endl;
        return;
      }
      glm::vec3 new_point_position = glm::vec3(new_point, function(new_point));
      camera_position = new_point_position + getCameraDirection();
      point_position = new_point_position;
      view = glm::lookAt(camera_position, point_position, glm::vec3(0, 0, 1));
      points.push_back(new_point_position);
    }
  }
  else {
    button_pressed = false;
  }
}

void MyApplication::loop() {
  // exit on window close button pressed
  if (glfwWindowShouldClose(getWindow()))
    exit();

  changeOptimizer();

  // set matrix : projection + view
  projection = glm::perspective(float(2.0 * atan(getHeight() / 1920.f)),
                                getWindowRatio(), 0.1f, 1000.f);
  moveView();
  rotateView();
  zoomView();
  float t = getTime();
  if (t - last_refresh_time > 0.1f) {
    createGraph();
    last_refresh_time = t;
  }

  // clear
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  shaderProgram.use();

  // send uniforms
  shaderProgram.setUniform("projection", projection);
  shaderProgram.setUniform("view", view);
  shaderProgram.setUniform("model", glm::mat4(1.0));

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

  // draw the axes
  glCheckError(__FILE__, __LINE__);
  glDrawElements(GL_LINES,  // mode
                 6,         // count
                 GL_UNSIGNED_INT,  // type
                 (GLvoid*)(size * size * 2 * 3 * sizeof(GLuint))  // element array buffer offset
  );

  // draw the axes at the point position
  shaderProgram.setUniform("model", glm::translate(glm::mat4(1.0), point_position));
  glCheckError(__FILE__, __LINE__);
  glDrawElements(GL_LINES,  // mode
                 6,         // count
                 GL_UNSIGNED_INT,  // type
                 (GLvoid*)((size * size * 2 * 3 + 6) * sizeof(GLuint))  // element array buffer offset
  );

  for (auto &point : points) {
    // draw sphere
    shaderProgram.setUniform(
      "model", 
      glm::scale(
        glm::translate(
          glm::mat4(1.0), 
          point
        ), getCameraDistance() * 0.008f * glm::vec3(1.0, 1.0, 1.0)
      )
    );
    glCheckError(__FILE__, __LINE__);
    glDrawElements(GL_TRIANGLES,  // mode
                  6 * 20 * 20,   // count
                  GL_UNSIGNED_INT,  // type
                  (GLvoid*)((size * size * 2 * 3 + 6 + 6) * sizeof(GLuint))  // element array buffer offset
    );
  }

  for (size_t i = 1; i < points.size(); ++i) {
    // draw line
    glm::vec3 p1 = points[i - 1];
    // glm::vec3 p1 = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 p2 = points[i];
    // create the model matrix for the line from p1 to p2 based on line from (0, 0, 0) to (1, 0, 0)
    // Calculate the translation, scaling and rotation
    glm::vec3 translation = p1;
    float scale = glm::length(p2 - p1);
    glm::vec3 direction = glm::normalize(p2 - p1);
    glm::vec3 axis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), direction);
    float angle = glm::acos(glm::dot(glm::vec3(1.0f, 0.0f, 0.0f), direction));

    // Create the model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translation);
    model = glm::rotate(model, angle, axis);
    model = glm::scale(model, glm::vec3(scale, 1.0f, 1.0f));
    shaderProgram.setUniform("model", model);

    glCheckError(__FILE__, __LINE__);
    glDrawElements(GL_LINES,  // mode
                  2,         // count
                  GL_UNSIGNED_INT,  // type
                  (GLvoid*)((size * size * 2 * 3 + 6 + 6 + 6 * 20 * 20) * sizeof(GLuint))  // element array buffer offset
    );
  }

  shaderProgram.unuse();

  glCheckError(__FILE__, __LINE__);
  glBindVertexArray(vaotext);
  glBindBuffer(GL_ARRAY_BUFFER, vbotext);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibotext);

  // draw text
  shaderProgramText.use();

  shaderProgramText.setUniform("color", glm::vec4(1.0, 1.0, 1.0, 1.0));

  std::string point_position_str = "x:" + std::to_string(point_position.x) + ", y:" + std::to_string(point_position.y) + ", z: " + 
    std::to_string(point_position.z) + ", f(x,y): " + std::to_string(function(glm::vec2(point_position.x, point_position.y)));
  std::string optimizer_str = "Optimizer: " + (optimizer 
    ? (optimizer->toString() + " at (" + std::to_string(points[points.size() - 1].x) + ", " + std::to_string(points[points.size() - 1].y)) + ")" 
    : "None");

  float sx = 2.0 / getWidth();
  float sy = 2.0 / getHeight();
  renderText(point_position_str,
              -1 + 8 * sx, -1 + 10 * sy, sx, sy);
  renderText(optimizer_str,
              -1 + 8 * sx, 1 - 12 * sy, sx, sy);

  shaderProgramText.unuse();

  glCheckError(__FILE__, __LINE__); 
  glBindVertexArray(0);
}
