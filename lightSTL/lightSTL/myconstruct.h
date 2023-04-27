#pragma once
//�����������ʵ��������
namespace lightstl {
	//��ָ��ָ����ڴ�ռ��½�һ������
	template<class T>
	void construct(T* pointer, T value) {
		::new ((void*)pointer) T(value);
	}

	//����������λ
	template <class T>
	void destroy(T* pointer)
	{
		if (pointer != nullptr)
		{
			pointer->~T();
		}
	}

	//����ָ������
	template <class T>
	void destroy(T* left, T* right) {
		for (; left < right; left++) {
			destroy(&*left);
		}
	}
}