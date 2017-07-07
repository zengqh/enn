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
// Time: 2014/06/11
// File: enn_light_node.h

#pragma once

#include "enn_scene_node.h"
#include "enn_light.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class LightNode : public SceneNode
{
	ENN_DECLARE_RTTI(LightNode);

public:
	LightNode();
	virtual ~LightNode();

	void setLightType(LightType type);
	LightType getLightType() const { return light_->getLightType(); }

	Light* getLight() const { return light_; }

	template <class T> T* getLightAs() const { return static_cast<T*>(light_); }

	bool intersects(const AxisAlignedBox& box) const;
	float squareDistance(const Vector3& pos) const;

protected:
	virtual bool checkUpdateTransform();
	virtual void checkUpdateWorldAABB();

protected:
	Light*  light_;
};

}