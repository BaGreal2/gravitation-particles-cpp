#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

float distance(const sf::Vector2f &point1, const sf::Vector2f &point2);
float norm(const sf::Vector2f &vector);
sf::Vector2f normalize(const sf::Vector2f &vector);
sf::Vector2f randomInCircle(float radius, float padding, sf::Vector2f center);
sf::Vector2f randomOnScreen();
sf::Vector2f randomSpeed();
sf::Color multiColourLerp(std::vector<sf::Color> &colors, float t);
