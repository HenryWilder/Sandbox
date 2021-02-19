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
uniform vec2 resolution = vec2(1920, 1080);

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;
	
	vec2 warpedPos = fragTexCoord;

	finalColor = texture2D(texture0, warpedPos);
}

uniform sampler2D src;
const float pi = 3.141592653589793238462643383279502884197169;
varying vec3 vTexCoord;
void main() {
	vec3 direction = normalize(vTexCoord);
	vec2 longlat = vec2(atan(direction.z, direction.x), acos(-direction.y)) ;
	vec2 uv = longlat / vec2(2.0 * pi, pi);
	uv.x += 0.5;
	vec4 tex = texture2D(src, uv);
	gl_FragColor = tex;
}
