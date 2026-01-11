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

		Texture _cover_texture;
		Texture _cover_blurred_texture;
		Shader _blur;

		Image _cover;
		Image _mask;
		Image _cover_blurred;

	};