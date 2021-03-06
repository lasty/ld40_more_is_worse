#pragma once

#include <iostream>
#include <string>


//Set stream number formats to be readable
void SetStreamFormat(std::ostream &os);


//Game stuff
std::string GetInputName(int button_or_key);


//Maths stuff
std::ostream &operator<<(std::ostream &out, struct vec2 const &v2);
std::ostream &operator<<(std::ostream &out, struct vec3 const &v3);
std::ostream &operator<<(std::ostream &out, struct col4 const &v4);
std::ostream &operator<<(std::ostream &out, struct mat4 const &mat);


//GL stuff
typedef uint32_t GLenum;
std::string GLenum_ToString(GLenum e);


//CPP stuff
std::string CPPVersion();
