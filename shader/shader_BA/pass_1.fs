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


void mainImage (out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = vec2(1.0) - fragCoord / iResolution.xy;
    vec2 off = 1.0 / iResolution.xy;
    
    float edge = 0.0;
    edge -= 4.0 * length(texture(webcam_tex, uv).xyz);
    edge += 1.0 * length(texture(webcam_tex, uv + vec2(+off.x, 0.0)).xyz);
    edge += 1.0 * length(texture(webcam_tex, uv + vec2(-off.x, 0.0)).xyz);
    edge += 1.0 * length(texture(webcam_tex, uv + vec2(0.0, +off.y)).xyz);
    edge += 1.0 * length(texture(webcam_tex, uv + vec2(0.0, -off.y)).xyz);
    
    fragColor = vec4(edge);
}



#ifndef IN_SHADERTOY
void main() {
    mainImage(gl_FragColor, gl_FragCoord.xy);
}
#endif
