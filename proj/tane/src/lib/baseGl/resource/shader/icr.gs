#version 430 compatibility

#define IN_RENDERABLE_LOCATION          0
#define IN_INSTANCING_MTX_LOCATION      1
#define IN_COLOR_LOCATION               5
#define IN_WIRECOLOR_LOCATION           6

layout(points) in;
layout(points, max_vertices = 1) out;

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

flat in int    VVisible[1];
in mat4        VInstancingMtx[1];
in vec4        VColor[1];
in vec4        VWireColor[1];

#define INST_MTX_LOD0_STREAM 0
#define INST_MTX_LOD1_STREAM 1
#define INST_MTX_LOD2_STREAM 2

layout(stream=INST_MTX_LOD0_STREAM) out;
out vec4 OutInstancingMtxLOD00;
out vec4 OutInstancingMtxLOD01;
out vec4 OutInstancingMtxLOD02;
out vec4 OutInstancingMtxLOD03;
out vec4 OutColorLOD0;
out vec4 OutWireColorLOD0;

layout(stream=INST_MTX_LOD1_STREAM) out;
out vec4 OutInstancingMtxLOD10;
out vec4 OutInstancingMtxLOD11;
out vec4 OutInstancingMtxLOD12;
out vec4 OutInstancingMtxLOD13;
out vec4 OutColorLOD1;
out vec4 OutWireColorLOD1;

layout(stream=INST_MTX_LOD2_STREAM) out;
out vec4 OutInstancingMtxLOD20;
out vec4 OutInstancingMtxLOD21;
out vec4 OutInstancingMtxLOD22;
out vec4 OutInstancingMtxLOD23;
out vec4 OutColorLOD2;
out vec4 OutWireColorLOD2;

void emitLOD0()
{
    OutInstancingMtxLOD00 = VInstancingMtx[0][0];
    OutInstancingMtxLOD01 = VInstancingMtx[0][1];
    OutInstancingMtxLOD02 = VInstancingMtx[0][2];
    OutInstancingMtxLOD03 = VInstancingMtx[0][3];
    OutColorLOD0        = VColor[0];
    OutWireColorLOD0    = VWireColor[0];

    EmitStreamVertex(INST_MTX_LOD0_STREAM);
}
void emitLOD1()
{
    OutInstancingMtxLOD10 = VInstancingMtx[0][0];
    OutInstancingMtxLOD11 = VInstancingMtx[0][1];
    OutInstancingMtxLOD12 = VInstancingMtx[0][2];
    OutInstancingMtxLOD13 = VInstancingMtx[0][3];
    OutColorLOD1        = VColor[0];
    OutWireColorLOD1    = VWireColor[0];

    EmitStreamVertex(INST_MTX_LOD1_STREAM);
}
void emitLOD2()
{
    OutInstancingMtxLOD20 = VInstancingMtx[0][0];
    OutInstancingMtxLOD21 = VInstancingMtx[0][1];
    OutInstancingMtxLOD22 = VInstancingMtx[0][2];
    OutInstancingMtxLOD23 = VInstancingMtx[0][3];
    OutColorLOD2        = VColor[0];
    OutWireColorLOD2    = VWireColor[0];

    EmitStreamVertex(INST_MTX_LOD2_STREAM);
}

void dynamicLOD()
{
    vec3 inPoint = VInstancingMtx[0][3].xyz;
	float distance = length(gl_ModelViewMatrix * vec4(inPoint, 1.0));

	if ( distance < kMinDistance )
	{
	    emitLOD0();
    }
	else if ( distance < kMaxDistance )
	{
	    emitLOD1();
	}
	else
    {
        emitLOD2();
    }
}

void main()
{
	if(VVisible[0] == 1)
	{
	    if(kEnableLOD == 1)
	    {
	        dynamicLOD();
	    }
        else
        {
            emitLOD0();
        }
	}
}