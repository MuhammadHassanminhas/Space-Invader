#pragma once

#include <SFML/Graphics.hpp>
#include "Global.hpp"

struct Powerup
{
	float x;
	float y;
	unsigned char type;
	bool dead;

	Powerup(float i_x, float i_y, unsigned char i_type)
		: x(i_x), y(i_y), type(i_type), dead(false)
	{
	}

	sf::IntRect get_hitbox() const
	{
		return sf::IntRect(static_cast<int>(x), static_cast<int>(y), BASE_SIZE, BASE_SIZE);
	}
};
