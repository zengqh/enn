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
// File: enn_d3d11_render_device.cpp
//

#include "enn_d3d11_render_device.h"
#include "enn_root.h"

namespace enn
{

D3D11RenderDevice* g_render_device_ = 0;

//////////////////////////////////////////////////////////////////////////
D3D11RenderDevice::D3D11RenderDevice()
: gi_factory_(0)
, device_(0)
, device_ctx_(0)
, curr_frame_no_(0)
, num_batches_rendered_(0)
, num_prim_rendered_(0)
, depth_sky_state_(0)
{
	g_render_device_ = this;

	samplers_cache_list_.resize(SHADER_COUNT);
	shader_res_views_cache_list_.resize(SHADER_COUNT);
	constant_buffers_cache_list_.resize(SHADER_COUNT);
}

D3D11RenderDevice::~D3D11RenderDevice()
{

}

IDXGIFactory* D3D11RenderDevice::getGIFactory() const
{
	return gi_factory_;
}

void D3D11RenderDevice::init()
{
	// create gi factory
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&gi_factory_);
	ENN_SUCCEED(hr);

	IDXGIAdapter1* adapter = 0;
	hr = gi_factory_->EnumAdapters1(0, &adapter);
	ENN_SUCCEED(hr);

	IDXGIOutput* adapterOutput = 0;
	hr = adapter->EnumOutputs(0, &adapterOutput);
	ENN_SUCCEED(hr);

	uint32 numModes = 0;
	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, 0);
	ENN_SUCCEED(hr);

	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	ENN_CLEAR_ZERO(displayModeList, sizeof(DXGI_MODE_DESC)* numModes);

	hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numModes, displayModeList);
	ENN_SUCCEED(hr);

	const RootSetting& root_setting = g_root_->getRootSetting();

	int displayWidth = root_setting.render_setting_.width_;
	int displayHeight = root_setting.render_setting_.height_;

	for (int i = 0; i < (int)numModes; ++i)
	{
		if (displayModeList[i].Width == (unsigned int)displayWidth)
		{
			if (displayModeList[i].Height == (unsigned int)displayHeight)
			{
				numerator_ = displayModeList[i].RefreshRate.Numerator;
				denominator_ = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	DXGI_ADAPTER_DESC1 adapter_desc;
	hr = adapter->GetDesc1(&adapter_desc);
	ENN_SUCCEED(hr);

	video_card_memory_ = (int)(adapter_desc.DedicatedVideoMemory / 1024 / 1024);

	size_t stringLength;
	int error = wcstombs_s(&stringLength, video_card_desc_, 128, adapter_desc.Description, 128);
	ENN_ASSERT(!error);

	delete[] displayModeList;
	displayModeList = 0;

	adapterOutput->Release();
	adapterOutput = 0;

	adapter->Release();
	adapter = 0;

	// create device & context
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_11_0
	};


	hr = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, levels, 1,
		D3D11_SDK_VERSION, &device_, 0, &device_ctx_);
	ENN_SUCCEED(hr);

	createCommState();
}

void D3D11RenderDevice::shutdown()
{
	clearRenderStates();

	ENN_SAFE_RELEASE2(gi_factory_);
	ENN_SAFE_RELEASE2(device_ctx_);
	ENN_SAFE_RELEASE2(device_);
}

void D3D11RenderDevice::beginFrame()
{
	curr_frame_no_++;

	num_batches_rendered_ = 0;
	num_prim_rendered_ = 0;
}

void D3D11RenderDevice::endFrame()
{

}

