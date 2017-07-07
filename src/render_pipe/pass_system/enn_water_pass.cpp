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
// File: enn_water_pass.cpp
//

#include "enn_water_pass.h"
#include "enn_render_pipe.h"

namespace enn
{

WaterPass::WaterPass()
{
	back_rtt_ = 0;
	depth_rtt_ = 0;
}

WaterPass::~WaterPass()
{
	ENN_SAFE_DELETE(back_rtt_);
}

void WaterPass::render()
{
	checkRTT();
	copyBackRTT();

	const WaterNodeList& water_nodes = g_render_pipe_->getWaterNodeList();

	ENN_FOR_EACH_CONST(WaterNodeList, water_nodes, it)
	{
		WaterObject* water_obj = (*it)->getWaterObject();
		water_obj->render();
	}
}

void WaterPass::checkRTT()
{
	const D3D11Viewport& vp = g_render_pipe_->getMainVP();

	if (back_rtt_)
	{
		uint32 w, h;
		back_rtt_->getDimension(w, h);

		if (uint32(vp.getWidth() + 0.5f) != w || uint32(vp.getHeight() + 0.5) != h)
		{
			ENN_SAFE_DELETE(back_rtt_);
			ENN_SAFE_DELETE(depth_rtt_);
		}
	}

	if (!back_rtt_)
	{
		back_rtt_ = ENN_NEW D3D11RTT2DHelper();

		const D3D11Viewport& vp = g_render_pipe_->getMainVP();

		back_rtt_->setDimension(static_cast<uint32>(vp.getWidth() + 0.5f), static_cast<uint32>(vp.getHeight() + 0.5f));
		back_rtt_->createColorBuffer(0, EF_ARGB8);

		depth_rtt_ = ENN_NEW D3D11RTT2DHelper();
		depth_rtt_->setDimension(static_cast<uint32>(vp.getWidth() + 0.5f), static_cast<uint32>(vp.getHeight() + 0.5f));
		depth_rtt_->createDepthBuffer(EF_D24S8);
	}
}

void WaterPass::copyBackRTT()
{
	D3D11RTT2DHelper* src_back_rtt = g_render_pipe_->getPostRTT(RenderPipe::SRC_POST);

	back_rtt_->getRTT()->beginRender();

	g_render_device_->getDeviceContext()->CopyResource(back_rtt_->getColorBuffer(0)->getResourceProxy()->resource_->getResource(),
		src_back_rtt->getColorBuffer(0)->getResourceProxy()->resource_->getResource());

	// copy depth
	g_render_device_->getDeviceContext()->CopyResource(depth_rtt_->getDepthBuffer()->getResourceProxy()->resource_->getResource(),
		g_render_wnd_->getDepthStencilBuffer()->getResource());

	back_rtt_->getRTT()->endRender();
}

}