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
// Time: 2014/03/03
// File: enn_d3d11_render_target.h
//

#pragma once

#include "enn_d3d11_render_view.h"

namespace enn
{

class D3D11RenderTarget : public AllocatedObject
{
public:
	D3D11RenderTarget();
	virtual ~D3D11RenderTarget();

public:
	void attach(uint32 i, D3D11RenderTargetView* color);
	void attachDepthStencil(D3D11DepthStencilView* depthStencil);

	D3D11RenderTargetView* getRTView(uint32 i) const;
	D3D11DepthStencilView* getDSView() const;

	void clear(const Color& clr = Color::BLUE, float depth = 1.0f, int stencil = 0);
	void clearColor(const Color& clr = Color::BLUE);
	void clearDepthStencil(float depth = 1.0f, int stencil = 0);

	virtual void beginRender();
	virtual void endRender();

protected:
	D3D11RenderTargetView*		render_views_[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	D3D11DepthStencilView*		depth_stencil_view_;
};

}