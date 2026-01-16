#pragma once

#include <raylib.h>
#include <string>

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
	Shader _perspective;

	Image _cover;
	Image _mask;
	Image _cover_blurred;

	Vector2 _mouse_pos;

	bool _paused = false;
	float _pause_fade_anim = 0.0f;
	float _pause_hover_anim = 0.0f;

	FilePathList _songs;
	unsigned int _current_song_index;
	Music _song;
	const char* _current_song_path;
	bool _skip = true;


	

};