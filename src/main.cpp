#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/System/Vector2.hpp"
#include "SFML/Window/Keyboard.hpp"
#include "SFML/Window/Window.hpp"
#include "defines.hpp"
#include "helpers.hpp"
#include "quadtree.hpp"
#include "rectangle.hpp"
#include "spawns.hpp"
#include "utils.hpp"
#include <SFML/Graphics.hpp>
#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace fs = std::filesystem;

using std::vector, std::string, std::time_t, sf::Vector2f, sf::RenderWindow,
    sf::Clock, sf::Time;

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
