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
// Time: 2015/12/16
// File: enn_ps_render.h
//

#pragma once

namespace enn
{

class ParticleSystem;
class PSTechnique;
class PSRender : public AllocatedObject
{
public:
	PSRender();
	virtual ~PSRender();

	PSTechnique* getTechnique() const;
	void setTechnique(PSTechnique* tech);

	virtual String getType() const = 0;

	virtual void init(ParticleSystem* parent);
	ParticleSystem* getParent() const;

	virtual int update(float elapsedTime);
	void doEmit(PSEmitter* emitter, float elapsedTime);
	void doAffect(PSAffector* affector, float elapsedTime);

	const AxisAlignedBox& getBound() const;

	void setEnable(bool enable);
	bool isEnable() const;

	void setQuota(int quota);
	int getQuota() const;

	void setLocalSpace(bool b);
	bool isLocalSpace() const;

	void setScaleAble(bool b);
	bool isScaleAble() const;

	void setSorting(bool b);
	bool getSorting() const;

protected:
	ParticleSystem*							parent_;
	Array<Particle>							particle_pool_;
	std::stack<Particle*>					particle_free_stack_;
	Array<PSEmitter*>						emitters_;
	Array<PSAffector*>						affectors_;
	Array<Particle*>						particles_;

	AxisAlignedBox							bound_;
};

}