#include <string>
#include <unordered_map>
#include <raylib.h>

class Textures {
public:
	Vector2 file(std::string path, float x, float y, float width, float height) noexcept;

	~Textures() noexcept;

private:
	std::unordered_map<std::string, Texture> _textures;
};