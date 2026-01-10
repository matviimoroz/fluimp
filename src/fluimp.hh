#pragma once

#include <raylib.h>

class Fluimp {
public:
	bool running{ true };

	void init() noexcept;

	void update() noexcept;
	void render() noexcept;

	void cleanup() noexcept;

private:
	Image _cover;
	Texture2D _tex;
	Shader _fs_corner_radius;

};