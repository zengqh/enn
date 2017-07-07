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
// Time: 2014/03/10
// File: enn_d3d11_resource_proxy.cpp
//

#include "enn_d3d11_resource_proxy.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

D3D11ResourceProxy::D3D11ResourceProxy()
: resource_(0)
, srv_view_(0)
, rtv_view_(0)
, dsv_view_(0)
, uav_view_(0)
, buffer_desc_(0)
, tex_1d_desc_(0)
, tex_2d_desc_(0)
, tex_3d_desc_(0)
, dsv_desc_(0)
, srv_desc_(0)
, uav_desc_(0)
, rtv_desc_(0)
{
	
}

// buffer
D3D11ResourceProxy::D3D11ResourceProxy(D3D11Resource* res, const D3D11BufferDesc& desc, 
	D3D11ShaderResourceViewDesc* srv_desc,
	D3D11RenderTargetViewDesc* rtv_desc,
	D3D11UnorderedAccessViewDesc* uav_desc)
	: resource_(res)
	, srv_view_(0)
	, rtv_view_(0)
	, dsv_view_(0)
	, uav_view_(0)
	, buffer_desc_(0)
	, tex_1d_desc_(0)
	, tex_2d_desc_(0)
	, tex_3d_desc_(0)
	, dsv_desc_(0)
	, srv_desc_(0)
	, uav_desc_(0)
	, rtv_desc_(0)
{
	commonConstructor(desc.buffer_desc_.BindFlags, srv_desc, rtv_desc, uav_desc);

	buffer_desc_ = ENN_NEW D3D11BufferDesc();
	*buffer_desc_ = desc;
}

// texture 1d
D3D11ResourceProxy::D3D11ResourceProxy(D3D11Resource* res, const D3D11Texture1DDesc& desc, 
	D3D11ShaderResourceViewDesc* srv_desc,
	D3D11RenderTargetViewDesc* rtv_desc,
	D3D11UnorderedAccessViewDesc* uav_desc)
	: resource_(res)
	, srv_view_(0)
	, rtv_view_(0)
	, dsv_view_(0)
	, uav_view_(0)
	, buffer_desc_(0)
	, tex_1d_desc_(0)
	, tex_2d_desc_(0)
	, tex_3d_desc_(0)
	, dsv_desc_(0)
	, srv_desc_(0)
	, uav_desc_(0)
	, rtv_desc_(0)
{
	ENN_ASSERT(0);
}

// texture 2d
D3D11ResourceProxy::D3D11ResourceProxy(D3D11Resource* res, const D3D11Texture2DDesc& desc, 
	D3D11ShaderResourceViewDesc* srv_desc,
	D3D11RenderTargetViewDesc* rtv_desc,
	D3D11UnorderedAccessViewDesc* uav_desc,
	D3D11DepthStencilViewDesc* dsv_desc)
	: resource_(res)
	, srv_view_(0)
	, rtv_view_(0)
	, dsv_view_(0)
	, uav_view_(0)
	, buffer_desc_(0)
	, tex_1d_desc_(0)
	, tex_2d_desc_(0)
	, tex_3d_desc_(0)
	, dsv_desc_(0)
	, srv_desc_(0)
	, uav_desc_(0)
	, rtv_desc_(0)
{
	const D3D11_TEXTURE2D_DESC& tex2d_desc = desc.getRaw();
	commonConstructor(tex2d_desc.BindFlags, srv_desc, rtv_desc, uav_desc, dsv_desc);

	tex_2d_desc_ = ENN_NEW D3D11Texture2DDesc();
	*tex_2d_desc_ = desc;
}

D3D11ResourceProxy::D3D11ResourceProxy(D3D11Resource* res, D3D11ShaderResourceView* view,
	const D3D11Texture2DDesc& desc,
	D3D11ShaderResourceViewDesc* srv_desc,
	D3D11RenderTargetViewDesc* rtv_desc,
	D3D11UnorderedAccessViewDesc* uav_desc,
	D3D11DepthStencilViewDesc* dsv_desc)
	: resource_(res)
	, srv_view_(0)
	, rtv_view_(0)
	, dsv_view_(0)
	, uav_view_(0)
	, buffer_desc_(0)
	, tex_1d_desc_(0)
	, tex_2d_desc_(0)
	, tex_3d_desc_(0)
	, dsv_desc_(0)
	, srv_desc_(0)
	, uav_desc_(0)
	, rtv_desc_(0)
{
	srv_view_ = view;

	const D3D11_TEXTURE2D_DESC& tex2d_desc = desc.getRaw();
	commonConstructor(tex2d_desc.BindFlags, srv_desc, rtv_desc, uav_desc, dsv_desc);

	tex_2d_desc_ = ENN_NEW D3D11Texture2DDesc();
	*tex_2d_desc_ = desc;
}

