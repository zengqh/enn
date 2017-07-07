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
// File: enn_d3d11_render_device.h

#pragma once

#include "enn_d3d11_gpu_buffer.h"
#include "enn_d3d11_texture_obj.h"
#include "enn_d3d11_render_state.h"
#include "enn_d3d11_render_target.h"
#include "enn_d3d11_gpu_program.h"
#include "enn_d3d11_render_view.h"
#include "enn_d3d11_render_window.h"
#include "enn_d3d11_resource_proxy.h"
#include "enn_d3d11_viewport.h"
#include "enn_d3d11_input_layout.h"

namespace enn
{

class D3D11RenderDevice : public AllocatedObject
{
	friend class D3D11RenderWindow;

	typedef map<D3D11BlendStateDesc, D3D11BlendStateObject*>::type					BSPool;
	typedef map<D3D11DepthStencilStateDesc, D3D11DepthStencilStateObject*>::type	DSPool;
	typedef map<D3D11RasterizerStateDesc, D3D11RasterizerStateObject*>::type		RSPool;
	typedef map<D3D11SamplerStateDesc, D3D11SamplerStateObject*>::type				SSPool;

	typedef list<D3D11RenderTarget*>::type RTStack;

	typedef unordered_map<uint32, D3D11SamplerStateObject*>::type					SamplersCache;
	typedef unordered_map<uint32, D3D11ShaderResourceView*>::type					ShaderResourceViewsCache;
	typedef unordered_map<uint32, D3D11ConstanceBuffer*>::type						ConstantBuffersCache;

	typedef vector<SamplersCache>::type												SamplersCacheList;
	typedef vector<ShaderResourceViewsCache>::type									ShaderResourceViewsCacheList;
	typedef vector<ConstantBuffersCache>::type										ConstantBuffersCacheList;
	
public:
	D3D11RenderDevice();
	~D3D11RenderDevice();

	IDXGIFactory*			getGIFactory() const;
	ID3D11Device*			getDevice() const { return device_; }
	ID3D11DeviceContext*	getDeviceContext() const { return device_ctx_; }

public:
	virtual void init();
	virtual void shutdown();

	virtual void beginFrame();
	virtual void endFrame();

	uint32 getCurrFrameNo() const { return curr_frame_no_; }
	virtual void setRenderTarget(D3D11RenderTarget* rt);

public:
	void setRasterizerState(D3D11RasterizerStateObject* state);
	void setDepthStencilState(D3D11DepthStencilStateObject* state, uint16 stencil_ref);
	void setBlendState(D3D11BlendStateObject* state, const Color& blend_factor = Color::WHITE, uint32 sample_mask = 0xFFFFFFFF);

public:
	D3D11ResourceProxy* createTexture2D(
		const D3D11Texture2DDesc& desc, 
		D3D11_SUBRESOURCE_DATA* pData,
		D3D11ShaderResourceViewDesc* srv_desc = 0,
		D3D11RenderTargetViewDesc* rtv_desc = 0,
		D3D11UnorderedAccessViewDesc* uav_desc = 0,
		D3D11DepthStencilViewDesc* dsv_desc = 0);

	D3D11ResourceProxy* loadTexture2D(void* pData, size_t size, bool auto_gen_mipmap = true, bool use_srgb = true);
	D3D11ResourceProxy* loadTexture2DDDS(void* pData, size_t size, bool auto_gen_mipmap = true, bool use_srgb = true);
	D3D11ResourceProxy* loadTexture2DTGA(void* pData, size_t size, bool auto_gen_mipmap = true, bool use_srgb = true);

	D3D11ResourceProxy* loadTexture1D(void* pData, size_t size, bool auto_gen_mipmap = true, bool use_srgb = true);

	D3D11ResourceProxy* createConstantBuffer(const D3D11BufferDesc& desc, D3D11_SUBRESOURCE_DATA* pData);
	D3D11ResourceProxy* createConstantBuffer(void* data, size_t size);
	D3D11ResourceProxy* createVertexBuffer(const D3D11BufferDesc& desc, D3D11_SUBRESOURCE_DATA* pData);
	D3D11ResourceProxy* createIndexBuffer(const D3D11BufferDesc& desc, D3D11_SUBRESOURCE_DATA* pData);

