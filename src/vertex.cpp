
#include "vertex.hpp"
#include "gl.hpp"
#include "maths.hpp"

#include <cassert>
#include <stdexcept>


////////////////////


VertexDataTextured::VertexDataTextured(GLenum usage, GLenum primitive_type)
: usage(usage)
, primitive_type(primitive_type)
{
  buffer_id = GL::CreateBuffers();
  vao_id = GL::CreateVertexArrays();

  glBindVertexArray(vao_id);
  glBindBuffer(GL_ARRAY_BUFFER, buffer_id);

  GL::ATTACH_ATTRIBUTE(0, Vertex, position);
  GL::ATTACH_ATTRIBUTE(1, Vertex, colour);
  GL::ATTACH_ATTRIBUTE(2, Vertex, uv);


  glBindVertexArray(0);

  if (GL::CheckError()) throw std::runtime_error("VertexDataTextured() failed");
}


VertexDataTextured::~VertexDataTextured()
{
  glBindVertexArray(vao_id);

  GL::DetachAttribute(0);
  GL::DetachAttribute(1);
  GL::DetachAttribute(2);

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


int VertexDataTextured::GetVAO() const
{
  return vao_id;
}


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
