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
// File: enn_d3d11_render_window.cpp
//

#include "enn_d3d11_render_window.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

D3D11RenderWindow* g_render_wnd_ = 0;

D3D11RenderWindow::D3D11RenderWindow(const RenderSetting& setting)
									 : render_setting_(&setting)
									 , hwnd_((HWND)setting.window_handle_)
									 , swap_chain_(0)
									 , color_buffer_(0)
									 , depth_stencil_buffer_(0)
									 , color_tex_obj_(0)
									 , depth_tex_obj_(0)
									 , color_resource_proxy_(0)
									 , depth_stencil_resource_proxy_(0)
{
	g_render_wnd_ = this;

	full_screen_ = render_setting_->full_screen_;
	vsync_enable_ = render_setting_->vsync_enable_;
	width_ = render_setting_->width_;
	height_ = render_setting_->height_;
}

D3D11RenderWindow::~D3D11RenderWindow()
{
	destroy();
}

void D3D11RenderWindow::init()
{
	HRESULT result;

	// gamma
	back_buffer_format_ = D3D11RenderDevice::MakeDXGIFormatSRGB(DXGI_FORMAT_B8G8R8A8_UNORM);
	depth_stencil_format_ = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ENN_CLEAR_BLOCK_ZERO(sc_desc_);

	// create swap chain
	sc_desc_.BufferCount = 2;
	sc_desc_.BufferDesc.Width = this->getWidth();
	sc_desc_.BufferDesc.Height = this->getHeight();
	sc_desc_.BufferDesc.Format = back_buffer_format_;
	sc_desc_.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sc_desc_.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (!render_setting_->vsync_enable_)
	{
		sc_desc_.BufferDesc.RefreshRate.Numerator = 0;
		sc_desc_.BufferDesc.RefreshRate.Denominator = 1;
	}
	else
	{
		sc_desc_.BufferDesc.RefreshRate.Numerator = g_render_device_->numerator_;
		sc_desc_.BufferDesc.RefreshRate.Denominator = g_render_device_->denominator_;
	}


	sc_desc_.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sc_desc_.OutputWindow = hwnd_;
	sc_desc_.SampleDesc.Count = std::min(static_cast<uint32>(D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT), render_setting_->sample_count_);
	sc_desc_.SampleDesc.Quality = render_setting_->sample_quality_;
	sc_desc_.Windowed = !this->isFullScreen();
	sc_desc_.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sc_desc_.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ID3D11Device* d3d_device = g_render_device_->getDevice();
	IDXGIFactory* gi_factory = g_render_device_->getGIFactory();

	result = gi_factory->CreateSwapChain(d3d_device, &sc_desc_, &swap_chain_);
	ENN_SUCCEED(result);

	updateSurfaces();
}

void D3D11RenderWindow::destroy()
{
	ENN_SAFE_DELETE(color_render_view_);
	ENN_SAFE_DELETE(depth_render_view_);
	ENN_SAFE_DELETE(color_buffer_);
	ENN_SAFE_DELETE(depth_stencil_buffer_);

	ENN_SAFE_RELEASE2(swap_chain_);
}

void D3D11RenderWindow::present()
{
	if (vsync_enable_)
	{
		swap_chain_->Present(1, 0);
	}
	else
	{
		swap_chain_->Present(0, 0);
	}
	
}

void D3D11RenderWindow::resize(uint32 width, uint32 height)
{
	if (width_ == width && height_ == height)
	{
		return;
	}

	HRESULT result;

	width_ = width;
	height_ = height;

	ID3D11DeviceContext* d3d_imm_ctx = g_render_device_->getDeviceContext();
	d3d_imm_ctx->OMSetRenderTargets(0, 0, 0);

	for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		ENN_SAFE_DELETE(render_views_[i]);
	}

	ENN_SAFE_DELETE(depth_stencil_view_);

	ENN_SAFE_DELETE(color_buffer_);
	ENN_SAFE_DELETE(depth_stencil_buffer_);

	uint32 flags = 0;
	if (isFullScreen())
	{
		flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	}

	sc_desc_.BufferDesc.Width = width_;
	sc_desc_.BufferDesc.Height = height_;

	swap_chain_->ResizeBuffers(2, width_, height_, back_buffer_format_, flags);

	updateSurfaces();
}

