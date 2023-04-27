#pragma once
//这里实现一个allocator类，相比这个类，更重要的是一些静态的分配方法
//只负责分配空间，而不使用初始化操作
#include "myconstruct.h"

namespace lightstl {

	template<class T>
	class myallocator {
	public:
		typedef size_t size_type;
		typedef T value_type;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T* pointer;

		static T* allocate();
		static T* allocate(size_type n);
		static void deallocate(T* ptr);
		static void deallocate(T* ptr, size_type n);
		static void destroy_obj(T* ptr);
		static void destroy_obj(T* lptr, T* rptr);
		static void destroy_obj(T* lptr, size_type n);
	};

	template <class T>
	T* myallocator<T>::allocate() {
		//默认分配方式
		//static_cast转换类型为类指针
		//返还类型为T*,不是迭代器？
		//::operator new()是分配内存用的
		return static_cast<T*>(::operator new(sizeof(T)));
	}

	template <class T>
	T* myallocator<T>::allocate(size_type n)
	{
		//分配n个单位空间
		if (n == 0)
			return nullptr;//处理n==0的情况
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}

	//释放指针指向的单位空间
	//destroy负责析构空间内存放的对象并回收空间，而deallocate负责回收没有存放对象的空间
	template <class T>
	void myallocator<T>::deallocate(T* ptr)
	{
		if (ptr == nullptr)
			return;
		::operator delete(ptr);
	}

	//释放指针指向的一块连续空间
	template <class T>
	void myallocator<T>::deallocate(T* ptr, size_type n)
	{
		if (ptr == nullptr)
			return;
		::operator delete(ptr);		
	}

	//单个对象的析构
	template<class T>
	void myallocator<T>::destroy_obj(T* ptr)
	{
		destroy(ptr);
	}

	//区间析构
	template<class T>
	void myallocator<T>::destroy_obj(T* lptr,T* rptr)
	{
		destroy(lptr, rptr);
	}

	//区间析构
	template<class T>
	void myallocator<T>::destroy_obj(T* lptr, size_type n)
	{
		destroy(lptr, lptr+n);
	}



}

