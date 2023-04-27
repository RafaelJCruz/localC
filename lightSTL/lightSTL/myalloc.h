#pragma once
//����ʵ��һ��allocator�࣬�������࣬����Ҫ����һЩ��̬�ķ��䷽��
//ֻ�������ռ䣬����ʹ�ó�ʼ������
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
		//Ĭ�Ϸ��䷽ʽ
		//static_castת������Ϊ��ָ��
		//��������ΪT*,���ǵ�������
		//::operator new()�Ƿ����ڴ��õ�
		return static_cast<T*>(::operator new(sizeof(T)));
	}

	template <class T>
	T* myallocator<T>::allocate(size_type n)
	{
		//����n����λ�ռ�
		if (n == 0)
			return nullptr;//����n==0�����
		return static_cast<T*>(::operator new(n * sizeof(T)));
	}

	//�ͷ�ָ��ָ��ĵ�λ�ռ�
	//destroy���������ռ��ڴ�ŵĶ��󲢻��տռ䣬��deallocate�������û�д�Ŷ���Ŀռ�
	template <class T>
	void myallocator<T>::deallocate(T* ptr)
	{
		if (ptr == nullptr)
			return;
		::operator delete(ptr);
	}

	//�ͷ�ָ��ָ���һ�������ռ�
	template <class T>
	void myallocator<T>::deallocate(T* ptr, size_type n)
	{
		if (ptr == nullptr)
			return;
		::operator delete(ptr);		
	}

	//�������������
	template<class T>
	void myallocator<T>::destroy_obj(T* ptr)
	{
		destroy(ptr);
	}

	//��������
	template<class T>
	void myallocator<T>::destroy_obj(T* lptr,T* rptr)
	{
		destroy(lptr, rptr);
	}

	//��������
	template<class T>
	void myallocator<T>::destroy_obj(T* lptr, size_type n)
	{
		destroy(lptr, lptr+n);
	}



}

