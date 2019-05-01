#version 430 compatibility

in  vec3  inValue;
out float geoValue;

void main()
{
    //geoValue = inValue;
    geoValue = gl_VertexID;
}
