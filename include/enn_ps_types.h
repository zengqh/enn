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
// Time: 2015/12/09
// File: enn_ps_types.h
//

#pragma once

#include "enn_math.h"

namespace enn
{


enum PS_BillboardType
{
	POINT,
	ORIENTED,
	ORIENTED_COMMON,
	PERPENDICULAR,
	PERPENDICULAR_COMMON,
};
	
enum PS_BlendMode
{
	OPACITY,
	ADD,
	ALPHA_BLEND,
	COLOR_BLEND,
};

enum PS_Operation
{
	SET,
	ADD,
	SUB,
	MUL,
	AVG
};

struct PS_Vertex
{
	float3 position;
	float4 color;
	float2 uv;
};


struct PS_SortOp
{
	PSRender * mRender;
	float3 mCameraPosition;

	bool operator()(Particle * p1, Particle * p2)
	{
		float3 position1 = p1->Position;
		float3 position2 = p2->Position;

		if (mRender->IsLocalSpace())
		{
			const Matrix4 & form = mRender->GetParent()->GetWorldTM();

			position1.TransformA(form);
			position2.TransformA(form);
		}

		float lenSq1 = mCameraPosition.squaredDistance(p1->Position);  
		float lenSq2 = mCameraPosition.squaredDistance(p2->Position);

		return lenSq1 <= lenSq2;
	}
};

//////////////////////////////////////////////////////////////////////////
template<class T>
class PS_TKeyFrame : public AllocatedObject
{
public:
	enum { K_MAX_KEY = 16 };
	struct KeyFrame 
	{
		float time;
		T value;
	};

	struct __KF_LessOp
	{
		bool operator ()(const KeyFrame & a, const KeyFrame & b) const
		{
			return a.time < b.time;
		}
	};

public:
	PS_TKeyFrame() {}
	virtual ~PS_TKeyFrame() {}

	void
		Clear() { mKeyFrames.clear(); }

	int
		Size() { return mKeyFrames.size(); }

	void
		Append(float _time, const void* _value) { Insert(Size(), _time, _value); }
	void
		Append(KeyFrame & _frame) { Insert(Size(), _frame); }

	void
		Insert(int _i, float _time, const void * _value)
	{
		ENN_ASSERT(_time <= 1.0f);

		KeyFrame kf;
		kf.time = _time;

		if (_value != NULL)
		{
			kf.value.Copy(_value);
		}

		Insert(_i, kf);
	}

	void
		Insert(int _i, KeyFrame & _frame)
	{
		if (mKeyFrames.Size() < K_MAX_KEY)
		{
			List::iterator it = mKeyFrames.begin();
			std::advance(it, i);

			mKeyFrames.insert(it, _frame);
		}
	}

	void
		Erase(int _i)
	{ 
		List::iterator it = mKeyFrames.begin();
		std::advance(it, _i);
		mKeyFrames.Erase(it);
	}

	bool
		GetValue(T & _value, float _time, bool _lerp) const
	{
		if (mKeyFrames.Size() == 0)
			return false;

		int k1 = 0, k2 = 0;

		for (int i = 1; i < mKeyFrames.Size(); ++i)
		{
			if (mKeyFrames[i].time > _time)
				break;

			k1 = i;
		}

		if (_time < mKeyFrames[k1].time || k1 == mKeyFrames.Size() - 1)
			k2 = k1;
		else
			k2 = k1 + 1;

		if (_lerp && k2 > k1)
		{
			const KeyFrame & key1 = mKeyFrames[k1];
			const KeyFrame & key2 = mKeyFrames[k2];

			float dk = (_time - key1.time) / (key2.time - key1.time);

			_value = key1.value.Lerp(key2.value, dk);
		}
		else
		{
			_value = mKeyFrames[k1].value;
		}

		return true;
	}

	void
		Copy(PS_TKeyFrame * rk)
	{
		Clear();

		for (int i = 0; i < rk->mKeyFrames.size(); ++i)
		{
			mKeyFrames.push_back(rk->mKeyFrames[i]);
		}
	}

	void
		Adjust()
	{
		__KF_LessOp op;

		if (mKeyFrames.size() > 0)
		{
			std::sort(mKeyFrames.begin(), mKeyFrames.end(), op);
		}
	}

	/*
	void
		Serialize(Serializer & sl)
	{
		if (sl.IsOut())
		{
			OSerializer & OS = (OSerializer &)sl;

			OS << mKeyFrames.Size();
			for (int i = 0; i < mKeyFrames.Size(); ++i)
			{
				const KeyFrame & kf = mKeyFrames[i];

				OS << kf.time;
				OS << kf.value.data;
			}
		}
		else
		{
			ISerializer & IS = (ISerializer &)sl;

			int count = 0;
			IS >> count;
			for (int i = 0; i < count; ++i)
			{
				KeyFrame kf;

				IS >> kf.time;
				IS >> kf.value.data;

				mKeyFrames.PushBack(kf);
			}
		}
	}

	*/

	void
		SetKeyFrame(int _i, float time, const void * val)
	{
		mKeyFrames[_i].time = time;

		if (val)
		{
			mKeyFrames[_i].value.Copy(val);
		}
	}

	void
		GetKeyFrame(int _i, float * time, void ** val)
	{
		if (time != NULL)
			*time = mKeyFrames[_i].time;

		if (val != NULL)
			*val = &mKeyFrames[_i].value;
	}

	KeyFrame &
		operator [](int _i)
	{
		return mKeyFrames[_i];
	}

protected:
	Array<KeyFrame> mKeyFrames;
};

}