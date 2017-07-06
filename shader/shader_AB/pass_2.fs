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

	#define noise_tex iChannel0
	#define pass_1_tex iChannel1
	#define pass_2_tex iChannel2
	
#endif


void mainImage (out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fragCoord / iResolution.xy;
    
    
    vec3 base_color = texture(pass_1_tex, uv).rgb;
    
    
    
    fragColor = vec4(base_color, 1.0);
}



#ifndef IN_SHADERTOY
void main() {
    mainImage(gl_FragColor, gl_FragCoord.xy);
}
#endif
