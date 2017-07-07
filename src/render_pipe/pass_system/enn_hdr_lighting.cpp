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
// File: enn_hdr_lighting.cpp
//

#include "enn_hdr_lighting.h"
#include "enn_root.h"
#include "enn_general_render.h"

namespace enn
{

HDRLighting::HDRLighting()
{

}

HDRLighting::~HDRLighting()
{

}

void HDRLighting::prepare()
{

}

void HDRLighting::render()
{

}

void HDRLighting::initRT()
{
	rt_quad_ = ENN_NEW D3D11RTT2DHelper();
	rt_bloom0_ = ENN_NEW D3D11RTT2DHelper();
	rt_bloom1_ = ENN_NEW D3D11RTT2DHelper();
	rt_bloom2_ = ENN_NEW D3D11RTT2DHelper();
	rt_64x64_ = ENN_NEW D3D11RTT2DHelper();
	rt_16x16_ = ENN_NEW D3D11RTT2DHelper();
	rt_4x4_ = ENN_NEW D3D11RTT2DHelper();
	rt_1x1_ = ENN_NEW D3D11RTT2DHelper();

	const D3D11Viewport& vp = g_render_pipe_->getMainVP();

	
	int width = (vp.getWidth() + 0.5f) / 2;
	int height = (vp.getHeight() + 0.5f) / 2;

	rt_quad_->setDimension(width, height);
	rt_quad_->createColorBuffer(0, EF_ABGR16F);

	rt_bloom0_->setDimension(width, height);
	rt_bloom0_->createColorBuffer(0, EF_ARGB8);

	rt_bloom1_->setDimension(width / 2, height / 2);
	rt_bloom1_->createColorBuffer(0, EF_ARGB8);

	rt_bloom2_->setDimension(width / 4, height / 4);
	rt_bloom2_->createColorBuffer(0, EF_ARGB8);

	rt_64x64_->setDimension(64, 64);
	rt_64x64_->createColorBuffer(0, EF_R16F);

	rt_16x16_->setDimension(16, 16);
	rt_16x16_->createColorBuffer(0, EF_R16F);

	rt_4x4_->setDimension(4, 4);
	rt_4x4_->createColorBuffer(0, EF_R16F);

	rt_1x1_->setDimension(1, 1);
	rt_1x1_->createColorBuffer(0, EF_R16F);
}

void HDRLighting::initEffect()
{

}

}