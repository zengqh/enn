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
// Time: 2014/03/13
// File: enn_effect_template.cpp
//

#include "enn_effect_template.h"
#include "enn_root.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
// EffectMethod
#define _IMPL_EFF_METHOD(item, i) \
void EffectMethod::enable##item() \
{ \
types_.set_flag(i); \
} \
bool EffectMethod::has##item() const \
{ \
return types_.test_flag(i); \
}

_IMPL_EFF_METHOD(ColorMap,       0)
_IMPL_EFF_METHOD(DiffuseMap,     1)
_IMPL_EFF_METHOD(Diffuse2Map,    2)
_IMPL_EFF_METHOD(NormalMap,      3)
_IMPL_EFF_METHOD(SpecularMap,    4)
_IMPL_EFF_METHOD(EmissiveMap,	 5)
_IMPL_EFF_METHOD(Specular,       6)
_IMPL_EFF_METHOD(Emissive,       7)
_IMPL_EFF_METHOD(Lighting,       8)
_IMPL_EFF_METHOD(Fog,            9)
_IMPL_EFF_METHOD(TexAni,         10)
_IMPL_EFF_METHOD(Shadow,         11)
_IMPL_EFF_METHOD(Skeleton,       12)
_IMPL_EFF_METHOD(AlphaTest,      13)
_IMPL_EFF_METHOD(DirLit,		 14)
_IMPL_EFF_METHOD(PointLits,		 15)
_IMPL_EFF_METHOD(SpotLits,		 16)
_IMPL_EFF_METHOD(PSSM,			 17)
_IMPL_EFF_METHOD(DETAIL_LEVEL1,  18)
_IMPL_EFF_METHOD(DETAIL_LEVEL2,  19)
_IMPL_EFF_METHOD(DETAIL_LEVEL3,  20)
_IMPL_EFF_METHOD(DETAIL_LEVEL4,  21)
_IMPL_EFF_METHOD(UNDERWATER,	 22)

//////////////////////////////////////////////////////////////////////////
// EffectTemplate
EffectTemplate::EffectTemplate(EffectTemplateManager* owner)
: owner_(owner)
{

}

EffectTemplate::~EffectTemplate()
{
	destory();
}

void EffectTemplate::destory()
{
	ENN_FOR_EACH(EffectContextMap, eff_ctx_map_, it)
	{
		EffectContext* ec = it->second;
		ENN_SAFE_DELETE(ec);
	}

	eff_ctx_map_.clear();
}

EffectMethod EffectTemplate::getMethod(Renderable* ra)
{
	EffectMethod em;

	Material* mtr = ra->getMaterial();
	LightsInfo* light_info = ra->getLightsInfo();

	if (mtr->isEnableLighting() && light_info)
	{
		em.enableLighting();
		em.enableDirLit();

		if (mtr->isEnablePointLighting())
		{
			em.enablePointLits();
		}

		if (mtr->isEnableSpotLighting())
		{
			em.enableSpotLits();
		}

		if (mtr->hasSpecular())
		{
			em.enableSpecular();
		}
	}

	if (mtr->hasEmissive())
	{
		em.enableEmissive();
	}

	if (mtr->getTexClr())
	{
		em.enableColorMap();
	}

	if (mtr->getTexDiff())
	{
		em.enableDiffuseMap();
	}

	if (mtr->getTexDiff2())
	{
		em.enableDiffuse2Map();
	}

	if (mtr->getTexNormal())
	{
		em.enableNormalMap();
	}

	if (mtr->isEnableFog())
	{
		em.enableFog();
	}

	if (mtr->hasAlphaTest())
	{
		em.enableAlphaTest();
	}

	if (0)
	{
		em.enableSkeleton();
	}


	return em;
}

