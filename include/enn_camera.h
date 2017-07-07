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
// File: enn_camera.h
//

#pragma once

#include "enn_frustum.h"
#include "enn_viewport.h"

namespace enn
{

class Viewport;
class Camera : public Frustum
{
public:
	Camera() : m_viewport(0) {}

	virtual ~Camera() 
	{
		
	}

public:
	void setViewport( Viewport* vp ) { m_viewport = vp; }
	Viewport* getViewport() const { return m_viewport; }

public:
	Ray getScreenRay(float screenX, float screenY);
	void getScreenRay(float screenX, float screenY, Ray* outRay);

	bool screenToWorldPositionClipGround(float screenX, float screenY, Vector3& out_pos);
	Vector2 worldPositionToScreen(const Vector3& world_pos);

protected:
	Viewport* m_viewport;
};

}