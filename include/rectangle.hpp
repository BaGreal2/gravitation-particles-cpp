#pragma once

#include "particle.hpp"
#include <SFML/Graphics.hpp>

class Rectangle {
public:
  sf::Vector2f top_left_pos;
  float w, h;

  Rectangle(sf::Vector2f _top_left_pos, float _w, float _h);

  bool contains(Particle &particle);

  bool intersects(Rectangle &rect);

  void show(sf::RenderWindow &window);
};
