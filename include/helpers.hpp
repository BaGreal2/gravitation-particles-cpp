#pragma once

#include "particle.hpp"
#include "quadtree.hpp"
#include <vector>
#include <string>
#include "SFML/Graphics/RenderWindow.hpp"

using std::vector, std::string, sf::Clock, sf::Time, sf::RenderWindow;

void calc_new_pos(Particle &particle, QuadTree &qt);
void calc_avg_vel(float &min_vel_avg, float &max_vel_avg,
                  vector<Particle> particles, int frame_cnt);
void update_title(Clock &clock, Time &elapsed, RenderWindow &window,
                  int &frame_cnt_second);
void save_video(string &ffmpeg_command);
void save_screen(RenderWindow &window, int &frame_cnt);
