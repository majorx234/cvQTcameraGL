//Sampler2d

uniform sampler2D pass_1_tex;
uniform sampler2D pass_2_tex;
uniform sampler2D pass_3_tex;
uniform sampler2D pass_final_tex;

uniform sampler2D webcam_tex;


uniform float iGlobalTime;
uniform vec2 iResolution;


void main() {
	vec4 color = vec4(
		mod(iGlobalTime, 1.0),
		step(iResolution.x, gl_FragCoord.x),
		step(iResolution.y, gl_FragCoord.y),
		1.0
	);

	vec2 uv = gl_FragCoord.xy / iResolution;

	color = texture2D (pass_1_tex, uv);
	
	
	gl_FragColor = color;
} 
