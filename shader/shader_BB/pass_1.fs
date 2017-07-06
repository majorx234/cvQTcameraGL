//#define IN_SHADERTOY 1

#ifndef IN_SHADERTOY
    uniform sampler2D webcam_tex;
    uniform sampler2D noise_tex;

    uniform sampler2D pass_1_tex;
    uniform sampler2D pass_2_tex;
    uniform sampler2D pass_3_tex;
    uniform sampler2D pass_final_tex;

    uniform vec2 iResolution;
    uniform float iGlobalTime;

    #define texture texture2D
    

#else

	#define webcam_tex iChannel0
	#define pass_1_tex iChannel1
	//#define pass_final_tex iChannel3

#endif


float rand(float x) {
	return sin(cos(x * 3479.6321) * 2131.213) * 0.5 + 0.5;
}

void mainImage (out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = vec2(1.0) - fragCoord / iResolution.xy;
    
    float r = distance(uv, vec2(0.5));
    float a = atan(uv.y - 0.5, uv.x - 0.5);
    
    float counter = r * 100.0;
    
    float id = floor(counter);
    
    float t = (floor(rand(id) * 4.0) / 4.0 * 0.5 + 0.5) * iGlobalTime;
    float a_off = 3.1415 * 2.0 * cos(t);
    
    a += a_off;
    
    vec2 p = vec2(cos(a), sin(a)) * r + vec2(0.5);
    
    vec3 color = texture(webcam_tex, p).rgb;
    
    fragColor = vec4(color, 1.0);
}



#ifndef IN_SHADERTOY
void main() {
    mainImage(gl_FragColor, gl_FragCoord.xy);
}
#endif
