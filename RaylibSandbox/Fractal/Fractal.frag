#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(720, 720);
uniform float maxCount = 200.0;
uniform float zoom = 1.0;
uniform vec2 offset = vec2(0.0, 0.0);

vec2 ComplexAdd(vec2 a, float b)
{
	return a + vec2(b, 0.0);
}
vec2 ComplexMultiply(vec2 a, vec2 b)
{
	return vec2(
		a.x * b.x - a.y * b.y,
		a.x * b.y - b.x * a.y);
}
vec2 ComplexScale(vec2 a, float scale)
{
	return vec2(
		a.x * scale,
	    -scale * a.y);
}
vec2 ComplexDivide(vec2 a, vec2 b)
{
	return vec2(
		(a.x * b.x + a.y * b.y) / (b.x * b.x + b.y * b.y),
		(a.x * b.x + a.x * b.y) / (b.x * b.x + b.y * b.y));
}
vec2 ComplexAbsolute(vec2 c)
{
	return vec2(length(c), 0.0);
}
vec2 ComplexSqrt(vec2 c)
{
	float z_abs = length(c); // Absolute value of complex number
	return vec2(sqrt((z_abs + c.x) / 2.0), (c.y / abs(c.y)) * sqrt((z_abs - c.x) / 2.0));
}

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	// x = real, y = imaginary

	// Code here
	vec2 z = vec2(0.0);
	vec2 c = ((fragTexCoord / zoom) - vec2(0.5)) * 2.0 + offset;
	float i = 0;
	for (; ((z.x * z.x + z.y * z.y) < 4) && (i < maxCount); ++i)
	{
		vec2 z1 = ComplexSqrt(
						(ComplexMultiply(z, z) + ComplexAdd(c, -1)) /
					    (ComplexScale   (z, 2) + ComplexAdd(c, -2)));
		z = z1;
	}

	finalColor = vec4(vec3(i / maxCount,0.0,0.0), 1.0);
}
