#pragma once

#include "rectangle.hpp"
#include <SFML/Graphics.hpp>

class QuadTree {
public:
  Rectangle bounds;
  std::shared_ptr<QuadTree> children[4];
  std::shared_ptr<Particle> particle;
  float mass;
  sf::Vector2f m_center_pos;

  QuadTree(Rectangle &_bounds);
  void calc_force(Particle &calculationParticle);
  void insert(Particle &insertParticle);
  void show(sf::RenderWindow &window, float minVel, float maxVel);
  void show(sf::RenderWindow &window, std::vector<int> &particlesToDraw,
            float minVel, float maxVel);
  std::vector<int> query(Rectangle &rect);

private:
  bool is_divided();
  void subdivide();
  void update_mass();
};
