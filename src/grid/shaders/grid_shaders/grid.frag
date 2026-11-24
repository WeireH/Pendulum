#version 410 core

out vec4 FragColor;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform vec3 cameraPosShader;

uniform vec2 u_resolution;

void main()
{
    vec2 st = (gl_FragCoord.xy / u_resolution);
    vec2 ndc = st * 2.0 - 1.0;
    vec4 clip = vec4(ndc, -1.0, 1.0);

    vec4 viewPos = inverse(projection) * clip;
    viewPos /= viewPos.w;
    vec4 rayDirection = vec4(viewPos.xyz, 0.0);
    vec3 worldDirection = normalize((inverse(view) * rayDirection).xyz);

    vec3 rayOrigin = (inverse(view) * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    float t = -rayOrigin.y / worldDirection.y;
    vec3 intersection = rayOrigin + t * worldDirection;

    vec4 clipPos = projection * view * vec4(intersection, 1.0);
    gl_FragDepth = (clipPos.z / clipPos.w) * 0.5 + 0.5;

    float square_size = 1.0f;
    vec2 grid_position = intersection.xz / square_size + 0.5f;
    vec2 grid_distance = abs(fract(grid_position) - 0.5);
    vec2 derivative = fwidth(grid_position);

    float dist = distance(cameraPosShader, intersection);

    vec2 line = 1.0 - smoothstep(
        vec2(0.001),
        vec2(0.001) + derivative,
        grid_distance
    );

    float grid = max(line.x, line.y);

    if (grid >= 0.01) {
        FragColor = vec4(vec3(0.5), grid);

    }   else {
        discard;
    }

    float fade = 1 - smoothstep(15, 30, dist);

    if (fade > 0.0) {
        FragColor = vec4(vec3(0.5), grid * fade);
    } else {
        discard;
    }
} 
