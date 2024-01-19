#ifndef UTILS_HPP
#define UTILS_HPP

#include <functional>
#include <glm/glm.hpp>

using func_t = std::function<float(glm::vec2)>;
using grad_t = std::function<glm::vec2(glm::vec2)>;
using hess_t = std::function<glm::mat2(glm::vec2)>;

#endif // UTILS_HPP