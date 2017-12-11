
#include "vertex.hpp"
#include "gl.hpp"
#include "maths.hpp"

#include <cassert>
#include <stdexcept>


template<typename T>
void AttachAttribute(int attrib_id, size_t stride, size_t offset)
{
  const GLvoid *offset_ptr = reinterpret_cast<GLvoid *>(offset);

  if constexpr (std::is_same<T, vec2>::value)
  {
    glVertexAttribPointer(attrib_id, 2, GL_FLOAT, GL_FALSE, stride, offset_ptr);
  }
  else if constexpr (std::is_same<T, vec3>::value)
  {
    glVertexAttribPointer(attrib_id, 3, GL_FLOAT, GL_FALSE, stride, offset_ptr);
  }
  else if constexpr (std::is_same<T, col4>::value)
  {
    glVertexAttribPointer(attrib_id, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, offset_ptr);
  }
  else
  {
    throw std::runtime_error("Unknown attrib type");
  }

  glEnableVertexAttribArray(attrib_id);
}


#define ATTACH_ATTRIBUTE(attrib_id, v, field) \
  AttachAttribute<typeof(v::field)>(attrib_id, sizeof(v), offsetof(v, field))


void DetachAttribute(int attrib_id)
{
  // glBindVertexArray(vao_id);
  glDisableVertexAttribArray(attrib_id);
  // glBindVertexArray(0);
}


VertexDataBasic::VertexDataBasic(GLenum usage)
: usage(usage)
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

  // AttachAttribute(0, 2, 0, GL_FLOAT); //position location = 0
  // AttachAttribute(1, 4, 2, GL_FLOAT); //colour, location = 1
  ATTACH_ATTRIBUTE(0, Vertex, position);
  ATTACH_ATTRIBUTE(1, Vertex, colour);


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

void VertexDataBasic::AddVertex(const Vertex &v)
{
  vertex_data.push_back(v);
}


void VertexDataBasic::AddVertex(const vec2 &position, const col4 &colour)
{
  AddVertex({position, colour});
}


void VertexDataBasic::AddVertex(float x, float y, const col4 &colour)
{
  AddVertex({{x, y}, colour});
}


void VertexDataBasic::AddLine(const vec2 &p1, const col4 &c1, const vec2 &p2, const col4 &c2)
{
  AddVertex(p1, c1);
  AddVertex(p2, c2);
}


void VertexDataBasic::UpdateVertexes()
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex_data.size(), vertex_data.data(), usage);
  // glBindBuffer(GL_ARRAY_BUFFER, 0);
}


int VertexDataBasic::GetOffset() const
{
  return vertex_data.size();
}


int VertexDataBasic::GetNumVertexes() const
{
  return vertex_data.size();
}


int VertexDataBasic::GetVAO() const
{
  return vao_id;
}


// void VertexDataBasic::AttachAttribute(int attrib_id, int size, int offset, GLenum type)
// {
//   const GLvoid *offset_ptr = reinterpret_cast<GLvoid *>(offset * sizeof(float));
//   // glBindVertexArray(vao_id);
//   // glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
//   glVertexAttribPointer(attrib_id, size, type, GL_FALSE, stride, offset_ptr);
//   glEnableVertexAttribArray(attrib_id);
//   // glBindBuffer(GL_ARRAY_BUFFER, 0);
//   // glBindVertexArray(0);
// }


// void VertexDataBasic::DetachAttribute(int attrib_id)
// {
//   // glBindVertexArray(vao_id);
//   glDisableVertexAttribArray(attrib_id);
//   // glBindVertexArray(0);
// }


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


VertexDataTextured::VertexDataTextured(GLenum usage, GLenum primitive_type)
: usage(usage)
, primitive_type(primitive_type)
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

  ATTACH_ATTRIBUTE(0, Vertex, position);
  ATTACH_ATTRIBUTE(1, Vertex, colour);
  ATTACH_ATTRIBUTE(2, Vertex, uv);


  glBindVertexArray(0);

  if (GL::CheckError()) throw std::runtime_error("VertexDataTextured() failed");
}


VertexDataTextured::~VertexDataTextured()
{
  glBindVertexArray(vao_id);

  DetachAttribute(0);
  DetachAttribute(1);
  DetachAttribute(2);

  GL::DeleteBuffers(buffer_id);

  glBindVertexArray(0);
  GL::DeleteVertexArrays(vao_id);
}


void VertexDataTextured::Clear()
{
  vertex_data.clear();
}


void VertexDataTextured::AddVertex(const Vertex &v)
{
  vertex_data.push_back(v);
}


void VertexDataTextured::AddVertex(const vec2 &position, const vec3 &uv, const col4 &colour)
{
  vertex_data.push_back({position, colour, uv});
}


// void VertexDataTextured::DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2)
// {
//   const col4 white{1.0f, 1.0f, 1.0f, 1.0f};
//   return DrawQuad(pos1, uv1, pos2, uv2, white);
// }


void VertexDataTextured::DrawQuad(vec2 pos1, vec2 uv1, vec2 pos2, vec2 uv2, col4 colour, int layer)
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


void VertexDataTextured::UpdateVertexes()
{
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertex_data.size(), vertex_data.data(), usage);
}


// int VertexDataTextured::GetNumVertexes() const
// {
//   return vertex_data.size();
// }


int VertexDataTextured::GetVAO() const
{
  return vao_id;
}


// void VertexDataTextured::AttachAttribute(int attrib_id, int size, int offset, GLenum type)
// {
//   const GLvoid *offset_ptr = reinterpret_cast<GLvoid *>(offset * sizeof(float));
//   // glBindVertexArray(vao_id);
//   // glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
//   glVertexAttribPointer(attrib_id, size, type, GL_FALSE, stride, offset_ptr);
//   glEnableVertexAttribArray(attrib_id);
//   //glBindVertexArray(0);
//   //glBindBuffer(GL_ARRAY_BUFFER, 0);
// }


// void VertexDataTextured::DetachAttribute(int attrib_id)
// {
//   // glBindVertexArray(vao_id);
//   glDisableVertexAttribArray(attrib_id);
//   // glBindVertexArray(0);
// }


void VertexDataTextured::Draw() const
{
  glDrawArrays(primitive_type, 0, vertex_data.size());
}


void VertexDataTextured::Flush()
{
  UpdateVertexes();

  Draw();

  Clear();
}
