#define IN_SHADERTOY 1

#ifndef IN_SHADERTOY
	uniform vec2 iResolution;
	uniform vec3 iGlobalTime;
#endif



#define webcam_tex iChannel0

#define pass_1_tex iChannel1
#define pass_2_tex iChannel2
#define pass_3_tex iChannel3
//#define pass_final_tex iChannel3


void mainImage (out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fragCoord / iResolution.xy;
    
    vec3 base_color = vec3(0);
	
    float v_stripes = 16.0;
    float h_stripes = 16.0;
    vec2 sub_uv = mod(uv, vec2(1.0, 1.0 / v_stripes)) * vec2(h_stripes, v_stripes);
    
    vec2 sub_id = floor(uv * vec2(h_stripes, v_stripes));
    sub_uv.x += mod(sub_id.y, 2.0);
    sub_uv.x += cos(0.1 * iGlobalTime * 3.1415 + sub_id.y / v_stripes * 1234.0) * h_stripes; 
                      
    if (abs(mod(sub_uv.x, 2.0) - 1.0) < mod(sub_uv.y, 1.0)) {
    
        if (abs(mod(iGlobalTime * 1.5 * 0.5 + 0.5, 2.0) - 1.0) < mod(uv.y, 1.0)) {
            base_color = texture(webcam_tex, uv).rgb;

        } else {
            base_color = texture(pass_1_tex, uv).rgb;

        }
    } else {
        
        if (abs(mod(iGlobalTime * 2.0 * 0.5, 2.0) - 1.0) < 1.0 - mod(uv.y, 1.0)) {
            base_color = texture(webcam_tex, uv).rgb;

        } else {
            base_color = texture(pass_1_tex, uv).rgb;

        }
    }
        
    
    fragColor = vec4(base_color, 1.0);
}