void D3D11RenderDevice::setRenderTarget(D3D11RenderTarget* rt)
{
	if (curr_rtt_ == rt)
	{
		return;
	}

	curr_rtt_ = rt;

	vector<ID3D11RenderTargetView*>::type rt_views;
	ID3D11DepthStencilView* ds_view = 0;

	for (uint32 i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	{
		D3D11RenderTargetView* rt_vew = rt->getRTView(i);

		if (rt_vew)
		{
			ENN_ASSERT(rt_vew->raw_);
			rt_views.push_back(rt_vew->raw_);
		}
	}

	if (rt->getDSView())
	{
		ds_view = rt->getDSView()->raw_;
	}

	D3D11ShaderResourceView view(0);
	for (int i = 0; i < 8; ++i)
	{
		setD3D11ShaderResourceView(PIXEL_SHADER, i, &view);
	}

	if (!rt_views.empty())
	{
		device_ctx_->OMSetRenderTargets(rt_views.size(), &rt_views[0], ds_view);
	}
}

void D3D11RenderDevice::setRasterizerState(D3D11RasterizerStateObject* state)
{
	if (state != rs_state_cache_)
	{
		device_ctx_->RSSetState(state->getRaw());
		rs_state_cache_ = state;
	}
}

void D3D11RenderDevice::setDepthStencilState(D3D11DepthStencilStateObject* state, uint16 stencil_ref)
{
	if (state != ds_state_cache_ || stencil_ref != stencil_ref_cache_)
	{
		device_ctx_->OMSetDepthStencilState(state->getRaw(), stencil_ref);
		ds_state_cache_ = state;
		stencil_ref_cache_ = stencil_ref;
	}
}

void D3D11RenderDevice::setBlendState(D3D11BlendStateObject* state, const Color& blend_factor,
	uint32 sample_mask)
{
	if (state != bs_state_cache_ || blend_factor != blend_factor_cache_
		|| sample_mask != sample_mask_cache_)
	{
		device_ctx_->OMSetBlendState(state->getRaw(), &blend_factor.r, sample_mask);
		bs_state_cache_ = state;
		blend_factor_cache_ = blend_factor;
		sample_mask_cache_ = sample_mask;
	}
}

D3D11ResourceProxy* D3D11RenderDevice::createTexture2D(
									const D3D11Texture2DDesc& desc, 
									D3D11_SUBRESOURCE_DATA* pData,
									D3D11ShaderResourceViewDesc* srv_desc,
									D3D11RenderTargetViewDesc* rtv_desc,
									D3D11UnorderedAccessViewDesc* uav_desc,
									D3D11DepthStencilViewDesc* dsv_desc)
{
	ID3D11Texture2D* raw = 0;
	HRESULT hr = device_->CreateTexture2D(&desc.getRaw(), pData, &raw);

	ENN_ASSERT(SUCCEEDED(hr));

	D3D11Texture2D* texture = ENN_NEW D3D11Texture2D(raw);

	return ENN_NEW D3D11ResourceProxy(texture, desc, srv_desc, rtv_desc, uav_desc, dsv_desc);
}

D3D11ResourceProxy* D3D11RenderDevice::loadTexture2D(void* pData, size_t size, bool auto_gen_mipmap, bool use_srgb)
{
	ID3D11Resource* res = 0;
	ID3D11ShaderResourceView* textureView = 0;

	HRESULT hr = DirectX::CreateWICTextureFromMemoryEx(device_, auto_gen_mipmap ? device_ctx_ : 0, (uint8*)pData, size, 
		0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, use_srgb, &res,
		auto_gen_mipmap ? &textureView : 0);

	if (FAILED(hr))
	{
		D3D11ResourceProxy* resproxy = 0;
		if (resproxy = loadTexture2DTGA(pData, size, auto_gen_mipmap, use_srgb))
		{
			return resproxy;
		}
		else
		{
			return loadTexture2DDDS(pData, size, auto_gen_mipmap, use_srgb);
		}
	}

	ID3D11Texture2D* pTexture = static_cast<ID3D11Texture2D*>(res);
	D3D11Texture2DDesc desc;
	pTexture->GetDesc(&desc.getRaw());

	D3D11Texture2D* tex = ENN_NEW D3D11Texture2D(pTexture);

	if (textureView)
	{
		D3D11ShaderResourceView* shader_view = ENN_NEW D3D11ShaderResourceView(textureView);
		return ENN_NEW D3D11ResourceProxy(tex, shader_view, desc);
	}
	else
	{
		return ENN_NEW D3D11ResourceProxy(tex, desc);
	}
}

D3D11ResourceProxy* D3D11RenderDevice::loadTexture2DDDS(void* pData, size_t size, bool auto_gen_mipmap, bool use_srgb)
{
	ID3D11Resource* res = 0;
	ID3D11ShaderResourceView* textureView = 0;

	HRESULT hr = DirectX::CreateDDSTextureFromMemoryEx(device_, auto_gen_mipmap ? device_ctx_ : 0, (uint8*)pData, size, 
		0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, use_srgb, &res,
		auto_gen_mipmap ? &textureView : 0);

	if (FAILED(hr))
	{
		ENN_ASSERT(0);
		return 0;
	}

	ID3D11Texture2D* pTexture = static_cast<ID3D11Texture2D*>(res);
	D3D11Texture2DDesc desc;
	pTexture->GetDesc(&desc.getRaw());

	D3D11Texture2D* tex = ENN_NEW D3D11Texture2D(pTexture);

	if (textureView)
	{
		D3D11ShaderResourceView* shader_view = ENN_NEW D3D11ShaderResourceView(textureView);
		return ENN_NEW D3D11ResourceProxy(tex, shader_view, desc);
	}
	else
	{
		return ENN_NEW D3D11ResourceProxy(tex, desc);
	}
}

D3D11ResourceProxy* D3D11RenderDevice::loadTexture2DTGA(void* pData, size_t size, bool auto_gen_mipmap, bool use_srgb)
{
	DirectX::ScratchImage* image = new DirectX::ScratchImage();
	HRESULT hr = DirectX::LoadFromTGAMemory(pData, size, 0, *image);

	if (FAILED(hr))
	{
		ENN_SAFE_DELETE(image);
		return 0;
	}

	const DirectX::Image* img0 = image->GetImage(0, 0, 0);
	ENN_ASSERT(img0);

	if (auto_gen_mipmap)
	{
		UINT fmtSupport = 0;
		hr = device_->CheckFormatSupport(img0->format, &fmtSupport);
		if (SUCCEEDED(hr) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
		{
			auto_gen_mipmap = true;
		}
		else
		{
			auto_gen_mipmap = false;
		}
	}
	

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = img0->width;
	desc.Height = img0->height;
	desc.MipLevels = (auto_gen_mipmap) ? 0 : 1;
	desc.ArraySize = 1;
	desc.Format = use_srgb ? MakeDXGIFormatSRGB(img0->format) : img0->format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	if (auto_gen_mipmap)
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	}
	else
	{
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MiscFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = img0->pixels;
	initData.SysMemPitch = static_cast<UINT>(img0->rowPitch);
	initData.SysMemSlicePitch = static_cast<UINT>(0);

	ID3D11Texture2D* tex = 0;
	hr = device_->CreateTexture2D(&desc, (auto_gen_mipmap) ? 0 : &initData, &tex);

	if (FAILED(hr))
	{
		ENN_SAFE_DELETE(image);
		return 0;
	}

	ID3D11ShaderResourceView* textureView = 0;

	if (SUCCEEDED(hr) && tex != 0)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		memset(&SRVDesc, 0, sizeof(SRVDesc));
		SRVDesc.Format = desc.Format;

		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = (auto_gen_mipmap) ? -1 : 1;

		hr = device_->CreateShaderResourceView(tex, &SRVDesc, &textureView);
		if (FAILED(hr))
		{
			tex->Release();
			ENN_SAFE_DELETE(image);
			return 0;
		}

		if (auto_gen_mipmap)
		{
			device_ctx_->UpdateSubresource(tex, 0, 0, img0->pixels, static_cast<UINT>(img0->rowPitch), static_cast<UINT>(img0->rowPitch * img0->height));
			device_ctx_->GenerateMips(textureView);
		}
	}

	ENN_SAFE_DELETE(image);
	D3D11Texture2DDesc desc2d;
	desc2d.raw_ = desc;

	D3D11Texture2D* tex2d = ENN_NEW D3D11Texture2D(tex);
	D3D11ShaderResourceView* shader_view = ENN_NEW D3D11ShaderResourceView(textureView);
	return ENN_NEW D3D11ResourceProxy(tex2d, shader_view, desc2d);
}

D3D11ResourceProxy* D3D11RenderDevice::loadTexture1D(void* pData, size_t size, bool auto_gen_mipmap, bool use_srgb)
{
	return 0;
	/*
	ID3D11Resource* res = 0;
	ID3D11ShaderResourceView* textureView = 0;

	HRESULT hr = DirectX::CreateWICTextureFromMemoryEx(device_, auto_gen_mipmap ? device_ctx_ : 0, (uint8*)pData, size,
		0, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE, 0, 0, use_srgb, &res,
		auto_gen_mipmap ? &textureView : 0);

	if (FAILED(hr))
	{
		
	}

	ID3D11Texture1D* pTexture = static_cast<ID3D11Texture1D*>(res);
	D3D11Texture1DDesc desc;
	pTexture->GetDesc(&desc.getRaw());

	D3D11Texture1D* tex = ENN_NEW D3D11Texture1D(pTexture);

	if (textureView)
	{
		D3D11ShaderResourceView* shader_view = ENN_NEW D3D11ShaderResourceView(textureView);
		return ENN_NEW D3D11ResourceProxy(tex, shader_view, desc);
	}
	else
	{
		return ENN_NEW D3D11ResourceProxy(tex, desc);
	}
	*/
}

D3D11ResourceProxy* D3D11RenderDevice::createConstantBuffer(const D3D11BufferDesc& desc, D3D11_SUBRESOURCE_DATA* pData)
{
	ID3D11Buffer* raw = 0;
	HRESULT hr = device_->CreateBuffer(&desc.buffer_desc_, pData, &raw);

	ENN_SUCCEED(hr);

	D3D11ConstanceBuffer* cb = ENN_NEW D3D11ConstanceBuffer(raw);

	return ENN_NEW D3D11ResourceProxy(cb, desc);
}

D3D11ResourceProxy* D3D11RenderDevice::createConstantBuffer(void* data, size_t size)
{
	D3D11BufferDesc desc;
	desc.setDefaultConstantBuffer(size, true);

	if (data)
	{
		D3D11_SUBRESOURCE_DATA sub_res_data;
		sub_res_data.pSysMem = data;
		sub_res_data.SysMemPitch = 0;
		sub_res_data.SysMemSlicePitch = 0;

		return createConstantBuffer(desc, &sub_res_data);
	}
	else
	{
		return createConstantBuffer(desc, 0);
	}
}

D3D11ResourceProxy* D3D11RenderDevice::createVertexBuffer(const D3D11BufferDesc& desc, D3D11_SUBRESOURCE_DATA* pData)
{
	ID3D11Buffer* raw = 0;
	HRESULT hr = device_->CreateBuffer(&desc.buffer_desc_, pData, &raw);

	ENN_SUCCEED(hr);

	D3D11VertexBuffer* vb = ENN_NEW D3D11VertexBuffer(raw);

	return ENN_NEW D3D11ResourceProxy(vb, desc);
}

D3D11ResourceProxy* D3D11RenderDevice::createIndexBuffer(const D3D11BufferDesc& desc, D3D11_SUBRESOURCE_DATA* pData)
{
	ID3D11Buffer* raw = 0;
	HRESULT hr = device_->CreateBuffer(&desc.buffer_desc_, pData, &raw);

	ENN_SUCCEED(hr);

	D3D11IndexBuffer* ib = ENN_NEW D3D11IndexBuffer(raw);

	return ENN_NEW D3D11ResourceProxy(ib, desc);
}

D3D11ResourceProxy* D3D11RenderDevice::createVertexBuffer(void* data, size_t size, bool dynamic)
{
	D3D11BufferDesc buffer_desc;
	buffer_desc.setDefaultVertexBuffer(size, dynamic);
	D3D11_SUBRESOURCE_DATA res_data;

	res_data.pSysMem = data;
	res_data.SysMemPitch = 0;
	res_data.SysMemSlicePitch = 0;

	return createVertexBuffer(buffer_desc, data ? &res_data : 0);
}

D3D11ResourceProxy* D3D11RenderDevice::createIndexBuffer(void* data, size_t size, bool dynamic)
{
	D3D11BufferDesc ib_desc;

	ib_desc.setDefaultIndexBuffer(size, dynamic);
	D3D11_SUBRESOURCE_DATA sub_data;

	sub_data.pSysMem = data;
	sub_data.SysMemPitch = 0;
	sub_data.SysMemSlicePitch = 0;

	return createIndexBuffer(ib_desc, data ? &sub_data : 0);
}

D3D11BlendStateObject* D3D11RenderDevice::createBlendState(const D3D11BlendStateDesc& desc)
{
	BSPool::iterator it = bs_pool_.find(desc);
	if (it != bs_pool_.end())
	{
		return it->second;
	}

	ID3D11BlendState* blend_state_ptr = 0;

	HRESULT hr = device_->CreateBlendState(&desc.raw_, &blend_state_ptr);
	ENN_SUCCEED(hr);

	D3D11BlendStateObject* state = ENN_NEW D3D11BlendStateObject(blend_state_ptr);

	bs_pool_.insert(BSPool::value_type(desc, state));

	return state;
}

D3D11DepthStencilStateObject* D3D11RenderDevice::createDepthStencilState(const D3D11DepthStencilStateDesc& desc)
{
	DSPool::iterator it = ds_pool_.find(desc);
	if (it != ds_pool_.end())
	{
		return it->second;
	}

	ID3D11DepthStencilState* ds_state_ptr = 0;
	HRESULT hr = device_->CreateDepthStencilState(&desc.raw_, &ds_state_ptr);
	ENN_SUCCEED(hr);

	D3D11DepthStencilStateObject* state = ENN_NEW D3D11DepthStencilStateObject(ds_state_ptr);

	ds_pool_.insert(DSPool::value_type(desc, state));

	return state;
}

D3D11RasterizerStateObject* D3D11RenderDevice::createRasterizerState(const D3D11RasterizerStateDesc& desc)
{
	RSPool::iterator it = rs_pool_.find(desc);
	if (it != rs_pool_.end())
	{
		return it->second;
	}

	ID3D11RasterizerState* rs_state_ptr = 0;
	HRESULT hr = device_->CreateRasterizerState(&desc.raw_, &rs_state_ptr);
	ENN_SUCCEED(hr);

	D3D11RasterizerStateObject* state = ENN_NEW D3D11RasterizerStateObject(rs_state_ptr);

	rs_pool_.insert(RSPool::value_type(desc, state));

	return state;
}

D3D11SamplerStateObject* D3D11RenderDevice::createSamplerState(const D3D11SamplerStateDesc& desc)
{
	SSPool::iterator it = ss_pool_.find(desc);
	if (it != ss_pool_.end())
	{
		return it->second;
	}

	ID3D11SamplerState* ss_state_ptr = 0;
	HRESULT hr = device_->CreateSamplerState(&desc.raw_, &ss_state_ptr);
	ENN_SUCCEED(hr);

	D3D11SamplerStateObject* state = ENN_NEW D3D11SamplerStateObject(ss_state_ptr);

	ss_pool_.insert(SSPool::value_type(desc, state));

	return state;
}

D3D11ShaderResourceView* D3D11RenderDevice::createShaderResourceView(D3D11Resource* res, const D3D11ShaderResourceViewDesc& desc)
{
	ID3D11ShaderResourceView* raw = 0;

	HRESULT hr = device_->CreateShaderResourceView(res->getResource(), &desc.getRaw(), &raw);

	ENN_ASSERT(SUCCEEDED(hr));

	return ENN_NEW D3D11ShaderResourceView(raw);
}

D3D11RenderTargetView* D3D11RenderDevice::createRenderTargetView(D3D11Resource* res, const D3D11RenderTargetViewDesc& desc)
{
	ID3D11RenderTargetView* raw = 0;

	HRESULT hr = device_->CreateRenderTargetView(res->getResource(), &desc.getRaw(), &raw);

	ENN_SUCCEED(hr);

	return ENN_NEW D3D11RenderTargetView(raw);
}

D3D11DepthStencilView* D3D11RenderDevice::createDepthStencilView(D3D11Resource* res, const D3D11DepthStencilViewDesc& desc)
{
	ID3D11DepthStencilView* raw = 0;

	HRESULT hr = device_->CreateDepthStencilView(res->getResource(), &desc.getRaw(), &raw);

	ENN_SUCCEED(hr);

	return ENN_NEW D3D11DepthStencilView(raw);
}

D3D11UnorderedAccessView* D3D11RenderDevice::createUnorderedAccessView(D3D11Resource* res, const D3D11UnorderedAccessViewDesc& desc)
{
	ID3D11UnorderedAccessView* raw = 0;

	HRESULT hr = device_->CreateUnorderedAccessView(res->getResource(), &desc.getRaw(), &raw);

	ENN_SUCCEED(hr);

	return ENN_NEW D3D11UnorderedAccessView(raw);
}


D3D11ShaderResourceView* D3D11RenderDevice::createShaderResourceView(D3D11Resource* res, const D3D11ShaderResourceViewDesc* desc)
{
	ID3D11ShaderResourceView* raw = 0;

	HRESULT hr = device_->CreateShaderResourceView(res->getResource(), desc ? &(desc->getRaw()) : 0, &raw);

	ENN_ASSERT(SUCCEEDED(hr));

	return ENN_NEW D3D11ShaderResourceView(raw);
}

D3D11RenderTargetView* D3D11RenderDevice::createRenderTargetView(D3D11Resource* res, const D3D11RenderTargetViewDesc* desc)
{
	ID3D11RenderTargetView* raw = 0;

	HRESULT hr = device_->CreateRenderTargetView(res->getResource(), desc ? &(desc->getRaw()) : 0, &raw);

	ENN_SUCCEED(hr);

	return ENN_NEW D3D11RenderTargetView(raw);
}

D3D11DepthStencilView* D3D11RenderDevice::createDepthStencilView(D3D11Resource* res, const D3D11DepthStencilViewDesc* desc)
{
	ID3D11DepthStencilView* raw = 0;

	HRESULT hr = device_->CreateDepthStencilView(res->getResource(), desc ? &(desc->getRaw()) : 0, &raw);

	ENN_SUCCEED(hr);

	return ENN_NEW D3D11DepthStencilView(raw);
}

D3D11UnorderedAccessView* D3D11RenderDevice::createUnorderedAccessView(D3D11Resource* res, const D3D11UnorderedAccessViewDesc* desc)
{
	ID3D11UnorderedAccessView* raw = 0;

	HRESULT hr = device_->CreateUnorderedAccessView(res->getResource(), desc ? &(desc->getRaw()) : 0, &raw);

	ENN_SUCCEED(hr);

	return ENN_NEW D3D11UnorderedAccessView(raw);
}

void D3D11RenderDevice::createViewport(D3D11Viewport& vp, float x, float y, float w, float h,
	float min_depth, float max_depth)
{
	D3D11_VIEWPORT d3d_vp;

	d3d_vp.TopLeftX = x;
	d3d_vp.TopLeftY = y;
	d3d_vp.Width = w;
	d3d_vp.Height = h;
	d3d_vp.MinDepth = min_depth;
	d3d_vp.MaxDepth = max_depth;

	vp.update(d3d_vp);
}

D3D11_MAPPED_SUBRESOURCE D3D11RenderDevice::mapResource(D3D11Resource* res, uint32 sub_res, 
	D3D11_MAP actions, uint32 flags)
{
	ENN_ASSERT(res);

	D3D11_MAPPED_SUBRESOURCE data;

	data.pData = 0;
	data.DepthPitch = data.RowPitch = 0;

	ID3D11Resource* pRes = res->getResource();
	ENN_ASSERT(pRes);

	HRESULT hr = device_ctx_->Map(pRes, sub_res, actions, flags, &data);
	ENN_SUCCEED(hr);

	return data;
}

void D3D11RenderDevice::unMapResource(D3D11Resource* res, uint32 sub_res)
{
	ENN_ASSERT(res);

	ID3D11Resource* pRes = res->getResource();

	ENN_ASSERT(pRes);

	device_ctx_->Unmap(pRes, sub_res);
}

D3D11_MAPPED_SUBRESOURCE D3D11RenderDevice::mapResource(D3D11ResourceProxy* res_proxy, uint32 sub_res,
	D3D11_MAP actions, uint32 flags)
{
	return mapResource(res_proxy->resource_, sub_res, actions, flags);
}

void D3D11RenderDevice::unMapResource(D3D11ResourceProxy* res_proxy, uint32 sub_res)
{
	return unMapResource(res_proxy->resource_, sub_res);
}

void* D3D11RenderDevice::lock(D3D11ResourceProxy* res_proxy, D3D11_MAP actions)
{
	D3D11_MAPPED_SUBRESOURCE data = mapResource(res_proxy, 0, actions, 0);

	return data.pData;
}

void D3D11RenderDevice::unLock(D3D11ResourceProxy* res_proxy)
{
	unMapResource(res_proxy, 0);
}

D3D11Shader* D3D11RenderDevice::loadShader(ShaderType type, const String& shader_text, 
										   const String& shader_function, const String& shader_model,
										   const D3D_SHADER_MACRO* pDefines)
{
	HRESULT hr = S_OK;
	ID3DBlob* pCompiledShader = generateShader(type, shader_text, shader_function, shader_model,
		pDefines);

	if (!pCompiledShader)
	{
		ENN_ASSERT(0);
		return 0;
	}

	D3D11Shader* shader_wrapper = 0;

	switch (type)
	{
	case VERTEX_SHADER:
		{
			ID3D11VertexShader* pShader = 0;
			hr = device_->CreateVertexShader(pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(), 0, &pShader);

			ENN_ASSERT(SUCCEEDED(hr));

			shader_wrapper = ENN_NEW D3D11VertexShader(pShader);
		}
		break;

	case PIXEL_SHADER:
		{
			ID3D11PixelShader* pShader = 0;
			hr = device_->CreatePixelShader(pCompiledShader->GetBufferPointer(),
				pCompiledShader->GetBufferSize(), 0, &pShader);

			ENN_ASSERT(SUCCEEDED(hr));

			shader_wrapper = ENN_NEW D3D11PixelShader(pShader);
		}

		break;
	}

	shader_wrapper->compiled_shader_ = pCompiledShader;

	return shader_wrapper;
}

ID3DBlob* D3D11RenderDevice::generateShader(ShaderType type, const String& shader_text, 
						 const String& shader_function,
						 const String& shader_model,
						 const D3D_SHADER_MACRO* pDefines)
{
	uint32 flags = 0;

#if 1
	flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ID3DBlob* pCompiledShader = 0;
	ID3DBlob* pErrorMessages = 0;

	if (FAILED(D3DCompile(shader_text.c_str(), shader_text.size(), 0, pDefines,
		0, shader_function.c_str(), shader_model.c_str(), flags, 0, &pCompiledShader, &pErrorMessages)))
	{
		if (pErrorMessages)
		{
			LPVOID pCompileErrors = pErrorMessages->GetBufferPointer();
			const char* pMessage = (const char*)pCompileErrors;

			printf("%s\n", pMessage);
			
			ENN_ASSERT(0);
		}

		ENN_SAFE_RELEASE2(pCompiledShader);
		ENN_SAFE_RELEASE2(pErrorMessages);

		return 0;
	}

	ENN_SAFE_RELEASE2(pErrorMessages);

	return pCompiledShader;
}

void D3D11RenderDevice::setVertexShader(D3D11Shader* shader)
{
	if (vertex_shader_cache_ != shader)
	{
		D3D11VertexShader* vertex_shader = static_cast<D3D11VertexShader*>(shader);
		 
		device_ctx_->VSSetShader(vertex_shader->getRaw(), 0, 0);

		vertex_shader_cache_ = vertex_shader;
	}
}

void D3D11RenderDevice::setPixelShader(D3D11Shader* shader)
{
	if (pixel_shader_cache_ != shader)
	{
		D3D11PixelShader* pixel_shader = static_cast<D3D11PixelShader*>(shader);

		device_ctx_->PSSetShader(pixel_shader->getRaw(), 0, 0);

		pixel_shader_cache_ = pixel_shader;
	}
}

void D3D11RenderDevice::setViewport(const D3D11Viewport& vp)
{
	if (!(vp_cache_ == vp))
	{
		device_ctx_->RSSetViewports(1, &vp.raw_);
		vp_cache_ = vp;
	}
}

uint32 D3D11RenderDevice::getDXGIFormatSize(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R32G32B32A32_TYPELESS:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32A32_UINT:
	case DXGI_FORMAT_R32G32B32A32_SINT:
		return 128;

	case DXGI_FORMAT_R32G32B32_TYPELESS:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R32G32B32_UINT:
	case DXGI_FORMAT_R32G32B32_SINT:
		return 96;

	case DXGI_FORMAT_R16G16B16A16_TYPELESS:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R16G16B16A16_UINT:
	case DXGI_FORMAT_R16G16B16A16_SNORM:
	case DXGI_FORMAT_R16G16B16A16_SINT:
	case DXGI_FORMAT_R32G32_TYPELESS:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_R32G32_UINT:
	case DXGI_FORMAT_R32G32_SINT:
	case DXGI_FORMAT_R32G8X24_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
		return 64;

	case DXGI_FORMAT_R10G10B10A2_TYPELESS:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UINT:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R8G8B8A8_TYPELESS:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R8G8B8A8_UINT:
	case DXGI_FORMAT_R8G8B8A8_SNORM:
	case DXGI_FORMAT_R8G8B8A8_SINT:
	case DXGI_FORMAT_R16G16_TYPELESS:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_R16G16_UINT:
	case DXGI_FORMAT_R16G16_SNORM:
	case DXGI_FORMAT_R16G16_SINT:
	case DXGI_FORMAT_R32_TYPELESS:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R32_UINT:
	case DXGI_FORMAT_R32_SINT:
	case DXGI_FORMAT_R24G8_TYPELESS:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return 32;

	case DXGI_FORMAT_R8G8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_R8G8_UINT:
	case DXGI_FORMAT_R8G8_SNORM:
	case DXGI_FORMAT_R8G8_SINT:
	case DXGI_FORMAT_R16_TYPELESS:
	case DXGI_FORMAT_R16_FLOAT:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R16_UINT:
	case DXGI_FORMAT_R16_SNORM:
	case DXGI_FORMAT_R16_SINT:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return 16;

	case DXGI_FORMAT_R8_TYPELESS:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_R8_UINT:
	case DXGI_FORMAT_R8_SNORM:
	case DXGI_FORMAT_R8_SINT:
	case DXGI_FORMAT_A8_UNORM:
		return 8;

	// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
		return 128;

	// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R1_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
		return 64;

	// Compressed format; http://msdn2.microsoft.com/en-us/library/bb694531(VS.85).aspx
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		return 32;

	// These are compressed, but bit-size information is unclear.
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		return 32;

	case DXGI_FORMAT_UNKNOWN:
	default:
		ENN_ASSERT(0);
		return 0;
	}
}

