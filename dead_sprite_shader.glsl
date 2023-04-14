// Source code of fragment shader
#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec2 uv_interp;

// Texture sampler
uniform sampler2D onetex;
uniform int tiles;

void main()
{
    // Sample texture
    vec4 color = texture2D(onetex, uv_interp * tiles);
    float average = (color.r + color.g + color.b) / 3;

    // Assign color to fragment
    gl_FragColor = vec4(average, average, average, color.a);

    // Check for transparency
    if(color.a < 1.0)
    {
         discard;
    }
}
