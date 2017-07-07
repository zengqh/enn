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
// Time: 2015/10/30
// File: enn_water_pass.h
//

#pragma once

#include "enn_pass_base.h"
#include "enn_water_block.h"
#include "enn_water_plane.h"

namespace enn
{

class WaterPass : public RenderPassBase
{
public:
	WaterPass();
	~WaterPass();

	virtual void render();
	D3D11ResourceProxy* getBackRTT() const { return back_rtt_->getColorBuffer(0)->getResourceProxy(); }
	D3D11ResourceProxy* getDepthRTT() const { return depth_rtt_->getDepthBuffer()->getResourceProxy(); }
protected:
	void checkRTT();
	void copyBackRTT();

protected:
	D3D11RTT2DHelper*		back_rtt_;
	D3D11RTT2DHelper*		depth_rtt_;
};

}