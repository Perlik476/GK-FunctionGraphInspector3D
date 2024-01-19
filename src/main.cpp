/**
 * Main.cpp skeleton
 * Contributors:
 *      * Arthur Sonzogni
 * Licence:
 *      * MIT
 */

#include "MyApplication.hpp"
#include "utils.hpp"
#include <optional>

int main(int argc, const char* argv[]) {
  auto function = [](glm::vec2 position) {
    return sin(position.x) + cos(position.y);
  };
  auto gradient = [](glm::vec2 position) {
    return glm::vec2(cos(position.x), -sin(position.y));
  };
  auto hessian = [](glm::vec2 position) {
    return glm::mat2(-sin(position.x), 0.0, 0.0, -cos(position.y));
  };
  MyApplication app = MyApplication(function, std::make_optional(gradient), std::make_optional(hessian));
  app.run();
  return 0;
}
