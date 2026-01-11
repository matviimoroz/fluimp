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

		Shader _circle_clip;

		Image _cover;
		Image _mask;
		Image _cover_blurred;

		Vector2 _mouse_pos;

		bool _paused = false;
		float _pause_fade = 0.0f;
		float _hover_anim = 0.0f;

	};