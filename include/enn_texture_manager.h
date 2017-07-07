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
// File: enn_texture_manager.h
//

#pragma once

#include "enn_resource_manager.h"
#include "enn_texture.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class TextureAutoResCreator : public ResAutoCreatorBase
{
public:
	TextureAutoResCreator();
	virtual ~TextureAutoResCreator();

public:
	virtual bool buildResource(Resource* res);

protected:
	virtual bool prepareResourceImpl(Resource* res, MemoryBuffer* buff);
};

//////////////////////////////////////////////////////////////////////////
class TextureColorCreatorImpl : public AllocatedObject, public IResourceCreator
{
public:
	TextureColorCreatorImpl(const Color& clr) : m_color(clr) {}

	virtual void release()
	{
		ENN_DELETE this;
	}

	virtual bool prepareResource(Resource* res, void* context)
	{
		return true;
	}

	virtual bool buildResource(Resource* res)
	{
		const int texSize = 64;

		uchar* data = (uchar*)ENN_MALLOC(texSize * texSize * 4);

		for (int y = 0; y < texSize; ++y)
		{
			for (int x = 0; x < texSize; ++x)
			{
				uchar* curr = data + y * texSize * 4 + x * 4;

				curr[0] = static_cast<uchar>(m_color.r * 0xff);
				curr[1] = static_cast<uchar>(m_color.g * 0xff);
				curr[2] = static_cast<uchar>(m_color.b * 0xff);
				curr[3] = static_cast<uchar>(m_color.a * 0xff);
			}
		}

		Texture* tex = static_cast<Texture*>(res);

		TexObjLoadRawParams params;

		params.width_ = texSize;
		params.height_ = texSize;
		params.format_ = DXGI_FORMAT_R8G8B8A8_UNORM;
		params.data_ = data;

		D3D11SamplerStateDesc& desc = tex->getSamplerStateDesc();
		desc.setDefault();

		ENN_ASSERT_IMPL(tex->loadRawTex(params));
		ENN_SAFE_FREE(data);

		return true;
	}

private:
	Color m_color;
};

//////////////////////////////////////////////////////////////////////////
class TextureManager : public ResourceManager
{
public:
	TextureManager();
	virtual ~TextureManager();

public:
	virtual void init();

	Texture* getDefaultTexture();
	Texture* getRandomTexture();
	Texture* getNormalFitTex();

	KHAOS_RESMAN_COMM_OVERRIDE(Texture)

protected:
	virtual Resource* createImpl();

private:
	Texture*			default_;
	Texture*			random_;
	Texture*			normal_fit_;
};

extern TextureManager*		g_tex_manager_;

}