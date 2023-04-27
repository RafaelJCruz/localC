#pragma once
//myiterator��Ҫʵ�ֵ��������ܣ����ڽṹ�϶����ڷ���������
//�������ں�һ����Ƕ���͵�ָ��ָ��Ԫ�أ����ܶ������������͵�ʵ��ǰ��������
//ͬʱӦ���ز�������ÿһ���������Ӧ�õ�������
//����vector������˵���ռ��������ģ���˲���Ҫ�����������ֱ����ָ��������
//���ڷ�����������ͨ���㷨����Ҫ���ص�����
#include <cstddef>
// iterator ģ��
namespace lightstl {
	typedef size_t size_type;
	//��Distanceƫ�ػ�,
	template <class Category, class T, class Distance = ptrdiff_t,
		class Pointer = T*, class Reference = T&>
	class iterator
	{
		//ֻ��������������û�ж���ʲô��Ա��������Ϊ���ǿտ�
		//ָ�����ͣ���ʱ�ò���
		typedef Category                             iterator_category;
		//��Ƕ��
		typedef T                                    value_type;
		//ָ��
		typedef Pointer                              pointer;
		//��֪����ʲô�Ĳ���
		typedef Reference                            reference;
		//���룬���������õõ���
		typedef Distance                             difference_type;
	};

	template <class Iter>
	size_type distance(Iter left,Iter right) {
		size_type n = 0;
		for (; left < right; left++) {
			n++;
		}
		return n;
	}





}
