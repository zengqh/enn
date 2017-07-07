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
// Time: 2014/03/14
// File: enn_material.h
//

#pragma once

#include "enn_resource.h"
#include "enn_d3d11_render_state.h"
#include "enn_texture.h"

namespace enn
{

class Texture;
class TextureObjUnit;
class TextureAnimation;

//////////////////////////////////////////////////////////////////////////
class Material : public Resource
{
	ENN_DECLARE_RTTI(Material)

public:
	Material();
	virtual ~Material();

	void setName(const String& mtr_name);
	const String& getName() const;

	bool load(Deserializer& source);
	bool save(Serializer& dest);

public:
	void setDiffuse(const Color& clr);
	void setSpecularShiness(const Color& clr);
	void setEmissive(const Color& clr);

	const Color& getDiffuse() const;
	Color& getDiffuse();

	const Color& getSpecularShiness() const;
	Color& getSpecularShiness();

	const Color& getEmissive() const;
	Color& getEmissive();

public:
	bool isEnableLighting() const;
	void enableLighting(bool en);

	bool isEnablePointLighting() const;
	void enablePointLighting(bool en);

	bool isEnableSpotLighting() const;
	void enableSpotLighting(bool en);

	bool isEnableFog() const;
	void enableFog(bool en);

public:
	bool hasSpecular() const;
	bool hasEmissive() const;
	bool hasAlpha() const;

	float getAlpha() const;
	void setAlpha(float a);

	void setCullMode(int cm);
	int getCullMode() const;

	void setWireFrame(bool en);
	bool isWireFrame() const;

	void setAlphaTest(float a);
	float getAlphaTest() const;
	bool hasAlphaTest() const;
	void disableAlphaTest();

	void setTexClr(const Color& clr);
	Color* getTexClr() const;
	void clearTexClr();

	bool needSaveDiffuse() const;
	bool needSaveSpecularShiness() const;
	bool needSaveEmissive() const;
	bool needSaveEnLit() const;
	bool needSaveEnFog() const;
	bool needSaveWireFrame() const;
	bool needSaveCullMode() const;

public:

#define DEF_METHOD_USE_TEX_(name) \
public:	\
	TextureObjUnit* useTex##name(); \
	void clearTex##name(); \
	const TextureObjUnit* getTex##name() const { return m_tex##name; } \
	TextureObjUnit* getTex##name() { return m_tex##name; } \
protected: \
	TextureObjUnit* m_tex##name;

	DEF_METHOD_USE_TEX_(Diff)
	DEF_METHOD_USE_TEX_(Diff2)
	DEF_METHOD_USE_TEX_(Normal)
	DEF_METHOD_USE_TEX_(Specular)
	DEF_METHOD_USE_TEX_(Emissive)

#undef DEF_METHOD_USE_TEX_

protected:
	String							name_;
	
	Color							diffuse_;
	Color							specular_;
	Color							emissive_;

	Color*							tex_clr_;		

	bool							is_lighting_enable_;
	bool							is_enable_fog_;

	bool							is_point_light_enable_;
	bool							is_spot_light_enable_;

	bool							is_wire_frame_;

	int								cull_mode_;
	float							custom_alpha_value_;
	float							custom_alpha_ref_;
};

//////////////////////////////////////////////////////////////////////////
class TextureObjUnit : public AllocatedObject
{
public:
	TextureObjUnit();
	virtual ~TextureObjUnit();

public:
	const String& getTexName() const { return tex_name_; }
	void setTexName(const String& name);
	void setTexNameNoLoading(const String& name);

	void setLoadAsync(bool b);

	D3D11SamplerStateDesc& getSamplerDesc() { return sampler_desc_; }
	const D3D11SamplerStateDesc& getSamplerDesc() const { return sampler_desc_; }

	Texture* getTexture() const { return tex_.get(); }
	bool& useSRGB() { return use_srgb_; }

protected:
	String							tex_name_;
	D3D11SamplerStateDesc			sampler_desc_;
	bool							load_async_;
	bool							use_srgb_;
	TexturePtr						tex_;
};

typedef ResPtr<Material> MaterialPtr;

}
