#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 shadowMatrices[6];

out vec4 FragPos; // En espacio de mundo

void main()
{
    for(int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // Indica a qué cara del cubemap se renderiza

        for(int i = 0; i < 3; ++i) // Cada vértice del triángulo
        {
            FragPos = gl_in[i].gl_Position; // Esto debe ser en espacio de mundo

            // Transformamos la posición de mundo a espacio de luz
            gl_Position = shadowMatrices[face] * FragPos;
            EmitVertex();
        }    
        EndPrimitive();
    }
}