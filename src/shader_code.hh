const constexpr char* blur_shader = R"(
#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform sampler2D texture0;
uniform vec2 texSize;

uniform vec2 center;   // in pixels
uniform float radius;  // in pixels
uniform float strength; // blur strength

void main()
{
    vec2 uv = fragTexCoord;
    vec2 pos = uv * texSize;

    float dist = distance(pos, center);

    // Outside circle → no blur
    if (dist > radius)
    {
        finalColor = texture(texture0, uv);
        return;
    }

    vec2 texel = 1.0 / texSize;

    vec3 sum = vec3(0.0);
    float count = 0.0;

    // Simple circular kernel
    for (int x = -4; x <= 4; x++)
    for (int y = -4; y <= 4; y++)
    {
        vec2 offset = vec2(x, y) * texel * strength;

        // keep kernel circular
        if (length(vec2(x, y)) > 4.0) continue;

        sum += texture(texture0, uv + offset).rgb;
        count += 1.0;
    }

    finalColor = vec4(sum / count, 1.0);
}
)";