// texture 3d
D3D11ResourceProxy::D3D11ResourceProxy(D3D11Resource* res, const D3D11Texture3DDesc& desc, 
	D3D11ShaderResourceViewDesc* srv_desc,
	D3D11RenderTargetViewDesc* rtv_desc,
	D3D11UnorderedAccessViewDesc* uav_desc)
	: resource_(res)
	, srv_view_(0)
	, rtv_view_(0)
	, dsv_view_(0)
	, uav_view_(0)
	, buffer_desc_(0)
	, tex_1d_desc_(0)
	, tex_2d_desc_(0)
	, tex_3d_desc_(0)
	, dsv_desc_(0)
	, srv_desc_(0)
	, uav_desc_(0)
	, rtv_desc_(0)
{
	ENN_ASSERT(0);
}

D3D11ResourceProxy::~D3D11ResourceProxy()
{
	ENN_SAFE_DELETE(buffer_desc_);
	ENN_SAFE_DELETE(tex_1d_desc_);
	ENN_SAFE_DELETE(tex_2d_desc_);
	ENN_SAFE_DELETE(tex_3d_desc_);
	ENN_SAFE_DELETE(dsv_desc_);
	ENN_SAFE_DELETE(srv_desc_);
	ENN_SAFE_DELETE(uav_desc_);
	ENN_SAFE_DELETE(rtv_desc_);

	ENN_SAFE_DELETE(srv_view_);
	ENN_SAFE_DELETE(rtv_view_);
	ENN_SAFE_DELETE(dsv_view_);
	ENN_SAFE_DELETE(uav_view_);

	ENN_SAFE_DELETE(resource_);
}

void D3D11ResourceProxy::commonConstructor(uint32 bind_flags, 
					   D3D11ShaderResourceViewDesc* srv_desc,
					   D3D11RenderTargetViewDesc* rtv_desc,
					   D3D11UnorderedAccessViewDesc* uav_desc,
					   D3D11DepthStencilViewDesc* dsv_desc)
{
	if (srv_desc)
	{
		srv_desc_ = ENN_NEW D3D11ShaderResourceViewDesc();
		*srv_desc_ = *srv_desc;
	}

	if (rtv_desc)
	{
		rtv_desc_ = ENN_NEW D3D11RenderTargetViewDesc();
		*rtv_desc_ = *rtv_desc;
	}

	if (uav_desc)
	{
		uav_desc_ = ENN_NEW D3D11UnorderedAccessViewDesc();
		*uav_desc_ = *uav_desc;
	}

	if (dsv_desc)
	{
		dsv_desc_ = ENN_NEW D3D11DepthStencilViewDesc();
		*dsv_desc_ = *dsv_desc;
	}

	//////////////////////////////////////////////////////////////////////////
	// Depending on the bind flags used to create the resource, we create a set
	// of default views to be used.
	if (!srv_view_ && (bind_flags & D3D11_BIND_SHADER_RESOURCE))
	{
		srv_view_ = g_render_device_->createShaderResourceView(resource_, srv_desc_);
	}

	if (!rtv_view_ && (bind_flags & D3D11_BIND_RENDER_TARGET))
	{
		rtv_view_ = g_render_device_->createRenderTargetView(resource_, rtv_desc_);
	}

	if (!dsv_view_ && (bind_flags & D3D11_BIND_DEPTH_STENCIL))
	{
		dsv_view_ = g_render_device_->createDepthStencilView(resource_, dsv_desc_);
	}

	if (!uav_view_ && (bind_flags & D3D11_BIND_UNORDERED_ACCESS))
	{
		uav_view_ = g_render_device_->createUnorderedAccessView(resource_, uav_desc);
	}
}

}