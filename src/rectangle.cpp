#include "../include/rectangle.hpp"
#include <SFML/Graphics.hpp>

Rectangle::Rectangle(sf::Vector2f _topLeftPos, float _w, float _h) {
  topLeftPos = _topLeftPos;
  w = _w;
  h = _h;
}

bool Rectangle::contains(Particle &particle) {
  return topLeftPos.x <= particle.pos.x && topLeftPos.y <= particle.pos.y &&
         topLeftPos.x + w > particle.pos.x && topLeftPos.y + h > particle.pos.y;
}

bool Rectangle::intersects(Rectangle &rect) {
  bool up = rect.topLeftPos.y + rect.h < topLeftPos.y;
  bool down = rect.topLeftPos.y > topLeftPos.y + h;
  bool left = rect.topLeftPos.x + rect.w < topLeftPos.x;
  bool right = rect.topLeftPos.x > topLeftPos.x + w;
  return !(up || down || left || right);
}

void Rectangle::show(sf::RenderWindow &window) {
  sf::RectangleShape rect(sf::Vector2f(w, h));
  rect.setPosition(topLeftPos);
  rect.setOutlineColor(sf::Color::White);
  rect.setFillColor(sf::Color::Transparent);
  rect.setOutlineThickness(1);
  window.draw(rect);
}
