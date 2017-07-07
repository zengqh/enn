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
// File: enn_material.cpp
//

#include "enn_material.h"
#include "enn_texture_manager.h"

namespace enn
{

Material::Material()
: is_lighting_enable_(false)
, is_point_light_enable_(true)
, is_spot_light_enable_(true)
, is_enable_fog_(0)
, is_wire_frame_(0)
, cull_mode_(CULL_CW)
, custom_alpha_value_(1.0f)
, custom_alpha_ref_(1.0f)
, m_texDiff(0)
, m_texDiff2(0)
, m_texNormal(0)
, m_texSpecular(0)
, m_texEmissive(0)
, tex_clr_(0)
, emissive_(Color::BLACK)
, diffuse_(Color::WHITE)
{

}

Material::~Material()
{
	clearTexDiff();
	clearTexDiff2();
	clearTexNormal();
	clearTexSpecular();
	clearTexEmissive();
	clearTexClr();
}

const String& Material::getName() const
{
	return name_;
}

void Material::setName(const String& name)
{
	name_ = name;
}

bool Material::load(Deserializer& source)
{
	XMLFile* xml = ENN_NEW XMLFile;

	xml->Load(source);

	XMLElement materialElem = xml->GetRoot("material");

	// texture
	if (XMLElement diff_tex_elem = materialElem.GetChild("tex_diff"))
	{
		String diff_str_name = diff_tex_elem.GetAttribute("value");
		useTexDiff()->setTexName(diff_str_name);
	}

	if (XMLElement diff_tex_elem2 = materialElem.GetChild("tex_diff2"))
	{
		String diff_str_name2 = diff_tex_elem2.GetAttribute("value");
		useTexDiff2()->setTexName(diff_str_name2);
	}
	
	if (XMLElement normal_tex_elem = materialElem.GetChild("tex_normal"))
	{
		String normal_str_name = normal_tex_elem.GetAttribute("value");

		useTexNormal()->useSRGB() = false;
		useTexNormal()->setTexName(normal_str_name);
	}

	if (XMLElement spec_tex_elem = materialElem.GetChild("tex_spec"))
	{
		String spec_str_name = spec_tex_elem.GetAttribute("value");
		useTexSpecular()->setTexName(spec_str_name);
	}
	

	if (XMLElement emissive_tex_elem = materialElem.GetChild("tex_emissive"))
	{
		String emissive_str_name = emissive_tex_elem.GetAttribute("value");

		useTexEmissive()->setTexName("emissive_str_name");
	}
	

	// color
	if (XMLElement diff_clr_elem = materialElem.GetChild("clr_diffuse"))
	{
		Color diff_color = diff_clr_elem.GetColor("value");
		setDiffuse(diff_color);
	}
	
	if (XMLElement spec_clr_elem = materialElem.GetChild("clr_specular_shiness"))
	{
		Color spec_color = spec_clr_elem.GetColor("value");
		setSpecularShiness(spec_color);
	}
	
	if (XMLElement emissive_clr_elem = materialElem.GetChild("clr_emissive"))
	{
		Color emissive_color = emissive_clr_elem.GetColor("value");
		setEmissive(emissive_color);
	}
	
	if (XMLElement tex_clr_elem = materialElem.GetChild("clr_tex"))
	{
		Color tex_color = tex_clr_elem.GetColor("value");
		setTexClr(tex_color);
	}

	// enable lighting
	if (XMLElement en_lit_elem = materialElem.GetChild("en_lit"))
	{
		bool enalbe_lit = en_lit_elem.GetBool("value");
		enableLighting(enalbe_lit);
	}
	

	// set wireframe
	if (XMLElement wire_frame_elem = materialElem.GetChild("is_wire_frame"))
	{
		bool is_wireframe = wire_frame_elem.GetBool("value");
		setWireFrame(is_wireframe);
	}
	

	// alpha
	if (XMLElement alpha_info_elem = materialElem.GetChild("alpha_value"))
	{
		float alpha = alpha_info_elem.GetFloat("value");
		setAlpha(alpha);
	}
	

	// alpha ref
	if (XMLElement alpha_test_elem = materialElem.CreateChild("alpha_ref"))
	{
		float alpha_ref = alpha_test_elem.GetFloat("value");
		setAlphaTest(alpha_ref);
	}
	

	return true;
}

bool Material::save(Serializer& dest)
{
	XMLFile* xml = ENN_NEW XMLFile;

	XMLElement materialElem = xml->CreateRoot("material");

	// texture diffuse
	if (m_texDiff)
	{
		XMLElement diff_tex_elem = materialElem.CreateChild("tex_diff");
		diff_tex_elem.SetString("value", m_texDiff->getTexName());
	}

	// texture diffuse 2
	if (m_texDiff2)
	{
		XMLElement diff2_tex_elem = materialElem.CreateChild("tex_diff2");
		diff2_tex_elem.SetString("value", m_texDiff2->getTexName());
	}

	// normal map
	if (m_texNormal)
	{
		XMLElement norm_tex_elem = materialElem.CreateChild("tex_normal");
		norm_tex_elem.SetString("value", m_texNormal->getTexName());
	}

	// specular map
	if (m_texSpecular)
	{
		XMLElement spec_tex_elem = materialElem.CreateChild("tex_spec");
		spec_tex_elem.SetString("value", m_texSpecular->getTexName());
	}

	// emissive map
	if (m_texEmissive)
	{
		XMLElement emis_tex_elem = materialElem.CreateChild("tex_emissive");
		emis_tex_elem.SetString("value", m_texEmissive->getTexName());
	}

	// diffuse color
	if (needSaveDiffuse())
	{
		XMLElement diff_clr_elem = materialElem.CreateChild("clr_diffuse");
		diff_clr_elem.SetColor("value", getDiffuse());
	}

	// specular color
	if (needSaveSpecularShiness())
	{
		XMLElement spec_clr_elem = materialElem.CreateChild("clr_specular_shiness");
		spec_clr_elem.SetColor("value", getSpecularShiness());
	}

	// emissive color
	if (needSaveEmissive())
	{
		XMLElement emis_clr_elem = materialElem.CreateChild("clr_emissive");
		emis_clr_elem.SetColor("value", getEmissive());
	}

	if (getTexClr())
	{
		XMLElement tex_clr_elem = materialElem.CreateChild("clr_tex");
		tex_clr_elem.SetColor("value", *getTexClr());
	}

	if (needSaveEnLit())
	{
		XMLElement en_lit_elem = materialElem.CreateChild("en_lit");
		en_lit_elem.SetBool("value", true);
	}

	if (needSaveWireFrame())
	{
		XMLElement wire_frame_elem = materialElem.CreateChild("is_wire_frame");
		wire_frame_elem.SetBool("value", isWireFrame());
	}


	if (hasAlpha())
	{
		XMLElement alpha_info_elem = materialElem.CreateChild("alpha_value");
		alpha_info_elem.SetFloat("value", getAlpha());
	}

	if (hasAlphaTest())
	{
		XMLElement alpha_test_elem = materialElem.CreateChild("alpha_ref");
		alpha_test_elem.SetFloat("value", getAlphaTest());
	}

	// texture animation
	/*
	if (TextureAnimation* tex_ani = mtr->getTexAni())
	{
		XMLElement tex_ani_elem = materialElem.CreateChild("tex_animation");

		XMLElement u_speed_elem = tex_ani_elem.CreateChild("u_speed");
		u_speed_elem.SetFloat("value", tex_ani->getUSpeed());

		XMLElement v_speed_elem = tex_ani_elem.CreateChild("v_speed");
		v_speed_elem.SetFloat("value", tex_ani->getVSpeed());

		XMLElement u_scale_elem = tex_ani_elem.CreateChild("u_scale");
		u_scale_elem.SetFloat("value", tex_ani->getUScale());

		XMLElement v_scale_elem = tex_ani_elem.CreateChild("v_scale");
		v_speed_elem.SetFloat("value", tex_ani->getVScale());

		if (tex_ani->getSliceFrames())
		{
			XMLElement slice_frame_elem = tex_ani_elem.CreateChild("slice_frame");

			XMLElement slice_nums_elem = slice_frame_elem.CreateChild("slice_nums");
			slice_nums_elem.SetInt("value", tex_ani->getSliceFrames());

			XMLElement slice_frames_per_row_elem = slice_frame_elem.CreateChild("slice_frames_per_row");
			slice_frames_per_row_elem.SetInt("value", tex_ani->getSliceFramesPerRow());

			XMLElement slice_width_elem = slice_frame_elem.CreateChild("slice_width");
			slice_width_elem.SetFloat("value", tex_ani->getSliceWidth());

			XMLElement slice_height_elem = slice_frame_elem.CreateChild("slice_height");
			slice_height_elem.SetFloat("value", tex_ani->getSliceHeight());

			XMLElement slice_interval_elem = slice_height_elem.CreateChild("slice_interval");
			slice_interval_elem.SetFloat("value", tex_ani->getSliceInterval());
		}
	}
	*/

	xml->Save(dest);

	ENN_SAFE_DELETE(xml);

	return true;
}

const Color& Material::getDiffuse() const
{
	return diffuse_;
}

Color& Material::getDiffuse()
{
	return diffuse_;
}

const Color& Material::getSpecularShiness() const
{
	return specular_;
}

Color& Material::getSpecularShiness()
{
	return specular_;
}

const Color& Material::getEmissive() const
{
	return emissive_;
}

Color& Material::getEmissive()
{
	return emissive_;
}

void Material::setDiffuse(const Color& clr)
{
	diffuse_ = clr;
}

void Material::setSpecularShiness(const Color& clr)
{
	specular_ = clr;
}

void Material::setEmissive(const Color& clr)
{
	emissive_ = clr;
}

bool Material::isEnableLighting() const
{
	return is_lighting_enable_;
}

void Material::enableLighting(bool en)
{
	is_lighting_enable_ = en;
}

bool Material::isEnablePointLighting() const
{
	return is_point_light_enable_;
}

void Material::enablePointLighting(bool en)
{
	is_point_light_enable_ = en;
}

bool Material::isEnableSpotLighting() const
{
	return is_spot_light_enable_;
}

void Material::enableSpotLighting(bool en)
{
	is_spot_light_enable_ = en;
}

bool Material::isEnableFog() const
{
	return is_enable_fog_;
}

void Material::enableFog(bool en)
{
	is_enable_fog_ = en;
}

bool Material::hasSpecular() const
{
	if (!Math::realEqual(specular_.r, 0.0f)
		|| !Math::realEqual(specular_.g, 0.0f)
		|| !Math::realEqual(specular_.b, 0.0f))
	{
		return true;
	}

	return false;
}

bool Material::hasEmissive() const
{
	if (!Math::realEqual(emissive_.r, 0.0f)
		|| !Math::realEqual(emissive_.g, 0.0f)
		|| !Math::realEqual(emissive_.b, 0.0f))
	{
		return true;
	}

	return false;
}

bool Material::hasAlpha() const
{
	if (!Math::realEqual(custom_alpha_value_, 1.0f))
	{
		return true;
	}

	return false;
}

float Material::getAlpha() const
{
	return custom_alpha_value_;
}

void Material::setAlpha(float a)
{
	custom_alpha_value_ = a;
}

void Material::setCullMode(int cm)
{
	cull_mode_ = cm;
}

int Material::getCullMode() const
{
	return cull_mode_;
}

void Material::setWireFrame(bool en)
{
	is_wire_frame_ = en;
}

bool Material::isWireFrame() const
{
	return is_wire_frame_;
}

void Material::setAlphaTest(float a)
{
	custom_alpha_ref_ = a;
}

float Material::getAlphaTest() const
{
	return custom_alpha_ref_;
}

bool Material::hasAlphaTest() const
{
	return (!Math::realEqual(custom_alpha_ref_, 1.0f));
}

void Material::disableAlphaTest()
{
	custom_alpha_ref_ = 1.0f;
}

void Material::setTexClr(const Color& clr)
{
	if (!tex_clr_)
	{
		tex_clr_ = ENN_NEW_T(Color);
	}

	*tex_clr_ = clr;
}

Color* Material::getTexClr() const
{
	return tex_clr_;
}

void Material::clearTexClr()
{
	ENN_SAFE_DELETE_T(tex_clr_);
}

#define IMPL_METHOD_USE_TEX_(name) \
TextureObjUnit* Material::useTex##name() \
{ \
	if (m_tex##name) return m_tex##name; \
	m_tex##name = ENN_NEW TextureObjUnit; \
	return m_tex##name; \
} \
\
void Material::clearTex##name() \
{ \
	ENN_SAFE_DELETE(m_tex##name); \
}

IMPL_METHOD_USE_TEX_(Diff)
IMPL_METHOD_USE_TEX_(Diff2)
IMPL_METHOD_USE_TEX_(Normal)
IMPL_METHOD_USE_TEX_(Specular)
IMPL_METHOD_USE_TEX_(Emissive)

#undef IMPL_METHOD_USE_TEX_

bool Material::needSaveDiffuse() const
{
	if (!Math::realEqual(diffuse_.r, 0.0f)
		|| !Math::realEqual(diffuse_.g, 0.0f)
		|| !Math::realEqual(diffuse_.b, 0.0f))
	{
		return true;
	}

	return false;
}

bool Material::needSaveSpecularShiness() const
{
	if (!Math::realEqual(specular_.r, 0.0f)
		|| !Math::realEqual(specular_.g, 0.0f)
		|| !Math::realEqual(specular_.b, 0.0f))
	{
		return true;
	}

	return false;
}

bool Material::needSaveEmissive() const
{
	if (!Math::realEqual(emissive_.r, 0.0f)
		|| !Math::realEqual(emissive_.g, 0.0f)
		|| !Math::realEqual(emissive_.b, 0.0f))
	{
		return true;
	}

	return false;
}

bool Material::needSaveEnLit() const
{
	return !is_lighting_enable_;
}

bool Material::needSaveEnFog() const
{
	return is_enable_fog_;
}

bool Material::needSaveWireFrame() const
{
	return is_wire_frame_;
}

bool Material::needSaveCullMode() const
{
	return cull_mode_ != CULL_CW;
}

//////////////////////////////////////////////////////////////////////////
TextureObjUnit::TextureObjUnit()
: load_async_(false), use_srgb_(true)
{
	sampler_desc_.setLineWrap();
}

TextureObjUnit::~TextureObjUnit()
{

}

void TextureObjUnit::setTexName(const String& name)
{
	tex_name_ = name;

	if (!tex_name_.empty())
	{
		Texture* tex = static_cast<Texture*>(g_tex_manager_->getOrCreateResource(tex_name_));

		tex->setLoadAsync(load_async_);
		tex->getSRGBFormat() = use_srgb_;

		tex->getSamplerStateDesc() = sampler_desc_;

		tex->checkLoad();

		tex_.attach(tex);
	}
}

void TextureObjUnit::setTexNameNoLoading(const String& name)
{
	tex_name_ = name;
}

void TextureObjUnit::setLoadAsync(bool b)
{
	load_async_ = b;
}

}