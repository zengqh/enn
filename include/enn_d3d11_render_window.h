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
// File: enn_d3d11_render_window.h

#pragma once

#include "enn_render_setting.h"
#include "enn_d3d11_render_state.h"
#include "enn_d3d11_render_target.h"
#include "enn_d3d11_render_view.h"
#include "enn_d3d11_texture_obj.h"

namespace enn
{

class D3D11Adapter;
class D3D11RenderWindow : public D3D11RenderTarget
{
public:
	D3D11RenderWindow(const RenderSetting& setting);
	~D3D11RenderWindow();

	void init();
	void destroy();

	void present();

	void resize(uint32 width, uint32 height);

	bool isFullScreen() const;

	uint32 getWidth() const;

	uint32 getHeight() const;

	D3D11Texture2D* getColorBuffer() const { return color_buffer_; }
	D3D11Texture2D* getDepthStencilBuffer() const { return depth_stencil_buffer_; }

	D3D11RenderTargetView* getColorBufferRTV() const { return color_render_view_; }
	D3D11DepthStencilView* getDepthStencilRTV() const { return depth_stencil_view_; }
	D3D11ShaderResourceView* getDepthStencilSRV() const { return depth_resource_view_; }

protected:
	void updateSurfaces();

protected:
	HWND							hwnd_;
	DXGI_SWAP_CHAIN_DESC			sc_desc_;
	IDXGISwapChain*					swap_chain_;

	D3D11Texture2D*					color_buffer_;
	D3D11Texture2D*					depth_stencil_buffer_;

	const RenderSetting*			render_setting_;

	D3D11RenderTargetView*			color_render_view_;
	D3D11DepthStencilView*			depth_render_view_;
	D3D11ShaderResourceView*		depth_resource_view_;

	D3D11TextureObj*				color_tex_obj_;
	D3D11TextureObj*				depth_tex_obj_;

	bool							full_screen_;
	bool							vsync_enable_;
	int								width_;
	int								height_;

	DXGI_FORMAT						back_buffer_format_;
	DXGI_FORMAT						depth_stencil_format_;

	D3D11ResourceProxy*				color_resource_proxy_;
	D3D11ResourceProxy*				depth_stencil_resource_proxy_;
};

extern D3D11RenderWindow* g_render_wnd_;

}