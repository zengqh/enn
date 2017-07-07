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
// Time: 2014/06/20
// File: enn_scene_object.h

#pragma once

#include "enn_platform_headers.h"
#include "enn_math.h"

namespace enn
{

struct ISceneObjectListener
{
	virtual void onObjectAABBDirty() = 0;
};

//////////////////////////////////////////////////////////////////////////
class SceneObject : public AllocatedObject
{
public:
	SceneObject() : m_listener(0) {}
	virtual ~SceneObject() {}

	void setObjectListener(ISceneObjectListener* listener)
	{
		m_listener = listener;
	}

protected:
	void fireAABBDirty()
	{
		if (m_listener)
		{
			m_listener->onObjectAABBDirty();
		}
	}

protected:
	ISceneObjectListener* m_listener;
};

}