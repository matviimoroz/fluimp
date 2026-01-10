const constexpr char* fs_corner_radius = R"(
#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform sampler2D texture0;
uniform float radius;
uniform vec2 texSize;

void main()
{
    vec2 uv = fragTexCoord;
    vec2 pos = uv * texSize;

    // Calculate distance to each corner
    vec2 tl = pos;                         
    vec2 tr = vec2(texSize.x - pos.x, pos.y); 
    vec2 bl = vec2(pos.x, texSize.y - pos.y); 
    vec2 br = texSize - pos;             

    float r = radius;

    if (tl.x < r && tl.y < r && length(tl - vec2(r,r)) > r) discard;
    if (tr.x < r && tr.y < r && length(tr - vec2(r,r)) > r) discard;
    if (bl.x < r && bl.y < r && length(bl - vec2(r,r)) > r) discard;
    if (br.x < r && br.y < r && length(br - vec2(r,r)) > r) discard;

    finalColor = texture(texture0, fragTexCoord) * fragColor;
}
)";