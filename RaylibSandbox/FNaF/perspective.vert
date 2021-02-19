#version 330

// Input vertex attributes
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 fragTexCoord;
out vec4 fragColor;

// Custom uniforms
uniform float uv_rotation;
const float pi = 3.141592653589793238462643383279502884197169;

void main()
{
	float s = sin ( (uv_rotation / 180.0) * pi);
	float c = cos ( (uv_rotation / 180.0) * pi);
	mat2 rotationMatrix = mat2(c, -s, s, c);
	rotationMatrix *= 0.5;
	rotationMatrix += 0.5;
	rotationMatrix = rotationMatrix * 2.0 - 1.0;
	vertexTexCoord = vertexPosition.xy;
	vertexTexCoord = vertexPosition.xy * rotationMatrix;
	gl_Position = mProj * modelViewMatrix * vec4(vertexPosition, 0.0, 1.0);
}