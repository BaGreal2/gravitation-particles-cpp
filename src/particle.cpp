#include "../include/particle.hpp"
#include "../include/defines.hpp"
#include "../include/utils.hpp"
#include <SFML/Graphics.hpp>

Particle::Particle(sf::Vector2f _pos, sf::Vector2f _vel, float _mass,
                   float _radius, int _index) {
  pos = _pos;
  vel = _vel;
  netForce = sf::Vector2f(0.0, 0.0);
  mass = _mass;
  radius = _radius;
  index = _index;
}

sf::Vector2f Particle::getAttractionForce(const Particle *anotherParticle) {
  float r = std::sqrt(std::pow(distance(pos, anotherParticle->pos), 2) +
                      std::pow(SOFTENING, 2));
  sf::Vector2f dir = normalize(anotherParticle->pos - pos);
  float magnitude = G_CONST * ((mass * anotherParticle->mass) / std::pow(r, 2));
  sf::Vector2f force = dir * magnitude;

  return force;
}

float Particle::getDistanceTo(sf::Vector2f object) {
  return hypot(object.x - pos.x, object.y - pos.y);
}

sf::Color Particle::getColor(float value, sf::Color &left, sf::Color &right) {
  sf::Color color(((1.0 - value) * left.r + value * right.r),
                  ((1.0 - value) * left.g + value * right.g),
                  ((1.0 - value) * left.b + value * right.b));
  return color;
}

void Particle::show(sf::RenderWindow &window, float minVel, float maxVel) {
  float midVel = (minVel + maxVel) / 2.0;
  sf::Color left = sf::Color(42, 110, 187);
  sf::Color middle = sf::Color(122, 59, 160);
  sf::Color right = sf::Color(197, 63, 63);
  std::vector<sf::Color> colors = {left, middle, right};

  float normVel = norm(vel);
  sf::Color newColor;

  newColor = multiColourLerp(colors, normVel / maxVel);

  if (normVel < midVel) {
    newColor = getColor((normVel - minVel) / midVel, left, middle);
  } else {
    newColor = getColor((normVel - minVel - midVel) / midVel, middle, right);
  }

  // sf::RectangleShape point(sf::Vector2f(1.0, 1.0));
  sf::CircleShape point(1.0);
  point.setPosition(pos);
  point.setFillColor(left);
  point.setFillColor(newColor);
  window.draw(point);
}