void D3D11RenderDevice::setSolidState(bool solid)
{
	setDepthStencilState(default_deptph_state_, 1);
	setBlendState(solid ? solid_blend_state_ : trans_blend_state_);
	setRasterizerState(cull_cw_state_);
}

void D3D11RenderDevice::setSolidStateLessAndEqual(bool solid)
{
	setDepthStencilState(less_and_equal_deptph_state_, 1);
	setBlendState(solid ? solid_blend_state_ : trans_blend_state_);
	setRasterizerState(cull_cw_state_);
}

void D3D11RenderDevice::push_active_render_target()
{
	if (!curr_rtt_)
	{
		return;
	}

	if (!rt_stack_.empty())
	{
		D3D11RenderTarget* last_rtt = rt_stack_.back();

		if (last_rtt == curr_rtt_)
		{
			return;
		}
	}

	rt_stack_.push_back(curr_rtt_);
}

void D3D11RenderDevice::pop_render_target()
{
	ENN_ASSERT(!rt_stack_.empty());
	setRenderTarget(rt_stack_.back());
	rt_stack_.pop_back();
}

void D3D11RenderDevice::resetRenderStates()
{
	clearRenderStates();
	createCommState();

	vertex_shader_cache_ = 0;
	pixel_shader_cache_ = 0;

	rs_state_cache_ = 0;
	ds_state_cache_ = 0;
	stencil_ref_cache_ = -1;
	bs_state_cache_ = 0;
	blend_factor_cache_ = Color(1, 1, 1, 1);
	sample_mask_cache_ = 0xFFFFFFFF;
}

