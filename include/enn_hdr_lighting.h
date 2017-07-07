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
// Time: 2015/03/17
// File: enn_hdr_lighting.h
//

#pragma once

#include "enn_pass_base.h"
#include "enn_d3d11_texture_obj.h"
#include "enn_effect_template.h"

namespace enn
{

class HDRLighting : public RenderPassBase
{
public:
	HDRLighting();
	virtual ~HDRLighting();

	virtual void prepare();
	void render();

protected:
	void initRT();
	void initEffect();

protected:
	D3D11RTT2DHelper*		rt_quad_;
	D3D11RTT2DHelper*		rt_bloom0_;
	D3D11RTT2DHelper*		rt_bloom1_;
	D3D11RTT2DHelper*		rt_bloom2_;
	D3D11RTT2DHelper*		rt_64x64_;
	D3D11RTT2DHelper*		rt_16x16_;
	D3D11RTT2DHelper*		rt_4x4_;
	D3D11RTT2DHelper*		rt_1x1_;


	EffectContext*			down_scale_2x_;
	EffectContext*			down_scale_4x_;
	EffectContext*			lum_initial_;
	EffectContext*			lum_final_;
	EffectContext*			adapted_;
	EffectContext*			bright_;
	EffectContext*			blur_h_;
	EffectContext*			blur_v_;
	EffectContext*			tone_map_;
};

}

