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
// Time: 2014/03/13
// File: enn_d3d11_texture_obj.cpp
//

#include "enn_d3d11_texture_obj.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

DXGI_FORMAT D3D11TextureDesc::getTextureFormat(ElementFormat format)
{
	switch (format)
	{
	case EF_D16:
		return DXGI_FORMAT_R16_TYPELESS;
		break;

	case EF_D24S8:
		return DXGI_FORMAT_R24G8_TYPELESS;
		break;

	case EF_D32F:
		return DXGI_FORMAT_R32_TYPELESS;
		break;

	default:
		return D3D11RenderDevice::MappingFormat(format);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
D3D11Texture1DDesc::D3D11Texture1DDesc()
{
	setDefault();
}

D3D11Texture1DDesc::~D3D11Texture1DDesc()
{

}

void D3D11Texture1DDesc::setDefault()
{
	raw_.Width = 1;
	raw_.MipLevels = 1;
	raw_.ArraySize = 1;
	raw_.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	raw_.Usage = D3D11_USAGE_DEFAULT;
	raw_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	raw_.CPUAccessFlags = 0;
	raw_.MiscFlags = 0;
}

//////////////////////////////////////////////////////////////////////////
D3D11Texture2DDesc::D3D11Texture2DDesc()
{
	setDefault();
}

D3D11Texture2DDesc::~D3D11Texture2DDesc()
{

}

void D3D11Texture2DDesc::setDefault()
{
	raw_.Width = 1;
	raw_.Height = 1;
	raw_.MipLevels = 1;
	raw_.ArraySize = 1;
	raw_.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	raw_.SampleDesc.Count = 1;
	raw_.SampleDesc.Quality = 0;
	raw_.Usage = D3D11_USAGE_DEFAULT;
	raw_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	raw_.CPUAccessFlags = 0;
	raw_.MiscFlags = 0;
}

void D3D11Texture2DDesc::setColorBuffer(uint32 width, uint32 height)
{
	raw_.Width = width;
	raw_.Height = height;
	raw_.MipLevels = 1;
	raw_.ArraySize = 1;
	raw_.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	raw_.SampleDesc.Count = 1;
	raw_.SampleDesc.Quality = 0;
	raw_.Usage = D3D11_USAGE_DEFAULT;
	raw_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	raw_.CPUAccessFlags = 0;
	raw_.MiscFlags = 0;
}

void D3D11Texture2DDesc::setColorBuffer(uint32 width, uint32 height, DXGI_FORMAT format, bool dynamic)
{
	raw_.Width = width;
	raw_.Height = height;
	raw_.MipLevels = 1;
	raw_.ArraySize = 1;
	raw_.Format = format;
	raw_.SampleDesc.Count = 1;
	raw_.SampleDesc.Quality = 0;
	raw_.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	raw_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	raw_.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;
	raw_.MiscFlags = 0;
}

void D3D11Texture2DDesc::setDepthBuffer(uint32 width, uint32 height)
{
	raw_.Width = width;
	raw_.Height = height;
	raw_.MipLevels = 1;
	raw_.ArraySize = 1;
	raw_.Format = DXGI_FORMAT_D32_FLOAT;
	raw_.SampleDesc.Count = 1;
	raw_.SampleDesc.Quality = 0;
	raw_.Usage = D3D11_USAGE_DEFAULT;
	raw_.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	raw_.CPUAccessFlags = 0;
	raw_.MiscFlags = 0;
}

void D3D11Texture2DDesc::setColorRTTBuffer(uint32 width, uint32 height, DXGI_FORMAT format)
{
	raw_.Width = width;
	raw_.Height = height;
	raw_.MipLevels = 1;
	raw_.ArraySize = 1;
	raw_.Format = format;
	raw_.SampleDesc.Count = 1;
	raw_.SampleDesc.Quality = 0;
	raw_.Usage = D3D11_USAGE_DEFAULT;
	raw_.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	raw_.CPUAccessFlags = 0;
	raw_.MiscFlags = 0;
}

void D3D11Texture2DDesc::setRTTDepthBuffer(uint32 width, uint32 height, DXGI_FORMAT format)
{
	raw_.Width = width;
	raw_.Height = height;
	raw_.MipLevels = 1;
	raw_.ArraySize = 1;
	raw_.Format = format;
	raw_.SampleDesc.Count = 1;
	raw_.SampleDesc.Quality = 0;
	raw_.Usage = D3D11_USAGE_DEFAULT;
	raw_.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	raw_.CPUAccessFlags = 0;
	raw_.MiscFlags = 0;
}

void D3D11Texture2DDesc::setColorRTTBuffer(uint32 width, uint32 height, ElementFormat format)
{
	raw_.Width = width;
	raw_.Height = height;
	raw_.MipLevels = 1;
	raw_.ArraySize = 1;
	raw_.Format = getTextureFormat(format);
	raw_.SampleDesc.Count = 1;
	raw_.SampleDesc.Quality = 0;
	raw_.Usage = D3D11_USAGE_DEFAULT;
	raw_.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	raw_.CPUAccessFlags = 0;
	raw_.MiscFlags = 0;
}

void D3D11Texture2DDesc::setRTTDepthBuffer(uint32 width, uint32 height, ElementFormat format)
{
	raw_.Width = width;
	raw_.Height = height;
	raw_.MipLevels = 1;
	raw_.ArraySize = 1;
	raw_.Format = getTextureFormat(format);
	raw_.SampleDesc.Count = 1;
	raw_.SampleDesc.Quality = 0;
	raw_.Usage = D3D11_USAGE_DEFAULT;
	raw_.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	raw_.CPUAccessFlags = 0;
	raw_.MiscFlags = 0;
}

//////////////////////////////////////////////////////////////////////////
D3D11Texture3DDesc::D3D11Texture3DDesc()
{

}


D3D11Texture3DDesc::~D3D11Texture3DDesc()
{

}

void D3D11Texture3DDesc::setDefault()
{
	raw_.Width = 1;
	raw_.Height = 1;
	raw_.Depth = 1;
	raw_.MipLevels = 1;
	raw_.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	raw_.Usage = D3D11_USAGE_DEFAULT;
	raw_.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	raw_.CPUAccessFlags = 0;
	raw_.MiscFlags = 0;
}

//////////////////////////////////////////////////////////////////////////
D3D11Texture1D::D3D11Texture1D(ID3D11Texture1D* raw) : raw_(raw)
{
	
}

D3D11Texture1D::~D3D11Texture1D()
{
	ENN_SAFE_RELEASE2(raw_);
}

D3D11Texture2D::D3D11Texture2D(ID3D11Texture2D* raw) : raw_(raw)
{
	ENN_ASSERT(raw);
	raw_->GetDesc(&desc_);
}

D3D11Texture2D::~D3D11Texture2D()
{
	ENN_SAFE_RELEASE2(raw_);
}

D3D11Texture3D::D3D11Texture3D(ID3D11Texture3D* raw) : raw_(raw)
{
	ENN_ASSERT(raw);
	raw_->GetDesc(&desc_);
}

D3D11Texture3D::~D3D11Texture3D()
{
	ENN_SAFE_RELEASE2(raw_);
}

//////////////////////////////////////////////////////////////////////////
D3D11TextureObj::D3D11TextureObj()
: width_(0), height_(0), tex_proxy_(0)
{
	
}

D3D11TextureObj::~D3D11TextureObj()
{
	ENN_SAFE_DELETE(tex_proxy_);
}

bool D3D11TextureObj::load(const TexObjLoadParams& params)
{
	tex_proxy_ = g_render_device_->loadTexture2D(params.data_, params.data_size_, params.auto_generate_mipmap_, params.use_srgb_);

	width_ = (int)(tex_proxy_->tex_2d_desc_->getRaw().Width);
	height_ = (int)(tex_proxy_->tex_2d_desc_->getRaw().Height);

	return true;
}

bool D3D11TextureObj::loadRaw(const TexObjLoadRawParams& params)
{
	D3D11Texture2DDesc desc;
	desc.setColorBuffer(params.width_, params.height_, params.format_, params.dynamic_);
	
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = params.data_;
	data.SysMemPitch = params.width_ * D3D11RenderDevice::getDXGIFormatSize(params.format_) / 8;
	data.SysMemSlicePitch = data.SysMemPitch * params.height_;

	tex_proxy_ = g_render_device_->createTexture2D(desc, params.data_ ? &data : 0);

	return true;
}

bool D3D11TextureObj::loadFile(const String& name, bool use_srgb, bool auto_generate_mip)
{
	MemoryBuffer* buff = 0;
	if (!g_file_system_->getFileData(name, buff))
	{
		ENN_SAFE_DELETE(buff);
		return false;
	}

	TexObjLoadParams params;
	params.data_ = buff->GetHandle();
	params.data_size_ = buff->GetSize();
	params.auto_generate_mipmap_ = auto_generate_mip;
	params.use_srgb_ = use_srgb;

	return load(params);
}

bool D3D11TextureObj::loadRTTColor(const TexObjRTTRawParams& params)
{
	D3D11Texture2DDesc desc;
	desc.setColorRTTBuffer(params.width_, params.height_, params.format_);

	tex_proxy_ = g_render_device_->createTexture2D(desc, 0);
	ENN_ASSERT(tex_proxy_);

	return true;
}

bool D3D11TextureObj::loadRTTDepth(const TexObjRTTRawParams& params)
{
	D3D11Texture2DDesc desc;
	desc.setRTTDepthBuffer(params.width_, params.height_, params.format_);

	// srv
	D3D11ShaderResourceViewDesc srv_desc;

	D3D11_TEX2D_SRV SRVTex2D;
	SRVTex2D.MipLevels = 1;
	SRVTex2D.MostDetailedMip = 0;

	srv_desc.setTexture2D(SRVTex2D);
	srv_desc.raw_.Format = D3D11ShaderResourceViewDesc::getSRVFormat(params.format_);

	// dsv
	D3D11DepthStencilViewDesc dsv_desc;
	D3D11_TEX2D_DSV DSVTex2D;
	DSVTex2D.MipSlice = 0;

	dsv_desc.setTexture2D(DSVTex2D);
	dsv_desc.getRaw().Format = D3D11DepthStencilViewDesc::getDSVFormat(params.format_);

	tex_proxy_ = g_render_device_->createTexture2D(desc, 0, &srv_desc, 0, 0, &dsv_desc);

	ENN_ASSERT(tex_proxy_);

	return true;
}

//////////////////////////////////////////////////////////////////////////
D3D11RTT2DHelper::D3D11RTT2DHelper()
: w_(0), h_(0), depth_buffer_(0), rtt_(0), depth_buffers_own_(true)
{
	for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		color_buffers_[i] = 0;
		color_buffers_own_[i] = true;
	}
}

D3D11RTT2DHelper::~D3D11RTT2DHelper()
{
	for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		if (color_buffers_own_[i]) ENN_SAFE_DELETE(color_buffers_[i]);
	}

	if (depth_buffers_own_)
	{
		ENN_SAFE_DELETE(depth_buffer_);
	}
}

