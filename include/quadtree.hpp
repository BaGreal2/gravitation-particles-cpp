#pragma once

#include <SFML/Graphics.hpp>
#include "../include/rectangle.hpp"

class QuadTree {
public:
  Rectangle bounds;
  std::shared_ptr<QuadTree> children[4];
  std::shared_ptr<Particle> particle;
  float mass;
  sf::Vector2f mCenterPos;

  QuadTree(Rectangle &_bounds);
  void calculateForce(Particle &calculationParticle);
  void insert(Particle *insertParticle, std::vector<QuadTree> &qts);
  void show(sf::RenderWindow &window, bool showBounds, float minVel,
            float maxVel);
  void show(sf::RenderWindow &window, std::vector<int> &particlesToDraw,
            bool showBounds, float minVel, float maxVel);
  std::vector<int> query(Rectangle &rect);
private:
  bool isDivided();
  void subdivide(std::vector<QuadTree> &qts);
  void updateMass();
};