ID3D11InputLayout* D3D11RenderDevice::createInputLayout(vector<D3D11_INPUT_ELEMENT_DESC>::type elems,
	D3D11VertexShader* vs)
{
	ID3D11InputLayout* pLayout;
	HRESULT hr = device_->CreateInputLayout(&elems[0], elems.size(),
		vs->compiled_shader_->GetBufferPointer(), vs->compiled_shader_->GetBufferSize(), &pLayout);

	ENN_SUCCEED(hr);

	return pLayout;
}

void D3D11RenderDevice::setD3D11Effect(D3D11Effect* eff)
{
	setVertexShader(eff->getShader(VERTEX_SHADER));
	setPixelShader(eff->getShader(PIXEL_SHADER));
}

void D3D11RenderDevice::setD3D11SamplerObject(ShaderType type, uint32 slot, D3D11SamplerStateObject* obj)
{
	SamplersCache& sampler_cache_ = samplers_cache_list_[type];

	switch (type)
	{
	case enn::VERTEX_SHADER:
		if (sampler_cache_[slot] != obj)
		{
			ID3D11SamplerState* sampler[] =
			{
				obj->getRaw(),
			};

			device_ctx_->VSSetSamplers(slot, 1, sampler);
			sampler_cache_[slot] = obj;
		}

		break;

	case enn::HULL_SHADER:
		break;

	case enn::DOMAIN_SHADER:
		break;

	case enn::GEOMETRY_SHADER:
		break;

	case enn::PIXEL_SHADER:
		if (sampler_cache_[slot] != obj)
		{
			ID3D11SamplerState* sampler[] =
			{
				obj->getRaw(),
			};

			device_ctx_->PSSetSamplers(slot, 1, sampler);
			sampler_cache_[slot] = obj;
		}

		break;

	case enn::COMPUTE_SHADER:
		break;

	default:
		ENN_ASSERT(0);
		break;
	}
}