	D3D11ResourceProxy* createVertexBuffer(void* data, size_t size, bool dynamic = false);
	D3D11ResourceProxy* createIndexBuffer(void* data, size_t size, bool dynamic = false);

public:
	D3D11BlendStateObject*			createBlendState(const D3D11BlendStateDesc& desc);
	D3D11DepthStencilStateObject*	createDepthStencilState(const D3D11DepthStencilStateDesc& desc);
	D3D11RasterizerStateObject*		createRasterizerState(const D3D11RasterizerStateDesc& desc);
	D3D11SamplerStateObject*		createSamplerState(const D3D11SamplerStateDesc& desc);

	D3D11ShaderResourceView*		createShaderResourceView(D3D11Resource* res, const D3D11ShaderResourceViewDesc& desc);
	D3D11RenderTargetView*			createRenderTargetView(D3D11Resource* res, const D3D11RenderTargetViewDesc& desc);
	D3D11DepthStencilView*			createDepthStencilView(D3D11Resource* res, const D3D11DepthStencilViewDesc& desc);
	D3D11UnorderedAccessView*		createUnorderedAccessView(D3D11Resource* res, const D3D11UnorderedAccessViewDesc& desc);
	
	D3D11ShaderResourceView*		createShaderResourceView(D3D11Resource* res, const D3D11ShaderResourceViewDesc* desc);
	D3D11RenderTargetView*			createRenderTargetView(D3D11Resource* res, const D3D11RenderTargetViewDesc* desc);
	D3D11DepthStencilView*			createDepthStencilView(D3D11Resource* res, const D3D11DepthStencilViewDesc* desc);
	D3D11UnorderedAccessView*		createUnorderedAccessView(D3D11Resource* res, const D3D11UnorderedAccessViewDesc* desc);

	void							createViewport(D3D11Viewport& vp, float x, float y, float w, float h, float min_depth = 0.0f, float max_depth = 1.0f);
public:
	D3D11_MAPPED_SUBRESOURCE		mapResource(D3D11Resource* res, uint32 sub_res, D3D11_MAP actions, uint32 flags);
	void							unMapResource(D3D11Resource* res, uint32 sub_res);

	D3D11_MAPPED_SUBRESOURCE		mapResource(D3D11ResourceProxy* res_proxy, uint32 sub_res, D3D11_MAP actions, uint32 flags);
	void							unMapResource(D3D11ResourceProxy* res_proxy, uint32 sub_res);

	void*							lock(D3D11ResourceProxy* res_proxy, D3D11_MAP actions = D3D11_MAP_WRITE_DISCARD);
	void							unLock(D3D11ResourceProxy* res_proxy);

public:
	D3D11Shader* loadShader(ShaderType type, const String& shader_text, 
		const String& shader_function,
		const String& shader_model,
		const D3D_SHADER_MACRO* pDefines);

	ID3DBlob* generateShader(ShaderType type, const String& shader_text, 
		const String& shader_function,
		const String& shader_model,
		const D3D_SHADER_MACRO* pDefines);

	void setVertexShader(D3D11Shader* shader);
	void setPixelShader(D3D11Shader* shader);

	void setViewport(const D3D11Viewport& vp);

public:
	static uint32 getDXGIFormatSize(DXGI_FORMAT format);

public:
	void setSolidState(bool solid);
	void setSolidStateLessAndEqual(bool solid);

	void push_active_render_target();
	void pop_render_target();

	void resetRenderStates();

	ID3D11InputLayout* createInputLayout(vector<D3D11_INPUT_ELEMENT_DESC>::type elems, D3D11VertexShader* vs);

public:
	void setD3D11Effect(D3D11Effect* eff);
	void setD3D11SamplerObject(ShaderType type, uint32 slot, D3D11SamplerStateObject* obj);
	void setD3D11ShaderResourceView(ShaderType type, uint32 slot, D3D11ShaderResourceView* view);
	void setD3D11ConstantBuffer(ShaderType type, uint32 slot, D3D11ConstanceBuffer* cbuffer);

