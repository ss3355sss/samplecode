#version 430 compatibility
#define MAX_NUM_LIGHT                   8

#define IN_RENDERABLE_LOCATION          0
#define IN_INSTANCING_MTX_LOCATION      1
#define IN_COLOR_LOCATION               5
#define IN_WIRE_COLOR_LOCATION          6

/* Transform */
uniform mat4    kXformMtx = mat4(1.0);
/* Icr */
uniform int     kEnableIcr = 0;
uniform vec3    kMinCorner = vec3(0.0, 0.0, 0.0);
uniform vec3    kMaxCorner = vec3(0.0, 0.0, 0.0);
/* LOD */
uniform int     kEnableLOD      = 0;
uniform float   kMinDistance    = 50.0;
uniform float   kMaxDistance    = 100.0;

layout(location = IN_RENDERABLE_LOCATION)       in int  InRenderable;
layout(location = IN_INSTANCING_MTX_LOCATION)   in mat4 InInstancingMtx;
layout(location = IN_COLOR_LOCATION)            in vec4 InColor;
layout(location = IN_WIRE_COLOR_LOCATION)       in vec4 InWireColor;

flat out int    VVisible;
out mat4        VInstancingMtx;
out vec4        VColor;
out vec4        VWireColor;

int icr(vec3 inPoint)
{
    vec4 corners[8];
    // create the corner of bbox
    //   v5-------v4
    //   /|       /|
    //  v1-------v0|
    //  | |      | |
    //  | v7-----|-v6
    //  |/       |/
    //  v3-------v2

    mat4 transformMtx = gl_ModelViewProjectionMatrix * InInstancingMtx * kXformMtx;
    corners[0] = transformMtx * vec4(kMaxCorner[0], kMaxCorner[1], kMaxCorner[2], 1.0 ); //v0
    corners[1] = transformMtx * vec4(kMinCorner[0], kMaxCorner[1], kMaxCorner[2], 1.0 ); //v1
    corners[2] = transformMtx * vec4(kMaxCorner[0], kMinCorner[1], kMaxCorner[2], 1.0 ); //v2
    corners[3] = transformMtx * vec4(kMinCorner[0], kMinCorner[1], kMaxCorner[2], 1.0 ); //v3
    corners[4] = transformMtx * vec4(kMaxCorner[0], kMaxCorner[1], kMinCorner[2], 1.0 ); //v4
    corners[5] = transformMtx * vec4(kMinCorner[0], kMaxCorner[1], kMinCorner[2], 1.0 ); //v5
    corners[6] = transformMtx * vec4(kMaxCorner[0], kMinCorner[1], kMinCorner[2], 1.0 ); //v6
    corners[7] = transformMtx * vec4(kMinCorner[0], kMinCorner[1], kMinCorner[2], 1.0 ); //v7


    //check how the bounding box resides regarding to the view frustum
    int outOfBound[6] = int[6]( 0, 0, 0, 0, 0, 0 );

    for (int i=0; i<8; i++)
    {
        if (corners[i].x >  corners[i].w ) outOfBound[0]++;
        if (corners[i].x < -corners[i].w ) outOfBound[1]++;
        if (corners[i].y >  corners[i].w ) outOfBound[2]++;
        if (corners[i].y < -corners[i].w ) outOfBound[3]++;
        if (corners[i].z >  corners[i].w ) outOfBound[4]++;
        if (corners[i].z < -corners[i].w ) outOfBound[5]++;
    }

	int inFrustum = 1;
    for (int i=0; i<6; i++)
    {
        if ( outOfBound[i] == 8 )
        {
            inFrustum = 0;
        }
    }
    return inFrustum;
}

void main(void)
{
    vec3 inPoint = InInstancingMtx[3].xyz;

    if(InRenderable == 1)
    {
        if(kEnableIcr == 1)
            VVisible = icr(inPoint);
        else
            VVisible = 1; /* passThrough */
    }
    else
    {
        VVisible = 0;
    }

    VInstancingMtx  = InInstancingMtx;
    VColor          = InColor;
    VWireColor      = InWireColor;
}