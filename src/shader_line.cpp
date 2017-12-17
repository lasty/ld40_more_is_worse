
#include "shader_line.hpp"

#include <stdexcept>
#include <string>

#include "gl.hpp"
#include "maths.hpp"

namespace {

const std::string vertex_src =
  R"(#version 330

layout(location=0) in vec2 v;
layout(location=1) in vec4 col;

out vec4 vertex_colour;

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
}
)";


const std::string fragment_src =
  R"(#version 330

uniform vec4 colour;

in vec4 vertex_colour;
out vec4 out_colour;

void main(void)
{
  out_colour = colour * vertex_colour;
  //out_colour.a += 0.5;
}

)";
}

namespace Shader {

Line::LineVertexArray::LineVertexArray()
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

  GL::ATTACH_ATTRIBUTE(0, Vertex, position);
  GL::ATTACH_ATTRIBUTE(1, Vertex, colour);

  glBindVertexArray(0);
}


Line::LineVertexArray::~LineVertexArray()
{
  glBindVertexArray(vao_id);

  GL::DetachAttribute(0);
  GL::DetachAttribute(1);

  glBindVertexArray(0);

  GL::DeleteVertexArrays(vao_id);
  GL::DeleteBuffers(buffer_id);
}


void Line::LineVertexArray::Update()
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * size(), data(), GL_DYNAMIC_DRAW);
}


void Line::LineVertexArray::Line(const vec2 &p1, const col4 &c1, const vec2 &p2, const col4 &c2)
{
  push_back({p1, c1});
  push_back({p2, c2});
}

void Line::LineVertexArray::Circle(const vec2 &position, float radius, const col4 &colour)
{
  int segments = 32;
  for (int i = 0; i < segments; i++)
    for (int j = 0; j <= 1; j++)
    {
      int a = (i + j) % segments;

      float angle = float(2 * PI) * (float(a) / float(segments));

      float x = position.x + radius * cosf(angle);
      float y = position.y + radius * sinf(angle);

      push_back({{x, y}, colour});
    }
}


void Line::LineVertexArray::Rect(vec2 position, vec2 size, col4 colour)
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
  SetOffset(0.0f, 0.0f);
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


void Line::SetOffset(int x, int y)
{
  glProgramUniform2f(program_id, uniforms.offset, x, y);
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


void Line::Update(LineVertexArray &array)
{
  array.Update();
}


void Line::Render(LineVertexArray &array)
{
  glUseProgram(program_id);
  glBindVertexArray(array.vao_id);

  glDrawArrays(GL_LINES, 0, array.size());
}


} //namespace Shader
