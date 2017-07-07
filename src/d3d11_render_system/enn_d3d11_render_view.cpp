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
// File: enn_d3d11_render_view.cpp
//

#include "enn_d3d11_render_view.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
D3D11ShaderResourceViewDesc::D3D11ShaderResourceViewDesc()
{
	setDefault();
}

D3D11ShaderResourceViewDesc::~D3D11ShaderResourceViewDesc()
{
	
}

void D3D11ShaderResourceViewDesc::setDefault()
{
	raw_.Format = DXGI_FORMAT_UNKNOWN;
	raw_.ViewDimension = D3D11_SRV_DIMENSION_UNKNOWN;
}

void D3D11ShaderResourceViewDesc::setBuffer(D3D11_BUFFER_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	raw_.Buffer = state;
}

void D3D11ShaderResourceViewDesc::setBufferEx(D3D11_BUFFEREX_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	raw_.BufferEx = state;
}

void D3D11ShaderResourceViewDesc::setTexture1D(D3D11_TEX1D_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	raw_.Texture1D = state;
}

void D3D11ShaderResourceViewDesc::setTexture1DArray(D3D11_TEX1D_ARRAY_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
	raw_.Texture1DArray = state;
}

void D3D11ShaderResourceViewDesc::setTexture2D(D3D11_TEX2D_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	raw_.Texture2D = state;
}

void D3D11ShaderResourceViewDesc::setTexture2DArray(D3D11_TEX2D_ARRAY_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	raw_.Texture2DArray = state;
}

void D3D11ShaderResourceViewDesc::setTexture2DMS(D3D11_TEX2DMS_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	raw_.Texture2DMS = state;
}

void D3D11ShaderResourceViewDesc::setTexture2DMSArray(D3D11_TEX2DMS_ARRAY_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
	raw_.Texture2DMSArray = state;
}

void D3D11ShaderResourceViewDesc::setTexture3D(D3D11_TEX3D_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	raw_.Texture3D = state;
}

void D3D11ShaderResourceViewDesc::setTextureCube(D3D11_TEXCUBE_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	raw_.TextureCube = state;
}

void D3D11ShaderResourceViewDesc::setTextureCubeArray(D3D11_TEXCUBE_ARRAY_SRV state)
{
	raw_.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
	raw_.TextureCubeArray = state;
}