bool D3D11RenderWindow::isFullScreen() const
{
	return full_screen_;
}

uint32 D3D11RenderWindow::getWidth() const
{
	return width_;
}

uint32 D3D11RenderWindow::getHeight() const
{
	return height_;
}

void D3D11RenderWindow::updateSurfaces()
{
	HRESULT result;

	ID3D11Device* d3d_device = g_render_device_->getDevice();

	ID3D11Texture2D* color_buffer = 0;
	result = swap_chain_->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&color_buffer);
	ENN_ASSERT(SUCCEEDED(result));

	D3D11_TEXTURE2D_DESC bb_desc;
	color_buffer->GetDesc(&bb_desc);
	D3D11_RENDER_TARGET_VIEW_DESC rtv_desc;

	rtv_desc.Format = bb_desc.Format;
	if (bb_desc.SampleDesc.Count > 1)
	{
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		rtv_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtv_desc.Texture2D.MipSlice = 0;
	}

	// create render target view
	ID3D11RenderTargetView* render_target_view = 0;
	result = d3d_device->CreateRenderTargetView(color_buffer, &rtv_desc, &render_target_view);
	ENN_ASSERT(SUCCEEDED(result));

	// create depth stencil texture
	D3D11_TEXTURE2D_DESC ds_desc;

	ENN_CLEAR_BLOCK_ZERO(ds_desc);
	ds_desc.Width = this->getWidth();
	ds_desc.Height = this->getHeight();
	ds_desc.MipLevels = 1;
	ds_desc.ArraySize = 1;
	ds_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	ds_desc.SampleDesc.Count = bb_desc.SampleDesc.Count;
	ds_desc.SampleDesc.Quality = bb_desc.SampleDesc.Quality;
	ds_desc.Usage = D3D11_USAGE_DEFAULT;
	ds_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	ds_desc.CPUAccessFlags = 0;
	ds_desc.MiscFlags = 0;

	ID3D11Texture2D* depth_stencil_buffer = 0;
	result = d3d_device->CreateTexture2D(&ds_desc, 0, &depth_stencil_buffer);
	ENN_ASSERT(SUCCEEDED(result));

	// Create the depth stencil view, for gbuffer.
	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	ENN_CLEAR_BLOCK_ZERO(dsv_desc);

	dsv_desc.Format = depth_stencil_format_;
	dsv_desc.Flags = 0;

	if (bb_desc.SampleDesc.Count > 1)
	{
		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsv_desc.Texture2D.MipSlice = 0;
	}

	ID3D11DepthStencilView* depth_stencil_view = 0;
	result = d3d_device->CreateDepthStencilView(depth_stencil_buffer, &dsv_desc, &depth_stencil_view);
	ENN_ASSERT(SUCCEEDED(result));

	// create depth shader resource view, for light buffer.
	D3D11_SHADER_RESOURCE_VIEW_DESC depth_srv_desc;
	ENN_CLEAR_BLOCK_ZERO(depth_srv_desc);
	depth_srv_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	if (bb_desc.SampleDesc.Count > 1)
	{
		depth_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	}
	else
	{
		depth_srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		depth_srv_desc.Texture2D.MipLevels = 1;
	}

	ID3D11ShaderResourceView* depth_res_view;
	result = d3d_device->CreateShaderResourceView(depth_stencil_buffer, &depth_srv_desc, &depth_res_view);
	ENN_ASSERT(SUCCEEDED(result));

	color_buffer_ = ENN_NEW D3D11Texture2D(color_buffer);
	depth_stencil_buffer_ = ENN_NEW D3D11Texture2D(depth_stencil_buffer);
	color_render_view_ = ENN_NEW D3D11RenderTargetView(render_target_view);
	depth_stencil_view_ = ENN_NEW D3D11DepthStencilView(depth_stencil_view);
	depth_resource_view_ = ENN_NEW D3D11ShaderResourceView(depth_res_view);

	attach(0, color_render_view_);
	attachDepthStencil(depth_stencil_view_);
}

}