void D3D11RenderDevice::setD3D11ShaderResourceView(ShaderType type, uint32 slot, D3D11ShaderResourceView* view)
{
	ShaderResourceViewsCache& srv_cache = shader_res_views_cache_list_[type];

	switch (type)
	{
	case enn::VERTEX_SHADER:
		if (srv_cache[slot] != view)
		{
			device_ctx_->VSSetShaderResources(slot, 1, &(view->raw_));
			srv_cache[slot] = view;
		}
		break;

	case enn::HULL_SHADER:
		break;
	case enn::DOMAIN_SHADER:
		break;
	case enn::GEOMETRY_SHADER:
		break;
	case enn::PIXEL_SHADER:
		if (srv_cache[slot] != view)
		{
			device_ctx_->PSSetShaderResources(slot, 1, &(view->raw_));
			srv_cache[slot] = view;
		}
		break;

	case enn::COMPUTE_SHADER:
		break;
	default:
		break;
	}
}

void D3D11RenderDevice::setD3D11ConstantBuffer(ShaderType type, uint32 slot, D3D11ConstanceBuffer* cbuffer)
{
	ConstantBuffersCache& cbuffer_cache = constant_buffers_cache_list_[type];

	switch (type)
	{
	case enn::VERTEX_SHADER:
		if (1)
		{
			ID3D11Buffer* buffer[] =
			{
				cbuffer->getBuffer(),
			};

			device_ctx_->VSSetConstantBuffers(slot, 1, buffer);
			cbuffer_cache[slot] = cbuffer;
		}
		break;

	case enn::HULL_SHADER:
		break;
	case enn::DOMAIN_SHADER:
		break;
	case enn::GEOMETRY_SHADER:
		break;
	case enn::PIXEL_SHADER:
		if (1)
		{
			ID3D11Buffer* buffer[] =
			{
				cbuffer->getBuffer(),
			};

			device_ctx_->PSSetConstantBuffers(slot, 1, buffer);
			cbuffer_cache[slot] = cbuffer;
		}
		break;

	case enn::COMPUTE_SHADER:
		break;
	case enn::SHADER_COUNT:
		break;
	default:
		break;
	}
}

void D3D11RenderDevice::setD3D11ConstantBuffers(ShaderType type,
	vector<D3D11ConstanceBuffer*>::type cbuffers)
{
	vector<ID3D11Buffer*>::type raws;
	for (size_t i = 0; i < cbuffers.size(); ++i)
	{
		raws.push_back(cbuffers[i]->getBuffer());
	}

	switch (type)
	{
	case enn::VERTEX_SHADER:
		device_ctx_->VSSetConstantBuffers(0, raws.size(), &raws[0]);
		break;

	case enn::HULL_SHADER:
		break;
	case enn::DOMAIN_SHADER:
		break;
	case enn::GEOMETRY_SHADER:
		break;
	case enn::PIXEL_SHADER:
		device_ctx_->PSSetConstantBuffers(0, raws.size(), &raws[0]);
		break;

	case enn::COMPUTE_SHADER:
		break;
	case enn::SHADER_COUNT:
		break;
	default:
		break;
	}
}

void D3D11RenderDevice::doRender(D3D11InputLayout& rl, D3D11Effect* effect)
{
	uint32_t const num_vertex_streams = rl.getNumVertexStreams();
	uint32_t const all_num_vertex_stream = num_vertex_streams + (rl.getInstanceStream() ? 1 : 0);

	vector<ID3D11Buffer*>::type vbs(all_num_vertex_stream);
	vector<UINT>::type strides(all_num_vertex_stream);
	vector<UINT>::type offsets(all_num_vertex_stream);

	bool draw_instance = false;

	for (uint32_t i = 0; i < num_vertex_streams; ++i)
	{
		D3D11VertexBuffer* stream = rl.getVertexStream(i);
		vbs[i] = stream->getBuffer();
		strides[i] = rl.getVertexSize(i);
		offsets[i] = 0;
	}

	if (rl.getInstanceStream())
	{
		uint32_t number = num_vertex_streams;
		D3D11VertexBuffer* stream = rl.getInstanceStream();
		vbs[number] = stream->getBuffer();
		strides[number] = rl.getInstanceSize();
		offsets[number] = 0;

		draw_instance = true;
	}

	if (all_num_vertex_stream != 0)
	{
		device_ctx_->IASetVertexBuffers(0, all_num_vertex_stream, &vbs[0], &strides[0], &offsets[0]);
		
		D3D11VertexShader* vs = static_cast<D3D11VertexShader*>(effect->getShader(VERTEX_SHADER));
		device_ctx_->IASetInputLayout(rl.getInputLayout(vs));
	}
	else
	{
		device_ctx_->IASetInputLayout(0);
	}

	device_ctx_->IASetPrimitiveTopology(getMapping(rl.getTopology()));

	uint32_t prim_count;
	uint32_t indices = rl.getNumIndices();

	switch (rl.getTopology())
	{
	case TT_POINT_LIST:
		prim_count = indices;
		break;

	case TT_LINE_LIST:
		prim_count = indices / 2;
		break;

	case TT_TRIANGLE_LIST:
		prim_count = indices / 3;
		break;

	case TT_TRIANGLE_STRIP:
		prim_count = indices / 2;
		break;

	default:
		ENN_ASSERT(0);
		prim_count = indices / 3;
		break;
	}

	uint32_t const num_instances = rl.getNumInstances();
	num_prim_rendered_ += num_instances * prim_count;

	D3D11IndexBuffer* ib = rl.getIndexStream();
	device_ctx_->IASetIndexBuffer(ib->getBuffer(), MappingFormat(rl.getIndexStreamFormat()), 0);

	if (num_instances > 1 || draw_instance)
	{
		device_ctx_->DrawIndexedInstanced(indices, num_instances, 
			rl.getStartIndexLocation(), rl.getStartVertexLocation(), 
			rl.getStartInstanceLocation());
	}
	else
	{
		device_ctx_->DrawIndexed(indices, rl.getStartIndexLocation(),
			rl.getStartVertexLocation());
	}

	num_batches_rendered_++;
}

