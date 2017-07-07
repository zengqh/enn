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
// Time: 2015/12/25
// File: enn_ps_technique.h
//

#pragma once

#include "enn_ps_render.h"
#include "enn_ps_emitter.h"
#include "enn_ps_affector.h"

namespace enn
{

class PSTechnique : public AllocatedObject
{
	typedef Array<PSEmitter*>		PSEmitterList;
	typedef Array<PSAffector*>		PSAffectorList;
public:
	PSTechnique();
	virtual ~PSTechnique();

	void setParentSystem(ParticleSystem* sys);
	ParticleSystem* getParentSystem() const;

	PSEmitter* createEmitter(const String& type);
	int getEmitterCount() const;

	PSEmitter* getEmitter(int index) const;
	PSEmitter* getEmitter(const String& emitterName) const;

	void destroyEmitter(int index);
	void destroyEmitter(PSEmitter* emitter);
	void destroyAllEmitter();

	PSAffector* createAffector(const String& type);
	int getAffectorCount() const;

	PSAffector* getAffector(int index);
	PSAffector* getAffector(const String& name);

	void destroyAffector(int index);
	void destroyAffector(PSAffector* affector);
	void destroyAllAffector();

	int getParticleCount() const;
	const Particle* getParticle(int index) const;

	void setVisualQuota(int quota);

	virtual void allocQuota(int quota);

protected:
	virtual Particle * _quest_particle();
	virtual void _free_particle(Particle * p);
	virtual void _clear_particle();

protected:
	ParticleSystem*				parent_system_;
	String						name_;
	int							visual_quota_;

	PSRender*					renderer_;

	PSEmitterList				emitters_;
	PSAffectorList				affectors_;

	Array<Particle*>			particles_;
	Array<Particle>				particle_pool_;
	Array<Particle*>			particle_free_stack_;
};

}