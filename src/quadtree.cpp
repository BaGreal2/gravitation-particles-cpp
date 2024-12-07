#include "../include/quadtree.hpp"

using std::vector, sf::Vector2f, sf::RenderWindow;

QuadTree::QuadTree(Rectangle &_bounds)
    : bounds(Vector2f(0.0, 0.0), 0.0, 0.0) {
  bounds = _bounds;
  for (int i = 0; i < 4; i++) {
    children[i] = nullptr;
  }
  particle = nullptr;
  mass = 0.0;
  mCenterPos = Vector2f(0.0, 0.0);
}

void QuadTree::calculateForce(Particle &calculationParticle) {
  if (!isDivided()) {
    if (particle != nullptr) {
      if (particle->index != calculationParticle.index) {
        Vector2f attractionForce =
            calculationParticle.getAttractionForce(particle.get());
        calculationParticle.netForce += attractionForce;
      }
    }
    return;
  }

  float ratio = bounds.w / calculationParticle.getDistanceTo(mCenterPos);
  if (ratio < 0.5) {
    Vector2f zeroVector(0.0, 0.0);
    Particle tempParticle(mCenterPos, zeroVector, mass, 1.0, 1000000);
    Vector2f attractionForce =
        calculationParticle.getAttractionForce(&tempParticle);
    calculationParticle.netForce += attractionForce;
    return;
  }

  for (int i = 0; i < 4; i++) {
    children[i]->calculateForce(calculationParticle);
  }
}

void QuadTree::insert(Particle *insertParticle, vector<QuadTree> &qts) {
  if (!bounds.contains(*insertParticle)) {
    return;
  }

  if (particle == nullptr) {
    particle = std::make_shared<Particle>(*insertParticle);
  } else {
    if (!isDivided()) {
      subdivide(qts);
    }

    for (int i = 0; i < 4; i++) {
      children[i]->insert(insertParticle, qts);
    }

    updateMass();
  }
}

void QuadTree::show(RenderWindow &window, bool showBounds, float minVel,
                    float maxVel) {
  if (showBounds) {
    bounds.show(window);
  }

  if (children[0] != nullptr) {
    for (int i = 0; i < 4; i++) {
      children[i]->show(window, showBounds, minVel, maxVel);
    }
  }

  if (particle != nullptr) {
    particle->show(window, minVel, maxVel);
  }
}

void QuadTree::show(RenderWindow &window, vector<int> &particlesToDraw,
                    bool showBounds, float minVel, float maxVel) {
  if (showBounds) {
    bounds.show(window);
  }

  if (children[0] != nullptr) {
    for (int i = 0; i < 4; i++) {
      children[i]->show(window, particlesToDraw, showBounds, minVel, maxVel);
    }
  }

  if (particle != nullptr) {
    int targetIndex = particle->index;
    auto findParticle = std::find_if(
        particlesToDraw.begin(), particlesToDraw.end(),
        [&targetIndex](const auto &index) { return index == targetIndex; });

    if (findParticle != particlesToDraw.end()) {
      particle->show(window, minVel, maxVel);
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
      // std::cout << "pushed!\n";
    }
  }
  if (isDivided()) {
    for (int i = 0; i < 4; i++) {
      vector<int> leafResults = children[i]->query(rect);
      results.insert(results.end(), leafResults.begin(), leafResults.end());
    }
  }

  return results;
}

bool QuadTree::isDivided() { return children[0] != nullptr; }

void QuadTree::subdivide(vector<QuadTree> &qts) {
  float x = bounds.topLeftPos.x;
  float y = bounds.topLeftPos.y;
  float w = bounds.w;
  float h = bounds.h;
  Rectangle topleft(Vector2f(x, y), w / 2.0, h / 2.0);
  Rectangle topright(Vector2f(x + w / 2.0, y), w / 2.0, h / 2.0);
  Rectangle bottomleft(Vector2f(x, y + h / 2.0), w / 2.0, h / 2.0);
  Rectangle bottomright(Vector2f(x + w / 2.0, y + h / 2.0), w / 2.0,
                        h / 2.0);
  qts.push_back(QuadTree(topleft));
  qts.push_back(QuadTree(topright));
  qts.push_back(QuadTree(bottomleft));
  qts.push_back(QuadTree(bottomright));
  children[0] = std::make_shared<QuadTree>(qts[qts.size() - 4]);
  children[1] = std::make_shared<QuadTree>(qts[qts.size() - 3]);
  children[2] = std::make_shared<QuadTree>(qts[qts.size() - 2]);
  children[3] = std::make_shared<QuadTree>(qts[qts.size() - 1]);
}

void QuadTree::updateMass() {
  if (!isDivided()) {
    if (particle == nullptr) {
      return;
    }
    mass = particle->mass;
    mCenterPos = particle->pos;
    return;
  }
  float massSum = 0.0;
  float centerX = 0.0;
  float centerY = 0.0;

  for (int i = 0; i < 4; i++) {
    children[i]->updateMass();
    massSum += children[i]->mass;
    centerX += children[i]->mCenterPos.x * children[i]->mass;
    centerY += children[i]->mCenterPos.y * children[i]->mass;
  }
  mass = massSum;
  centerX /= massSum;
  centerY /= massSum;
  mCenterPos = Vector2f(centerX, centerY);
}
