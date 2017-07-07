/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/04/16
* File: string_util.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_string_util.h"

namespace enn
{

static String currentLine_;
static vector<String>::type arguments_;

unsigned CountElements(const char* buffer, char separator)
{
	if (!buffer) return 0;

	const char* endPos = buffer + strlen(buffer);
	const char* pos = buffer;
	unsigned ret = 0;

	while (pos < endPos)
	{
		if (*pos != separator)
			break;
		++pos;
	}

	while (pos < endPos)
	{
		const char* start = pos;

		while (start < endPos)
		{
			if (*start == separator)
				break;

			++start;
		}

		if (start == endPos)
		{
			++ret;
			break;
		}

		const char* end = start;

		while (end < endPos)
		{
			if (*end != separator)
				break;

			++end;
		}

		++ret;
		pos = end;
	}

	return ret;
}

float		to_float(const enn::String& source)
{
	return to_float(source.c_str());
}

float		to_float(const char* source)
{
	if (!source)
	{
		return 0;
	}

	return (float)strtod(source, 0);
}

int32		to_int(const enn::String& source)
{
	return to_int(source.c_str());
}

int32		to_int(const char* source)
{
	if (!source) return 0;

	return strtol(source, 0, 0);
}

uint32		to_uint(const enn::String& source)
{
	return to_uint(source.c_str());
}

uint32		to_uint(const char* source)
{
	if (!source) return 0;

	return strtoul(source, 0, 0);
}

vec2f		to_vec2(const enn::String& source)
{
	return to_vec2(source.c_str());
}

vec2f		to_vec2(const char* source)
{
	vec2f ret(vec2f::ZERO);

	unsigned elements = CountElements(source, ' ');
	if (elements < 2)
		return ret;

	char* ptr = (char*)source;
	ret.x = (float)strtod(ptr, &ptr);
	ret.y = (float)strtod(ptr, &ptr);

	return ret;
}

vec3f		to_vec3(const enn::String& source)
{
	return to_vec3(source.c_str());
}

vec3f		to_vec3(const char* source)
{
	vec3f ret;

	unsigned elements = CountElements(source, ' ');
	if (elements < 3)
		return ret;

	char* ptr = (char*)source;
	ret.x = (float)strtod(ptr, &ptr);
	ret.y = (float)strtod(ptr, &ptr);
	ret.z = (float)strtod(ptr, &ptr);

	return ret;
}

vec4f		to_vec4(const enn::String& source)
{
	return to_vec4(source.c_str());
}

vec4f		to_vec4(const char* source)
{
	vec4f ret(vec4f::ZERO);

	unsigned elements = CountElements(source, ' ');
	char* ptr = (char*)source;
	
	if (elements < 4)
		return ret;

	ret.x = (float)strtod(ptr, &ptr);
	ret.y = (float)strtod(ptr, &ptr);
	ret.z = (float)strtod(ptr, &ptr);
	ret.w = (float)strtod(ptr, &ptr);

	return ret;
}

String to_string( int i )
{
	char buf[CONVERSION_BUFFER_LENGTH] = { 0 };
	sprintf(buf, "%d", i );
	return buf;
}

String to_string(float f)
{
	char tempBuffer[CONVERSION_BUFFER_LENGTH] = { 0 };
	sprintf(tempBuffer, "%g", f);

	return tempBuffer;
}

String to_string(long i)
{
	char buf[CONVERSION_BUFFER_LENGTH] = { 0 };
	sprintf(buf, "%ld", i);
	return buf;
}

String to_string(short i)
{
	char buf[CONVERSION_BUFFER_LENGTH] = { 0 };
	sprintf(buf, "%d", i);
	return buf;
}

String to_string(long long i)
{
	char buf[CONVERSION_BUFFER_LENGTH] = { 0 };
	sprintf(buf, "%lld", i);
	return buf;
}

String to_string(unsigned int i)
{
	char buf[CONVERSION_BUFFER_LENGTH] = { 0 };
	sprintf(buf, "%u", i);
	return buf;
}

String to_string(unsigned long i)
{
	char buf[CONVERSION_BUFFER_LENGTH] = { 0 };
	sprintf(buf, "%d", i);
	return buf;
}

String to_string(unsigned short i)
{
	char buf[CONVERSION_BUFFER_LENGTH] = { 0 };
	sprintf(buf, "%d", i);
	return buf;
}

String to_string(unsigned long long i)
{
	char buf[CONVERSION_BUFFER_LENGTH] = { 0 };
	sprintf(buf, "%d", i);
	return buf;
}

String	to_string(bool value)
{
	return value ? "true" : "false";
}

void replace_ch(String& str, char replaceThis, char replaceWith)
{
	for (unsigned i = 0; i < str.size(); ++i)
	{
		if (str[i] == replaceThis)
		{
			str[i] = replaceWith;
		}
	}
}

void replace_str(String& str, const String& replaceThis, const String& replaceWith)
{
	size_t nextPos = 0;

	size_t length = str.size();

	while ((int)nextPos < length)
	{
		size_t pos = str.find(replaceThis, nextPos);

		if (pos == String::npos) break;

		str.replace(pos, replaceThis.size(), replaceWith);

		nextPos = pos + replaceWith.size();
	}
}


void BufferToString(String& dest, const void* data, unsigned size)
{
	// Precalculate needed string size
	const unsigned char* bytes = (const unsigned char*)data;
	unsigned length = 0;
	for (unsigned i = 0; i < size; ++i)
	{
		// Room for separator
		if (i)
			++length;

		// Room for the value
		if (bytes[i] < 10)
			++length;
		else if (bytes[i] < 100)
			length += 2;
		else
			length += 3;
	}

	dest.resize(length);
	unsigned index = 0;

	// Convert values
	for (unsigned i = 0; i < size; ++i)
	{
		if (i)
			dest[index++] = ' ';

		if (bytes[i] < 10)
		{
			dest[index++] = '0' + bytes[i];
		}
		else if (bytes[i] < 100)
		{
			dest[index++] = '0' + bytes[i] / 10;
			dest[index++] = '0' + bytes[i] % 10;
		}
		else
		{
			dest[index++] = '0' + bytes[i] / 100;
			dest[index++] = '0' + bytes[i] % 100 / 10;
			dest[index++] = '0' + bytes[i] % 10;
		}
	}
}

void StringToBuffer(vector<unsigned char>::type& dest, const String& source)
{
	StringToBuffer(dest, source.c_str());
}

void StringToBuffer(vector<unsigned char>::type& dest, const char* source)
{
	if (!source)
	{
		dest.clear();
		return;
	}

	unsigned size = CountElements(source, ' ');
	dest.resize(size);

	bool inSpace = true;
	unsigned index = 0;
	unsigned value = 0;

	// Parse values
	const char* ptr = source;
	while (*ptr)
	{
		if (inSpace && *ptr != ' ')
		{
			inSpace = false;
			value = *ptr - '0';
		}
		else if (!inSpace && *ptr != ' ')
		{
			value *= 10;
			value += *ptr - '0';
		}
		else if (!inSpace && *ptr == ' ')
		{
			dest[index++] = value;
			inSpace = true;
		}

		++ptr;
	}

	// Write the final value
	if (!inSpace && index < size)
		dest[index] = value;
}

bool to_bool(const String& source)
{
	return to_bool(source.c_str());
}

bool to_bool(const char* source)
{
	size_t length = strlen(source);

	for (unsigned i = 0; i < length; ++i)
	{
		char c = tolower(source[i]);
		if (c == 't' || c == 'y' || c == '1')
			return true;
		else if (c != ' ' && c != '\t')
			break;
	}

	return false;
}

vector<String>::type split_str(const char* str, char separator)
{
	vector<String>::type ret;
	unsigned pos = 0;
	size_t length = strlen(str);

	while (pos < length)
	{
		if (str[pos] != separator)
			break;
		++pos;
	}

	while (pos < length)
	{
		unsigned start = pos;

		while (start < length)
		{
			if (str[start] == separator)
				break;

			++start;
		}

		if (start == length)
		{
			ret.push_back(String(&str[pos]));
			break;
		}

		unsigned end = start;

		while (end < length)
		{
			if (str[end] != separator)
				break;

			++end;
		}

		ret.push_back(String(&str[pos], start - pos));
		pos = end;
	}

	return ret;
}

vector<String>::type split_str(const String& str, char separator)
{
	return split_str(str.c_str(), separator);
}

Color to_color(const String& source)
{
	return to_color(source.c_str());
}

Color to_color(const char* source)
{
	Color ret;

	unsigned elements = CountElements(source, ' ');
	if (elements < 3)
		return ret;

	char* ptr = (char*)source;
	ret.r = (float)strtod(ptr, &ptr);
	ret.g = (float)strtod(ptr, &ptr);
	ret.b = (float)strtod(ptr, &ptr);
	if (elements > 3)
		ret.a = (float)strtod(ptr, &ptr);

	return ret;
}

IntRect to_int_rect(const String& source)
{
	return to_int_rect(source.c_str());
}

IntRect to_int_rect(const char* source)
{
	IntRect ret;

	unsigned elements = CountElements(source, ' ');
	if (elements < 4)
		return ret;

	char* ptr = (char*)source;
	ret.left = strtol(ptr, &ptr, 10);
	ret.top = strtol(ptr, &ptr, 10);
	ret.right = strtol(ptr, &ptr, 10);
	ret.bottom = strtol(ptr, &ptr, 10);

	return ret;
}

Quaternion to_quat(const String& source)
{
	return to_quat(source.c_str());
}

Quaternion to_quat(const char* source)
{
	unsigned elements = CountElements(source, ' ');
	char* ptr = (char*)source;

	if (elements < 4) return Quaternion::IDENTITY;

	Quaternion ret;
	ret.w = (float)strtod(ptr, &ptr);
	ret.x = (float)strtod(ptr, &ptr);
	ret.y = (float)strtod(ptr, &ptr);
	ret.z = (float)strtod(ptr, &ptr);

	return ret;
}

Matrix3 to_matrix3(const String& source)
{
	return to_matrix3(source.c_str());
}

Matrix3 to_matrix3(const char* source)
{
	Matrix3 ret(Matrix3::IDENTITY);

	unsigned elements = CountElements(source, ' ');
	if (elements < 9)
		return ret;

	char* ptr = (char*)source;
	ret.m[0][0] = (float)strtod(ptr, &ptr);
	ret.m[0][1] = (float)strtod(ptr, &ptr);
	ret.m[0][2] = (float)strtod(ptr, &ptr);
	ret.m[1][0] = (float)strtod(ptr, &ptr);
	ret.m[1][1] = (float)strtod(ptr, &ptr);
	ret.m[1][2] = (float)strtod(ptr, &ptr);
	ret.m[2][0] = (float)strtod(ptr, &ptr);
	ret.m[2][1] = (float)strtod(ptr, &ptr);
	ret.m[2][2] = (float)strtod(ptr, &ptr);

	return ret;
}

Matrix4 to_matrix4(const String& source)
{
	return to_matrix4(source.c_str());
}

Matrix4 to_matrix4(const char* source)
{
	Matrix4 ret(Matrix4::IDENTITY);

	unsigned elements = CountElements(source, ' ');
	if (elements < 16)
		return ret;

	char* ptr = (char*)source;
	ret.m[0][0] = (float)strtod(ptr, &ptr);
	ret.m[0][1] = (float)strtod(ptr, &ptr);
	ret.m[0][2] = (float)strtod(ptr, &ptr);
	ret.m[1][0] = (float)strtod(ptr, &ptr);
	ret.m[1][1] = (float)strtod(ptr, &ptr);
	ret.m[1][2] = (float)strtod(ptr, &ptr);
	ret.m[2][0] = (float)strtod(ptr, &ptr);
	ret.m[2][1] = (float)strtod(ptr, &ptr);
	ret.m[2][2] = (float)strtod(ptr, &ptr);
	ret.m[2][3] = (float)strtod(ptr, &ptr);
	ret.m[3][0] = (float)strtod(ptr, &ptr);
	ret.m[3][1] = (float)strtod(ptr, &ptr);
	ret.m[3][2] = (float)strtod(ptr, &ptr);
	ret.m[3][3] = (float)strtod(ptr, &ptr);

	return ret;
}

String trim_str(const String& source)
{
	size_t trimStart = 0;
	size_t trimEnd = source.size();

	while (trimStart < trimEnd)
	{
		char c = source[trimStart];
		if (c != ' ' && c != 9)
			break;
		++trimStart;
	}
	while (trimEnd > trimStart)
	{
		char c = source[trimEnd - 1];
		if (c != ' ' && c != 9)
			break;
		--trimEnd;
	}

	return source.substr(trimStart, trimEnd - trimStart);
}

String trim_str(const char* source)
{
	return trim_str(String(source));
}

String to_string(const char* formatString, ...)
{
	String ret;
	va_list args;
	va_start(args, formatString);
	//ret.AppendWithFormatArgs(formatString, args);
	va_end(args);
	return ret;
}

IntVector2 to_int_vec2(const String& source)
{
	return to_int_vec2(source.c_str());
}

IntVector2 to_int_vec2(const char* source)
{
	IntVector2 ret(IntVector2::ZERO);

	unsigned elements = CountElements(source, ' ');
	if (elements < 2)
		return ret;

	char* ptr = (char*)source;
	ret.x_ = strtol(ptr, &ptr, 10);
	ret.y_ = strtol(ptr, &ptr, 10);

	return ret;
}

const vector<String>::type& ParseArguments(int argc, char** argv)
{
	String cmdLine;
	char buffer[512] = { 0 };

	for (int i = 0; i < argc; ++i)
	{
		sprintf(buffer, "\"%s\" ", (const char*)argv[i]);
		cmdLine += buffer;
	}
		
	return ParseArguments(cmdLine);
}

const vector<String>::type& ParseArguments(const String& cmdLine, bool skipFirstArgument)
{
	arguments_.clear();

	unsigned cmdStart = 0, cmdEnd = 0;
	bool inCmd = false;
	bool inQuote = false;

	for (unsigned i = 0; i < cmdLine.size(); ++i)
	{
		if (cmdLine[i] == '\"')
			inQuote = !inQuote;
		if (cmdLine[i] == ' ' && !inQuote)
		{
			if (inCmd)
			{
				inCmd = false;
				cmdEnd = i;
				// Do not store the first argument (executable name)
				if (!skipFirstArgument)
					arguments_.push_back(cmdLine.substr(cmdStart, cmdEnd - cmdStart));
				skipFirstArgument = false;
			}
		}
		else
		{
			if (!inCmd)
			{
				inCmd = true;
				cmdStart = i;
			}
		}
	}
	if (inCmd)
	{
		cmdEnd = cmdLine.size();
		if (!skipFirstArgument)
			arguments_.push_back(cmdLine.substr(cmdStart, cmdEnd - cmdStart));
	}

	// Strip double quotes from the arguments
	for (unsigned i = 0; i < arguments_.size(); ++i)
	{
		replace_str(arguments_[i], "\"", "");
	}
		
	return arguments_;
}

void ErrorExit(const String& message, int exitCode)
{
	if (!message.empty())
	{
		PrintLine(message, true);
	}
	
	exit(exitCode);
}

void PrintLine(const String& str, bool error)
{
	fprintf(error ? stderr : stdout, "%s\n", str.c_str());
}

}