#pragma once

#include "particle.hpp"
#include <vector>
#include <SFML/Graphics.hpp>
#include "SFML/System/Vector2.hpp"

using std::vector, sf::Vector2f;

void spawn_circle(vector<Particle> &particles, Vector2f center);
void spawn_spinning_circle(vector<Particle> &particles, Vector2f center);
void spawn_galaxy(vector<Particle> &particles, Vector2f center,
                  Vector2f initial_vel, float sun_mass, float radius);
void spawn_screen(vector<Particle> &particles);
