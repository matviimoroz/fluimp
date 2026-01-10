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
	Texture2D _tex;
	Texture2D _main_border;
	Texture2D _cover_border;

	Shader _fs_corner_radius;

};