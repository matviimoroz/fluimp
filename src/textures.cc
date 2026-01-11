#include "textures.hh"

#include <raylib.h>

// textures from files are uniquely identified by path
Vector2 Textures::file(std::string path, float x, float y, float width, float height, Color color, bool aa) noexcept {
	if (!_textures.contains(path)) {
		_textures[path] = LoadTexture(path.c_str());
	} 

	Texture& tex = _textures[path];
	if (aa) {
		SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR | TEXTURE_FILTER_ANISOTROPIC_16X);
	}

	DrawTexturePro(tex, Rectangle{ 0, 0, (float)tex.width, (float)tex.height }, Rectangle{ x, y, width, height }, Vector2{ 0, 0 }, 0, color);

	return { (float)tex.width, (float)tex.height };
}

Textures::~Textures() noexcept {
	// std::pair<std::string path, Texture texture>
	for (auto& pair : _textures) {
		UnloadTexture(pair.second);
	}
}