D3D11_PRIMITIVE_TOPOLOGY D3D11RenderDevice::getMapping(TopologyType type)
{
	switch (type)
	{
	case enn::TT_POINT_LIST:
		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

	case enn::TT_LINE_LIST:
		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;

	case enn::TT_TRIANGLE_LIST:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	case enn::TT_TRIANGLE_STRIP:
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

	default:
		ENN_ASSERT(0);
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
}

void D3D11RenderDevice::convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest)
{
	dest = matrix;

	// Convert depth range from [-1,+1] to [0,1]
	dest[2][0] = (dest[2][0] + dest[3][0]) / 2;
	dest[2][1] = (dest[2][1] + dest[3][1]) / 2;
	dest[2][2] = (dest[2][2] + dest[3][2]) / 2;
	dest[2][3] = (dest[2][3] + dest[3][3]) / 2;

	dest[0][2] = -dest[0][2];
	dest[1][2] = -dest[1][2];
	dest[2][2] = -dest[2][2];
	dest[3][2] = -dest[3][2];
}

void D3D11RenderDevice::convertViewMatrix(const Matrix4& matrix, Matrix4& dest)
{
	dest = matrix;

	dest[2][0] = -dest[2][0];
	dest[2][1] = -dest[2][1];
	dest[2][2] = -dest[2][2];
	dest[2][3] = -dest[2][3];
	
}

DXGI_FORMAT D3D11RenderDevice::MappingFormat(ElementFormat format)
{
	switch (format)
	{
	case EF_A8:
		return DXGI_FORMAT_A8_UNORM;

	case EF_R5G6B5:
		return DXGI_FORMAT_B5G6R5_UNORM;

	case EF_A1RGB5:
		return DXGI_FORMAT_B5G5R5A1_UNORM;

	case EF_ARGB4:
		return DXGI_FORMAT_B4G4R4A4_UNORM;

	case EF_R8:
		return DXGI_FORMAT_R8_UNORM;

	case EF_SIGNED_R8:
		return DXGI_FORMAT_R8_SNORM;

	case EF_GR8:
		return DXGI_FORMAT_R8G8_UNORM;

	case EF_SIGNED_GR8:
		return DXGI_FORMAT_R8G8_SNORM;

	case EF_ARGB8:
		return DXGI_FORMAT_B8G8R8A8_UNORM;

	case EF_ABGR8:
		return DXGI_FORMAT_R8G8B8A8_UNORM;

	case EF_SIGNED_ABGR8:
		return DXGI_FORMAT_R8G8B8A8_SNORM;

	case EF_A2BGR10:
		return DXGI_FORMAT_R10G10B10A2_UNORM;

	case EF_SIGNED_A2BGR10:
		return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

	case EF_R8UI:
		return DXGI_FORMAT_R8_UINT;

	case EF_R8I:
		return DXGI_FORMAT_R8_SINT;

	case EF_GR8UI:
		return DXGI_FORMAT_R8G8_UINT;

	case EF_GR8I:
		return DXGI_FORMAT_R8G8_SINT;

	case EF_ABGR8UI:
		return DXGI_FORMAT_R8G8B8A8_UINT;

	case EF_ABGR8I:
		return DXGI_FORMAT_R8G8B8A8_SINT;

	case EF_A2BGR10UI:
		return DXGI_FORMAT_R10G10B10A2_UINT;

	case EF_R16:
		return DXGI_FORMAT_R16_UNORM;

	case EF_SIGNED_R16:
		return DXGI_FORMAT_R16_SNORM;

	case EF_GR16:
		return DXGI_FORMAT_R16G16_UNORM;

	case EF_SIGNED_GR16:
		return DXGI_FORMAT_R16G16_SNORM;

	case EF_ABGR16:
		return DXGI_FORMAT_R16G16B16A16_UNORM;

	case EF_SIGNED_ABGR16:
		return DXGI_FORMAT_R16G16B16A16_SNORM;

	case EF_R16UI:
		return DXGI_FORMAT_R16_UINT;

	case EF_R16I:
		return DXGI_FORMAT_R16_SINT;

	case EF_GR16UI:
		return DXGI_FORMAT_R16G16_UINT;

	case EF_GR16I:
		return DXGI_FORMAT_R16G16_SINT;

	case EF_ABGR16UI:
		return DXGI_FORMAT_R16G16B16A16_UINT;

	case EF_ABGR16I:
		return DXGI_FORMAT_R16G16B16A16_SINT;

	case EF_R32UI:
		return DXGI_FORMAT_R32_UINT;

	case EF_R32I:
		return DXGI_FORMAT_R32_SINT;

	case EF_GR32UI:
		return DXGI_FORMAT_R32G32_UINT;

	case EF_GR32I:
		return DXGI_FORMAT_R32G32_SINT;

	case EF_BGR32UI:
		return DXGI_FORMAT_R32G32B32_UINT;

	case EF_BGR32I:
		return DXGI_FORMAT_R32G32B32_SINT;

	case EF_ABGR32UI:
		return DXGI_FORMAT_R32G32B32A32_UINT;

	case EF_ABGR32I:
		return DXGI_FORMAT_R32G32B32A32_SINT;

	case EF_R16F:
		return DXGI_FORMAT_R16_FLOAT;

	case EF_GR16F:
		return DXGI_FORMAT_R16G16_FLOAT;

	case EF_B10G11R11F:
		return DXGI_FORMAT_R11G11B10_FLOAT;

	case EF_ABGR16F:
		return DXGI_FORMAT_R16G16B16A16_FLOAT;

	case EF_R32F:
		return DXGI_FORMAT_R32_FLOAT;

	case EF_GR32F:
		return DXGI_FORMAT_R32G32_FLOAT;

	case EF_BGR32F:
		return DXGI_FORMAT_R32G32B32_FLOAT;

	case EF_ABGR32F:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;

	case EF_BC1:
		return DXGI_FORMAT_BC1_UNORM;

	case EF_BC2:
		return DXGI_FORMAT_BC2_UNORM;

	case EF_BC3:
		return DXGI_FORMAT_BC3_UNORM;

	case EF_BC4:
		return DXGI_FORMAT_BC4_UNORM;

	case EF_SIGNED_BC4:
		return DXGI_FORMAT_BC4_SNORM;

	case EF_BC5:
		return DXGI_FORMAT_BC5_UNORM;

	case EF_SIGNED_BC5:
		return DXGI_FORMAT_BC5_SNORM;

	case EF_BC6:
		return DXGI_FORMAT_BC6H_UF16;

	case EF_SIGNED_BC6:
		return DXGI_FORMAT_BC6H_SF16;

	case EF_BC7:
		return DXGI_FORMAT_BC7_UNORM;

	case EF_D16:
		return DXGI_FORMAT_D16_UNORM;

	case EF_D24S8:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;

	case EF_D32F:
		return DXGI_FORMAT_D32_FLOAT;

	case EF_ARGB8_SRGB:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

	case EF_ABGR8_SRGB:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	case EF_BC1_SRGB:
		return DXGI_FORMAT_BC1_UNORM_SRGB;

	case EF_BC2_SRGB:
		return DXGI_FORMAT_BC2_UNORM_SRGB;

	case EF_BC3_SRGB:
		return DXGI_FORMAT_BC3_UNORM_SRGB;

	case EF_BC7_SRGB:
		return DXGI_FORMAT_BC7_UNORM_SRGB;

	default:
		ENN_ASSERT(0);
		return DXGI_FORMAT_B8G8R8A8_UNORM;
	}
}

