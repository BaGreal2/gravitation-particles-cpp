#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

float distance(const sf::Vector2f &point1, const sf::Vector2f &point2);
float norm(const sf::Vector2f &vector);
sf::Vector2f normalize(const sf::Vector2f &vector);
sf::Vector2f random_in_circle(float radius, float padding, sf::Vector2f center);
sf::Vector2f random_on_screen();
sf::Vector2f random_speed();
sf::Color multi_color_lerp(std::vector<sf::Color> &colors, float t);
