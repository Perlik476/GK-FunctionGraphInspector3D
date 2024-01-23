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
    return 0.0001f * pow(position.x, 4) + 0.0001f * pow(position.y, 4) + sin(position.x + position.y);
  };
  auto gradient = [](glm::vec2 position) {
    return glm::vec2(0.0001f * 4 * pow(position.x, 3) + cos(position.x + position.y),
                     0.0001f * 4 * pow(position.y, 3) + cos(position.x + position.y));
  };
  auto hessian = [](glm::vec2 position) {
    return glm::mat2(
      0.0001f * 12.0 * pow(position.x, 2) - sin(position.x + position.y),
      -sin(position.x + position.y),
      -sin(position.x + position.y),
      0.0001f * 12.0 * pow(position.y, 2) - sin(position.x + position.y)
    );
  };
  MyApplication app = MyApplication(function, std::make_optional(gradient), std::make_optional(hessian));
  app.run();
  return 0;
}
