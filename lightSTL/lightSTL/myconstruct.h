#pragma once
//负责构造和析构实例化对象
namespace lightstl {
	//在指针指向的内存空间新建一个对象
	template<class T>
	void construct(T* pointer, T value) {
		::new ((void*)pointer) T(value);
	}

	//析构单个单位
	template <class T>
	void destroy(T* pointer)
	{
		if (pointer != nullptr)
		{
			pointer->~T();
		}
	}

	//析构指定区间
	template <class T>
	void destroy(T* left, T* right) {
		for (; left < right; left++) {
			destroy(&*left);
		}
	}
}