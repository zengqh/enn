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
// Time: 2014/02/19
// File: enn_viewport.h
//

#pragma once

#include "enn_math.h"

namespace enn
{

class Camera;
class Viewport : public AllocatedObject
{
public:
	Viewport()
	{

	}

	~Viewport()
	{

	}

public:
	void setRect(const IntRect& rect)
	{
		m_rect = rect;
	}

	void setRect( int l, int t, int r, int b )
	{
		m_rect.set( l, t, r, b );
	}

	void makeRect( int l, int t, int w, int h )
	{
		m_rect.make( l, t, w, h );
	}

	const IntRect& getRect() const
	{
		return m_rect;
	}

	int getWidth()  const { return m_rect.getWidth(); }
	int getHeight() const { return m_rect.getHeight(); }

public:
	void linkCamera(Camera* cam);

	Camera* getCamera() const;

protected:
	IntRect       		m_rect;

	Camera*				cam_;
};

}