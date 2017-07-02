#define IN_SHADERTOY 1

#ifndef IN_SHADERTOY
	uniform vec2 iResolution;
	uniform vec3 iGlobalTime;
#endif



#define pass_1_tex iChannel0
#define pass_2_tex iChannel1
#define pass_3_tex iChannel2
#define pass_final_tex iChannel3


void mainImage (out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fragCoord / iResolution.xy;
    
    vec4 base_color = texture(pass_3_tex, uv);
    
    fragColor = base_color;
}
