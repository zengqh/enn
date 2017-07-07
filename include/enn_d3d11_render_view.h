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
// File: enn_d3d11_render_view.h

#pragma once

#include "enn_d3d11_graphics_def.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class D3D11ShaderResourceViewDesc : public AllocatedObject
{
public:
	D3D11ShaderResourceViewDesc();
	~D3D11ShaderResourceViewDesc();

	void setDefault();

	void setBuffer(D3D11_BUFFER_SRV state);
	void setBufferEx(D3D11_BUFFEREX_SRV state);

	void setTexture1D(D3D11_TEX1D_SRV state);
	void setTexture1DArray(D3D11_TEX1D_ARRAY_SRV state);

	void setTexture2D(D3D11_TEX2D_SRV state);
	void setTexture2DArray(D3D11_TEX2D_ARRAY_SRV state);

	void setTexture2DMS(D3D11_TEX2DMS_SRV state);
	void setTexture2DMSArray(D3D11_TEX2DMS_ARRAY_SRV state);

	void setTexture3D(D3D11_TEX3D_SRV state);

	void setTextureCube(D3D11_TEXCUBE_SRV state);
	void setTextureCubeArray(D3D11_TEXCUBE_ARRAY_SRV state);

	static DXGI_FORMAT getSRVFormat(ElementFormat format);

public:
	D3D11_SHADER_RESOURCE_VIEW_DESC& getRaw() { return raw_; }
	const D3D11_SHADER_RESOURCE_VIEW_DESC& getRaw() const { return raw_; }

	D3D11_SHADER_RESOURCE_VIEW_DESC		raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11DepthStencilViewDesc : public AllocatedObject
{
public:
	D3D11DepthStencilViewDesc();
	~D3D11DepthStencilViewDesc();

	void setDefault();
	void setTexture2D(D3D11_TEX2D_DSV state);

	D3D11_DEPTH_STENCIL_VIEW_DESC& getRaw() { return raw_; }
	const D3D11_DEPTH_STENCIL_VIEW_DESC& getRaw() const { return raw_; }

	static DXGI_FORMAT getDSVFormat(ElementFormat format);

public:
	D3D11_DEPTH_STENCIL_VIEW_DESC		raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11UnorderedAccessViewDesc : public AllocatedObject
{
public:
	D3D11UnorderedAccessViewDesc();
	~D3D11UnorderedAccessViewDesc();

public:
	void setDefault();

	void setBuffer(D3D11_BUFFER_UAV state);

	void setTexture1D(D3D11_TEX1D_UAV state);
	void setTexture1DArray(D3D11_TEX1D_ARRAY_UAV state);

	void setTexture2D(D3D11_TEX2D_UAV state);
	void setTexture2DArray(D3D11_TEX2D_ARRAY_UAV state);

	void setTexture3D(D3D11_TEX3D_UAV state);

public:
	D3D11_UNORDERED_ACCESS_VIEW_DESC& getRaw() { return raw_; }
	const D3D11_UNORDERED_ACCESS_VIEW_DESC& getRaw() const { return raw_; }

	D3D11_UNORDERED_ACCESS_VIEW_DESC raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11RenderTargetViewDesc : public AllocatedObject
{
public:
	D3D11RenderTargetViewDesc();
	~D3D11RenderTargetViewDesc();

public:
	void setDefault();

	void setBuffer(D3D11_BUFFER_RTV state);
	void setTexture1D(D3D11_TEX1D_RTV state);
	void setTexture1DArray(D3D11_TEX1D_ARRAY_RTV state);
	void setTexture2D(D3D11_TEX2D_RTV state);
	void setTexture2DArray(D3D11_TEX2D_ARRAY_RTV state);
	void setTexture2DMS(D3D11_TEX2DMS_RTV state);
	void setTexture2DMSArray(D3D11_TEX2DMS_ARRAY_RTV state);
	void setTexture3D(D3D11_TEX3D_RTV state);

public:
	D3D11_RENDER_TARGET_VIEW_DESC& getRaw() { return raw_; }
	const D3D11_RENDER_TARGET_VIEW_DESC& getRaw() const { return raw_; }

	D3D11_RENDER_TARGET_VIEW_DESC	raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11ShaderResourceView : public AllocatedObject
{
public:
	D3D11ShaderResourceView(ID3D11ShaderResourceView* raw) : raw_(raw) {}
	~D3D11ShaderResourceView();

	ID3D11ShaderResourceView*	raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11RenderTargetView : public AllocatedObject
{
public:
	D3D11RenderTargetView(ID3D11RenderTargetView* raw) : raw_(raw) {}
	~D3D11RenderTargetView();

	void clear(const Color& clr);

	ID3D11RenderTargetView*		raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11DepthStencilView : public AllocatedObject
{
public:
	D3D11DepthStencilView(ID3D11DepthStencilView* raw) : raw_(raw) {}
	~D3D11DepthStencilView();

	void clear(float depth, int stencil);
	void clearDepth(float depth);
	void clearStencil(int stencil);

	ID3D11DepthStencilView* raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11UnorderedAccessView : public AllocatedObject
{
public:
	D3D11UnorderedAccessView(ID3D11UnorderedAccessView* raw) : raw_(raw) {}
	~D3D11UnorderedAccessView();

	ID3D11UnorderedAccessView* raw_;
};

}