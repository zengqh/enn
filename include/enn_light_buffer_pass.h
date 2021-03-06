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
// Time: 2014/07/16
// File: enn_light_buffer_pass.h

#pragma once

#include "enn_pass_base.h"
#include "enn_light_buffer_effect_template.h"

namespace enn
{

class LightBufferPass : public RenderPassBase
{
public:
	LightBufferPass();
	~LightBufferPass();

public:
	virtual void prepare();
	virtual void render();

	D3D11ResourceProxy* getLitBuffer() const;

protected:
	void checkRTT();

	void beginRender();
	void renderPointLit(PointLight* lit);
	void renderSpotLit(SpotLight* spot);
	void endRender();

	void renderDebug();

protected:
	D3D11RTT2DHelper*		rtt_;

	EffectContext*			point_ec_;
	EffectContext*			spot_ec_;
};

}