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
// Time: 2014/01/23
// File: enn_d3d11_frame_buffer.cpp
//

#include "enn_d3d11_render_target.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

D3D11RenderTarget::D3D11RenderTarget()
{
	for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		render_views_[i] = 0;
	}

	depth_stencil_view_ = 0;
}

D3D11RenderTarget::~D3D11RenderTarget()
{

}

void D3D11RenderTarget::attach(uint32 i, D3D11RenderTargetView* color)
{
	render_views_[i] = color;
}

void D3D11RenderTarget::attachDepthStencil(D3D11DepthStencilView* depthStencil)
{
	depth_stencil_view_ = depthStencil;
}

D3D11RenderTargetView* D3D11RenderTarget::getRTView(uint32 i) const
{
	return render_views_[i];
}

D3D11DepthStencilView* D3D11RenderTarget::getDSView() const
{
	return depth_stencil_view_;
}

void D3D11RenderTarget::clear(const Color& clr, float depth, int stencil)
{
	for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		if (render_views_[i])
		{
			render_views_[i]->clear(clr);
		}
	}

	if (depth_stencil_view_)
	{
		depth_stencil_view_->clear(depth, stencil);
	}
}

void D3D11RenderTarget::clearColor(const Color& clr)
{
	for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		if (render_views_[i])
		{
			render_views_[i]->clear(clr);
		}
	}
}

void D3D11RenderTarget::clearDepthStencil(float depth, int stencil)
{
	if (depth_stencil_view_)
	{
		depth_stencil_view_->clear(depth, stencil);
	}
}

void D3D11RenderTarget::beginRender()
{
	g_render_device_->push_active_render_target();
	g_render_device_->setRenderTarget(this);
}

void D3D11RenderTarget::endRender()
{
	g_render_device_->pop_render_target();
}

}