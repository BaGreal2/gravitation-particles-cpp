#include "defines.hpp"
#include "quadtree.hpp"
#include "rectangle.hpp"
#include "utils.hpp"
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

using std::vector, std::string, std::sqrt, std::to_string, std::time,
    std::time_t, std::system, sf::Vector2f, sf::RenderWindow, sf::Clock,
    sf::Time, sf::Texture;

void spawn_circle(vector<Particle> &particles, Vector2f center) {
  for (int i = 0; i < PARTICLES_AMOUNT; i++) {
    Vector2f pos = random_in_circle(PARTICLE_RADIUS, 0.0, center);
    Particle newParticle(pos, Vector2f(0.0, 0.0), PARTICLE_MASS, 0.00001, i);
    particles.push_back(newParticle);
  }
}

void spawn_spinning_circle(vector<Particle> &particles, Vector2f center) {
  for (int i = 0; i < PARTICLES_AMOUNT; i++) {
    Vector2f pos = random_in_circle(PARTICLE_RADIUS, 1.0, center);

    float distance_to_center = distance(pos, center);
    float orbital_vel = sqrt((G_CONST * 900.0) / distance_to_center);

    Vector2f dir = normalize(Vector2f(pos.y - center.y, center.x - pos.x));
    Particle new_particle(pos, dir * orbital_vel, PARTICLE_MASS, 0.00001, i);

    particles.push_back(new_particle);
  }
}

void spawn_galaxy(vector<Particle> &particles, Vector2f center,
                  Vector2f initial_vel, float sun_mass, float radius) {
  for (int i = 0; i < PARTICLES_AMOUNT; i++) {
    Vector2f pos = random_in_circle(radius, 5.0, center);

    float distance_to_center = distance(pos, center);
    float orbital_vel = sqrt((G_CONST * sun_mass) / distance_to_center);

    Vector2f dir = normalize(Vector2f(pos.y - center.y, center.x - pos.x));
    Particle new_particle(pos, dir * orbital_vel, PARTICLE_MASS, 0.00001, i);

    particles.push_back(new_particle);
  }
  Particle sun(center, initial_vel, sun_mass, PARTICLE_RADIUS + 0.5,
               PARTICLES_AMOUNT);
  particles.push_back(sun);
}

void spawn_screen(vector<Particle> &particles) {
  for (int i = 0; i < PARTICLES_AMOUNT; i++) {
    Vector2f pos = random_on_screen();
    Vector2f speed = random_speed();
    Particle new_particle(pos, speed, PARTICLE_MASS, 0.00001, i);
    particles.push_back(new_particle);
  }
}

void calc_new_pos(Particle &particle, QuadTree &qt) {
  particle.netForce = Vector2f(0.0, 0.0);
  qt.calc_force(particle);

  Vector2f acceleration = particle.netForce / particle.mass;
  particle.vel += acceleration;
  particle.pos += particle.vel;
}

void calc_avg_vel(float &min_vel_avg, float &max_vel_avg,
                  vector<Particle> particles, int frame_cnt) {
  float max_vel = norm(particles[0].vel);
  float min_vel = norm(particles[0].vel);

  for (int i = 0; i < particles.size(); i++) {
    if (norm(particles[i].vel) > max_vel) {
      max_vel = norm(particles[i].vel);
    }
    if (norm(particles[i].vel) < min_vel) {
      min_vel = norm(particles[i].vel);
    }
  }

  max_vel_avg =
      (max_vel_avg * (float)frame_cnt + max_vel) / ((float)frame_cnt + 1.0);
  min_vel_avg =
      (min_vel_avg * (float)frame_cnt + min_vel) / ((float)frame_cnt + 1.0);
}

void update_title(Clock &clock, Time &elapsed, RenderWindow &window,
                  int &frame_cnt_second) {
  elapsed = clock.getElapsedTime();

  if (elapsed.asSeconds() >= 1.0) {
    int fps = frame_cnt_second;
    frame_cnt_second = 0;
    clock.restart();

    string title = "FPS: " + to_string(fps);
    window.setTitle(title);
  }
}

void save_video(string &ffmpeg_command) {
  time_t current_time = time(nullptr);
  struct std::tm *time_info = std::localtime(&current_time);
  const char *format = "%Y-%m-%d_%H.%M.%S";
  char time_formatted[100];

  std::strftime(time_formatted, sizeof(time_formatted), format, time_info);

  int return_code = system((ffmpeg_command + time_formatted + ".mp4").c_str());

  if (return_code == 0) {
    std::cout << "[LOG] Video saved." << std::endl;
    fs::path cachePath("image-cache");
    for (const auto &entry : fs::directory_iterator(cachePath)) {
      if (fs::is_regular_file(entry)) {
        fs::remove(entry.path());
      }
    }
  } else {
    std::cout << "[ERROR] FFmpeg command execution failed." << std::endl;
  }
}

void save_screen(RenderWindow &window, int &frame_cnt) {
  Texture texture;
  texture.create(window.getSize().x, window.getSize().y);
  texture.update(window);

  int desiredWidth = 5;
  char paddingChar = '0';
  string frame_str = to_string(frame_cnt);

  int zeros_amount = desiredWidth - frame_str.length();
  if (zeros_amount > 0) {
    frame_str = string(zeros_amount, '0') + frame_str;
  }

  if (texture.copyToImage().saveToFile("image-cache/" + frame_str + ".png")) {
    std::cout << "[LOG] Screenshot #" + to_string(frame_cnt) << " saved.\n";
  } else {
    std::cout << "[ERROR] Failed to save a screenshot.\n";
  }
}

int main() {
  RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "GraviPar");

  fs::path cache_path("image-cache");
  if (!fs::create_directory(cache_path)) {
    std::cout << "[LOG] Cache folder already exists.\n";
  }
  fs::path results_path("results");
  if (!fs::create_directory(results_path)) {
    std::cout << "[LOG] Results folder already exists.\n";
  }

  vector<Particle> particles;
  spawn_galaxy(particles, Vector2f(WIDTH / 2.0, HEIGHT / 2.0),
               Vector2f(0.0, 0.0), 1000.0, 200.0);

  float min_vel_avg = 0.0;
  float max_vel_avg = 0.0;

  Clock clock;
  Time elapsed;
  int frame_cnt = 0;
  int frame_cnt_second = 0;
  bool recording = RECORD_FROM_START;

  string ffmpeg_command =
      "ffmpeg -framerate 60 -pattern_type glob -i \"image-cache/*.png\" -vf "
      "eq=saturation=2 -c:v libx264 -pix_fmt yuv420p results/";

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed)
        window.close();

      if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::R) {
          recording = true;
          std::cout << "[LOG] Recording started.\n";
        } else if (event.key.code == sf::Keyboard::S) {
          recording = false;
          std::cout << "[LOG] Recording stopped.\n";
          save_video(ffmpeg_command);
        }
      }
    }

    window.clear();

    update_title(clock, elapsed, window, frame_cnt_second);

    Rectangle bounds(Vector2f(0.0, 0.0), WIDTH, HEIGHT);
    QuadTree qt(bounds);

    for (int i = 0; i < particles.size(); i++) {
      qt.insert(particles[i]);
    }

    for (int i = 0; i < particles.size(); i++) {
      calc_new_pos(particles[i], qt);
    }

    calc_avg_vel(min_vel_avg, max_vel_avg, particles, frame_cnt);

    qt.show(window, min_vel_avg, max_vel_avg);

    if (recording) {
      save_screen(window, frame_cnt);
    }

    window.display();
    frame_cnt++;
    frame_cnt_second++;
  }

  return 0;
}
