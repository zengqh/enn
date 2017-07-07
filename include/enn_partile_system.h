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
// Time: 2015/12/21
// File: enn_partile_system.h
//

#pragma once

#include "enn_scene_node.h"

namespace enn
{

class ParticleSystem : public SceneNode
{
public:
	ParticleSystem();
	virtual ~ParticleSystem();

	void setFileName(const String& file_name);
	const String& getFileName() const;

	void setEnable(bool b);
	virtual void update();

	PSRender* newRender(int type);
	void deleteRender(PSRender* render);

	PSRender* getRender(int index);
	int getRenderCount() const;

	bool isRenderValid(PSRender* render);

protected:
	void onParticleLoaded();
	void onParticleUnloaded();

protected:
	Array<PSRender*>		render_array_;

};

}