ElementFormat D3D11RenderDevice::MappingFormat(DXGI_FORMAT format)
{
#pragma warning(push)
#pragma warning(disable: 4063)
	switch (format)
	{
	case DXGI_FORMAT_A8_UNORM:
		return EF_A8;

	case DXGI_FORMAT_B5G6R5_UNORM:
		return EF_R5G6B5;

	case DXGI_FORMAT_B5G5R5A1_UNORM:
		return EF_A1RGB5;

	case DXGI_FORMAT_B4G4R4A4_UNORM:
		return EF_ARGB4;

	case DXGI_FORMAT_R8_UNORM:
		return EF_R8;

	case DXGI_FORMAT_R8_SNORM:
		return EF_SIGNED_R8;

	case DXGI_FORMAT_R8G8_UNORM:
		return EF_GR8;

	case DXGI_FORMAT_R8G8_SNORM:
		return EF_SIGNED_GR8;

	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return EF_ARGB8;

	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return EF_ABGR8;

	case DXGI_FORMAT_R8G8B8A8_SNORM:
		return EF_SIGNED_ABGR8;

	case DXGI_FORMAT_R10G10B10A2_UNORM:
		return EF_A2BGR10;

	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
		return EF_SIGNED_A2BGR10;

	case DXGI_FORMAT_R8_UINT:
		return EF_R8UI;

	case DXGI_FORMAT_R8_SINT:
		return EF_R8I;

	case DXGI_FORMAT_R8G8_UINT:
		return EF_GR8UI;

	case DXGI_FORMAT_R8G8_SINT:
		return EF_GR8I;

	case DXGI_FORMAT_R8G8B8A8_UINT:
		return EF_ABGR8UI;

	case DXGI_FORMAT_R8G8B8A8_SINT:
		return EF_ABGR8I;

	case DXGI_FORMAT_R10G10B10A2_UINT:
		return EF_A2BGR10UI;

	case DXGI_FORMAT_R16_UNORM:
		return EF_R16;

	case DXGI_FORMAT_R16_SNORM:
		return EF_SIGNED_R16;

	case DXGI_FORMAT_R16G16_UNORM:
		return EF_GR16;

	case DXGI_FORMAT_R16G16_SNORM:
		return EF_SIGNED_GR16;

	case DXGI_FORMAT_R16G16B16A16_UNORM:
		return EF_ABGR16;

	case DXGI_FORMAT_R16G16B16A16_SNORM:
		return EF_SIGNED_ABGR16;

	case DXGI_FORMAT_R16_UINT:
		return EF_R16UI;

	case DXGI_FORMAT_R16_SINT:
		return EF_R16I;

	case DXGI_FORMAT_R16G16_UINT:
		return EF_GR16UI;

	case DXGI_FORMAT_R16G16_SINT:
		return EF_GR16I;

	case DXGI_FORMAT_R16G16B16A16_UINT:
		return EF_ABGR16UI;

	case DXGI_FORMAT_R16G16B16A16_SINT:
		return EF_ABGR16I;

	case DXGI_FORMAT_R32_UINT:
		return EF_R32UI;

	case DXGI_FORMAT_R32_SINT:
		return EF_R32I;

	case DXGI_FORMAT_R32G32_UINT:
		return EF_GR32UI;

	case DXGI_FORMAT_R32G32_SINT:
		return EF_GR32I;

	case DXGI_FORMAT_R32G32B32_UINT:
		return EF_BGR32UI;

	case DXGI_FORMAT_R32G32B32_SINT:
		return EF_BGR32I;

	case DXGI_FORMAT_R32G32B32A32_UINT:
		return EF_ABGR32UI;

	case DXGI_FORMAT_R32G32B32A32_SINT:
		return EF_ABGR32I;

	case DXGI_FORMAT_R16_FLOAT:
		return EF_R16F;

	case DXGI_FORMAT_R16G16_FLOAT:
		return EF_GR16F;

	case DXGI_FORMAT_R11G11B10_FLOAT:
		return EF_B10G11R11F;

	case DXGI_FORMAT_R16G16B16A16_FLOAT:
		return EF_ABGR16F;

	case DXGI_FORMAT_R32_FLOAT:
		return EF_R32F;

	case DXGI_FORMAT_R32G32_FLOAT:
		return EF_GR32F;

	case DXGI_FORMAT_R32G32B32_FLOAT:
		return EF_BGR32F;

	case DXGI_FORMAT_R32G32B32A32_FLOAT:
		return EF_ABGR32F;

	case DXGI_FORMAT_BC1_UNORM:
		return EF_BC1;

	case DXGI_FORMAT_BC2_UNORM:
		return EF_BC2;

	case DXGI_FORMAT_BC3_UNORM:
		return EF_BC3;

	case DXGI_FORMAT_BC4_UNORM:
		return EF_BC4;

	case DXGI_FORMAT_BC4_SNORM:
		return EF_SIGNED_BC4;

	case DXGI_FORMAT_BC5_UNORM:
		return EF_BC5;

	case DXGI_FORMAT_BC5_SNORM:
		return EF_SIGNED_BC5;

	case DXGI_FORMAT_BC6H_UF16:
		return EF_BC6;

	case DXGI_FORMAT_BC6H_SF16:
		return EF_SIGNED_BC6;

	case DXGI_FORMAT_BC7_UNORM:
		return EF_BC7;

	case DXGI_FORMAT_D16_UNORM:
		return EF_D16;

	case DXGI_FORMAT_D24_UNORM_S8_UINT:
		return EF_D24S8;

	case DXGI_FORMAT_D32_FLOAT:
		return EF_D32F;

	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
		return EF_ARGB8_SRGB;

	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
		return EF_ABGR8_SRGB;

	case DXGI_FORMAT_BC1_UNORM_SRGB:
		return EF_BC1_SRGB;

	case DXGI_FORMAT_BC2_UNORM_SRGB:
		return EF_BC2_SRGB;

	case DXGI_FORMAT_BC3_UNORM_SRGB:
		return EF_BC3_SRGB;

	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return EF_BC7_SRGB;

	default:
		ENN_ASSERT(0);
		return EF_ARGB8;
	}
#pragma warning(pop)
}

void D3D11RenderDevice::Mapping(
	InputElemsType& elements,
	size_t stream,
	const VertexElementsType& vertex_elems, 
	StreamType st, uint32 freq)
{
	elements.resize(vertex_elems.size());

	uint16_t elem_offset = 0;
	for (uint32_t i = 0; i < elements.size(); ++i)
	{
		VertexElement const & vs_elem = vertex_elems[i];

		D3D11_INPUT_ELEMENT_DESC& element = elements[i];
		element.SemanticIndex = vs_elem.usage_index;
		element.Format = MappingFormat(vs_elem.format);
		element.InputSlot = static_cast<WORD>(stream);
		element.AlignedByteOffset = elem_offset;

		if (ST_GEOMETRY == st)
		{
			element.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			element.InstanceDataStepRate = 0;
		}
		else
		{
			ENN_ASSERT(ST_INSTANCE == st);

			element.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			element.InstanceDataStepRate = freq;
		}

		switch (vs_elem.usage)
		{
			// Vertex xyzs
		case ELEMENT_POSITION:
			element.SemanticName = "POSITION";
			break;

			// Normal
		case ELEMENT_NORMAL:
			element.SemanticName = "NORMAL";
			break;

			// Vertex colors
		case ELEMENT_DIFFUSE:
			element.SemanticName = "COLOR";
			break;

			// Vertex speculars
		case ELEMENT_SPECULAR:
			element.SemanticName = "COLOR";
			break;

			// Blend Weights
		case ELEMENT_BLENDWEIGHTS:
			element.SemanticName = "BLENDWEIGHT";
			break;

			// Blend Indices
		case ELEMENT_BLENDINDICES:
			element.SemanticName = "BLENDINDICES";
			if (EF_ABGR8 == vs_elem.format)
			{
				element.Format = DXGI_FORMAT_R8G8B8A8_UINT;
			}
			break;

			// Do texture coords
		case ELEMENT_TEXCOORD:
			element.SemanticName = "TEXCOORD";
			break;

		case ELEMENT_TANGENT:
			element.SemanticName = "TANGENT";
			break;

		case ELEMENT_BINORMAL:
			element.SemanticName = "BINORMAL";
			break;
		}

		elem_offset = static_cast<uint16_t>(elem_offset + vs_elem.element_size());
	}
}

