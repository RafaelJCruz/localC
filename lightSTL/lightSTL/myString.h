#pragma once
#include "myVector.h"

//适配器，未经测试,正在开发中
namespace lightstl {
	//可以参考vector来实现string
	template <class T = char, class Container = lightstl::vector<T>>
	class string {
	public:
		typedef Container                           container_type;
		// 使用底层容器的型别
		typedef typename Container::value_type      value_type;
		typedef typename Container::size_type       size_type;
		typedef typename Container::reference       reference;
		typedef typename Container::const_reference const_reference;
		typedef typename Container::iterator        iterator;

		

	private:
			container_type con_;//使用vector作为底层容器

	public:
		//构造函数可以直接调用容器
		string()
		{
			con_();
		}

		string(size_type n, value_type& value) {
			con_(n, value);
		}

		template <class Iter>
		string(Iter left, Iter right) {
			con_(left, right);
		}

		string(const T& otherStr) {
			con_(otherStr);
		}

		string(std::initializer_list<value_type> ilist) {
			con_(ilist.begin(), ilist.end());
		}

		~string() = default;

		//迭代器操作
		iterator begin() const noexcept
		{
			return con_.begin();
		}
		const iterator cbegin() const noexcept
		{
			return con_.cbegin();
		}
		iterator end() const noexcept
		{
			return con_.end();
		}
		const iterator cend()const noexcept
		{
			return con_.cend();
		}

		//容量相关
		bool empty()  const
		{
			return con_.empty();
		}
		size_type size() const
		{
			return con_.size();
		}
		size_type capacity() const
		{
			return con_.capacoty();
		}
		////预留空间
		//void reserve(size_type n);
		////放弃多余的容量
		//void shrink_to_fit();

	
		
	};
};