DXGI_FORMAT D3D11ShaderResourceViewDesc::getSRVFormat(ElementFormat format)
{
	switch (format)
	{
	case EF_D16:
		return DXGI_FORMAT_R16_UNORM;
		break;

	case EF_D24S8:
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		break;

	case EF_D32F:
		return DXGI_FORMAT_R32_FLOAT;
		break;

	default:
		return D3D11RenderDevice::MappingFormat(format);
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
D3D11DepthStencilViewDesc::D3D11DepthStencilViewDesc()
{
	setDefault();
}

D3D11DepthStencilViewDesc::~D3D11DepthStencilViewDesc()
{

}

void D3D11DepthStencilViewDesc::setDefault()
{
	raw_.Flags = 0;
	raw_.Format = DXGI_FORMAT_UNKNOWN;
	raw_.ViewDimension = D3D11_DSV_DIMENSION_UNKNOWN;
}

void D3D11DepthStencilViewDesc::setTexture2D(D3D11_TEX2D_DSV state)
{
	raw_.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	raw_.Texture2D = state;
}

DXGI_FORMAT D3D11DepthStencilViewDesc::getDSVFormat(ElementFormat format)
{
	return D3D11RenderDevice::MappingFormat(format);
}

//////////////////////////////////////////////////////////////////////////
D3D11UnorderedAccessViewDesc::D3D11UnorderedAccessViewDesc()
{
	setDefault();
}

D3D11UnorderedAccessViewDesc::~D3D11UnorderedAccessViewDesc()
{

}

void D3D11UnorderedAccessViewDesc::setDefault()
{
	raw_.Format = DXGI_FORMAT_UNKNOWN;
	raw_.ViewDimension = D3D11_UAV_DIMENSION_UNKNOWN;
}

void D3D11UnorderedAccessViewDesc::setBuffer(D3D11_BUFFER_UAV state)
{
	raw_.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	raw_.Buffer = state;
}

void D3D11UnorderedAccessViewDesc::setTexture1D(D3D11_TEX1D_UAV state)
{
	raw_.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
	raw_.Texture1D = state;
}

void D3D11UnorderedAccessViewDesc::setTexture1DArray(D3D11_TEX1D_ARRAY_UAV state)
{
	raw_.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
	raw_.Texture1DArray = state;
}

void D3D11UnorderedAccessViewDesc::setTexture2D(D3D11_TEX2D_UAV state)
{
	raw_.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	raw_.Texture2D = state;
}

void D3D11UnorderedAccessViewDesc::setTexture2DArray(D3D11_TEX2D_ARRAY_UAV state)
{
	raw_.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	raw_.Texture2DArray = state;
}

void D3D11UnorderedAccessViewDesc::setTexture3D(D3D11_TEX3D_UAV state)
{
	raw_.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
	raw_.Texture3D = state;
}

//////////////////////////////////////////////////////////////////////////
D3D11RenderTargetViewDesc::D3D11RenderTargetViewDesc()
{
	setDefault();
}

D3D11RenderTargetViewDesc::~D3D11RenderTargetViewDesc()
{

}

void D3D11RenderTargetViewDesc::setDefault()
{
	raw_.Format = DXGI_FORMAT_UNKNOWN;
	raw_.ViewDimension = D3D11_RTV_DIMENSION_UNKNOWN;
}

void D3D11RenderTargetViewDesc::setBuffer(D3D11_BUFFER_RTV state)
{
	raw_.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
	raw_.Buffer = state;
}

void D3D11RenderTargetViewDesc::setTexture1D(D3D11_TEX1D_RTV state)
{
	raw_.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
	raw_.Texture1D = state;
}

void D3D11RenderTargetViewDesc::setTexture1DArray(D3D11_TEX1D_ARRAY_RTV state)
{
	raw_.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
	raw_.Texture1DArray = state;
}

void D3D11RenderTargetViewDesc::setTexture2D(D3D11_TEX2D_RTV state)
{
	raw_.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	raw_.Texture2D = state;
}

void D3D11RenderTargetViewDesc::setTexture2DArray(D3D11_TEX2D_ARRAY_RTV state)
{
	raw_.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	raw_.Texture2DArray = state;
}

void D3D11RenderTargetViewDesc::setTexture2DMS(D3D11_TEX2DMS_RTV state)
{
	raw_.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
	raw_.Texture2DMS = state;
}

void D3D11RenderTargetViewDesc::setTexture2DMSArray(D3D11_TEX2DMS_ARRAY_RTV state)
{
	raw_.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
	raw_.Texture2DMSArray = state;
}

void D3D11RenderTargetViewDesc::setTexture3D(D3D11_TEX3D_RTV state)
{
	raw_.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
	raw_.Texture3D = state;
}

//////////////////////////////////////////////////////////////////////////
D3D11ShaderResourceView::~D3D11ShaderResourceView()
{
	ENN_SAFE_RELEASE2(raw_);
}

//////////////////////////////////////////////////////////////////////////
D3D11RenderTargetView::~D3D11RenderTargetView()
{
	ENN_SAFE_RELEASE2(raw_);
}

void D3D11RenderTargetView::clear(const Color& clr)
{
	ID3D11DeviceContext* d3d_imm_ctx = g_render_device_->getDeviceContext();

	d3d_imm_ctx->ClearRenderTargetView(raw_, clr.ptr());
}

//////////////////////////////////////////////////////////////////////////
D3D11DepthStencilView::~D3D11DepthStencilView()
{
	ENN_SAFE_RELEASE2(raw_);
}

void D3D11DepthStencilView::clear(float depth, int stencil)
{
	ID3D11DeviceContext* d3d_imm_ctx = g_render_device_->getDeviceContext();

	d3d_imm_ctx->ClearDepthStencilView(raw_, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
		depth, static_cast<uint8>(stencil));

}

void D3D11DepthStencilView::clearDepth(float depth)
{
	ID3D11DeviceContext* d3d_imm_ctx = g_render_device_->getDeviceContext();

	d3d_imm_ctx->ClearDepthStencilView(raw_, D3D11_CLEAR_DEPTH, depth, 0);
}

void D3D11DepthStencilView::clearStencil(int stencil)
{
	ID3D11DeviceContext* d3d_imm_ctx = g_render_device_->getDeviceContext();

	d3d_imm_ctx->ClearDepthStencilView(raw_, D3D11_CLEAR_STENCIL, 1, static_cast<uint8>(stencil));
}

//////////////////////////////////////////////////////////////////////////
D3D11UnorderedAccessView::~D3D11UnorderedAccessView()
{
	ENN_SAFE_RELEASE2(raw_);
}

}
