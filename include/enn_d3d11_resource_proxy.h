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
// File: enn_d3d11_resource_proxy.h
//

#pragma once

#include "enn_d3d11_gpu_buffer.h"
#include "enn_d3d11_texture_obj.h"
#include "enn_d3d11_render_state.h"
#include "enn_d3d11_render_target.h"
#include "enn_d3d11_gpu_program.h"
#include "enn_d3d11_render_view.h"

#pragma once

namespace enn
{

class D3D11ResourceProxy : public AllocatedObject
{
public:
	D3D11ResourceProxy();

	// buffer
	D3D11ResourceProxy(D3D11Resource* res, const D3D11BufferDesc& desc, 
		D3D11ShaderResourceViewDesc* srv_desc = 0,
		D3D11RenderTargetViewDesc* rtv_desc = 0,
		D3D11UnorderedAccessViewDesc* uav_desc = 0);

	// texture 1d
	D3D11ResourceProxy(D3D11Resource* res, const D3D11Texture1DDesc& desc, 
		D3D11ShaderResourceViewDesc* srv_desc = 0,
		D3D11RenderTargetViewDesc* rtv_desc = 0,
		D3D11UnorderedAccessViewDesc* uav_desc = 0);

	// texture 2d with view provided
	D3D11ResourceProxy(D3D11Resource* res, D3D11ShaderResourceView* view,
		const D3D11Texture1DDesc& desc,
		D3D11ShaderResourceViewDesc* srv_desc = 0,
		D3D11RenderTargetViewDesc* rtv_desc = 0,
		D3D11UnorderedAccessViewDesc* uav_desc = 0,
		D3D11DepthStencilViewDesc* dsv_desc = 0);

	// texture 2d
	D3D11ResourceProxy(D3D11Resource* res, const D3D11Texture2DDesc& desc, 
		D3D11ShaderResourceViewDesc* srv_desc = 0,
		D3D11RenderTargetViewDesc* rtv_desc = 0,
		D3D11UnorderedAccessViewDesc* uav_desc = 0,
		D3D11DepthStencilViewDesc* dsv_desc = 0);

	// texture 2d with view provided
	D3D11ResourceProxy(D3D11Resource* res, D3D11ShaderResourceView* view,
		const D3D11Texture2DDesc& desc,
		D3D11ShaderResourceViewDesc* srv_desc = 0,
		D3D11RenderTargetViewDesc* rtv_desc = 0,
		D3D11UnorderedAccessViewDesc* uav_desc = 0,
		D3D11DepthStencilViewDesc* dsv_desc = 0);

	// texture 3d
	D3D11ResourceProxy(D3D11Resource* res, const D3D11Texture3DDesc& desc, 
		D3D11ShaderResourceViewDesc* srv_desc = 0,
		D3D11RenderTargetViewDesc* rtv_desc = 0,
		D3D11UnorderedAccessViewDesc* uav_desc = 0);

	D3D11ResourceProxy(D3D11Resource* res, D3D11ShaderResourceView* view);
	D3D11ResourceProxy(D3D11Resource* res, D3D11RenderTargetView* view);
	D3D11ResourceProxy(D3D11Resource* res, D3D11DepthStencilView* view);
	D3D11ResourceProxy(D3D11Resource* res, D3D11UnorderedAccessView* view);

	virtual ~D3D11ResourceProxy();

public:
	template<class T> T* getResourceT() const { return static_cast<T*>(resource_); }
	D3D11ConstanceBuffer* getCBuffer() const { return getResourceT<D3D11ConstanceBuffer>(); }
	D3D11VertexBuffer* getVertexBuffer() const { return getResourceT<D3D11VertexBuffer>(); }
	D3D11IndexBuffer* getIndexBuffer() const { return getResourceT<D3D11IndexBuffer>(); }

protected:
	void commonConstructor(uint32 bind_flags, 
		D3D11ShaderResourceViewDesc* srv_desc,
		D3D11RenderTargetViewDesc* rtv_desc,
		D3D11UnorderedAccessViewDesc* uav_desc,
		D3D11DepthStencilViewDesc* dsv_desc = 0);

public:
	D3D11Resource*								resource_;

	D3D11ShaderResourceView*					srv_view_;
	D3D11RenderTargetView*						rtv_view_;
	D3D11DepthStencilView*						dsv_view_;
	D3D11UnorderedAccessView*					uav_view_;

	D3D11BufferDesc*							buffer_desc_;
	D3D11Texture1DDesc*							tex_1d_desc_;
	D3D11Texture2DDesc*							tex_2d_desc_;
	D3D11Texture3DDesc*							tex_3d_desc_;
	D3D11DepthStencilViewDesc*					dsv_desc_;
	D3D11ShaderResourceViewDesc*				srv_desc_;
	D3D11UnorderedAccessViewDesc*				uav_desc_;
	D3D11RenderTargetViewDesc*					rtv_desc_;
};

}