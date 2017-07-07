//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2014/02/17
// File: enn_deserializer.cpp
//

#include "enn_deserializer.h"

namespace enn
{

Deserializer::Deserializer() :
position_(0),
size_(0)
{

}

Deserializer::Deserializer(unsigned size) :
position_(0),
size_(size)
{
}

Deserializer::~Deserializer()
{
}

String Deserializer::ReadFileID()
{
	String ret;
	ret.resize(4);
	Read(&ret[0], 4);
	return ret;
}

enn::String Deserializer::ReadString()
{
	enn::String ret;

	for (;;)
	{
		char c;
		ReadBin(c);
		if (!c)
			break;
		else
			ret += c;
	}

	return ret;
}

enn::String Deserializer::ReadLine()
{
	enn::String ret;

	while (!IsEof())
	{
		char c;
		ReadBin(c);

		if (c == 10)
			break;
		if (c == 13)
		{
			// Peek next char to see if it's 10, and skip it too
			if (!IsEof())
			{
				char next;
				ReadBin(next);
				if (next != 10)
					Seek(position_ - 1);
			}
			break;
		}

		ret += c;
	}

	return ret;
}

/// Read a len + null-terminated string.
enn::String Deserializer::ReadLenString()
{
	String str;

	int len = 0;
	ReadBin(len);

	char* raw_str = (char*)ENN_MALLOC(len);
	Read(raw_str, len);
	str = raw_str;

	ENN_SAFE_FREE(raw_str);

	return str;
}

/// Read a len + null-terminated string, align 4.
enn::String Deserializer::ReadLenStringAlign4()
{
	String str;

	int len = 0;
	ReadBin(len);

	ENN_ASSERT(len % 4 == 0);

	char* raw_str = (char*)ENN_MALLOC(len);
	Read(raw_str, len);
	str = raw_str;

	ENN_SAFE_FREE(raw_str);

	return str;
}

}