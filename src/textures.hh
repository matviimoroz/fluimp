#include <string>
#include <unordered_map>
#include <raylib.h>

class Texs {
public:
	void operator()(std::string texture_name, std::string path, unsigned int x, unsigned int y, unsigned int width, unsigned int height) noexcept;

	void clear() noexcept;

private:
	std::unordered_map<std::string, Texture> textures;
};