	void setD3D11ConstantBuffers(ShaderType type, vector<D3D11ConstanceBuffer*>::type cbuffers);
	
	void doRender(D3D11InputLayout& layout, D3D11Effect* effect);

	D3D11_PRIMITIVE_TOPOLOGY getMapping(TopologyType type);

	void convertProjectionMatrix(const Matrix4& matrix, Matrix4& dest);
	void convertViewMatrix(const Matrix4& matrix, Matrix4& dest);

	static DXGI_FORMAT MappingFormat(ElementFormat format);
	static ElementFormat MappingFormat(DXGI_FORMAT format);
	static void Mapping(InputElemsType& elements,
		size_t stream,
		const VertexElementsType& vertex_elems,
		StreamType st, uint32 freq);

	static uint32 getElementOffset(const VertexElementsType& elements, VertexElement ve);
	static bool hasElement(const VertexElementsType& elements, VertexElement ve);
	static uint32 getElementsSize(const VertexElementsType& elements);
	static DXGI_FORMAT MakeDXGIFormatSRGB(_In_ DXGI_FORMAT format);

protected:
	void createCommState();
	void clearRenderStates();

public:
	D3D11BlendStateObject*					solid_blend_state_;
	D3D11BlendStateObject*					trans_blend_state_;
	D3D11BlendStateObject*					trans_add_blend_state_;

	D3D11DepthStencilStateObject*			default_deptph_state_;
	D3D11DepthStencilStateObject*			less_and_equal_deptph_state_;
	D3D11DepthStencilStateObject*			depth_none_state_;
	D3D11DepthStencilStateObject*			depth_read_state_;
	D3D11DepthStencilStateObject*			depth_always_state_;
	D3D11DepthStencilStateObject*			depth_sky_state_;

	D3D11RasterizerStateObject*				cull_none_state_;
	D3D11RasterizerStateObject*				cull_cw_state_;
	D3D11RasterizerStateObject*				cull_ccw_state_;
	D3D11RasterizerStateObject*				wireframe_state_;

	D3D11SamplerStateObject*				point_wrap_state_;
	D3D11SamplerStateObject*				point_clamp_state_;
	D3D11SamplerStateObject*				linear_wrap_state_;
	D3D11SamplerStateObject*				linear_clamp_state_;
	D3D11SamplerStateObject*				anisotropic_wrap_state_;
	D3D11SamplerStateObject*				anisotropic_clamp_state_;

protected:
	IDXGIFactory1*							gi_factory_;
	ID3D11Device*							device_;
	ID3D11DeviceContext*					device_ctx_;

	uint32									curr_frame_no_;
	uint32									num_batches_rendered_;
	uint32									num_prim_rendered_;

	uint32									numerator_, denominator_;
	int										video_card_memory_;
	char									video_card_desc_[128];

	D3D11VertexShader*						vertex_shader_cache_;
	D3D11PixelShader*						pixel_shader_cache_;

	D3D11RasterizerStateObject*				rs_state_cache_;

	D3D11DepthStencilStateObject*			ds_state_cache_;
	uint16									stencil_ref_cache_;

	D3D11BlendStateObject*					bs_state_cache_;
	Color									blend_factor_cache_;
	uint32									sample_mask_cache_;

	BSPool									bs_pool_;
	DSPool									ds_pool_;
	RSPool									rs_pool_;
	SSPool									ss_pool_;

	RTStack									rt_stack_;
	D3D11RenderTarget*						curr_rtt_;

	SamplersCacheList						samplers_cache_list_;
	ShaderResourceViewsCacheList			shader_res_views_cache_list_;
	ConstantBuffersCacheList				constant_buffers_cache_list_;

	D3D11Viewport							vp_cache_;
};

extern D3D11RenderDevice* g_render_device_;

}