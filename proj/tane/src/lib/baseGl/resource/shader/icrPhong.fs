//#version 420 core
#version 430 compatibility

#define MAX_LIGHT                   8

#define IN_POINT_LOCATION           0
#define IN_NORMAL_LOCATION          1
#define IN_TEXCOORD_LOCATION        2

#define IN_INSTANCING_MTX_LOCATION  3
#define IN_COLOR_LOCATION           7
#define IN_WIRECOLOR_LOCATION       8

/* unforms */
uniform mat4  kXformMtx = mat4(1.0);

uniform int   kDrawMode = 0;          // 0 : kDrawShaded, 1 : kDrawWire

uniform int   kOverrideMask = 0;
uniform vec4  kMaskColor    = vec4(1.0, 1.0, 1.0, 1.0);

uniform int   kNoLighting        = 0;
uniform int   kOverrideTexture   = 0;

in vec3 VPosition;
in vec3 VNormal;
in vec3 VTexCoord;

in vec4 VColor;
in vec4 VWireColor;

vec4 phongModel(int i, vec3 p, vec3 n)
{
    vec3 l;
    if(gl_LightSource[i].position.w == 1.0)
        l = normalize(gl_LightSource[i].position.xyz - p);
    else
        l = normalize(gl_LightSource[i].position.xyz);

    vec3 e = normalize(-p);
    vec3 r = normalize(reflect(-l, n));

    /* calc ambient color */
    vec4 amb = gl_FrontLightProduct[i].ambient;
    amb = clamp(amb, 0.0, 1.0);

    /* calc diffuse color */
    vec4 diff = gl_FrontLightProduct[i].diffuse * max(dot(n, l), 0.0);
    diff = clamp(diff, 0.0, 1.0);

    /* calc specular color */
    vec4 spec = gl_FrontLightProduct[i].specular * pow(max(dot(r,e), 0.0), 0.3 * gl_FrontMaterial.shininess);
    spec = clamp(spec, 0.0, 1.0);

    return amb + diff + spec;
}
layout(location=0) out vec4 OutColor;
void main()
{
    vec4    surfaceColor;
    float   wireWidth;
    vec4    wireColor;
    if(kOverrideMask == 1)
    {
        surfaceColor = kMaskColor;
    }
    else
    {
        if(kOverrideTexture == 1)
        {
            /* get color from texture */
            surfaceColor = VColor;
        }
        if(kNoLighting == 1)
        {
            surfaceColor = VColor;
        }
        else
        {
            vec4 lightColor;
            for(int i = 0; i < MAX_LIGHT; i++)
                lightColor += phongModel(i, VPosition, VNormal);
            surfaceColor = clamp(surfaceColor * lightColor, 0.0, 1.0);
        }
    }

    if(kDrawMode == 0 /* drawShaded */)
    {
    }
    else if(kDrawMode == 1 /* drawWire*/)
    {
        surfaceColor = VWireColor;
    }
    else
    {
        discard;
    }
    OutColor = surfaceColor;
}


