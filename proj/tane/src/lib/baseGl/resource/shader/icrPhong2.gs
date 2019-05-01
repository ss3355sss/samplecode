//#version 430 core
#version 430 compatibility
#define IN_POINT_LOCATION           0
#define IN_NORMAL_LOCATION          1
#define IN_TEXCOORD_LOCATION        2

#define IN_INSTANCING_MTX_LOCATION  3
#define IN_COLOR_LOCATION           7
#define IN_WIRECOLOR_LOCATION       8

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

/* Transform */
uniform mat4 kXformMtx      = mat4(1.0);
uniform mat4 kViewportMtx   = mat4(1.0);

/* DisplayMode */
uniform int kDrawMode = 0;          // 0 : kDrawShaded, 1 : kDrawWire

/* ColorMode */
uniform int  kOverrideMask = 0;
uniform vec4 kMaskColor    = vec4(1.0, 1.0, 1.0, 1.0);

uniform int kNoLighting = 0;
uniform int kOverrideTexture = 0;

in vec3         VNormal[];
in vec3         VPosition[];
in vec4         VColor[];
in vec4         VWireColor[];

out vec4                GColor;
out vec4                GWireColor;
out vec3                GNormal;
out vec3                GPosition;
noperspective out vec3  GEdgeDistance;

void main()
{
    // Transform each vertex into viewport space
    vec3 p0 = vec3(kViewportMtx * (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
    vec3 p1 = vec3(kViewportMtx * (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
    vec3 p2 = vec3(kViewportMtx * (gl_in[2].gl_Position / gl_in[2].gl_Position.w));

    // Find the altitudes (ha, hb and hc)
    float a = length(p1 - p2);
    float b = length(p2 - p0);
    float c = length(p1 - p0);
    float alpha = acos( (b*b + c*c - a*a) / (2.0*b*c) );
    float beta = acos( (a*a + c*c - b*b) / (2.0*a*c) );
    float ha = abs( c * sin( beta ) );
    float hb = abs( c * sin( alpha ) );
    float hc = abs( b * sin( alpha ) );

    /* point0 */
    GEdgeDistance   = vec3( ha, 0, 0 );
    GColor   = VColor[0];
    GWireColor = VWireColor[0];
    GNormal         = VNormal[0];
    GPosition       = VPosition[0];
    gl_Position     = gl_in[0].gl_Position;
    EmitVertex();

    /* point1 */
    GEdgeDistance   = vec3( 0, hb, 0 );
    GColor   = VColor[1];
    GWireColor = VWireColor[1];
    GNormal         = VNormal[1];
    GPosition       = VPosition[1];
    gl_Position     = gl_in[1].gl_Position;
    EmitVertex();

    /* point2 */
    GEdgeDistance   = vec3( 0, 0, hc );
    GColor   = VColor[2];
    GWireColor = VWireColor[2];
    GNormal         = VNormal[2];
    GPosition       = VPosition[2];
    gl_Position     = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();

}


