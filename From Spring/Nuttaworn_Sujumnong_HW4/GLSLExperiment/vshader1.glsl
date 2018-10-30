#version 150

uniform mat4 projection_matrix;
uniform mat4 model_matrix;
uniform mat3 normal_matrix;
uniform mat4 MVP;

in  vec4 vPosition;
in  vec4 vColor;
in  vec3 vNormal;
in  vec2 vTexCoords;

out vec4 interpolatedColor;
out vec3 Position;
out vec3 Normal;
out vec2 texCoords;

void main() 
{
  // PROTIP #3
  // can extend to gl_Position = projection * camera * models * vertex
  // you may wish to view camera motion as a transform on the projection (projection * camera) * models ...
  // this could be thought of as moving the observer in space
  // or you may view camera motion as transforming all objects in the world while the observer remains still
  // projection * (camera * models) * ...
  // notice mathematically there is no difference
  // however in rare circumstances the order of the transform may affect the numerical stability
  // of the overall projection
  texCoords = vTexCoords;
  Normal = normalize(normal_matrix * vNormal);
  Position = vec3(model_matrix*vPosition);

  gl_Position = MVP*projection_matrix*model_matrix*vPosition;
  interpolatedColor = vColor;
} 
