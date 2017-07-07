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
// Time: 2014/06/13
// File: enn_effect_template_manager.h

#pragma once

#include "enn_forward_effect_template.h"
#include "enn_main_result_effect_template.h"
#include "enn_gbuffer_effect_template.h"
#include "enn_ssao_effect_template.h"
#include "enn_ssao_filter_effect_template.h"
#include "enn_sky_effect_template.h"
#include "enn_terrain_section_eff_template.h"
#include "enn_ocean_eff_template.h"

namespace enn
{

class EffectTemplateManager : public AllocatedObject
{
	typedef unordered_map<int, EffectTemplate*>::type	EffectTemplateMap;

public:
	EffectTemplateManager();
	~EffectTemplateManager();

public:
	void registerEffectTemplate(int et_id, EffectTemplate* et);
	EffectTemplate* getEffectTemplate(int et_id) const;
	void destroy();

protected:
	EffectTemplateMap		eff_template_map_;
};

extern EffectTemplateManager* g_eff_template_mgr_;

}