void D3D11RTT2DHelper::setDimension(uint32 width, uint32 height)
{
	w_ = width; h_ = height;
}

void D3D11RTT2DHelper::getDimension(uint32& widht, uint32& height)
{
	widht = w_;
	height = h_;
}

bool D3D11RTT2DHelper::createColorBuffer(int idx, ElementFormat format)
{
	D3D11TextureObj*& tex = color_buffers_[idx];
	ENN_ASSERT(!tex);

	tex = ENN_NEW D3D11TextureObj();

	TexObjRTTRawParams params;
	params.format_ = format;
	params.width_ = w_;
	params.height_ = h_;

	ENN_ASSERT_IMPL(tex->loadRTTColor(params));

	return true;
}

bool D3D11RTT2DHelper::createDepthBuffer(ElementFormat format)
{
	ENN_ASSERT(!depth_buffer_);

	D3D11TextureObj* tex = ENN_NEW D3D11TextureObj();

	TexObjRTTRawParams params;
	params.format_ = format;
	params.width_ = w_;
	params.height_ = h_;

	ENN_ASSERT_IMPL(tex->loadRTTDepth(params));
	depth_buffer_ = tex;

	return true;
}

D3D11RenderTarget* D3D11RTT2DHelper::getRTT()
{
	if (!rtt_)
	{
		rtt_ = ENN_NEW D3D11RenderTarget();
		
		updateRTT();
	}

	return rtt_;
}

