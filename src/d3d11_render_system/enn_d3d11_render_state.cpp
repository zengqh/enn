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
// Time: 2014/04/10
// File: enn_d3d11_render_state.cpp
//

#include "enn_d3d11_render_state.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
D3D11BlendStateDesc::D3D11BlendStateDesc()
{
	setDefault();
}

D3D11BlendStateDesc::~D3D11BlendStateDesc()
{
	
}

void D3D11BlendStateDesc::setDefault()
{
	raw_.AlphaToCoverageEnable = false;
	raw_.IndependentBlendEnable = false;

	for (int i = 0; i < 8; i++)
	{
		raw_.RenderTarget[i].BlendEnable = false;
		raw_.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		raw_.RenderTarget[i].DestBlend = D3D11_BLEND_ZERO;
		raw_.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		raw_.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		raw_.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ZERO;
		raw_.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		raw_.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
}

void D3D11BlendStateDesc::setSolid()
{
	setDefault();
}

void D3D11BlendStateDesc::setTrans()
{
	raw_.AlphaToCoverageEnable = false;
	raw_.IndependentBlendEnable = false;

	for (int i = 0; i < 8; i++)
	{
		raw_.RenderTarget[i].BlendEnable = true;
		raw_.RenderTarget[i].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		raw_.RenderTarget[i].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		raw_.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		raw_.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		raw_.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		raw_.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		raw_.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
}

void D3D11BlendStateDesc::setTransAdd()
{
	raw_.AlphaToCoverageEnable = false;
	raw_.IndependentBlendEnable = false;

	for (int i = 0; i < 8; i++)
	{
		raw_.RenderTarget[i].BlendEnable = true;
		raw_.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
		raw_.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
		raw_.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
		raw_.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
		raw_.RenderTarget[i].DestBlendAlpha = D3D11_BLEND_ONE;
		raw_.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		raw_.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}
}

bool operator<(const D3D11BlendStateDesc& lhs, const D3D11BlendStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

//////////////////////////////////////////////////////////////////////////
D3D11DepthStencilStateDesc::D3D11DepthStencilStateDesc()
{
	setDefault();
}

D3D11DepthStencilStateDesc::~D3D11DepthStencilStateDesc()
{

}

void D3D11DepthStencilStateDesc::setDefault()
{
	raw_.DepthEnable = true;
	raw_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	raw_.DepthFunc = D3D11_COMPARISON_LESS;
	raw_.StencilEnable = FALSE;
	raw_.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	raw_.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	raw_.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	raw_.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	raw_.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	raw_.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
}

void D3D11DepthStencilStateDesc::setLessAndEqual()
{
	raw_.DepthEnable = true;
	raw_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	raw_.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	raw_.StencilEnable = FALSE;
	raw_.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	raw_.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	raw_.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	raw_.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	raw_.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	raw_.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
}

void D3D11DepthStencilStateDesc::setNone()
{
	raw_.DepthEnable = false;
	raw_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	raw_.DepthFunc = D3D11_COMPARISON_LESS;
	raw_.StencilEnable = FALSE;
	raw_.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	raw_.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	raw_.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	raw_.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	raw_.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	raw_.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
}

void D3D11DepthStencilStateDesc::setReadonly()
{
	raw_.DepthEnable = true;
	raw_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	raw_.DepthFunc = D3D11_COMPARISON_LESS;
	raw_.StencilEnable = FALSE;
	raw_.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	raw_.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	raw_.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	raw_.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	raw_.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	raw_.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
}

void D3D11DepthStencilStateDesc::setAlways()
{
	raw_.DepthEnable = true;
	raw_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	raw_.DepthFunc = D3D11_COMPARISON_ALWAYS;
	raw_.StencilEnable = FALSE;
	raw_.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	raw_.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	raw_.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	raw_.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	raw_.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	raw_.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
}

void D3D11DepthStencilStateDesc::setSky()
{
	raw_.DepthEnable = true;
	raw_.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	raw_.DepthFunc = D3D11_COMPARISON_ALWAYS;
	raw_.StencilEnable = FALSE;
	raw_.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	raw_.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	raw_.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	raw_.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	raw_.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	raw_.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	raw_.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
}

bool operator<(const D3D11DepthStencilStateDesc& lhs, const D3D11DepthStencilStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

//////////////////////////////////////////////////////////////////////////
D3D11RasterizerStateDesc::D3D11RasterizerStateDesc()
{
	setDefault();
}

D3D11RasterizerStateDesc::~D3D11RasterizerStateDesc()
{

}

void D3D11RasterizerStateDesc::setDefault()
{
	raw_.FillMode = D3D11_FILL_SOLID;
	raw_.CullMode = D3D11_CULL_BACK;
	raw_.FrontCounterClockwise = false;
	raw_.DepthBias = 0;
	raw_.SlopeScaledDepthBias = 0.0f;
	raw_.DepthBiasClamp = 0.0f;
	raw_.DepthClipEnable = true;
	raw_.ScissorEnable = false;
	raw_.MultisampleEnable = false;
	raw_.AntialiasedLineEnable = false;
}

void D3D11RasterizerStateDesc::setCullNone()
{
	raw_.FillMode = D3D11_FILL_SOLID;
	raw_.CullMode = D3D11_CULL_NONE;
	raw_.FrontCounterClockwise = false;
	raw_.DepthBias = 0;
	raw_.SlopeScaledDepthBias = 0.0f;
	raw_.DepthBiasClamp = 0.0f;
	raw_.DepthClipEnable = true;
	raw_.ScissorEnable = false;
	raw_.MultisampleEnable = false;
	raw_.AntialiasedLineEnable = false;
}

void D3D11RasterizerStateDesc::setCullCW()
{
	raw_.FillMode = D3D11_FILL_SOLID;
	raw_.CullMode = D3D11_CULL_FRONT;
	raw_.FrontCounterClockwise = false;
	raw_.DepthBias = 0;
	raw_.SlopeScaledDepthBias = 0.0f;
	raw_.DepthBiasClamp = 0.0f;
	raw_.DepthClipEnable = true;
	raw_.ScissorEnable = false;
	raw_.MultisampleEnable = false;
	raw_.AntialiasedLineEnable = false;
}

void D3D11RasterizerStateDesc::setCullCCW()
{
	raw_.FillMode = D3D11_FILL_SOLID;
	raw_.CullMode = D3D11_CULL_BACK;
	raw_.FrontCounterClockwise = false;
	raw_.DepthBias = 0;
	raw_.SlopeScaledDepthBias = 0.0f;
	raw_.DepthBiasClamp = 0.0f;
	raw_.DepthClipEnable = true;
	raw_.ScissorEnable = false;
	raw_.MultisampleEnable = false;
	raw_.AntialiasedLineEnable = false;
}

void D3D11RasterizerStateDesc::setWireframe()
{
	raw_.FillMode = D3D11_FILL_WIREFRAME;
	raw_.CullMode = D3D11_CULL_FRONT;
	raw_.FrontCounterClockwise = false;
	raw_.DepthBias = 0;
	raw_.SlopeScaledDepthBias = 0.0f;
	raw_.DepthBiasClamp = 0.0f;
	raw_.DepthClipEnable = true;
	raw_.ScissorEnable = false;
	raw_.MultisampleEnable = false;
	raw_.AntialiasedLineEnable = false;
}

bool operator<(const D3D11RasterizerStateDesc& lhs, const D3D11RasterizerStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

//////////////////////////////////////////////////////////////////////////
D3D11SamplerStateDesc::D3D11SamplerStateDesc()
{
	setDefault();
}

D3D11SamplerStateDesc::~D3D11SamplerStateDesc()
{

}

void D3D11SamplerStateDesc::setDefault()
{
	raw_.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	raw_.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.MipLODBias = 0.0f;
	raw_.MaxAnisotropy = 16;
	raw_.ComparisonFunc = D3D11_COMPARISON_NEVER;
	raw_.BorderColor[0] = 0.0f; raw_.BorderColor[1] = 0.0f; raw_.BorderColor[2] = 0.0f; raw_.BorderColor[3] = 0.0f;
	raw_.MinLOD = 0;
	raw_.MaxLOD = D3D11_FLOAT32_MAX;
}

void D3D11SamplerStateDesc::setPointWrap()
{
	raw_.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	raw_.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.MipLODBias = 0.0f;
	raw_.MaxAnisotropy = 16;
	raw_.ComparisonFunc = D3D11_COMPARISON_NEVER;
	raw_.BorderColor[0] = 0.0f; raw_.BorderColor[1] = 0.0f; raw_.BorderColor[2] = 0.0f; raw_.BorderColor[3] = 0.0f;
	raw_.MinLOD = 0;
	raw_.MaxLOD = D3D11_FLOAT32_MAX;
}

void D3D11SamplerStateDesc::setPointClamp()
{
	raw_.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	raw_.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	raw_.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	raw_.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	raw_.MipLODBias = 0.0f;
	raw_.MaxAnisotropy = 16;
	raw_.ComparisonFunc = D3D11_COMPARISON_NEVER;
	raw_.BorderColor[0] = 0.0f; raw_.BorderColor[1] = 0.0f; raw_.BorderColor[2] = 0.0f; raw_.BorderColor[3] = 0.0f;
	raw_.MinLOD = 0;
	raw_.MaxLOD = D3D11_FLOAT32_MAX;
}

void D3D11SamplerStateDesc::setLineWrap()
{
	raw_.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	raw_.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.MipLODBias = 0.0f;
	raw_.MaxAnisotropy = 16;
	raw_.ComparisonFunc = D3D11_COMPARISON_NEVER;
	raw_.BorderColor[0] = 0.0f; raw_.BorderColor[1] = 0.0f; raw_.BorderColor[2] = 0.0f; raw_.BorderColor[3] = 0.0f;
	raw_.MinLOD = 0;
	raw_.MaxLOD = D3D11_FLOAT32_MAX;
}

void D3D11SamplerStateDesc::setLineClamp()
{
	raw_.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	raw_.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	raw_.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	raw_.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	raw_.MipLODBias = 0.0f;
	raw_.MaxAnisotropy = 16;
	raw_.ComparisonFunc = D3D11_COMPARISON_NEVER;
	raw_.BorderColor[0] = 0.0f; raw_.BorderColor[1] = 0.0f; raw_.BorderColor[2] = 0.0f; raw_.BorderColor[3] = 0.0f;
	raw_.MinLOD = 0;
	raw_.MaxLOD = D3D11_FLOAT32_MAX;
}

void D3D11SamplerStateDesc::setAnisotropicWrap()
{
	raw_.Filter = D3D11_FILTER_ANISOTROPIC;
	raw_.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	raw_.MipLODBias = 0.0f;
	raw_.MaxAnisotropy = 16;
	raw_.ComparisonFunc = D3D11_COMPARISON_NEVER;
	raw_.BorderColor[0] = 0.0f; raw_.BorderColor[1] = 0.0f; raw_.BorderColor[2] = 0.0f; raw_.BorderColor[3] = 0.0f;
	raw_.MinLOD = 0;
	raw_.MaxLOD = D3D11_FLOAT32_MAX;
}

void D3D11SamplerStateDesc::setAnisotropicClamp()
{
	raw_.Filter = D3D11_FILTER_ANISOTROPIC;
	raw_.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	raw_.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	raw_.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	raw_.MipLODBias = 0.0f;
	raw_.MaxAnisotropy = 16;
	raw_.ComparisonFunc = D3D11_COMPARISON_NEVER;
	raw_.BorderColor[0] = 0.0f; raw_.BorderColor[1] = 0.0f; raw_.BorderColor[2] = 0.0f; raw_.BorderColor[3] = 0.0f;
	raw_.MinLOD = 0;
	raw_.MaxLOD = D3D11_FLOAT32_MAX;
}

bool operator<(const D3D11SamplerStateDesc& lhs, const D3D11SamplerStateDesc& rhs)
{
	return std::memcmp(&lhs, &rhs, sizeof(lhs)) < 0;
}

//////////////////////////////////////////////////////////////////////////
D3D11BlendStateObject::D3D11BlendStateObject(ID3D11BlendState* state) : blend_state_(state)
{
		
}

D3D11BlendStateObject::~D3D11BlendStateObject()
{
	ENN_SAFE_RELEASE2(blend_state_);
}

//////////////////////////////////////////////////////////////////////////
D3D11DepthStencilStateObject::D3D11DepthStencilStateObject(ID3D11DepthStencilState* state) : ds_state_(state)
{

}

D3D11DepthStencilStateObject::~D3D11DepthStencilStateObject()
{
	ENN_SAFE_RELEASE2(ds_state_);
}

//////////////////////////////////////////////////////////////////////////
D3D11RasterizerStateObject::D3D11RasterizerStateObject(ID3D11RasterizerState* state) : rs_state_(state)
{

}

D3D11RasterizerStateObject::~D3D11RasterizerStateObject()
{
	ENN_SAFE_RELEASE2(rs_state_);
}

//////////////////////////////////////////////////////////////////////////
D3D11SamplerStateObject::D3D11SamplerStateObject(ID3D11SamplerState* state) : sample_state_(state)
{

}

D3D11SamplerStateObject::~D3D11SamplerStateObject()
{
	ENN_SAFE_RELEASE2(sample_state_);
}

}