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
// File: enn_texture.cpp
//

#include "enn_texture.h"

namespace enn
{

Texture::Texture() : texture_obj_(0), sampler_state_obj_(0), auto_generate_mips_(true), use_srgb_(true)
{

}

Texture::~Texture()
{
	destroy();
}

bool Texture::loadTex(const TexObjLoadParams& params)
{
	return createTexObj()->load(params);
}

bool Texture::loadRawTex(const TexObjLoadRawParams& params)
{
	return createTexObj()->loadRaw(params);
}

int Texture::getWidth() const
{
	return texture_obj_->getWidth();
}

int Texture::getHeight() const
{
	return texture_obj_->getHeight();
}

void Texture::updateSamplerStateObject()
{
	sampler_state_obj_ = g_render_device_->createSamplerState(sampler_state_desc_);
}

D3D11SamplerStateObject* Texture::getSamplerStateObject()
{
	if (!sampler_state_obj_)
	{
		updateSamplerStateObject();
	}

	return sampler_state_obj_;
}

void Texture::destructResImpl()
{
	destroy();
}

D3D11TextureObj* Texture::createTexObj()
{
	if (texture_obj_)
	{
		return texture_obj_;
	}

	texture_obj_ = ENN_NEW D3D11TextureObj();

	return texture_obj_;
}

void Texture::destroy()
{
	ENN_SAFE_DELETE(texture_obj_);
}

}

