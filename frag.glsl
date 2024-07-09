#version 330 core

in vec4 color;
out vec4 fragColor;

void main(void)
{
	// Set the color of this fragment to the interpolated color
	// value computed by the rasterizer.
	fragColor =color;//vec4(color.x +0.5, color.y +0.5, color.z, color.w);
}
