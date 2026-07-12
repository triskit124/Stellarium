#version 330 core
in vec3 WorldPos;
out vec4 FragColor;

uniform vec3 cameraPos;
uniform float cellSize;
uniform int cellsPerMajor;
uniform vec3 minorColor;
uniform vec3 majorColor;
uniform vec3 axisColorX;
uniform vec3 axisColorY;
uniform float fadeDistance;

// Anti-aliased grid line mask at one frequency, using screen-space derivatives so line
// width stays ~1px regardless of distance from the camera (see Ben Golus, "The Best
// Darn Grid Shader (Yet)").
float gridLine(vec2 coord)
{
    vec2 deriv = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / max(deriv, vec2(1e-7));
    return 1.0 - min(min(grid.x, grid.y), 1.0);
}

void main()
{
    vec2 coord = WorldPos.xy / cellSize;
    float minorLine = gridLine(coord);
    float majorLine = gridLine(coord / float(cellsPerMajor));

    vec3 color = mix(minorColor, majorColor, majorLine);
    float lineAlpha = max(minorLine, majorLine);

    // Highlight the world X/Y axes (where the other coordinate crosses zero).
    vec2 axisDeriv = max(fwidth(WorldPos.xy), vec2(1e-7));
    float xAxis = 1.0 - min(abs(WorldPos.y) / axisDeriv.y, 1.0);
    float yAxis = 1.0 - min(abs(WorldPos.x) / axisDeriv.x, 1.0);
    color = mix(color, axisColorX, xAxis);
    color = mix(color, axisColorY, yAxis);
    lineAlpha = max(lineAlpha, max(xAxis, yAxis));

    float dist = length(WorldPos.xy - cameraPos.xy);
    float fade = 1.0 - smoothstep(0.0, fadeDistance, dist);

    if (lineAlpha * fade < 0.02)
        discard;

    FragColor = vec4(color, lineAlpha * fade);
}
