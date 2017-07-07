/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_std_types.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_std_header.h"
#include "enn_memory.h"

namespace enn
{
template <class T> 
struct vector 
{ 
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename std::vector<T, STLAllocator<T> > type;
#else
	typedef typename std::vector<T> type;
#endif
};

template <typename T>
class Array : public vector<T>::type {};

template <class T> 
struct list 
{ 
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename std::list<T, STLAllocator<T> > type;
#else
	typedef typename std::list<T> type;
#endif
}; 

template <typename T>
class List : public list<T>::type {};

template <class K, class V, class P = std::less<K> > 
struct map
{ 
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename std::map<K, V, P, STLAllocator<std::pair<const K, V> > > type;
#else
	typedef typename std::map<K, V, P> type;
#endif
};

template <class K, class V>
class Map : public map<K, V>::type {};

template <class T, class P = std::less<T> > 
struct set 
{ 
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename std::set<T, P, STLAllocator<T> > type;
#else
	typedef typename std::set<T, P> type;
#endif
}; 

template <class T> class Set : public set<T>::type {};

template <class K, class V, class P = std::less<K> > 
struct multimap 
{ 
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename std::multimap<K, V, P, STLAllocator<std::pair<const K, V> > > type;
#else
	typedef typename std::multimap<K, V, P> type;
#endif
};

template <class K, class V> class Multimap : public multimap<K, V>::type {};

template <size_t N>
struct bitset
{
	typedef typename std::bitset<N> type;
};

template <size_t N> class Bitset : public bitset<N>::type {};
//////////////////////////////////////////////////////////////////////////
#if ENN_STRING_USE_CUSTOM_MEMORY_ALLOCATOR
typedef std::basic_string<char, std::char_traits<char>, STLAllocator<char> >            String;
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, STLAllocator<wchar_t> >   WString;
#else
typedef std::string     String;
typedef std::wstring    WString;
#endif

template<class K, class V, class H = hash_<K>, class E = std::equal_to<K> >
struct unordered_map
{
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename unordered_map_<K, V, H, E, STLAllocator<std::pair<const K, V> > > type;
#else
	typedef typename unordered_map_<K, V, H, E> type;
#endif
};

template<class K, class H = hash_<K>, class E = std::equal_to<K> >
struct unordered_set
{
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename unordered_set_<K, H, E, STLAllocator<K> > type;
#else
	typedef typename unordered_set_<K, H, E> type;
#endif
};

//////////////////////////////////////////////////////////////////////////
template <class T, int _Size>
class Field
{
public:
	Field()
	{
	}

	~Field()
	{
	}

	int Size() const
	{
		return _Size;
	}

	T & At(int i)
	{
		ENN_ASSERT(i >= 0 && i < Size());

		return mMember[i];
	}

	const T & At(int i) const
	{
		ENN_ASSERT(i >= 0 && i < Size());

		return mMember[i];
	}

	const T * c_ptr(int i = 0) const
	{
		ENN_ASSERT(i >= 0 && i < Size());

		return &mMember[i];
	}

	T * c_ptr(int i = 0)
	{
		ENN_ASSERT(i >= 0 && i < Size());

		return &mMember[i];
	}

	T & Front()
	{
		return At(0);
	}

	const T & Front() const
	{
		return At(0);
	}

	T & Back()
	{
		return At(Size() - 1);
	}

	const T & Back() const
	{
		return At(Size() - 1);
	}

	T & operator [](int i)
	{
		return At(i);
	}

	const T & operator [](int i) const
	{
		return At(i);
	}

	Field(const Field & rk)
	{
		for (int i = 0; i < Size(); ++i)
		{
			mMember[i] = rk.mMember[i];
		}
	}

	Field & operator =(const Field & rk)
	{
		for (int i = 0; i < Size(); ++i)
		{
			mMember[i] = rk.mMember[i];
		}

		return *this;
	}

protected:
	T mMember[_Size];
};


struct IntFloatUnion
{
	union {
		int v_int;
		float v_float;
	};
};

}