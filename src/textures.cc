#include "textures.hh"

#include <raylib.h>

Vector2 Textures::operator()(std::string path, float x, float y, float width, float height) noexcept {
	if (!_textures.contains(path)) {
		_textures[path] = LoadTexture(path.c_str());
	} 

	Texture& tex = _textures[path];
	DrawTexturePro(tex, Rectangle{ 0, 0, (float)tex.width, (float)tex.height }, Rectangle{ x, y, width, height }, Vector2{ 0, 0 }, 0, WHITE);

	return { (float)tex.width, (float)tex.height };
}

Textures::~Textures() noexcept {
	// std::pair<std::string path, Texture texture>
	for (auto& pair : _textures) {
		UnloadTexture(pair.second);
	}
}
