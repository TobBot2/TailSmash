uniform sampler2D iTexture;
uniform vec2 iResolution;
uniform float iTime;
uniform bool iShock;
uniform vec2 iShockPosition;
uniform float iShockStartTime;
uniform bool iFlash;

float seed = 0.32; // starting seed for explosion

vec4 shockwave( in vec2 fragCoord ) {
    // Sawtooth calc of time
    float relativeTime = iTime - iShockStartTime;
    float offset = (relativeTime - floor(relativeTime)) / relativeTime;
	float time = relativeTime * offset * 3;

    // Wave design params
	vec3 waveParams = vec3(100, .01, 0.1 );
    
    // Find coordinate, flexible to different resolutions
    vec2 uv = fragCoord.xy;
    
    // Find center, flexible to different resolutions
    vec2 position = vec2(iShockPosition.x, iResolution.y - iShockPosition.y);
    vec2 center = position / iResolution;

    // Distance to the center
    float dist = distance(uv, center);
    
    // Original color
	vec4 c = texture2D(iTexture, uv);
    
    // Limit to waves
	if (time > .05 && relativeTime < 1. && dist <= time + waveParams.z && dist >= time - waveParams.z) {
        // The pixel offset distance based on the input parameters
		float diff = (dist - time);
		float diffPow = (1.0 - pow(abs(diff * waveParams.x), waveParams.y));
		float diffTime = (diff  * diffPow);

        // The direction of the distortion
		vec2 dir = normalize(uv - center);
        
        // Perform the distortion and reduce the effect over time
		uv += ((dir * diffTime) / (time * dist * 8.));
        
        // Grab color for the new coord
		c = texture2D(iTexture, uv);

        // Optionally: Blow out the color for brighter-energy origin
        //c += (c * diffPow) / (time * dist * 2.);
	}
    
	return c;
}


vec4 explosion( in vec2 fragCoord )
{
    const float particles = 32.;
    const float res = 90.;

    vec2 position = vec2(iShockPosition.x, iResolution.y - iShockPosition.y);

	vec2 uv = (2. * fragCoord.xy * iResolution.xy / iResolution.y) - 2. * position / iResolution.y;
   	float clr = 0.0;
    float relativeTime = (iTime - iShockStartTime) * 1.5;

    if (relativeTime > 1.) {
        return vec4(0,0,0,0);
    }

    float timecycle = relativeTime-floor(relativeTime);
    seed = (seed+floor(relativeTime));
    
    //testing
    float invres=1.0/res;
    float invparticles = 1.0/particles;
    
    for( float i=0.0; i<particles; i+=1.0 )
    {
		seed+=i+tan(seed);
        vec2 tPos = (vec2(cos(seed),sin(seed)))*i*invparticles;
        
        vec2 pPos = vec2(0.0,0.0);
        pPos.x = ((tPos.x) * timecycle);
		pPos.y = tPos.y*timecycle+pPos.y; // - gravity*(timecycle*timecycle)
        
        pPos = floor(pPos*res)*invres; //-----------------------------------------comment this out for smooth version 

    	vec2 p1 = pPos;
    	vec4 r1 = vec4(vec2(step(p1,uv)),1.0-vec2(step(p1+invres,uv)));
    	float px1 = r1.x*r1.y*r1.z*r1.w;
        float px2 = smoothstep(0.0,200.0,(1.0/distance(uv, pPos+.015)));//added glow
        px1=max(px1,px2);
        
	    clr += px1*(sin(relativeTime*20.0+i)+1.0);
    }
    
	return vec4(clr*(1.0-timecycle))*vec4(4, 0.5, 0.1,1.0);
}

bool explosionFlash() {
    if (iTime - iShockStartTime < .02) {
        return true;
    }
    return false;
}

vec2 CRTCurveUV( vec2 uv )
{
    uv = uv * 2.0 - 1.0;
    vec2 offset = abs( uv.yx ) / vec2( 8.0, 5.0 );
    uv = uv + uv * offset * offset;
    uv = uv * 0.5 + 0.5;
    return uv;
}

void DrawVignette( inout vec3 color, vec2 uv )
{    
    float vignette = uv.x * uv.y * ( 1.0 - uv.x ) * ( 1.0 - uv.y );
    vignette = clamp( pow( 16.0 * vignette, 0.3 ), 0.0, 1.0 );
    color *= vignette;
}

void DrawScanline( inout vec3 color, vec2 uv )
{
    float scanline 	= clamp( 0.95 + 0.05 * cos( 3.14 * ( uv.y + 0.008 * iTime ) * 240.0 * 1.0 ), 0.0, 1.0 );
    float grille 	= 0.85 + 0.15 * clamp( 1.5 * cos( 3.14 * uv.x * 640.0 * 1.0 ), 0.0, 1.0 );    
    color *= scanline * grille * 1.2;
}

void main() {
    vec2 uv = CRTCurveUV(gl_TexCoord[0].xy);
    vec4 pixel = texture2D(iTexture, uv);

    // only do flashes/explosions in the first second
    if (iTime - iShockStartTime < 1) {
        if (iFlash && explosionFlash()) {
            if (pixel.r + pixel.g + pixel.b < 1) {
                pixel = vec4(0, 0, 0, 255);
            } else {
                pixel = vec4(255, 255, 255, 255);
            }

            vec2 position = vec2(iShockPosition.x, iResolution.y - iShockPosition.y);
            float dist = distance(position / iResolution, uv);
            float power = clamp(1 - dist * 2, 0, 1);
            pixel += vec4(power, power, power, 10);
        
            gl_FragColor = gl_Color * pixel;
            return;
        }

        if (iShock) {
            pixel = shockwave(uv);
            pixel += explosion(uv);
        }
    }

    DrawScanline(pixel.rgb, uv);
    DrawVignette(pixel.rgb, gl_TexCoord[0].xy);

    gl_FragColor = gl_Color * pixel;
}