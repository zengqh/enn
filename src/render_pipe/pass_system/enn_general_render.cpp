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
// File: enn_general_render.cpp
//

#include "enn_general_render.h"
#include "enn_effect_template_manager.h"
#include "enn_d3d11_render_device.h"
#include "enn_render_pipe.h"

namespace enn
{

GeneralRender* g_general_render_ = 0;
//////////////////////////////////////////////////////////////////////////
static bool _sortF2B(const RenderableContext&  lhs, const RenderableContext& rhs)
{
	return lhs.dist_ < rhs.dist_;
}

static bool _sortB2F(const RenderableContext& lhs, const RenderableContext& rhs)
{
	return lhs.dist_ > rhs.dist_;
}


//////////////////////////////////////////////////////////////////////////
RenderableContextList::RenderableContextList()
{

}

RenderableContextList::~RenderableContextList()
{

}

void RenderableContextList::sortBackToFront()
{
	std::sort(rc_list_.begin(), rc_list_.end(), _sortB2F);
}

void RenderableContextList::sortFrontToBack()
{
	std::sort(rc_list_.begin(), rc_list_.end(), _sortF2B);
}

void RenderableContextList::addRenderable(const RenderableContext& rc)
{
	rc_list_.push_back(rc);
}

void RenderableContextList::clear()
{
	rc_list_.clear();
}

//////////////////////////////////////////////////////////////////////////
KindRenderableList::KindRenderableList()
{

}

KindRenderableList::~KindRenderableList()
{
	clear();
}

void KindRenderableList::addRenderable(const RenderableContext& rc)
{
	RenderableContextList*& rcl = rc_list_[rc.eff_ctx_];
	if (!rcl)
	{
		rcl = list_pool_.requestObject();
	}

	rcl->addRenderable(rc);
}

void KindRenderableList::clear()
{
	ENN_FOR_EACH(RCListMap, rc_list_, it)
	{
		RenderableContextList* rcl = it->second;
		rcl->clear();
		list_pool_.freeObject(rcl);
	}

	rc_list_.clear();
}

void KindRenderableList::sortBackToFront()
{
	ENN_FOR_EACH(RCListMap, rc_list_, it)
	{
		RenderableContextList* rcList = it->second;
		rcList->sortBackToFront();
	}
}

void KindRenderableList::sortFrontToBack()
{
	ENN_FOR_EACH(RCListMap, rc_list_, it)
	{
		RenderableContextList* rcList = it->second;
		rcList->sortFrontToBack();
	}
}

//////////////////////////////////////////////////////////////////////////
GeneralRender::GeneralRender()
{
	g_general_render_ = this;
}

GeneralRender::~GeneralRender()
{

}

void GeneralRender::beginRender(uint32 pass)
{
	curr_pass_ = pass;

	switch (curr_pass_)
	{
	case GR_SHADOW:
		break;

	case GR_GBUFFER:
	case GR_FORWARD:
	case GR_MAIN_RESULT:
		solid_group_.clear();
		trans_group_.clear();
		break;

	default:
		break;
	}
}

void GeneralRender::addRenderable(Renderable* ra)
{
	Material* mtr = ra->getMaterial();

	if (curr_pass_ == GR_SHADOW || curr_pass_ == GR_GBUFFER)
	{
		if (mtr->hasAlpha())
		{
			return;
		}
	}


	EffectTemplate* eff_template = g_eff_template_mgr_->getEffectTemplate(curr_pass_);
	EffectContext* ec = eff_template->getEffectContext(ra);
	
	RenderableContext rc;
	rc.eff_ctx_ = ec;
	rc.ra_obj_ = ra;
	rc.dist_ = (g_render_pipe_->getMainCam()->getEyePos() - ra->getAABBWorld().getCenter()).squaredLength();

	switch (curr_pass_)
	{
	case GR_SHADOW:
		shadow_group_.addRenderable(rc);
		break;

	case GR_GBUFFER:
		solid_group_.addRenderable(rc);
		break;

	case GR_FORWARD:
	case GR_MAIN_RESULT:
		if (mtr->hasAlpha())
		{
			trans_group_.addRenderable(rc);
		}
		else
		{
			solid_group_.addRenderable(rc);
		}
		break;

	default:
		break;
	}
}

void GeneralRender::endAdd()
{

}

void GeneralRender::renderSolidDirect(const EffectParams& eff_params)
{
	renderList(solid_group_, eff_params);
}

void GeneralRender::renderTransDirect(const EffectParams& eff_params)
{
	renderList(trans_group_, eff_params);
}

void GeneralRender::renderShadowDirect(const EffectParams& eff_params)
{
	renderList(shadow_group_, eff_params);
}

void GeneralRender::renderSolidWS(const EffectParams& eff_params)
{
	g_render_device_->setSolidState(true);
	renderSolidDirect(eff_params);
}

void GeneralRender::renderTransWS(const EffectParams& eff_params)
{
	g_render_device_->setSolidState(false);
	renderTransDirect(eff_params);
}

void GeneralRender::renderSolidTransWS(const EffectParams& eff_params)
{
	renderSolidWS(eff_params);
	renderTransWS(eff_params);
}

void GeneralRender::renderList(const KindRenderableList& ra_list, const EffectParams& eff_params)
{
	ENN_FOR_EACH_CONST(KindRenderableList::RCListMap, ra_list.getRenderables(), it)
	{
		EffectContext* effContext = it->first;

		RenderableContextList* sub_list = it->second;

		for (int i = 0; i < sub_list->getSize(); ++i)
		{
			Renderable* ra = sub_list->at(i).ra_obj_;
			effContext->owner_->doSetParams(effContext, ra, const_cast<EffectParams*>(&eff_params));

			ra->render(effContext);
		}
	}
}

void GeneralRender::renderList(const RenderableContextList& ra_list, const EffectParams& eff_params)
{
	int size = ra_list.getSize();

	for (int i = 0; i < size; ++i)
	{
		const RenderableContext& rc = ra_list.at(i);

		EffectContext* effContext = rc.eff_ctx_;
		Renderable* ra = rc.ra_obj_;

		effContext->eff_->bind();

		effContext->owner_->doSetParams(effContext, ra, const_cast<EffectParams*>(&eff_params));

		ra->render(effContext);
	}
}

}