String EffectTemplate::makeMacro(const EffectMethod& em)
{
	String strMacro;

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	addMacro(strMacro, "ENN_PLATFORM_WIN32");
#elif ENN_PLATFORM == ENN_PLATFORM_ANDROID
	addMacro(strMacro, "ENN_PLATFORM_ANDROID");
#elif ENN_PLATFORM == ENN_PLATFORM_IOS
	addMacro(strMacro, "ENN_PLATFORM_IOS");
#else
	ENN_ASSERT(0);
#endif

	if (em.hasColorMap())
	{
		addMacro(strMacro, "HAS_COLORMAP");
	}

	if (em.hasDiffuseMap())
	{
		addMacro(strMacro, "HAS_DIFFUSEMAP");
	}

	if ( em.hasDiffuse2Map() )
	{
		addMacro(strMacro, "HAS_DIFFUSE2MAP");
	}

	if ( em.hasNormalMap() )
	{
		addMacro(strMacro, "HAS_NORMALMAP");
	}

	if ( em.hasSpecularMap() )
	{
		addMacro(strMacro, "HAS_SPECULARMAP");
	}

	if ( em.hasSpecular() )
	{
		addMacro(strMacro, "HAS_SPECULAR");
	}

	if ( em.hasEmissive() )
	{
		addMacro(strMacro, "HAS_EMISSIVE");
	}

	if ( em.hasLighting() )
	{
		addMacro(strMacro, "HAS_LIGHTING");

		if (em.hasDirLit())
		{
			addMacro(strMacro, "HAS_DIR_LITS");
		}

		if (em.hasPointLits())
		{
			addMacro(strMacro, "HAS_POINT_LITS");
		}

		if (em.hasSpotLits())
		{
			addMacro(strMacro, "HAS_SPOT_LITS");
		}

		if (em.hasSpecular())
		{
			addMacro(strMacro, "HAS_SPECULAR");
		}

		if (em.hasShadow())
		{
			addMacro(strMacro, "HAS_SHADOW");

			if (em.hasPSSM())
			{
				addMacro(strMacro, "HAS_PSSM");
			}
		}
	}


	if ( em.hasFog() )
	{
		addMacro(strMacro, "HAS_FOG");
	}

	if ( em.hasTexAni() )
	{
		addMacro(strMacro, "HAS_TEXANI");
	}

	if ( em.hasSkeleton() )
	{
		addMacro(strMacro, "HAS_SKELETON");
	}

	if ( em.hasAlphaTest() )
	{
		addMacro(strMacro, "HAS_ALPHA_TEST");
	}

	if ( em.hasEmissiveMap() )
	{
		addMacro(strMacro, "HAS_EMISSMAP");
	}

	if ( em.hasDiffuseMap() || em.hasNormalMap() || em.hasSpecularMap() )
	{
		addMacro(strMacro, "HAS_UV1");
	}

	if (em.hasDiffuse2Map())
	{
		addMacro(strMacro, "HAS_UV2");
	}

	if (em.hasDETAIL_LEVEL1())
	{
		addMacro(strMacro, "DETAIL_LEVEL1", 1);
	}

	if (em.hasDETAIL_LEVEL2())
	{
		addMacro(strMacro, "DETAIL_LEVEL2", 1);
	}

	if (em.hasDETAIL_LEVEL3())
	{
		addMacro(strMacro, "DETAIL_LEVEL3", 1);
	}

	if (em.hasDETAIL_LEVEL4())
	{
		addMacro(strMacro, "DETAIL_LEVEL4", 1);
	}

	if (em.hasUNDERWATER())
	{
		addMacro(strMacro, "UNDERWATER");
	}

	return strMacro;
}

void EffectTemplate::applyPipelineResources()
{
	bindShaderPrograms();
	bindConstantBuffers();
	bindSamplerObjects();
	bindShaderResourceViews();
	bindUnorderedAccessViews();
}

void EffectTemplate::addMacro(String& str_macro, const String& mac_name)
{
	str_macro += "#define " + mac_name + "\n";
}

void EffectTemplate::addMacro(String& str_macro, const String& mac_name, int val)
{
	String str_val = to_string(val);

	str_macro += "#define " + mac_name + " " + str_val + "\n";
}

void EffectTemplate::setTextureSampler(uint32 slot, Texture* tex)
{
	D3D11SamplerStateObject* sampler = tex->getSamplerStateObject();
	g_render_device_->setD3D11SamplerObject(PIXEL_SHADER, slot, sampler);
}

void EffectTemplate::setTextureShaderResourceView(uint32 slot, Texture* tex)
{
	if (!tex->getTextureObj())
	{
		tex = g_tex_manager_->getDefaultTexture();
	}

	D3D11ShaderResourceView* srv = tex->getTextureObj()->getResourceProxy()->srv_view_;
	g_render_device_->setD3D11ShaderResourceView(PIXEL_SHADER, slot, srv);
}

void EffectTemplate::setConstantBuffer(ShaderType type, uint32 slot, D3D11ResourceProxy* cbuffer)
{
	g_render_device_->setD3D11ConstantBuffer(type, slot, static_cast<D3D11ConstanceBuffer*>(cbuffer->resource_));
}

//////////////////////////////////////////////////////////////////////////
void setTextureSamplerHelper(ShaderType type, uint32 slot, Texture* tex)
{
	D3D11SamplerStateObject* sampler = tex->getSamplerStateObject();
	g_render_device_->setD3D11SamplerObject(type, slot, sampler);
}

void setTextureShaderResourceViewHelper(ShaderType type, uint32 slot, Texture* tex)
{
	D3D11ShaderResourceView* srv = tex->getTextureObj()->getResourceProxy()->srv_view_;
	g_render_device_->setD3D11ShaderResourceView(type, slot, srv);
}

void setTextureShaderResourceViewHelper(ShaderType type, uint32 slot, D3D11ResourceProxy* tex)
{
	D3D11ShaderResourceView* srv = tex->srv_view_;
	g_render_device_->setD3D11ShaderResourceView(type, slot, srv);
}

void setConstantBufferHelper(ShaderType type, uint32 slot, D3D11ResourceProxy* cbuffer)
{
	g_render_device_->setD3D11ConstantBuffer(type, slot, static_cast<D3D11ConstanceBuffer*>(cbuffer->resource_));
}

}