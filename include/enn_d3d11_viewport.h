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
// Time: 2014/06/19
// File: enn_d3d11_viewport.h

#pragma once

#include "enn_d3d11_graphics_def.h"

namespace enn
{

class D3D11Viewport : public AllocatedObject
{
public:
	D3D11Viewport();
	D3D11Viewport(const D3D11_VIEWPORT& raw);
	~D3D11Viewport();

	float getWidth() const { return raw_.Width; }
	float getHeight() const { return raw_.Height; }

	void update(const D3D11_VIEWPORT& raw);
	void update(float x, float y, float w, float h, float min_depth = 0.0f, float max_depth = 1.0f);

	bool operator==(const D3D11Viewport& rhs)
	{
		if ((Math::realEqual(raw_.TopLeftX, rhs.raw_.TopLeftX)
			&& Math::realEqual(raw_.TopLeftY, rhs.raw_.TopLeftY)
			&& Math::realEqual(raw_.Width, rhs.raw_.Width)
			&& Math::realEqual(raw_.Height, rhs.raw_.Height)
			&& Math::realEqual(raw_.MinDepth, rhs.raw_.MinDepth)
			&& Math::realEqual(raw_.MaxDepth, rhs.raw_.MaxDepth)))
		{
			return true;
		}

		return false;
	}

public:
	D3D11_VIEWPORT		raw_;
};

}