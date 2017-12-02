
#include "vertex.hpp"
#include "gl.hpp"
#include "maths.hpp"

#include <cassert>
#include <stdexcept>


VertexDataBasic::VertexDataBasic(GLenum usage)
: usage(usage)
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

  AttachAttribute(0, 2, 0, GL_FLOAT); //position location = 0
  AttachAttribute(1, 4, 2, GL_FLOAT); //colour, location = 1

  glBindVertexArray(0);
}


VertexDataBasic::~VertexDataBasic()
{
  glBindVertexArray(vao_id);

  DetachAttribute(0); //position location = 0
  DetachAttribute(1); //colour, location = 1

  GL::DeleteBuffers(buffer_id);

  glBindVertexArray(0);
  GL::DeleteVertexArrays(vao_id);
}


void VertexDataBasic::Clear()
{
  vertex_data.clear();
}


void VertexDataBasic::AddVertex(const vec2 &position, const col4 &colour)
{
  vertex_data.push_back(position.x);
  vertex_data.push_back(position.y);
  vertex_data.push_back(colour.r);
  vertex_data.push_back(colour.g);
  vertex_data.push_back(colour.b);
  vertex_data.push_back(colour.a);
}


void VertexDataBasic::AddVertex(float x, float y, const col4 &colour)
{
  vertex_data.push_back(x);
  vertex_data.push_back(y);
  vertex_data.push_back(colour.r);
  vertex_data.push_back(colour.g);
  vertex_data.push_back(colour.b);
  vertex_data.push_back(colour.a);

  assert(vertex_data.size() % floats_per_vertex == 0);
}


void VertexDataBasic::AddLine(const vec2 &p1, const col4 &c1, const vec2 &p2, const col4 &c2)
{
  AddVertex(p1, c1);
  AddVertex(p2, c2);
}


void VertexDataBasic::UpdateVertexes()
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), vertex_data.data(), usage);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int VertexDataBasic::GetOffset() const
{
  return vertex_data.size() / floats_per_vertex;
}


int VertexDataBasic::GetNumVertexes() const
{
  return vertex_data.size() / floats_per_vertex;
}


int VertexDataBasic::GetVAO() const
{
  return vao_id;
}


void VertexDataBasic::AttachAttribute(int attrib_id, int size, int offset, GLenum type)
{
  const GLvoid *offset_ptr = reinterpret_cast<GLvoid *>(offset * sizeof(float));
  // glBindVertexArray(vao_id);
  // glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glVertexAttribPointer(attrib_id, size, type, GL_FALSE, stride, offset_ptr);
  glEnableVertexAttribArray(attrib_id);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
  // glBindVertexArray(0);
}


void VertexDataBasic::DetachAttribute(int attrib_id)
{
  // glBindVertexArray(vao_id);
  glDisableVertexAttribArray(attrib_id);
  // glBindVertexArray(0);
}


void VertexDataBasic::DrawCircle(const vec2 &position, float radius, const col4 &colour)
{
  int segments = 32;
  for (int i = 0; i < segments; i++)
    for (int j = 0; j <= 1; j++)
    {
      int a = (i + j) % segments;

      float angle = float(2 * PI) * (float(a) / float(segments));

      float x = position.x + radius * cosf(angle);
      float y = position.y + radius * sinf(angle);

      AddVertex(x, y, colour);
    }
}


// void VertexDataBasic::MakeArrow(float radius, const col4 &colour)
// {
//   std::vector<float> out;

//   float width = (PI / 8);
//   for (float angle : {0.0f, PI - width, PI + width})
//   {
//     float x = radius * cosf(angle);
//     float y = radius * sinf(angle);

//     out.push_back(x);
//     out.push_back(y);

//     out.push_back(colour.r);
//     out.push_back(colour.g);
//     out.push_back(colour.b);
//     out.push_back(colour.a);
//   }

//   return out;
// }


