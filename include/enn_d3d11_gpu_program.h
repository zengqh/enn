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
// File: enn_d3d11_gpu_program.h
//

#pragma once

#include "enn_d3d11_graphics_def.h"

namespace enn
{
	
enum ShaderType
{
	VERTEX_SHADER = 0,
	HULL_SHADER = 1,
	DOMAIN_SHADER = 2,
	GEOMETRY_SHADER = 3,
	PIXEL_SHADER = 4,
	COMPUTE_SHADER = 5,
	SHADER_COUNT
};

//////////////////////////////////////////////////////////////////////////
class D3D11Shader : public AllocatedObject
{
public:
	D3D11Shader();
	virtual ~D3D11Shader();

	virtual ShaderType getShaderType() const = 0;

public:
	String		shader_text_;
	ID3DBlob*	compiled_shader_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11VertexShader : public D3D11Shader
{
public:
	D3D11VertexShader(ID3D11VertexShader* shader);
	~D3D11VertexShader();

	virtual ShaderType getShaderType() const { return VERTEX_SHADER; }
	ID3D11VertexShader* getRaw() const { return raw_; }

protected:
	ID3D11VertexShader* raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11PixelShader : public D3D11Shader
{
public:
	D3D11PixelShader(ID3D11PixelShader* shader);
	~D3D11PixelShader();

	virtual ShaderType getShaderType() const { return PIXEL_SHADER; }
	ID3D11PixelShader* getRaw() const { return raw_; }

protected:
	ID3D11PixelShader* raw_;
};

//////////////////////////////////////////////////////////////////////////
class D3D11Effect : public AllocatedObject
{
public:
	D3D11Effect();
	~D3D11Effect();

public:
	void attachShader(ShaderType type, D3D11Shader* shader);
	D3D11Shader* getShader(ShaderType type);

	void bind();

protected:
	D3D11VertexShader*				vertex_shader_;
	D3D11PixelShader*				pixel_shader_;
};

}