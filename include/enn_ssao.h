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
// Time: 2014/10/15
// File: enn_ssao.h
//

#pragma once

#include "enn_pass_base.h"
#include "enn_d3d11_texture_obj.h"
#include "enn_effect_template.h"

namespace enn
{

class SSAO : public RenderPassBase
{
public:
	SSAO();
	virtual ~SSAO();

	virtual void prepare();
	void render();

	void setResSize(ResolutionSize res);

protected:
	void checkRTT();

	void beginRender();
	void endRender();

	void prepareMesh();

	void generateAO();
	void blurH();
	void blurV();

	void renderDebug();

protected:
	D3D11RTT2DHelper*		rtt_;
	D3D11RTT2DHelper*		rtt_h_blur_;
	D3D11RTT2DHelper*		rtt_v_blur_;
	EffectContext*			ao_eff_;

	MeshPtr					screen_mesh_;
	ResolutionSize			res_size_;
};

}