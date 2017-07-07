/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/04/16
* File: enn_string_util.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_vector2.h"
#include "enn_vector3.h"
#include "enn_vector4.h"
#include "enn_color.h"
#include "enn_matrix3.h"
#include "enn_matrix4.h"
#include "enn_quaternion.h"
#include "enn_rect.h"

namespace enn
{

unsigned CountElements(const char* buffer, char separator);

float		to_float(const enn::String& source);
float		to_float(const char* source);
int32		to_int(const enn::String& source);
int32		to_int(const char* source);
uint32		to_uint(const enn::String& source);
uint32		to_uint(const char* source);
vec2f		to_vec2(const enn::String& source);
vec2f		to_vec2(const char* source);
vec3f		to_vec3(const enn::String& source);
vec3f		to_vec3(const char* source);
vec4f		to_vec4(const enn::String& source);
vec4f		to_vec4(const char* source);
String		to_string( int i );
String		to_string(float f);
String		to_string(long i);
String		to_string(short i);
String		to_string(long long i);
String		to_string(unsigned int i);
String		to_string(unsigned long i);
String		to_string(unsigned short i);
String		to_string(unsigned long long i);
String		to_string(bool value);
void		replace_ch(String& str, char replaceThis, char replaceWith);
void		replace_str(String& str, const String& replaceThis, const String& replaceWith);

void		BufferToString(String& dest, const void* data, unsigned size);
void		StringToBuffer(vector<unsigned char>::type& dest, const String& source);
void		StringToBuffer(vector<unsigned char>::type& dest, const char* source);


bool		to_bool(const String& source);
bool		to_bool(const char* source);

vector<String>::type split_str(const char* str, char separator);
vector<String>::type split_str(const String& str, char separator);

Color to_color(const String& source);
Color to_color(const char* source);

IntRect to_int_rect(const String& source);
IntRect to_int_rect(const char* source);

Quaternion to_quat(const String& source);
Quaternion to_quat(const char* source);

Matrix3 to_matrix3(const String& source);
Matrix3 to_matrix3(const char* source);

Matrix4 to_matrix4(const String& source);
Matrix4 to_matrix4(const char* source);

String trim_str(const String& source);
String trim_str(const char* source);

String to_string(const char* formatString, ...);

IntVector2 to_int_vec2(const String& source);
IntVector2 to_int_vec2(const char* source);

const vector<String>::type& ParseArguments(int argc, char** argv);

const vector<String>::type& ParseArguments(const String& cmdLine, bool skipFirstArgument = true);

void ErrorExit(const String& message = "", int exitCode = EXIT_FAILURE);

void PrintLine(const String& str, bool error = false);

}