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
// File: enn_memory_buffer.h
//

#pragma once

#include "enn_deserializer.h"
#include "enn_serializer.h"

namespace enn
{

class MemoryBuffer : public AllocatedObject, public Deserializer, public Serializer
{
public:
	MemoryBuffer(void* data, unsigned size, bool own = false);
	virtual ~MemoryBuffer();
	/// Read bytes from the memory area. Return number of bytes actually read.
	virtual unsigned Read(void* dest, unsigned size);
	/// Set position from the beginning of the memory area.
	virtual unsigned Seek(unsigned position);
	/// Write bytes to the memory area.
	virtual unsigned Write(const void* data, unsigned size);

	/// Return the file handle.
	void* GetHandle() const { return buffer_; }
	void setPosition(uint32 pos) 
	{ 
		position_ = pos; 
	}

protected:
	/// Pointer to the memory area.
	unsigned char* buffer_;
	bool own_;
};

}