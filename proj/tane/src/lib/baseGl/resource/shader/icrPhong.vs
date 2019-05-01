//#version 420 core
#version 430 compatibility
#define IN_POINT_LOCATION           0
#define IN_NORMAL_LOCATION          1
#define IN_TEXCOORD_LOCATION        2

#define IN_INSTANCING_MTX_LOCATION  3
#define IN_COLOR_LOCATION           7
#define IN_WIRECOLOR_LOCATION       8

layout(location = IN_POINT_LOCATION)            in vec3 InPoint;
layout(location = IN_NORMAL_LOCATION)           in vec3 InNormal;
layout(location = IN_TEXCOORD_LOCATION)         in vec3 InTexCoord;

layout(location = IN_INSTANCING_MTX_LOCATION)   in mat4 InInstancingMtx;
layout(location = IN_COLOR_LOCATION)            in vec4 InColor;
layout(location = IN_WIRECOLOR_LOCATION)        in vec4 InWireColor;

/* Transform */
uniform mat4 kXformMtx = mat4(1.0);

/* DisplayMode */
uniform int kDrawMode = 0;          // 0 : kDrawShaded, 1 : kDrawWire

/* ColorMode */
uniform int  kOverrideMask = 0;
uniform vec4 kMaskColor    = vec4(1.0, 1.0, 1.0, 1.0);

uniform int kNoLighting = 0;
uniform int kOverrideTexture = 0;

out vec3 VPosition;
out vec3 VNormal;
out vec3 VTexCoord;

out vec4 VColor;
out vec4 VWireColor;

void main()
{

    VNormal    = normalize(gl_NormalMatrix * mat3(InInstancingMtx) * mat3(kXformMtx) * InNormal);
    VPosition  = vec3(gl_ModelViewMatrix * InInstancingMtx * kXformMtx * vec4(InPoint, 1.0));

    VTexCoord  = InTexCoord;
    VColor     = InColor;
    VWireColor = InWireColor;

    gl_Position = gl_ModelViewProjectionMatrix * InInstancingMtx * kXformMtx * vec4(InPoint, 1.0);
}
