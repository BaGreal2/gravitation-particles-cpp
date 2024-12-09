#include "rectangle.hpp"
#include <SFML/Graphics.hpp>

Rectangle::Rectangle(sf::Vector2f _top_left_pos, float _w, float _h) {
  top_left_pos = _top_left_pos;
  w = _w;
  h = _h;
}

bool Rectangle::contains(Particle &particle) {
  return top_left_pos.x <= particle.pos.x && top_left_pos.y <= particle.pos.y &&
         top_left_pos.x + w > particle.pos.x && top_left_pos.y + h > particle.pos.y;
}

bool Rectangle::intersects(Rectangle &rect) {
  bool up = rect.top_left_pos.y + rect.h < top_left_pos.y;
  bool down = rect.top_left_pos.y > top_left_pos.y + h;
  bool left = rect.top_left_pos.x + rect.w < top_left_pos.x;
  bool right = rect.top_left_pos.x > top_left_pos.x + w;
  return !(up || down || left || right);
}

void Rectangle::show(sf::RenderWindow &window) {
  sf::RectangleShape rect(sf::Vector2f(w, h));
  rect.setPosition(top_left_pos);
  rect.setOutlineColor(sf::Color::White);
  rect.setFillColor(sf::Color::Transparent);
  rect.setOutlineThickness(1);
  window.draw(rect);
}
