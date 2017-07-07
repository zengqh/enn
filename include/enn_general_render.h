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
// Time: 2014/05/22
// File: enn_general_render.h
//

#pragma once

#include "enn_effect_template.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class RenderableContext : public AllocatedObject
{
public:
	Renderable*		ra_obj_;
	EffectContext*	eff_ctx_;
	float			dist_;
};

//////////////////////////////////////////////////////////////////////////
class RenderableContextList : public AllocatedObject
{
public:
	typedef enn::vector<RenderableContext>::type RCList;

public:
	RenderableContextList();
	~RenderableContextList();

	void sortBackToFront();
	void sortFrontToBack();

	void addRenderable(const RenderableContext& rc);

	int getSize() const
	{
		return rc_list_.size();
	}

	const RenderableContext& at(int idx) const
	{
		return rc_list_[idx];
	}

	RenderableContext& at(int idx)
	{
		return rc_list_[idx];
	}

	void clear();

protected:
	RCList rc_list_;
};

//////////////////////////////////////////////////////////////////////////
class KindRenderableList : public AllocatedObject
{
public:
	typedef enn::map<EffectContext*, RenderableContextList*>::type RCListMap;
	typedef ObjectRecycle<RenderableContextList, SPFM_DELETE_T> RcListRecycle;

public:
	KindRenderableList();
	~KindRenderableList();

	void addRenderable(const RenderableContext& rc);
	void clear();

	RCListMap& getRenderables() { return rc_list_; }
	const RCListMap& getRenderables() const { return rc_list_; }

	void sortBackToFront();
	void sortFrontToBack();

protected:
	RCListMap			rc_list_;
	RcListRecycle		list_pool_;
};


//////////////////////////////////////////////////////////////////////////
class GeneralRender : public AllocatedObject
{
public:
	enum
	{
		GR_SHADOW,
		GR_GBUFFER,
		GR_FORWARD,
		GR_LITBUFFER,
		GR_MAIN_RESULT,
		GR_SSAO,
		GR_SSAO_FILTER,
		GR_SKY,
		GR_TERRAIN,
		GR_OCEAN,
	};

public:
	GeneralRender();
	~GeneralRender();

	void beginRender(uint32 pass);
	void addRenderable(Renderable* ra);
	void endAdd();


	void renderSolidDirect(const EffectParams& eff_params);
	void renderTransDirect(const EffectParams& eff_params);
	void renderShadowDirect(const EffectParams& eff_params);

	void renderSolidWS(const EffectParams& eff_params);
	void renderTransWS(const EffectParams& eff_params);
	void renderSolidTransWS(const EffectParams& eff_params);

protected:
	void renderList(const KindRenderableList& ra_list, const EffectParams& eff_params);
	void renderList(const RenderableContextList& ra_list, const EffectParams& eff_params);

protected:
	uint32						curr_pass_;
	KindRenderableList			solid_group_;
	RenderableContextList		trans_group_;
	KindRenderableList			shadow_group_;
};

extern GeneralRender* g_general_render_;

}