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
// Time: 2014/03/07
// File: enn_d3d11_render_state.h
//

#pragma once

#include "enn_d3d11_graphics_def.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
struct D3D11BlendStateDesc : public AllocatedObject
{
	D3D11BlendStateDesc();
	~D3D11BlendStateDesc();
	void setDefault();
	void setSolid();
	void setTrans();
	void setTransAdd();

	friend bool operator<(const D3D11BlendStateDesc& lhs, const D3D11BlendStateDesc& rhs);

	D3D11_BLEND_DESC raw_;
};

//////////////////////////////////////////////////////////////////////////
struct D3D11DepthStencilStateDesc : public AllocatedObject
{
	D3D11DepthStencilStateDesc();
	~D3D11DepthStencilStateDesc();
	void setDefault();
	void setLessAndEqual();
	void setNone();
	void setReadonly();
	void setAlways();
	void setSky();

	friend bool operator<(const D3D11DepthStencilStateDesc& lhs, const D3D11DepthStencilStateDesc& rhs);

	D3D11_DEPTH_STENCIL_DESC raw_;
};

//////////////////////////////////////////////////////////////////////////
struct D3D11RasterizerStateDesc : public AllocatedObject
{
public:
	D3D11RasterizerStateDesc();
	~D3D11RasterizerStateDesc();
	void setDefault();

	void setCullNone();
	void setCullCW();
	void setCullCCW();
	void setWireframe();

	friend bool operator<(const D3D11RasterizerStateDesc& lhs, const D3D11RasterizerStateDesc& rhs);

	D3D11_RASTERIZER_DESC raw_;
};

//////////////////////////////////////////////////////////////////////////
struct D3D11SamplerStateDesc : public AllocatedObject
{
	D3D11SamplerStateDesc();
	~D3D11SamplerStateDesc();
	void setDefault();

	void setPointWrap();
	void setPointClamp();
	void setLineWrap();
	void setLineClamp();
	void setAnisotropicWrap();
	void setAnisotropicClamp();

	friend bool operator<(const D3D11SamplerStateDesc& lhs, const D3D11SamplerStateDesc& rhs);

	D3D11_SAMPLER_DESC raw_;
};


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class D3D11BlendStateObject : public AllocatedObject
{
public:
	D3D11BlendStateObject(ID3D11BlendState* state);
	~D3D11BlendStateObject();

	ID3D11BlendState* getRaw() const { return blend_state_; }

	ID3D11BlendState*			blend_state_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11DepthStencilStateObject : public AllocatedObject
{
public:
	D3D11DepthStencilStateObject(ID3D11DepthStencilState* state);
	~D3D11DepthStencilStateObject();

	ID3D11DepthStencilState* getRaw() const { return ds_state_; }

	ID3D11DepthStencilState*	ds_state_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11RasterizerStateObject : public AllocatedObject
{
public:
	D3D11RasterizerStateObject(ID3D11RasterizerState* state);
	~D3D11RasterizerStateObject();

	ID3D11RasterizerState* getRaw() const { return rs_state_; }
	
	ID3D11RasterizerState*		rs_state_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11SamplerStateObject : public AllocatedObject
{
public:
	D3D11SamplerStateObject(ID3D11SamplerState* state);
	~D3D11SamplerStateObject();

	ID3D11SamplerState* getRaw() const { return sample_state_; }

	ID3D11SamplerState*			sample_state_;
};

}