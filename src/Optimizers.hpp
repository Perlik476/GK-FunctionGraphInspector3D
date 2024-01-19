#ifndef OPTIMIZERS_HPP
#define OPTIMIZERS_HPP

#include <utils.hpp>
#include <string>

class Optimizer {
public:
    virtual glm::vec2 step() = 0;
    virtual void reset(glm::vec2 start) = 0;
    virtual std::string toString() = 0;
};

class Newton : public Optimizer {
public:
    Newton(func_t func, grad_t grad, hess_t hess) : func(func), grad(grad), hess(hess) {}

    glm::vec2 step() override {
        glm::vec2 grad = this->grad(point);
        glm::mat2 hess = this->hess(point);
        glm::vec2 step = -glm::inverse(hess) * grad; // TODO solve linear system
        point += step;
        return point;
    }

    void reset(glm::vec2 start) override {
        point = start;
    }

    std::string toString() override {
        return "Newton";
    }
private:
    func_t func;
    grad_t grad;
    hess_t hess;

    glm::vec2 point;
};

class GradientDescent : public Optimizer {
public:
    GradientDescent(func_t func, grad_t grad, float step_size) : func(func), grad(grad), step_size(step_size) {}

    glm::vec2 step() override {
        glm::vec2 grad = this->grad(point);
        glm::vec2 step = -grad;
        point += step_size * step;
        return point;
    }

    void reset(glm::vec2 start) override {
        point = start;
    }

    std::string toString() override {
        return "Gradient Descent";
    }
private:
    func_t func;
    grad_t grad;
    float step_size;

    glm::vec2 point;
};

#endif // OPTIMIZERS_HPP