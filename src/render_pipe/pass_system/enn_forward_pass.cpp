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
// File: enn_forward_pass.cpp
//

#include "enn_forward_pass.h"
#include "enn_general_render.h"
#include "enn_render_pipe.h"
#include "enn_forward_effect_template.h"

namespace enn
{

void RenderForwardPass::prepare()
{

}

void RenderForwardPass::render()
{
	g_general_render_->beginRender(GeneralRender::GR_FORWARD);

	const RenderableNodeList& ra_list = g_render_pipe_->getMainRAList();
	ENN_FOR_EACH_CONST(RenderableNodeList, ra_list, it)
	{
		RenderableNode* ra_node = *it;

		ra_node->addToRender();
	}

	g_general_render_->endAdd();

	ForwardEffectParams eff_params;
	eff_params.render_cam_ = g_render_pipe_->getMainCam();
	g_general_render_->renderSolidTransWS(eff_params);
}

}