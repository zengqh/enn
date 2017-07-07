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
// Time: 2014/03/11
// File: enn_d3d11_gpu_program.cpp
//

#include "enn_d3d11_gpu_program.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
D3D11Shader::D3D11Shader()
: compiled_shader_(0)
{

}

D3D11Shader::~D3D11Shader()
{
	ENN_SAFE_RELEASE2(compiled_shader_);
}

//////////////////////////////////////////////////////////////////////////
D3D11VertexShader::D3D11VertexShader(ID3D11VertexShader* shader)
: raw_(shader)
{

}

D3D11VertexShader::~D3D11VertexShader()
{
	ENN_SAFE_RELEASE2(raw_);
}

//////////////////////////////////////////////////////////////////////////
D3D11PixelShader::D3D11PixelShader(ID3D11PixelShader* shader)
: raw_(shader)
{

}

D3D11PixelShader::~D3D11PixelShader()
{
	ENN_SAFE_RELEASE2(raw_);
}

//////////////////////////////////////////////////////////////////////////
D3D11Effect::D3D11Effect()
: vertex_shader_(0)
, pixel_shader_(0)
{

}

D3D11Effect::~D3D11Effect()
{
	ENN_SAFE_DELETE(vertex_shader_);
	ENN_SAFE_DELETE(pixel_shader_);
}

void D3D11Effect::attachShader(ShaderType type, D3D11Shader* shader)
{
	switch (type)
	{
	case VERTEX_SHADER:
		vertex_shader_ = static_cast<D3D11VertexShader*>(shader);
		break;
	case PIXEL_SHADER:
		pixel_shader_ = static_cast<D3D11PixelShader*>(shader);
		break;
	}
}

D3D11Shader* D3D11Effect::getShader(ShaderType type)
{
	switch (type)
	{
	case VERTEX_SHADER:
		return vertex_shader_;
	case PIXEL_SHADER:
		return pixel_shader_;
	default:
		ENN_ASSERT(0);
		return 0;
	}
}

void D3D11Effect::bind()
{
	g_render_device_->setVertexShader(vertex_shader_);
	g_render_device_->setPixelShader(pixel_shader_);
}

}