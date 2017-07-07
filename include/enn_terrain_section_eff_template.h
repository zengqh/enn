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
// Time: 2015/02/13
// File: enn_terrain_section_eff_template.h
//

#pragma once

#include "enn_effect_template.h"
#include "enn_terrain_section.h"

namespace enn
{

class TerrainSectionEffectParams : public EffectParams
{
public:
	Camera*					curr_render_cam_;
	EffectMethod			em_;
	TerrainSection*			section_;
	D3D11TextureObj*		weight_map_;
	Vector4					gTransform;
	Vector4					gUVParam;
	Vector4					gUVScale;
	bool					is_deferred_;
	int						layer[4];
};

//////////////////////////////////////////////////////////////////////////
class TerrainSectionEffectTemplate : public EffectTemplate
{
	struct TerrainParams
	{
		DECLARE_CBUFF_PARAMS_FUNC
		Matrix4 matWV;
		Matrix4 matWVP;
		Vector4 gTransform;
		Vector4 gUVParam;
		Vector4 gUVScale;
	};

public:
	TerrainSectionEffectTemplate(EffectTemplateManager* owner);
	~TerrainSectionEffectTemplate();

public:
	virtual EffectContext* getEffectContext(Renderable* ra);

	virtual EffectContext* getEffectContextNormal(EffectMethod em);
	virtual EffectContext* getEffectContextDeferred(EffectMethod em);
	virtual void doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params);

protected:
	virtual void bindShaderPrograms();
	virtual void bindConstantBuffers();
	virtual void bindSamplerObjects();
	virtual void bindShaderResourceViews();
	virtual void bindUnorderedAccessViews();

protected:
	void createConstantBuffers();
	void setTransformParams();
	void setMtrParams();
	void setSkeletonParams();
	void applyConstantBuffers();

protected:
	EffectContext*						curr_eff_context_;
	Renderable*							curr_ra_;
	TerrainSectionEffectParams*			curr_eff_params_;

	bool								cbuffer_init_;

	TerrainParams						terrain_params_;

	D3D11ResourceProxy*					transform_cbuff_;

	TextureObjUnit*						normal_fit_tex_;

	EffectContextMap					deferred_eff_ctx_map_;
};

}