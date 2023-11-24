#include "../include/defines.hpp"
#include "../include/rectangle.hpp"
#include "../include/utils.hpp"
#include "SFML/Graphics/CircleShape.hpp"
#include "SFML/Graphics/Color.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Window.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class QuadTree {
public:
  Rectangle bounds;
  std::shared_ptr<QuadTree> children[4];
  std::shared_ptr<Particle> particle;
  float mass;
  sf::Vector2f mCenterPos;

  QuadTree(Rectangle &_bounds) : bounds(sf::Vector2f(0.0, 0.0), 0.0, 0.0) {
    bounds = _bounds;
    for (int i = 0; i < 4; i++) {
      children[i] = nullptr;
    }
    particle = nullptr;
    mass = 0.0;
    mCenterPos = sf::Vector2f(0.0, 0.0);
  }

  void calculateForce(Particle &calculationParticle) {
    if (!isDivided()) {
      if (particle != nullptr) {
        if (particle->index != calculationParticle.index) {
          sf::Vector2f attractionForce =
              calculationParticle.getAttractionForce(particle.get());
          calculationParticle.netForce += attractionForce;
        }
      }
      return;
    }

    float ratio = bounds.w / calculationParticle.getDistanceTo(mCenterPos);
    if (ratio < 0.5) {
      sf::Vector2f zeroVector(0.0, 0.0);
      Particle tempParticle(mCenterPos, zeroVector, mass, 1.0, 1000000);
      sf::Vector2f attractionForce =
          calculationParticle.getAttractionForce(&tempParticle);
      calculationParticle.netForce += attractionForce;
      return;
    }

    for (int i = 0; i < 4; i++) {
      children[i]->calculateForce(calculationParticle);
    }
  }
  void insert(Particle *insertParticle, std::vector<QuadTree> &qts) {
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

  void show(sf::RenderWindow &window, bool showBounds, float minVel,
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

  void show(sf::RenderWindow &window, std::vector<int> &particlesToDraw,
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
      auto findParticle =
          std::find_if(particlesToDraw.begin(), particlesToDraw.end(),
                       [&targetIndex](const auto &index) {
                         std::cout << index << " and " << targetIndex
                                   << std::endl;
                         return index == targetIndex;
                       });

      if (findParticle != particlesToDraw.end()) {
        particle->show(window, minVel, maxVel);
      }
    }
  }

  std::vector<int> query(Rectangle &rect) {
    std::vector<int> results;
    if (!bounds.intersects(rect)) {
      return results;
    }

    if (particle != nullptr) {
      if (rect.contains(*particle)) {
        results.push_back(particle->index);
        std::cout << "pushed!\n";
      }
    }
    if (isDivided()) {
      for (int i = 0; i < 4; i++) {
        std::vector<int> leafResults = children[i]->query(rect);
        results.insert(results.end(), leafResults.begin(), leafResults.end());
      }
    }

    return results;
  }

private:
  bool isDivided() { return children[0] != nullptr; }

  void subdivide(std::vector<QuadTree> &qts) {
    float x = bounds.topLeftPos.x;
    float y = bounds.topLeftPos.y;
    float w = bounds.w;
    float h = bounds.h;
    Rectangle topleft(sf::Vector2f(x, y), w / 2.0, h / 2.0);
    Rectangle topright(sf::Vector2f(x + w / 2.0, y), w / 2.0, h / 2.0);
    Rectangle bottomleft(sf::Vector2f(x, y + h / 2.0), w / 2.0, h / 2.0);
    Rectangle bottomright(sf::Vector2f(x + w / 2.0, y + h / 2.0), w / 2.0,
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

  void updateMass() {
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
    mCenterPos = sf::Vector2f(centerX, centerY);
  }
};

void spawnCircle(std::vector<Particle> &particles, sf::Vector2f center,
                 float radius, float particleMass, float particlesAmount) {
  for (int i = 0; i < particlesAmount; i++) {
    sf::Vector2f pos = randomInCircle(radius, 0.0, center);
    Particle newParticle(pos, sf::Vector2f(0.0, 0.0), particleMass, 0.00001, i);
    particles.push_back(newParticle);
  }
}

void spawnSpinningCircle(std::vector<Particle> &particles, sf::Vector2f center,
                         float radius, float particleMass,
                         float particlesAmount) {
  for (int i = 0; i < particlesAmount; i++) {
    sf::Vector2f pos = randomInCircle(radius, 1.0, center);
    float distanceToCenter = distance(pos, center);
    float orbitalVel = std::sqrt((G_CONST * 900.0) / distanceToCenter);
    sf::Vector2f dir =
        normalize(sf::Vector2f(pos.y - center.y, center.x - pos.x));
    Particle newParticle(pos, dir * orbitalVel, particleMass, 0.00001, i);
    particles.push_back(newParticle);
  }
}

void fillScreen(std::vector<Particle> &particles, float particleMass,
                float particlesAmount) {
  for (int i = 0; i < particlesAmount; i++) {
    sf::Vector2f pos = randomOnScreen();
    sf::Vector2f speed = randomSpeed();
    Particle newParticle(pos, speed, particleMass, 0.00001, i);
    particles.push_back(newParticle);
  }
}

void spawnGalaxy(std::vector<Particle> &particles, sf::Vector2f center,
                 sf::Vector2f initialVel, float radius, float sunMass,
                 float particleMass, float particlesAmount) {
  for (int i = 0; i < particlesAmount; i++) {
    sf::Vector2f pos = randomInCircle(radius, 5.0, center);
    float distanceToCenter = distance(pos, center);
    float orbitalVel = std::sqrt((G_CONST * sunMass) / distanceToCenter);
    sf::Vector2f dir =
        normalize(sf::Vector2f(pos.y - center.y, center.x - pos.x));
    Particle newParticle(pos, dir * orbitalVel, particleMass, 0.00001, i);
    particles.push_back(newParticle);
  }
  Particle sun(center, initialVel, sunMass, 1.5, particlesAmount);
  particles.push_back(sun);
}

void createQuadTree(std::vector<Particle> &particles,
                    std::vector<QuadTree> &qts, QuadTree &qt) {
  for (int i = 0; i < particles.size(); i++) {
    qt.insert(&particles[i], qts);
  }
}

void calculateNewPosition(Particle &particle, QuadTree &qt) {
  particle.netForce = sf::Vector2f(0.0, 0.0);
  qt.calculateForce(particle);

  sf::Vector2f acceleration = particle.netForce / particle.mass;
  particle.vel += acceleration;
  particle.pos += particle.vel;
}

void calculateMinMaxAvgVelocities(float &minVelAvg, float &maxVelAvg,
                                  std::vector<Particle> particles,
                                  int frameCount) {
  float maxVel = norm(particles[0].vel);
  float minVel = norm(particles[0].vel);

  for (int i = 0; i < particles.size(); i++) {
    if (norm(particles[i].vel) > maxVel) {
      maxVel = norm(particles[i].vel);
    }
    if (norm(particles[i].vel) < minVel) {
      minVel = norm(particles[i].vel);
    }
  }

  maxVelAvg =
      (maxVelAvg * (float)frameCount + maxVel) / ((float)frameCount + 1.0);
  minVelAvg =
      (minVelAvg * (float)frameCount + minVel) / ((float)frameCount + 1.0);
}

void updateTitle(sf::Clock &clock, sf::Time &elapsed, sf::RenderWindow &window,
                 int &frameCountForSecond) {
  elapsed = clock.getElapsedTime();

  if (elapsed.asSeconds() >= 1.0) {
    int fps = frameCountForSecond;
    frameCountForSecond = 0;
    clock.restart();

    std::string title = "FPS: " + std::to_string(fps);
    window.setTitle(title);
  }
}

void saveVideo(std::string &ffmpegCommand) {
  std::time_t currentTime = std::time(nullptr);
  struct std::tm *timeInfo = std::localtime(&currentTime);
  const char *format = "%Y-%m-%d_%H.%M.%S";
  char formattedTime[100];
  std::strftime(formattedTime, sizeof(formattedTime), format, timeInfo);
  int returnCode =
      std::system((ffmpegCommand + formattedTime + ".mp4").c_str());
  if (returnCode == 0) {
    std::cout << "FFmpeg command executed successfully." << std::endl;
    fs::path cachePath("image-cache");
    for (const auto &entry : fs::directory_iterator(cachePath)) {
      if (fs::is_regular_file(entry)) {
        fs::remove(entry.path());
      }
    }
  } else {
    std::cout << "FFmpeg command execution failed." << std::endl;
  }
}

void saveScreen(sf::RenderWindow &window, int &frameCount) {
  sf::Texture texture;
  texture.create(window.getSize().x, window.getSize().y);
  texture.update(window);
  int desiredWidth = 5;
  char paddingChar = '0';
  std::string framesStr = std::to_string(frameCount);
  int numZeros = desiredWidth - framesStr.length();
  if (numZeros > 0) {
    framesStr = std::string(numZeros, '0') + framesStr;
  }
  if (texture.copyToImage().saveToFile("image-cache/" + framesStr + ".png")) {
    std::cout << "Screenshot saved " + std::to_string(frameCount) << "\n";
  } else {
    std::cout << "Failed\n";
  }
}

int main() {
  sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT),
                          "Gravitation Particles");
  fs::path cachePath("image-cache");
  if (!fs::create_directory(cachePath)) {
    std::cout << "Cache folder already exists!\n";
  }
  fs::path resultsPath("results");
  if (!fs::create_directory(resultsPath)) {
    std::cout << "Results folder already exists!\n";
  }
  sf::Clock clock;
  int frameCountForSecond = 0;
  sf::Time elapsed;
  std::vector<Particle> particles;
  // spawnCircle(particles, sf::Vector2f(WIDTH / 2.0, HEIGHT / 2.0), 120.0, 3.0,
  //             PARTICLES_AMOUNT);
  // spawnSpinningCircle(particles, sf::Vector2f(WIDTH / 2.0, HEIGHT / 2.0),
  // 120.0, 3.0, PARTICLES_AMOUNT);
  // spawnSpinningCircle(particles, sf::Vector2f(200.0, 200.0), 200.0, 3.0,
  //                     PARTICLES_AMOUNT / 2.0);
  // spawnSpinningCircle(particles, sf::Vector2f(WIDTH - 200.0, HEIGHT - 200.0),
  //                     200.0, 3.0, PARTICLES_AMOUNT / 2.0);

  // spawnGalaxy(particles, sf::Vector2f(100.0, 100.0), sf::Vector2f(0.02, 0.0),
  //             100.0, 1000.0, 0.01, PARTICLES_AMOUNT);
  // spawnGalaxy(particles, sf::Vector2f(WIDTH - 100.0, HEIGHT - 100.0),
  //             sf::Vector2f(-0.02, 0.0), 100.0, 1000.0, 0.01,
  //             PARTICLES_AMOUNT);
  // spawnGalaxy(particles, sf::Vector2f(WIDTH / 2.0, HEIGHT / 2.0),
  //             sf::Vector2f(-0.2, 0.0), 100.0, 1000.0, 0.01,
  //             PARTICLES_AMOUNT);
  fillScreen(particles, 3.0, PARTICLES_AMOUNT);
  auto compareByMass = [](const Particle &a, const Particle &b) {
    return a.mass < b.mass;
  };
  std::sort(particles.begin(), particles.end(), compareByMass);

  float minVelAvg = 0.0;
  float maxVelAvg = 0.0;
  int frameCount = 0;
  bool recording = RECORD_FROM_START;
  std::string ffmpegCommand =
      "ffmpeg -framerate 60 -pattern_type glob -i \"image-cache/*.png\" -vf "
      "eq=saturation=2 -c:v libx264 -pix_fmt yuv420p results/";

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    window.clear();

    updateTitle(clock, elapsed, window, frameCountForSecond);

    std::vector<QuadTree> qts;
    Rectangle bounds(sf::Vector2f(0.0, 0.0), WIDTH, HEIGHT);
    QuadTree qt(bounds);
    qts.push_back(qt);
    createQuadTree(particles, qts, qt);

    for (int i = 0; i < particles.size(); i++) {
      calculateNewPosition(particles[i], qt);
    }

    calculateMinMaxAvgVelocities(minVelAvg, maxVelAvg, particles, frameCount);

    qt.show(window, false, minVelAvg, maxVelAvg);

    window.display();
    frameCountForSecond++;
    frameCount++;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
      recording = true;
      std::cout << "Recording!\n";
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
      recording = false;
      saveVideo(ffmpegCommand);
    }
    if (recording) {
      saveScreen(window, frameCount);
    }
  }

  return 0;
}
