
#include "shader_line.hpp"

#include <stdexcept>
#include <string>

#include "gl.hpp"
#include "maths.hpp"

namespace {

struct attrib
{
  constexpr static int position = 0;
  constexpr static int colour = 1;
  constexpr static int uv = 2;
};

const std::string vertex_src =
  R"(#version 330

layout(location=0) in vec2 v;
layout(location=1) in vec4 col;
layout(location=2) in vec2 uv;

out vec4 vertex_colour;
out vec2 vertex_uv;

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
  vertex_colour = col;
  vertex_uv = uv;
}
)";


const std::string fragment_src =
  R"(#version 330

uniform vec4 colour;

in vec4 vertex_colour;
in vec2 vertex_uv;
out vec4 out_colour;

void main(void)
{
  out_colour = colour * vertex_colour;
  out_colour.a = 1.0 - abs(vertex_uv.y);
  out_colour.a *= out_colour.a;
  // out_colour.a += 0.2;
}

)";
}

namespace Shader {

Line::VertexArray::VertexArray()
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
  // GL::AttachAttribute<vec2>(attrib::uv, sizeof(Vertex), offsetof(Vertex, uv));


  glBindVertexArray(0);
}


Line::VertexArray::~VertexArray()
{
  glBindVertexArray(vao_id);

  GL::DetachAttribute(0);
  GL::DetachAttribute(1);

  glBindVertexArray(0);

  GL::DeleteVertexArrays(vao_id);
  GL::DeleteBuffers(buffer_id);
}


void Line::VertexArray::Update()
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * size(), data(), GL_DYNAMIC_DRAW);
}


void Line::VertexArray::Line(const vec2 &p1, const col4 &c1, const vec2 &p2, const col4 &c2, const vec2 &normal)
{
  const float line_width = 2.0f;
  vec2 width = normal * line_width;

  Vertex tl{p1 + width, c1, {-1.0f, -1.0f}};
  Vertex tr{p2 + width, c2, {-1.0f, -1.0f}};
  Vertex bl{p1 - width, c1, {1.0f, 1.0f}};
  Vertex br{p2 - width, c2, {1.0f, 1.0f}};

  push_back(bl);
  push_back(tr);
  push_back(tl);

  push_back(tr);
  push_back(bl);
  push_back(br);
}


void Line::VertexArray::Line(const vec2 &p1, const col4 &c1, const vec2 &p2, const col4 &c2)
{
  Line(p1, c1, p2, c2, get_normal(p1, p2));
}


void Line::VertexArray::Circle(const vec2 &position, float radius, const col4 &colour)
{
  int segments = 32;
  for (int i = 0; i < segments; i++)
  {
    int a1 = (i) % segments;
    int a2 = (i + 1) % segments;

    float angle1 = float(2 * PI) * (float(a1) / float(segments));
    float angle2 = float(2 * PI) * (float(a2) / float(segments));

    vec2 p1{position.x + radius * cosf(angle1), position.y + radius * sinf(angle1)};
    vec2 p2{position.x + radius * cosf(angle2), position.y + radius * sinf(angle2)};

    Line(p1, colour, p2, colour);
  }
}


void Line::VertexArray::Rect(vec2 position, vec2 size, col4 colour)
{
  vec2 tl{position.x, position.y};
  vec2 tr{position.x + size.x, position.y};
  vec2 bl{position.x, position.y + size.y};
  vec2 br{position.x + size.x, position.y + size.y};

  Line(tl, colour, tr, colour);
  Line(tr, colour, br, colour);
  Line(br, colour, bl, colour);
  Line(bl, colour, tl, colour);
}


Line::Line()
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

  for (auto &u :
    {uniforms.screen_resolution, uniforms.offset, uniforms.rotation, uniforms.colour, uniforms.zoom})
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


Line::~Line()
{
  glDetachShader(program_id, vertex_shader_id);
  glDetachShader(program_id, fragment_shader_id);

  glDeleteShader(vertex_shader_id);
  glDeleteShader(fragment_shader_id);

  glDeleteProgram(program_id);
}


void Line::SetResolution(int width, int height)
{
  glProgramUniform2i(program_id, uniforms.screen_resolution, width, height);
}


void Line::SetOffset(vec2 const &offset)
{
  glProgramUniform2fv(program_id, uniforms.offset, 1, reinterpret_cast<const GLfloat *>(&offset));
}


void Line::SetRotation(float rot)
{
  glProgramUniform1f(program_id, uniforms.rotation, rot);
}


void Line::SetZoom(float zoom)
{
  glProgramUniform1f(program_id, uniforms.zoom, zoom);
}


void Line::SetColour(col4 const &colour)
{
  const float r{colour.r / 255.0f};
  const float g{colour.g / 255.0f};
  const float b{colour.b / 255.0f};
  const float a{colour.a / 255.0f};

  glProgramUniform4f(program_id, uniforms.colour, r, g, b, a);
}


void Line::Render(VertexArray &array)
{
  glUseProgram(program_id);
  glBindVertexArray(array.vao_id);

  //glDrawArrays(GL_LINES, 0, array.size());
  glDrawArrays(GL_TRIANGLES, 0, array.size());
}


} //namespace Shader
