
#include "shader_textured.hpp"

#include <stdexcept>
#include <string>

#include "gl.hpp"

#include "maths.hpp"


namespace {

struct attrib
{
  constexpr static auto position = 0;
  constexpr static auto colour = 1;
  constexpr static auto uv = 2;
};

const std::string vertex_src =
  R"(#version 330

layout(location=0) in vec2 v;
layout(location=1) in vec4 col;
layout(location=2) in vec3 uv;

out vec4 vertex_colour;
out vec3 uv_coords;

uniform ivec2 screen_resolution;

uniform vec2 offset;
uniform float rotation;
uniform float zoom;

vec2 ScreenToClip(const vec2 screen)
{
  float x = ((screen.x / float(screen_resolution.x)) * 2.0) - 1.0;
  float y = ((1.0 - (screen.y / float(screen_resolution.y))) * 2.0) - 1.0;
  return vec2(x,y);
}

mat2 RotateMatrix(float angle)
{
  return mat2(cos(angle), sin(angle), -sin(angle), cos(angle));
}

void main(void)
{
  vec2 v_rotated = RotateMatrix(rotation) * v;
  vec2 v_zoomed = v_rotated * zoom;
  vec2 screen_pos = v_zoomed + offset;

  gl_Position = vec4(ScreenToClip(screen_pos), 0.0, 1.0);
  uv_coords = uv;
  vertex_colour = col;
}
)";


const std::string fragment_src =
  R"(#version 330

uniform vec4 colour;
uniform sampler2DArray tex_id;

in vec4 vertex_colour;
in vec3 uv_coords;
out vec4 out_colour;

void main(void)
{
  vec3 uv_floats = vec3(uv_coords.xy / textureSize(tex_id, 0).xy, uv_coords.z);
  vec4 tex_colour = texture(tex_id, uv_floats);

  out_colour = tex_colour * colour * vertex_colour;
  //out_colour = vertex_colour;
}

)";
}


namespace Shader {


Textured::VertexArray::VertexArray()
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

  GL::ATTACH_ATTRIBUTE(attrib::position, Vertex, position);
  GL::ATTACH_ATTRIBUTE(attrib::colour, Vertex, colour);
  GL::ATTACH_ATTRIBUTE(attrib::uv, Vertex, uv);

  // GL::AttachAttribute<vec2>(attrib::position, sizeof(Vertex), offsetof(Vertex, position));
  // GL::AttachAttribute<col4>(attrib::colour, sizeof(Vertex), offsetof(Vertex, colour));
  // GL::AttachAttribute<vec3>(attrib::uv, sizeof(Vertex), offsetof(Vertex, uv));

  glBindVertexArray(0);
}


Textured::VertexArray::~VertexArray()
{
  glBindVertexArray(vao_id);

  GL::DetachAttribute(0);
  GL::DetachAttribute(1);
  GL::DetachAttribute(2);

  GL::DeleteBuffers(buffer_id);

  glBindVertexArray(0);
  GL::DeleteVertexArrays(vao_id);
}


void Textured::VertexArray::AddVertex(const Vertex &v)
{
  push_back(v);
}


void Textured::VertexArray::AddVertex(const vec2 &position, const vec3 &uv, const col4 &colour)
{
  push_back({position, colour, uv});
}


void Textured::VertexArray::DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2, col4 colour, int layer)
{
  vec2 tl{pos1.x, pos1.y};
  vec2 tr{pos2.x, pos1.y};
  vec2 bl{pos1.x, pos2.y};
  vec2 br{pos2.x, pos2.y};

  vec3 tl_uv{uv1.x, uv1.y, float(layer)};
  vec3 tr_uv{uv2.x, uv1.y, float(layer)};
  vec3 bl_uv{uv1.x, uv2.y, float(layer)};
  vec3 br_uv{uv2.x, uv2.y, float(layer)};

  AddVertex(tl, tl_uv, colour);
  AddVertex(bl, bl_uv, colour);
  AddVertex(tr, tr_uv, colour);

  AddVertex(tr, tr_uv, colour);
  AddVertex(bl, bl_uv, colour);
  AddVertex(br, br_uv, colour);
}


void Textured::VertexArray::Update()
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * size(), data(), GL_DYNAMIC_DRAW);
}


Textured::Textured()
{
  vertex_shader_id = GL::CreateShader(GL_VERTEX_SHADER, vertex_src);
  fragment_shader_id = GL::CreateShader(GL_FRAGMENT_SHADER, fragment_src);

  program_id = glCreateProgram();

  glAttachShader(program_id, vertex_shader_id);
  glAttachShader(program_id, fragment_shader_id);

  GL::LinkProgram(program_id);

  uniforms.screen_resolution = glGetUniformLocation(program_id, "screen_resolution");
  uniforms.offset = glGetUniformLocation(program_id, "offset");
  uniforms.rotation = glGetUniformLocation(program_id, "rotation");
  uniforms.colour = glGetUniformLocation(program_id, "colour");
  uniforms.zoom = glGetUniformLocation(program_id, "zoom");
  uniforms.texture = glGetUniformLocation(program_id, "tex_id");

  for (auto &u :
    {uniforms.screen_resolution, uniforms.offset, uniforms.rotation, uniforms.colour, uniforms.zoom, uniforms.texture})
  {
    if (u == -1) throw std::runtime_error("uniform is not valid");
  }

  //Set some sane defaults
  SetResolution(1280, 768);
  SetColour({1.0f, 1.0f, 1.0f, 1.0f});
  SetOffset({0.0f, 0.0f});
  SetRotation(0.0f);
  SetZoom(1.0f);
}


Textured::~Textured()
{
  glDetachShader(program_id, vertex_shader_id);
  glDetachShader(program_id, fragment_shader_id);

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  glDeleteProgram(program_id);
}


void Textured::SetResolution(int width, int height)
{
  glProgramUniform2i(program_id, uniforms.screen_resolution, width, height);
}


void Textured::SetOffset(vec2 const &offset)
{
  glProgramUniform2fv(program_id, uniforms.offset, 1, reinterpret_cast<const GLfloat *>(&offset));
}


void Textured::SetRotation(float rot)
{
  glProgramUniform1f(program_id, uniforms.rotation, rot);
}


void Textured::SetZoom(float zoom)
{
  glProgramUniform1f(program_id, uniforms.zoom, zoom);
}


void Textured::SetColour(col4 const &colour)
{
  const float r{colour.r / 255.0f};
  const float g{colour.g / 255.0f};
  const float b{colour.b / 255.0f};
  const float a{colour.a / 255.0f};

  glProgramUniform4f(program_id, uniforms.colour, r, g, b, a);
}


void Textured::SetTexture(int tex_unit)
{
  glProgramUniform1i(program_id, uniforms.texture, tex_unit);
}


void Textured::Render(VertexArray &array)
{
  glUseProgram(program_id);
  glBindVertexArray(array.vao_id);

  glDrawArrays(GL_TRIANGLES, 0, array.size());
}


} //namespace Shader
