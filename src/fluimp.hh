	#pragma once

	#include <raylib.h>

	#include "textures.hh"

	class Fluimp {
	public:
		bool running{ true };

		void init() noexcept;

		void update() noexcept;
		void render() noexcept;

		void cleanup() noexcept;

	private:
		Textures _textures;

		Shader _fs_corner_radius;

	};