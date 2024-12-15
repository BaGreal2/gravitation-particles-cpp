#include "quadtree.hpp"
#include "defines.hpp"
#include <thread>

using std::vector, sf::Vector2f, sf::RenderWindow;

QuadTree::QuadTree(Rectangle &_bounds) : bounds(Vector2f(0.0, 0.0), 0.0, 0.0) {
  bounds = _bounds;
  for (int i = 0; i < 4; i++) {
    children[i] = nullptr;
  }
  particle = nullptr;
  mass = 0.0;
  m_center_pos = Vector2f(0.0, 0.0);
}

void QuadTree::calc_force(Particle &another_particle) {
  if (!is_divided()) {
    if (particle != nullptr) {
      if (particle->index != another_particle.index) {
        Vector2f attractionForce =
            another_particle.get_attraction_force(particle.get());
        another_particle.netForce += attractionForce;
      }
    }
    return;
  }

  float ratio = bounds.w / another_particle.get_distance_to(m_center_pos);
  if (ratio < 0.5) {
    Vector2f zeroVector(0.0, 0.0);
    Particle tempParticle(m_center_pos, zeroVector, mass, 1.0, 1000000);
    Vector2f attractionForce =
        another_particle.get_attraction_force(&tempParticle);
    another_particle.netForce += attractionForce;
    return;
  }

  for (int i = 0; i < 4; i++) {
    children[i]->calc_force(another_particle);
  }
}

void QuadTree::insert(Particle &new_particle) {
  std::lock_guard<std::recursive_mutex> lock(mtx);
  if (!bounds.contains(new_particle)) {
    return;
  }

  if (particle == nullptr) {
    particle = std::make_shared<Particle>(new_particle);
  } else {
    if (!is_divided()) {
      subdivide();
    }

    for (int i = 0; i < 4; i++) {
      children[i]->insert(new_particle);
    }

    update_mass();
  }
}

void QuadTree::show(RenderWindow &window, float minVel, float maxVel) {
  if (SHOW_BOUNDS) {
    bounds.show(window);
  }

  if (children[0] != nullptr) {
    for (int i = 0; i < 4; i++) {
      children[i]->show(window, minVel, maxVel);
    }
  }

  if (particle != nullptr) {
    particle->show(window, minVel, maxVel);
  }
}

void QuadTree::show(RenderWindow &window, vector<int> &particles_to_draw,
                    float min_vel, float max_vel) {
  if (SHOW_BOUNDS) {
    bounds.show(window);
  }

  if (children[0] != nullptr) {
    for (int i = 0; i < 4; i++) {
      children[i]->show(window, particles_to_draw, min_vel, max_vel);
    }
  }

  if (particle != nullptr) {
    int targetIndex = particle->index;
    auto findParticle = std::find_if(
        particles_to_draw.begin(), particles_to_draw.end(),
        [&targetIndex](const auto &index) { return index == targetIndex; });

    if (findParticle != particles_to_draw.end()) {
      particle->show(window, min_vel, max_vel);
    }
  }
}

vector<int> QuadTree::query(Rectangle &rect) {
  vector<int> results;
  if (!bounds.intersects(rect)) {
    return results;
  }

  if (particle != nullptr) {
    if (rect.contains(*particle)) {
      results.push_back(particle->index);
    }
  }
  if (is_divided()) {
    for (int i = 0; i < 4; i++) {
      vector<int> leafResults = children[i]->query(rect);
      results.insert(results.end(), leafResults.begin(), leafResults.end());
    }
  }

  return results;
}

bool QuadTree::is_divided() { return children[0] != nullptr; }

void QuadTree::subdivide() {
  float x = bounds.top_left_pos.x;
  float y = bounds.top_left_pos.y;
  float w = bounds.w;
  float h = bounds.h;
  Rectangle topleft(Vector2f(x, y), w / 2.0, h / 2.0);
  Rectangle topright(Vector2f(x + w / 2.0, y), w / 2.0, h / 2.0);
  Rectangle bottomleft(Vector2f(x, y + h / 2.0), w / 2.0, h / 2.0);
  Rectangle bottomright(Vector2f(x + w / 2.0, y + h / 2.0), w / 2.0, h / 2.0);

  children[0] = std::make_shared<QuadTree>(topleft);
  children[1] = std::make_shared<QuadTree>(topright);
  children[2] = std::make_shared<QuadTree>(bottomleft);
  children[3] = std::make_shared<QuadTree>(bottomright);
}

void QuadTree::update_mass() {
  if (!is_divided()) {
    if (particle == nullptr) {
      return;
    }
    mass = particle->mass;
    m_center_pos = particle->pos;
    return;
  }
  float mass_sum = 0.0;
  float center_x = 0.0;
  float center_y = 0.0;

  for (int i = 0; i < 4; i++) {
    children[i]->update_mass();
    mass_sum += children[i]->mass;
    center_x += children[i]->m_center_pos.x * children[i]->mass;
    center_y += children[i]->m_center_pos.y * children[i]->mass;
  }
  mass = mass_sum;
  center_x /= mass_sum;
  center_y /= mass_sum;
  m_center_pos = Vector2f(center_x, center_y);
}
