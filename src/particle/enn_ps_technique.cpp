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
// File: enn_ps_technique.cpp
//

#include "enn_ps_technique.h"
#include "enn_ps_factory.h"

namespace enn
{

PSTechnique::PSTechnique()
{

}

PSTechnique::~PSTechnique()
{

}

void PSTechnique::setParentSystem(ParticleSystem* sys)
{
	parent_system_ = sys;
}

ParticleSystem* PSTechnique::getParentSystem() const
{
	return parent_system_;
}

PSEmitter* PSTechnique::createEmitter(const String& type)
{
	return g_ps_factory_->createEmitter(type);
}

int PSTechnique::getEmitterCount() const
{
	return emitters_.size();
}

PSEmitter* PSTechnique::getEmitter(int index) const
{
	return emitters_[index];
}

PSEmitter* PSTechnique::getEmitter(const String& emitterName) const
{
	if (emitterName.empty())
	{
		return 0;
	}

	ENN_FOR_EACH(PSEmitterList, emitters_, it)
	{
		if (it->getName() == emitterName)
		{
			return (*it);
		}
	}

	return 0;
}

void PSTechnique::destroyEmitter(int index)
{
	PSEmitterList::iterator it = emitters_.begin() + index;

	ENN_SAFE_DELETE(*it);

	emitters_.erase(it);
}

void PSTechnique::destroyEmitter(PSEmitter* emitter)
{
	ENN_FOR_EACH(PSEmitterList, emitters_, it)
	{
		if (emitter == *it)
		{
			ENN_SAFE_DELETE(emitter);
			emitters_.erase(it);
			break;
		}
	}
}

void PSTechnique::destroyAllEmitter()
{
	ENN_FOR_EACH(PSEmitterList, emitters_, it)
	{
		ENN_SAFE_DELETE(*it);
	}

	emitters_.clear();
}


PSAffector* PSTechnique::createAffector(const String& type)
{
	return g_ps_factory_->createAffector(type);
}

int PSTechnique::getAffectorCount() const
{
	return affectors_.size();
}

PSAffector* PSTechnique::getAffector(int index)
{
	return affectors_[index];
}

PSAffector* PSTechnique::getAffector(const String& name)
{
	if (name.empty())
	{
		return 0;
	}

	ENN_FOR_EACH(PSAffectorList, affectors_, it)
	{
		if (it->getName() == name)
		{
			return *it;
		}
	}
}

void PSTechnique::destroyAffector(int index)
{
	PSAffectorList::iterator it = affectors_.begin() + index;

	ENN_SAFE_DELETE(*it);

	affectors_.erase(it);
	
}

void PSTechnique::destroyAffector(PSAffector* affector)
{
	ENN_FOR_EACH(PSAffectorList, affectors_, it)
	{
		if (*it == affectors_)
		{
			ENN_SAFE_DELETE(affector);
			affectors_.erase(it);
			break;
		}
	}
}

void PSTechnique::destroyAllAffector()
{
	ENN_FOR_EACH(PSAffectorList, affectors_, it)
	{
		ENN_SAFE_DELETE(*it);
	}

	affectors_.clear();
}

int PSTechnique::getParticleCount() const
{
	return particles_.size();
}

const Particle* PSTechnique::getParticle(int index) const
{
	return particles_[index];
}

void PSTechnique::setVisualQuota(int quota)
{
	if (quota > 0)
	{
		_clear_particle();
		particle_pool_.resize(quota);

		for (int i = 0; i < quota; ++i)
		{
			particle_free_stack_.push_back(&particle_pool_[i]);
		}

		visual_quota_ = quota;

		allocQuota(quota);
	}
}

void PSTechnique::allocQuota(int quota)
{

}

}