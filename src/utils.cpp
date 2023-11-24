#include "../include/utils.hpp"
#include "../include/defines.hpp"
#include <cmath>
#include <random>

float distance(const sf::Vector2f &point1, const sf::Vector2f &point2) {
  float dx = point1.x - point2.x;
  float dy = point1.y - point2.y;
  return std::sqrt(dx * dx + dy * dy);
}

float norm(const sf::Vector2f &vector) {
  return std::sqrt(vector.x * vector.x + vector.y * vector.y);
}

sf::Vector2f normalize(const sf::Vector2f &vector) {
  float length = std::sqrt(vector.x * vector.x + vector.y * vector.y);

  if (length != 0) {
    return sf::Vector2f(vector.x / length, vector.y / length);
  } else {
    return vector;
  }
}

sf::Vector2f randomInCircle(float radius, float padding, sf::Vector2f center) {
  std::random_device rd;
  float angle = std::generate_canonical<float, 10>(rd) * 2.0 * M_PI;
  float distance =
      padding + std::generate_canonical<float, 10>(rd) * (radius - padding);

  return sf::Vector2f(distance * std::cos(angle), distance * std::sin(angle)) +
         center;
}

sf::Vector2f randomOnScreen() {
  std::random_device rd;
  float x = std::generate_canonical<float, 10>(rd) * WIDTH;
  float y = std::generate_canonical<float, 10>(rd) * HEIGHT;
  return sf::Vector2f(x, y);
}

sf::Vector2f randomSpeed() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<float> distribution(-0.45f, 0.45f);

  float x = distribution(gen);
  float y = distribution(gen);

  return sf::Vector2f(x, y);
}