void VertexDataBasic::DrawRect(vec2 position, vec2 size, col4 colour)
{
  vec2 tl{position.x, position.y};
  vec2 tr{position.x + size.x, position.y};
  vec2 bl{position.x, position.y + size.y};
  vec2 br{position.x + size.x, position.y + size.y};

  AddLine(tl, colour, tr, colour);
  AddLine(tr, colour, br, colour);
  AddLine(br, colour, bl, colour);
  AddLine(bl, colour, tl, colour);
}


////////////////////


VertexDataTextured::VertexDataTextured(GLenum usage)
: usage(usage)
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

  AttachAttribute(0, 2, 0, GL_FLOAT); //position location = 0
  AttachAttribute(1, 4, 2, GL_FLOAT); //colour, location = 1
  AttachAttribute(2, 2, 6, GL_FLOAT); //uv, location = 2

  glBindVertexArray(0);

  if (GL::CheckError()) throw std::runtime_error("VertexDataTextured() failed");
}


VertexDataTextured::~VertexDataTextured()
{
  glBindVertexArray(vao_id);

  DetachAttribute(0); //position location = 0
  DetachAttribute(1); //colour, location = 1
  DetachAttribute(2); //uv, location = 2

  GL::DeleteBuffers(buffer_id);

  glBindVertexArray(0);
  GL::DeleteVertexArrays(vao_id);
}


void VertexDataTextured::Clear()
{
  vertex_data.clear();
}


void VertexDataTextured::AddVertex(const vec2 &position, const vec2 &uv, const col4 &colour)
{
  vertex_data.push_back(position.x);
  vertex_data.push_back(position.y);
  vertex_data.push_back(colour.r);
  vertex_data.push_back(colour.g);
  vertex_data.push_back(colour.b);
  vertex_data.push_back(colour.a);
  vertex_data.push_back(uv.x);
  vertex_data.push_back(uv.y);

  assert(vertex_data.size() % floats_per_vertex == 0);
}


void VertexDataTextured::DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2)
{
  constexpr col4 white{1.0f, 1.0f, 1.0f, 1.0f};
  return DrawQuad(pos1, uv1, pos2, uv2, white);
}

void VertexDataTextured::DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2, col4 colour)
{
  vec2 tl{pos1.x, pos1.y};
  vec2 tr{pos2.x, pos1.y};
  vec2 bl{pos1.x, pos2.y};
  vec2 br{pos2.x, pos2.y};

  vec2 tl_uv{uv1.x, uv1.y};
  vec2 tr_uv{uv2.x, uv1.y};
  vec2 bl_uv{uv1.x, uv2.y};
  vec2 br_uv{uv2.x, uv2.y};

  AddVertex(tl, tl_uv, colour);
  AddVertex(bl, bl_uv, colour);
  AddVertex(tr, tr_uv, colour);

  AddVertex(tr, tr_uv, colour);
  AddVertex(bl, bl_uv, colour);
  AddVertex(br, br_uv, colour);
}


void VertexDataTextured::UpdateVertexes()
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertex_data.size(), vertex_data.data(), usage);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int VertexDataTextured::GetOffset() const
{
  return vertex_data.size() / floats_per_vertex;
}


int VertexDataTextured::GetNumVertexes() const
{
  return vertex_data.size() / floats_per_vertex;
}


int VertexDataTextured::GetVAO() const
{
  return vao_id;
}


void VertexDataTextured::AttachAttribute(int attrib_id, int size, int offset, GLenum type)
{
  const GLvoid *offset_ptr = reinterpret_cast<GLvoid *>(offset * sizeof(float));
  // glBindVertexArray(vao_id);
  // glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glVertexAttribPointer(attrib_id, size, type, GL_FALSE, stride, offset_ptr);
  glEnableVertexAttribArray(attrib_id);
  //glBindVertexArray(0);
  //glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void VertexDataTextured::DetachAttribute(int attrib_id)
{
  // glBindVertexArray(vao_id);
  glDisableVertexAttribArray(attrib_id);
  // glBindVertexArray(0);
}
