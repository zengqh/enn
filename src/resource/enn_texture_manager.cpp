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
// File: enn_texture_manager.cpp
//

#include "enn_texture_manager.h"

namespace enn
{

TextureManager*		g_tex_manager_ = 0;
//////////////////////////////////////////////////////////////////////////
struct TexBuildParams
{
	TexObjLoadParams		load_params_;
	MemoryBuffer*			buff_;
};

TextureAutoResCreator::TextureAutoResCreator()
{
	m_basePath = "texture";
}

TextureAutoResCreator::~TextureAutoResCreator()
{

}

bool TextureAutoResCreator::buildResource(Resource* res)
{
	TexBuildParams* params = static_cast<TexBuildParams*>(res->getUserData());

	ENN_ASSERT(params);

	if (!params)
	{
		return false;
	}

	Texture* tex = static_cast<Texture*>(res);

	params->load_params_.auto_generate_mipmap_ = tex->getAutoGenerateMips();
	params->load_params_.use_srgb_ = tex->getSRGBFormat();
	bool ret = tex->loadTex(params->load_params_);

	ENN_ASSERT(ret);

	g_file_system_->freeFileData(params->buff_);
	ENN_SAFE_DELETE_T(params);

	return true;
}

bool TextureAutoResCreator::prepareResourceImpl(Resource* res, MemoryBuffer* buff)
{
	TexBuildParams* params = ENN_NEW_T(TexBuildParams);
	
	params->buff_ = buff;
	params->load_params_.data_ = buff->GetHandle();
	params->load_params_.data_size_ = buff->GetSize();

	res->setUserData(params);

	return true;
}

//////////////////////////////////////////////////////////////////////////
TextureManager::TextureManager() : default_(0), random_(0), normal_fit_(0)
{
	g_tex_manager_ = this;
}

TextureManager::~TextureManager()
{
	ENN_SAFE_RELEASE(default_);
	ENN_SAFE_RELEASE(random_);
}

void TextureManager::init()
{
	ResourceManager::init();

	setAutoCreator(ENN_NEW TextureAutoResCreator());

	getDefaultTexture();
	getRandomTexture();
	getNormalFitTex();
}

Texture* TextureManager::getDefaultTexture()
{
	if (default_) return default_;

	default_ = static_cast<Texture*>(createResource("$DEFAULT"));
	if (default_) default_->setResCreator(ENN_NEW TextureColorCreatorImpl(Color::WHITE));

	default_->load(false);

	return default_;
}

Texture* TextureManager::getRandomTexture()
{
	if (random_)
	{
		return random_;
	}

	random_ = static_cast<Texture*>(createResource("randomB.png"));

	random_->getSRGBFormat() = false;
	random_->load(false);
	

	return random_;
}

Texture* TextureManager::getNormalFitTex()
{
	if (normal_fit_)
	{
		return normal_fit_;
	}

	normal_fit_ = static_cast<Texture*>(createResource("NormalsFitting.dds"));

	normal_fit_->getAutoGenerateMips() = false;

	normal_fit_->getSRGBFormat() = false;

	normal_fit_->load(false);

	return normal_fit_;
}

Resource* TextureManager::createImpl()
{
	Resource* res = ENN_NEW Texture;
	res->setManager(this);
	return res;
}

}