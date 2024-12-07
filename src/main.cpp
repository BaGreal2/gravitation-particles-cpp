#include "../include/defines.hpp"
#include "../include/rectangle.hpp"
#include "../include/quadtree.hpp"
#include "../include/utils.hpp"
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

using std::vector, std::string, std::sqrt, std::to_string, std::time, std::time_t, std::system, sf::Vector2f, sf::RenderWindow, sf::Clock, sf::Time, sf::Texture;

void spawnCircle(vector<Particle> &particles, Vector2f center,
                 float radius, float particleMass, float particlesAmount) {
  for (int i = 0; i < particlesAmount; i++) {
    Vector2f pos = randomInCircle(radius, 0.0, center);
    Particle newParticle(pos, Vector2f(0.0, 0.0), particleMass, 0.00001, i);
    particles.push_back(newParticle);
  }
}

void spawnSpinningCircle(vector<Particle> &particles, Vector2f center,
                         float radius, float particleMass,
                         float particlesAmount) {
  for (int i = 0; i < particlesAmount; i++) {
    Vector2f pos = randomInCircle(radius, 1.0, center);
    float distanceToCenter = distance(pos, center);
    float orbitalVel = sqrt((G_CONST * 900.0) / distanceToCenter);
    Vector2f dir =
        normalize(Vector2f(pos.y - center.y, center.x - pos.x));
    Particle newParticle(pos, dir * orbitalVel, particleMass, 0.00001, i);
    particles.push_back(newParticle);
  }
}

void fillScreen(vector<Particle> &particles, float particleMass,
                float particlesAmount) {
  for (int i = 0; i < particlesAmount; i++) {
    Vector2f pos = randomOnScreen();
    Vector2f speed = randomSpeed();
    Particle newParticle(pos, speed, particleMass, 0.00001, i);
    particles.push_back(newParticle);
  }
}

void spawnGalaxy(vector<Particle> &particles, Vector2f center,
                 Vector2f initialVel, float radius, float sunMass,
                 float particleMass, float particlesAmount) {
  for (int i = 0; i < particlesAmount; i++) {
    Vector2f pos = randomInCircle(radius, 5.0, center);
    float distanceToCenter = distance(pos, center);
    float orbitalVel = sqrt((G_CONST * sunMass) / distanceToCenter);
    Vector2f dir =
        normalize(Vector2f(pos.y - center.y, center.x - pos.x));
    Particle newParticle(pos, dir * orbitalVel, particleMass, 0.00001, i);
    particles.push_back(newParticle);
  }
  Particle sun(center, initialVel, sunMass, 1.5, particlesAmount);
  particles.push_back(sun);
}

void createQuadTree(vector<Particle> &particles,
                    vector<QuadTree> &qts, QuadTree &qt) {
  for (int i = 0; i < particles.size(); i++) {
    qt.insert(&particles[i], qts);
  }
}

void calculateNewPosition(Particle &particle, QuadTree &qt) {
  particle.netForce = Vector2f(0.0, 0.0);
  qt.calculateForce(particle);

  Vector2f acceleration = particle.netForce / particle.mass;
  particle.vel += acceleration;
  particle.pos += particle.vel;
}

void calculateMinMaxAvgVelocities(float &minVelAvg, float &maxVelAvg,
                                  vector<Particle> particles,
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

void updateTitle(Clock &clock, Time &elapsed, RenderWindow &window,
                 int &frameCountForSecond) {
  elapsed = clock.getElapsedTime();

  if (elapsed.asSeconds() >= 1.0) {
    int fps = frameCountForSecond;
    frameCountForSecond = 0;
    clock.restart();

    string title = "FPS: " + to_string(fps);
    window.setTitle(title);
  }
}

void saveVideo(string &ffmpegCommand) {
  time_t currentTime = time(nullptr);
  struct std::tm *timeInfo = std::localtime(&currentTime);
  const char *format = "%Y-%m-%d_%H.%M.%S";
  char formattedTime[100];
  std::strftime(formattedTime, sizeof(formattedTime), format, timeInfo);
  int returnCode =
      system((ffmpegCommand + formattedTime + ".mp4").c_str());
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

void saveScreen(RenderWindow &window, int &frameCount) {
  Texture texture;
  texture.create(window.getSize().x, window.getSize().y);
  texture.update(window);
  int desiredWidth = 5;
  char paddingChar = '0';
  string framesStr = to_string(frameCount);
  int numZeros = desiredWidth - framesStr.length();
  if (numZeros > 0) {
    framesStr = string(numZeros, '0') + framesStr;
  }
  if (texture.copyToImage().saveToFile("image-cache/" + framesStr + ".png")) {
    std::cout << "Screenshot saved " + to_string(frameCount) << "\n";
  } else {
    std::cout << "Failed\n";
  }
}

int main() {
  RenderWindow window(sf::VideoMode(WIDTH, HEIGHT),
                          "Gravitation Particles");
  fs::path cachePath("image-cache");
  if (!fs::create_directory(cachePath)) {
    std::cout << "Cache folder already exists!\n";
  }
  fs::path resultsPath("results");
  if (!fs::create_directory(resultsPath)) {
    std::cout << "Results folder already exists!\n";
  }
  Clock clock;
  int frameCountForSecond = 0;
  Time elapsed;
  vector<Particle> particles;
  // fillScreen(particles, 3.0, PARTICLES_AMOUNT);
  spawnGalaxy(particles,
              Vector2f(WIDTH / 2.0 - 110.0, HEIGHT / 2.0 - 110.0),
              Vector2f(0.3, 0.0), 120.0, 1000.0, 1.0, PARTICLES_AMOUNT);
  // spawnGalaxy(particles,
  //             Vector2f(WIDTH / 2.0 + 110.0, HEIGHT / 2.0 + 110.0),
  //             Vector2f(-0.3, 0.1), 120.0, 1000.0, 1.0, PARTICLES_AMOUNT);
  auto compareByMass = [](const Particle &a, const Particle &b) {
    return a.mass < b.mass;
  };
  std::sort(particles.begin(), particles.end(), compareByMass);

  float minVelAvg = 0.0;
  float maxVelAvg = 0.0;
  int frameCount = 0;
  bool recording = RECORD_FROM_START;
  string ffmpegCommand =
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

    vector<QuadTree> qts;
    Rectangle bounds(Vector2f(0.0, 0.0), WIDTH, HEIGHT);
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
