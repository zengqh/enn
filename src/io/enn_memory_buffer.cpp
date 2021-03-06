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
// File: enn_memory_buffer.cpp
//

#include "enn_memory_buffer.h"

namespace enn
{

MemoryBuffer::MemoryBuffer(void* data, unsigned size, bool own)
	: Deserializer(size)
	, buffer_(0)
	, own_(own)
{
	if (!data)
	{
		size = 0;
	}

	if (data && !own)
	{
		buffer_ = (uchar*)data;
	}

	if (own)
	{
		buffer_ = (uchar*)data;
	}
}

MemoryBuffer::~MemoryBuffer()
{
	if (own_)
	{
		ENN_SAFE_FREE(buffer_);
	}
}

unsigned MemoryBuffer::Read(void* dest, unsigned size)
{
	if (size + position_ > size_)
		size = size_ - position_;
	if (!size) return 0;

	unsigned char* srcPtr = &buffer_[position_];
	unsigned char* destPtr = (unsigned char*)dest;
	position_ += size;

	unsigned copySize = size;
	memcpy(destPtr, srcPtr, copySize);

	return size;
}

unsigned MemoryBuffer::Seek(unsigned position)
{
	if (position > size_)
		position = size_;

	position_ = position;
	return position_;
}

unsigned MemoryBuffer::Write(const void* data, unsigned size)
{
	if (size + position_ > size_)
		size = size_ - position_;
	if (!size)
		return 0;

	unsigned char* srcPtr = (unsigned char*)data;
	unsigned char* destPtr = &buffer_[position_];
	position_ += size;

	unsigned copySize = size;
	memcpy(destPtr, srcPtr, copySize);

	return size;
}

}