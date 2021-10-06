#shader vertex
#version 400 core
#extension GL_NV_shader_buffer_load : enable

layout(location = 0) in vec4 edgeCoords;

//out vec2 positionX;
//out vec2 positionY;
out vec4 pixelPosition;

uniform mat4 u_Projection;
uniform float u_Zoom = 1; 
uniform vec2 u_ZoomPosition;

vec2 split_double(double input)
{
    float hipart = float(input);
    double delta = input - (double(hipart));
    float lopart = float(delta);

    return vec2(hipart, lopart);
}

void main()
{
    gl_Position = edgeCoords;
    pixelPosition = edgeCoords;
    pixelPosition.x = ((pixelPosition.x + 1) / (2)) * (2 - -2)/u_Zoom + -2/u_Zoom;
    pixelPosition.y = ((pixelPosition.y + 1) / (2)) * (2 - -2)/u_Zoom + -2/u_Zoom;
    pixelPosition.x += u_ZoomPosition.x;
    pixelPosition.y += u_ZoomPosition.y;

    //positionX = split_double(pixelPosition.x);
    //positionY = split_double(pixelPosition.y);
}


#shader fragment
#version 400 core
out vec4 color;

//in vec2 positionX;
//in vec2 positionY;
in vec4 pixelPosition;

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main()
{
    //dvec2 position = dvec2(double(positionX.x)+double(positionX.y), double(positionY.x) + double(positionY.y));

    int iterations = 5000;
    int counter = 0;

    vec2 z = vec2(0, 0);

    //vec2 z = vec2(pixelPosition.x, pixelPosition.y);
    //vec2 c = vec2(-0.0109, 0.8611);

    while (counter < iterations && length(z) <= 2)
    {
        double x = ((z.x * z.x) - (z.y * z.y)) + pixelPosition.x;
        double y = ((z.y * z.x) + (z.x * z.y)) + pixelPosition.y;
        //double x = ((z.x * z.x) - (z.y * z.y)) + c.x;
        //double y = ((z.y * z.x) + (z.x * z.y)) + c.y;

        z = vec2(x, y);
        counter ++;
    }

    float sat = 1;
    double rgbColor = counter / double(iterations);
    if (counter == iterations) sat = 0;

    color = vec4(hsv2rgb(vec3(rgbColor + 0.75, 1, sat)), 0);
}