/**
 * Main.cpp skeleton
 * Contributors:
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#include "MyApplication.hpp"

int main(int argc, const char* argv[]) {
  auto function = [](glm::vec2 position) {
    return 2.0 * sin(position.x * position.y) * exp(-0.05 * (position.x * position.x + position.y * position.y)) 
    + 0.5 * sin(position.x * position.x) + exp(0.1 * position.x);
  };
  MyApplication app = MyApplication(function);
  app.run();
  return 0;
}
