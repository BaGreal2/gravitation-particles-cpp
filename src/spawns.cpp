#include "spawns.hpp"
#include "defines.hpp"
#include "utils.hpp"
#include <cmath>

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
