const constexpr char* circle = R"(
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 texSize;   // Texture size in pixels
uniform float radius;   // Circle radius in pixels
uniform vec2 center;    // Circle center in pixels

void main()
{
    vec2 pos = fragTexCoord * texSize;          // pixel coords
    float dist = length(pos - center);          // distance from circle center
    
    if(dist > radius) discard;                  // outside the circle
    
    finalColor = texture(texture0, fragTexCoord) * fragColor;
}
)";