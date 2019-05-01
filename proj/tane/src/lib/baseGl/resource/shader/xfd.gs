#version 430 compatibility

layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

in  float[] geoValue;
out float   outValue;

void main()
{
    for(int i = 0; i < gl_in.length(); i++)
    {
        outValue = geoValue[i];
        EmitVertex();
    }
    EndPrimitive();
}

