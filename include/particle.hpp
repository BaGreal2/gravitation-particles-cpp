#pragma once

#include <SFML/Graphics.hpp>

class Particle {
public:
  sf::Vector2f pos, vel, netForce;
  float mass, radius;
  int index;

  Particle();
  Particle(sf::Vector2f _pos, sf::Vector2f _vel, float _mass, float _radius,
           int _index);
  sf::Vector2f get_attraction_force(const Particle *anotherParticle);
  float get_distance_to(sf::Vector2f object);
  sf::Color get_color(float value, sf::Color &left, sf::Color &right);
  void show(sf::RenderWindow &window, float minVel, float maxVel);
};
