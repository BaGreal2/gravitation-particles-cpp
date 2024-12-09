#pragma once

#include "helpers.hpp"
#include "SFML/System/Vector2.hpp"
#include "utils.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

using sf::Vector2f, sf::Texture, sf::Clock, sf::Time, sf::RenderWindow,
    std::to_string;

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