uint32 D3D11RenderDevice::getElementOffset(const VertexElementsType& elements, VertexElement ve)
{
	uint32 offset = 0;
	bool found = false;

	for (size_t i = 0; i < elements.size(); ++i)
	{
		if (elements[i] == ve)
		{
			found = true;
			break;
		}

		offset += elements[i].element_size();
	}

	ENN_ASSERT(found);

	return offset;
}

bool D3D11RenderDevice::hasElement(const VertexElementsType& elements, VertexElement ve)
{
	bool found = false;

	for (size_t i = 0; i < elements.size(); ++i)
	{
		if (elements[i] == ve)
		{
			found = true;
			break;
		}
	}

	return found;
}

uint32 D3D11RenderDevice::getElementsSize(const VertexElementsType& elements)
{
	uint32 size = 0;
	for (size_t i = 0; i < elements.size(); ++i)
	{
		size += elements[i].element_size();
	}

	return size;
}

DXGI_FORMAT D3D11RenderDevice::MakeDXGIFormatSRGB(DXGI_FORMAT format)
{
	switch (format)
	{
	case DXGI_FORMAT_R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	case DXGI_FORMAT_BC1_UNORM:
		return DXGI_FORMAT_BC1_UNORM_SRGB;

	case DXGI_FORMAT_BC2_UNORM:
		return DXGI_FORMAT_BC2_UNORM_SRGB;

	case DXGI_FORMAT_BC3_UNORM:
		return DXGI_FORMAT_BC3_UNORM_SRGB;

	case DXGI_FORMAT_B8G8R8A8_UNORM:
		return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;

	case DXGI_FORMAT_B8G8R8X8_UNORM:
		return DXGI_FORMAT_B8G8R8X8_UNORM_SRGB;

	case DXGI_FORMAT_BC7_UNORM:
		return DXGI_FORMAT_BC7_UNORM_SRGB;

	default:
		return format;
	}
}

void D3D11RenderDevice::createCommState()
{
	// solid
	D3D11BlendStateDesc solid_desc;
	solid_desc.setSolid();

	solid_blend_state_ = createBlendState(solid_desc);

	// trans
	D3D11BlendStateDesc trans_desc;
	trans_desc.setTrans();

	trans_blend_state_ = createBlendState(trans_desc);

	D3D11BlendStateDesc trans_add_desc;
	trans_add_desc.setTransAdd();
	trans_add_blend_state_ = createBlendState(trans_add_desc);

	// depth none
	D3D11DepthStencilStateDesc depth_none_desc;
	depth_none_desc.setNone();

	depth_none_state_ = createDepthStencilState(depth_none_desc);

	// depth default
	D3D11DepthStencilStateDesc depth_default_desc;
	depth_default_desc.setDefault();

	default_deptph_state_ = createDepthStencilState(depth_default_desc);

	// less and equal
	D3D11DepthStencilStateDesc less_equal_default_desc;
	less_equal_default_desc.setLessAndEqual();

	less_and_equal_deptph_state_ = createDepthStencilState(less_equal_default_desc);

	// depth read only
	D3D11DepthStencilStateDesc depth_readonly_desc;
	depth_readonly_desc.setReadonly();

	depth_read_state_ = createDepthStencilState(depth_readonly_desc);

	// depth always only
	D3D11DepthStencilStateDesc depth_always_desc;
	depth_always_desc.setAlways();
	depth_always_state_ = createDepthStencilState(depth_always_desc);
	
	// sky
	D3D11DepthStencilStateDesc depth_sky_desc;
	depth_sky_desc.setSky();
	depth_sky_state_ = createDepthStencilState(depth_sky_desc);

	// cull none state
	D3D11RasterizerStateDesc cull_node_desc;
	cull_node_desc.setCullNone();

	cull_none_state_ = createRasterizerState(cull_node_desc);

	// cull cw state
	D3D11RasterizerStateDesc cull_cw_desc;
	cull_cw_desc.setCullCW();

	cull_cw_state_ = createRasterizerState(cull_cw_desc);

	// cull ccw state
	D3D11RasterizerStateDesc cull_ccw_desc;
	cull_ccw_desc.setCullCCW();

	cull_ccw_state_ = createRasterizerState(cull_ccw_desc);

	// wireframe state
	D3D11RasterizerStateDesc wireframe_desc;
	wireframe_desc.setWireframe();

	wireframe_state_ = createRasterizerState(wireframe_desc);

	// point wrap sampler
	D3D11SamplerStateDesc point_wrap_desc;
	point_wrap_desc.setPointWrap();

	point_wrap_state_ = createSamplerState(point_wrap_desc);

	// point clamp sampler
	D3D11SamplerStateDesc point_clamp_desc;
	point_clamp_desc.setPointClamp();

	point_clamp_state_ = createSamplerState(point_clamp_desc);

	// linear wrap sampler
	D3D11SamplerStateDesc linear_wrap_desc;
	linear_wrap_desc.setLineWrap();

	linear_wrap_state_ = createSamplerState(linear_wrap_desc);

	// linear clamp sampler
	D3D11SamplerStateDesc linear_clamp_desc;
	linear_clamp_desc.setLineClamp();

	linear_clamp_state_ = createSamplerState(linear_clamp_desc);

	// anisotropic wrap sampler
	D3D11SamplerStateDesc anisotropic_wrap_desc;
	anisotropic_wrap_desc.setAnisotropicWrap();

	anisotropic_wrap_state_ = createSamplerState(anisotropic_wrap_desc);

	// anisotropic clamp sampler
	D3D11SamplerStateDesc anisotropic_clamp_desc;
	anisotropic_clamp_desc.setAnisotropicClamp();

	anisotropic_clamp_state_ = createSamplerState(anisotropic_clamp_desc);
}

void D3D11RenderDevice::clearRenderStates()
{
	solid_blend_state_ = 0;
	trans_blend_state_ = 0;

	default_deptph_state_ = 0;
	depth_none_state_ = 0;
	depth_read_state_ = 0;

	cull_none_state_ = 0;
	cull_cw_state_ = 0;
	cull_ccw_state_ = 0;
	wireframe_state_ = 0;

	point_wrap_state_ = 0;
	point_clamp_state_ = 0;
	linear_wrap_state_ = 0;
	linear_clamp_state_ = 0;
	anisotropic_wrap_state_ = 0;

	ENN_FOR_EACH(BSPool, bs_pool_, it)
	{
		D3D11BlendStateObject* state = it->second;
		ENN_SAFE_DELETE(state);
	}

	ENN_FOR_EACH(DSPool, ds_pool_, it)
	{
		D3D11DepthStencilStateObject* state = it->second;
		ENN_SAFE_DELETE(state);
	}

	ENN_FOR_EACH(RSPool, rs_pool_, it)
	{
		D3D11RasterizerStateObject* state = it->second;
		ENN_SAFE_DELETE(state);
	}

	ENN_FOR_EACH(SSPool, ss_pool_, it)
	{
		D3D11SamplerStateObject* state = it->second;
		ENN_SAFE_DELETE(state);
	}
}

}