void D3D11RTT2DHelper::updateRTT()
{
	for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		if (D3D11TextureObj* tex = color_buffers_[i])
		{
			D3D11ResourceProxy* res_proxy = tex->getResourceProxy();

			D3D11RenderTargetView* rtt_view = res_proxy->rtv_view_;
			ENN_ASSERT(rtt_view);

			rtt_->attach(i, rtt_view);
		}
	}

	if (depth_buffer_)
	{
		D3D11ResourceProxy* res_proxy = depth_buffer_->getResourceProxy();
		D3D11DepthStencilView* depth_view = res_proxy->dsv_view_;

		ENN_ASSERT(depth_view);

		rtt_->attachDepthStencil(depth_view);
	}
}

void D3D11RTT2DHelper::attachColorBuffer(int idx, D3D11TextureObj* color)
{
	ENN_ASSERT(!color_buffers_[idx]);

	color_buffers_[idx] = color;

	color_buffers_own_[idx] = false;
}

void D3D11RTT2DHelper::attachDepthBuffer(D3D11TextureObj* depth)
{
	ENN_ASSERT(!depth_buffer_);
	depth_buffer_ = depth;
	depth_buffers_own_ = false;
}

void D3D11RTT2DHelper::attachRawDepthBuffer(D3D11DepthStencilView* depth)
{
	D3D11RenderTarget* rtt = getRTT();
	rtt->attachDepthStencil(depth);
}

}