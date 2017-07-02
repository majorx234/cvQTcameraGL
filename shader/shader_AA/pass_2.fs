
#define noise_tex iChannel0

#define pass_1_tex iChannel1
#define pass_2_tex iChannel2
#define pass_3_tex iChannel3
#define pass_final_tex iChannel3


void mainImage (out vec4 fragColor, in vec2 fragCoord) {
    vec2 uv = fragCoord / iResolution.xy;
    
    vec2 warp = vec2(texture(noise_tex, uv * 0.2 + iGlobalTime * 0.06125).r, texture(noise_tex, uv * 0.2 + vec2(1.0) + iGlobalTime * 0.06125).r) * 2.0 - vec2(1.0);
                       
                       
    vec2 uv_warped = uv + warp * 0.01; 
    vec4 base_color = texture(pass_1_tex, uv_warped);
    
    fragColor = base_color;
}
