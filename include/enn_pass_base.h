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
// Time: 2014/02/21
// File: enn_pass_base.h
//

#pragma once

#include "enn_node_factory.h"

namespace enn
{
	
typedef vector<RenderableNode*>::type		RenderableNodeList;
typedef vector<LightNode*>::type			LightNodeList;
typedef vector<EnvProbeNode*>::type			EnvProbeNodeList;
typedef Array<WaterNode*>					WaterNodeList;

enum ResolutionSize
{
	RESSIZE_FULL,
	RESSIZE_THREE_QUARTER,
	RESSIZE_HALF
};

class RenderPassBase : public AllocatedObject
{
public:
	virtual ~RenderPassBase() {}
	virtual void prepare() {}
	virtual void render() {};
};

inline IntVector2 getResolutionSize(int w, int h, ResolutionSize res)
{
	switch (res)
	{
	case enn::RESSIZE_FULL:
		return IntVector2(w, h);
	case enn::RESSIZE_THREE_QUARTER:
		return IntVector2(w * 0.75f, h * 0.75f);
	case enn::RESSIZE_HALF:
		return IntVector2(w * 0.5f, h * 0.5f);
	default:
		ENN_ASSERT(0);
		return IntVector2(w, h);
	}
}

}