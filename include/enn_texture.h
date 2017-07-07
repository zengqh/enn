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
// Time: 2014/04/15
// File: enn_texture.h
//

#pragma once

#include "enn_resource.h"
#include "enn_d3d11_render_device.h"

namespace enn
{

class Texture : public Resource
{
public:
	Texture();
	virtual ~Texture();

public:
	bool loadTex(const TexObjLoadParams& params);
	bool loadRawTex(const TexObjLoadRawParams& params);

	int getWidth() const;
	int getHeight() const;

	D3D11TextureObj* getTextureObj() const { return texture_obj_; }
	D3D11SamplerStateDesc& getSamplerStateDesc() { return sampler_state_desc_; }
	const D3D11SamplerStateDesc& getSamplerStateDesc() const { return sampler_state_desc_; }
	
	void updateSamplerStateObject();
	D3D11SamplerStateObject* getSamplerStateObject();

	bool& getAutoGenerateMips() { return auto_generate_mips_; }
	bool& getSRGBFormat() { return use_srgb_; }

public:
	virtual void destructResImpl();

protected:
	D3D11TextureObj* createTexObj();
	void destroy();

protected:
	D3D11TextureObj*			texture_obj_;
	D3D11SamplerStateDesc		sampler_state_desc_;
	D3D11SamplerStateObject*	sampler_state_obj_;
	bool						auto_generate_mips_;
	bool						use_srgb_;
};

typedef ResPtr<Texture>	TexturePtr;

}