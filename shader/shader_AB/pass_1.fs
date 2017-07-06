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
    vec2 uv = fragCoord / iResolution.xy;
    vec2 off = 1.0 / iResolution.xy * 4.0;
    
    vec2 p = uv - normalize(uv - vec2(0.5)) * off;
    
    vec3 color = texture(webcam_tex, vec2(1.0) - uv).rgb;
    float feedback_weight = max(0.0, 1.0 - length(color) * 2.0);
    
    color = length(color) * vec3(rand(floor(iGlobalTime * 10.0)), rand(floor(iGlobalTime * 10.0 + 2.0)), rand(floor(iGlobalTime * 10.0 + 5.0))); 
    
    color = mix(color, texture(pass_1_tex, p).rgb, step(0.05, feedback_weight));
    
    fragColor = vec4(color, 1.0);
}



#ifndef IN_SHADERTOY
void main() {
    mainImage(gl_FragColor, gl_FragCoord.xy);
}
#endif
