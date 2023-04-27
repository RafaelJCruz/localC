#pragma once
#include "myVector.h"

//��������δ������,���ڿ�����
namespace lightstl {
	//���Բο�vector��ʵ��string
	template <class T = char, class Container = lightstl::vector<T>>
	class string {
	public:
		typedef Container                           container_type;
		// ʹ�õײ��������ͱ�
		typedef typename Container::value_type      value_type;
		typedef typename Container::size_type       size_type;
		typedef typename Container::reference       reference;
		typedef typename Container::const_reference const_reference;
		typedef typename Container::iterator        iterator;

		

	private:
			container_type con_;//ʹ��vector��Ϊ�ײ�����

	public:
		//���캯������ֱ�ӵ�������
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

		//����������
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

		//�������
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
		////Ԥ���ռ�
		//void reserve(size_type n);
		////�������������
		//void shrink_to_fit();

	
		
	};
};