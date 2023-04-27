#pragma once
//myiterator需要实现迭代器功能，其在结构上独立于分配器存在
//迭代器内含一个内嵌类型的指针指向元素，并能独立于数据类型地实现前进、后退
//同时应重载操作符，每一类迭代器都应该单独重载
//对于vector类型来说，空间是连续的，因此不需要适配迭代器，直接用指针解决即可
//对于非连续容器和通用算法，需要重载迭代器
#include <cstddef>
// iterator 模板
namespace lightstl {
	typedef size_t size_type;
	//将Distance偏特化,
	template <class Category, class T, class Distance = ptrdiff_t,
		class Pointer = T*, class Reference = T&>
	class iterator
	{
		//只是做了重命名，没有定义什么成员，可以认为就是空壳
		//指针类型，暂时用不到
		typedef Category                             iterator_category;
		//内嵌类
		typedef T                                    value_type;
		//指针
		typedef Pointer                              pointer;
		//不知道干什么的参数
		typedef Reference                            reference;
		//距离，后续可能用得到？
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
