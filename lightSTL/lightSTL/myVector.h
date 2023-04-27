#pragma once
#include "myalgorithm.h"
#include "myutil.h"
#include "myiter.h"
#include "myexcept.h"
#include "myalloc.h"
#include "myuninitial.h"

//��ֹ�����ڣ�ԭ��lightstl��û�з��룬�����������ֵ�����������ߴ��󣬼ǵ�ȥ��Դ�����ļ�
namespace lightstl {

	template <class T>
	class vector {
	public:
		//�������ݷ������ͷ�������
		typedef lightstl::myallocator<T>             myallocator;
		typedef lightstl::myallocator<T>             data_allocator;

		typedef typename myallocator::value_type  value_type;
		typedef typename myallocator::reference  reference;

		typedef value_type*                       iterator;
		typedef size_t                            size_type;
		/*vector���趨��ר����������ʹ��ָ�뼴�����
		����ָ�������ῼ�ǵ�����ָ����������ͣ���˲�������ָ��++����
		����ָ�븺��ͷβ�����һ��Ԫ�ص���һ��λ��*/
	private:
		iterator begin_;
		iterator capacity_;
		//ע��end��λ�������һ��Ԫ�صĺ�һ��λ�ã��䱾��ָ����ڴ���û�з����
		iterator end_;
		//��С��ʼ���ռ䣬������ʱ�����ˣ��������shrink_to_fit����
		size_type min_init_space = 1;

	public:
		//�޲�����ʼ��
		vector() noexcept
		{
			try_init();
		}

		/*��������ʼ��,�������n��ĳ�����͵ĵ�λ�ռ䣬ʹ��Ĭ��ֵ����ʼ��
		size_t��һ���޷���������������ֻ�����ڻ������Ǳ�������������int������߿���ֲ��
		Ӧ����size_t������Ϊsize_type�Է������
		Ӧ���ܾ���ʽת������*/
		explicit vector(size_type n) {
			/*Ӧ��ʹ��Ĭ��ֵ����ʼ�������ʹ�����Ĭ��ֵ��
			����һ���Զ�����T��˵�������Լ���Ĭ�Ϲ��캯��T()
			����ԭ��������������int��˵�������Լ���Ĭ�Ϲ��췽ʽint()
			����int a=int();��a��ֵȡĬ��ֵ0*/
			fill_init(n, value_type());
		}

		//ʹ������ֵ����ʼ��vector������ʾ����vector(10,5)
		//const���β�����ֹ����Ķ���const������������������ֵ
		vector(size_type n, const value_type& value) {
			fill_init(n, value);
		}

		//��������ʼ����Ԥ�ȼ�������
		template <class Iter, typename std::enable_if<std::is_same<Iter,iterator>::value,int>::type = 0>
		vector(Iter first, Iter last)
		{
			//ָ�뱣��
			MYSTL_DEBUG(!(last < first));
			range_init(first, last);
		}

        //������ʼ��
		vector(const vector& otherVec) {
			range_init(otherVec.begin(), otherVec.end());
		}

		//�ƶ����캯��,��ǳ����ʹ��
		vector(vector&& rhs) noexcept
			:begin_(rhs.begin()),
			end_(rhs.end()),
			capacity_(rhs.capacity())
		{
			rhs.begin_ = nullptr;
			rhs.end_ = nullptr;
			rhs.capacity_ = nullptr;
		}

		//initializer_listҲ��һ��ģ�壬���ڴ��ݿɱ������Ĳ��������ƿɱ������
		//ʹ�õ�ʱ��������������뼴�ɣ�ʵ���б��ʼ��
		vector(std::initializer_list<value_type> ilist)
		{
			range_init(ilist.begin(), ilist.end());
		}

		//����
		~vector()
		{
			//����ָ���ͷſռ�
			destroy_and_recover(begin_, end_, capacity_ - begin_);
			//�ͷ�ָ��
			begin_ = end_ = capacity_ = nullptr;
		}

		//����������
		iterator begin() const noexcept
		{ return begin_; }
		const iterator cbegin() const noexcept 
		{return begin_;}
		iterator end() const noexcept
		{ return end_; }
		const iterator cend()const noexcept
		{return end_;}

		//�������
		bool empty()  const 
		{ return begin_ == end_; }
		size_type size() const 
		{return static_cast<size_type>(end_ - begin_);}
		size_type capacity() const 
		{return static_cast<size_type>(capacity_ - begin_);}
		//Ԥ���ռ�
		void reserve(size_type n);
		//�������������
		void shrink_to_fit();

		//�±�������
		reference operator[](size_type n) {
			//�������±겻��Խ��
			MYSTL_DEBUG(n < size());
			return *(begin_ + n);
		}
		reference at(size_type n)
		{
			THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
			return (*this)[n];
		}
		reference front() {//��ȡ��һ��Ԫ��
			MYSTL_DEBUG(!empty());
			return *begin_;
		}
		reference back() {//��ȡ���һ��
			MYSTL_DEBUG(!empty());
			return *(end_ - 1);
		}

		//���÷���
		void swap(vector& otherVec);
		void erase(iterator ptr);
		void print();
		size_type count(const T& value);
		iterator find(const T& value);
		void insert(iterator pos, const T& value);
		void resize(size_type n);
		void push_back(const T& value);
		

		//��ֵ������Ĳ�ͬ����
		vector& operator=(const vector& otherVec) {
			//������ʼ��һ������
			vector<T> tempVect(otherVec);
			swap(this, tempVect);
			//������ʱ�Ķ���
			~tempVect();
			return *this;
		}

		//vector& operator=(vector&& otherVec) {
		//	//ǳ������ֻ����ָ��
		//	destroy_and_recover(begin_, end_, capacity_ - begin_);
		//	begin_ = otherVec.begin_;
		//	end_ = otherVec.end_;
		//	capacity_ = otherVec.capacity_;
		//	otherVec.begin_ = nullptr;
		//	otherVec.end_ = nullptr;
		//	otherVec.capacity_ = nullptr;
		//	return *this;
		//}

		vector& operator=(std::initializer_list<value_type> ilist) {
			//�б��ʼ��
			vector<T> tempVect(ilist.begin(),ilist.end());
			swap(this, tempVect);
			~tempVect();
			return *this;
		}


	private:
		void init_space(size_type n, size_type cap);
		//��ʼ��ϵ�з���
		//�������׳�����������������ֻ�ᱨ���������󣬿��ܷ����Ų�
		void try_init() noexcept;
		void fill_init(size_type n, const value_type& value);
		//���ģ�����͵�����
		template<class Iter>
		void range_init(Iter left, Iter right);

		//�������ͷſռ�Ĳ���
		void destroy_and_recover(iterator left, iterator right, size_type n);

		//���·��䣬�÷�������СΪn�Ŀռ�����ԭ����
		void realloc_n(size_type n);
		//�÷�������nС��size��������������̣�
		void shrink_realloc_n(size_type n);
	};

	template<class T>
	void vector<T>::init_space(size_type size, size_type cap) {
		//�÷����������ռ䣬��С����Ϊ16����λ
		try {
			begin_ = data_allocator::allocate(min_init_space);
			end_ = begin_ + size;
			capacity_ = begin_ + cap;
		}
		catch (...) {
			begin_ = nullptr;
			end_ = nullptr;
			capacity_ = nullptr;
		}
	}
	//void vector<T>::try_init�ǽ���Ա����д������Ĺ̶���ʽ
	template <class T>
	void vector<T>::try_init() noexcept {
		//Ĭ�ϳ�ʼ�������Է���16����λ��ʵ�����Ķ���
		try {
			begin_ = data_allocator::allocate(min_init_space);
			end_ = begin_;//��Ȼ���䵽�˿ռ䣬���ǻ��ǿյ�
			capacity_ = begin_ + 16;
		}
		catch (...) {
			begin_ = nullptr;
			end_ = nullptr;
			capacity_ = nullptr;
		}
	}

	template <class T>
	void vector<T>::fill_init(size_type n, const value_type& value) {
		//static_castǿ������ת�����ò�������һ���ռ��Ҵ�С����Ϊ16
		const size_type init_size = max(static_cast<size_type>(min_init_space), n);
		init_space(init_size, n);
		//��������value���ÿռ�
		fill_section(begin_, n, value);
	}

	//�������ƺ���
	//��vector���ڴ��ƶ�ʱ��Ҳ��Ҫ�˺����ƺ�
	template<class T>
	inline void vector<T>::destroy_and_recover(iterator left, iterator right, size_type n)
	{
		//����ָ�뵽��ָ��Ŀռ���ȫ������
		data_allocator::destroy_obj(left, right);
		//ԭ�ռ����·���
		std::cout << "Object recycled." << std::endl;
	}

	//���г�ʼ����ʹ���������˵�ָ���ʼ��vector����
	//֧�ֵ�������ʼ����list��ʼ����vector��ʼ��
	template<class T>
	template<class Iter>
	void vector<T>::range_init(Iter left, Iter right) {
		const size_type len = distance(left, right);
		const size_type init_size = max(static_cast<size_type>(min_init_space),len);
		init_space(init_size, len);
		//�����ݴ���һ�������и��Ƶ�������������,ǧ�����
		copyrange_fill(left,right,begin_);
	}

	/*����һ������������ֱ�ӽ���ָ�뼴��,�÷��������⣬������ʱ����ͨ������ʱ���ڴ����
	���Ų�ú�������û�д��󣬴�����std::cout��ɵ��ڴ�й©*/
	template<class T>
	void vector<T>::swap(vector<T>& otherVec) {
		//if(this!=otherVec)��û��������������Ȼ���д�������Ӧ��ɾ������ע��
		{
			swap_(begin_, otherVec.begin_);
			swap_(end_, otherVec.end_);
			swap_(capacity_, otherVec.capacity_);
		}
	}

	template<class T>
	void vector<T>::erase(iterator ptr ) {
		/*�ú�һ�������ֵ��ǰһ������ֵ
		  �������һ������
		  ��ʱû������=���������˲�֧����Ƕvetcor,�����Ӧ��ɾ������ע��*��
		/*auto pos = ptr;
		for (; pos != (end_-1); pos++) {
			*(pos) = *(pos + 1);
		}*///���ڷ��ͱ�̿��ǣ��öα���װ��algo.base��˳������ʹ��
		auto pos=sequence_erase(ptr, end_);
		data_allocator::destroy_obj(++pos);
		end_--;
		capacity_--;
		pos = nullptr;
	}

	//�������û������T�����ͣ����ںܴ�����⣬���޲���ʱʹ��
	template <class T>
	void vector<T>::print() {
		auto ptr = begin_;
		for (; ptr!=end_; ptr++) {
			std::cout << *(ptr);
		}
		std::cout << std::endl;
		ptr = nullptr;
	}

	//count(),�������ֵvalue��ͬ�Ķ�������
	//const��������ͬʱ������ֵ����ֵ����
	template <class T>
	size_type vector<T>::count(const T& value) {
		return lightstl::count_value(begin_, end_, value);
	}

	//find()���ҵ�һ����ֵΪvalue�ĵ�����λ��
	//����дtypename vector<T>::iterator��Ϊ���������Ժ�iterator������
	template<class T>
	typename vector<T>::iterator vector<T>::find(const T& value)
	{
		return lightstl::find_value(begin_, end_, value);
	}

	/*insert�������Ԫ�أ����capacity<size, ��ô������λ���Ժ��Ԫ����ǰ��һ��λ��
	���capacity=size����ô��Ҫ�����µĿռ�*/
	template <class T> 
	void vector<T>::insert(iterator pos, const T& value) {
		if (capacity_ != end_) {//ʹ�ã�=�жϻ����
			sequence_insert(pos, capacity_,value);//ע�ⲻ��end_����capacity_
			capacity_++;
		}
		else {
			//��׷������ռ��Ǹ��õİ취
			//ֻ������һ���СΪsize()+1����λ�Ŀռ䲢ʹ�ø���ָ��ָ����
			//Ȼ�����θ���ԭ������ֵ��ʹ��copyrange_fill���֮
			//�ٽ��˿ռ��ָ����ԭ����ָ�뽻��������ͷ�ԭ�ռ䲢���ո���ָ��
			auto distance = pos - begin_;
			//�ӱ��������,���ʧ�ܾ͸�Ϊ����+1
			try {
				realloc_n(2*size());	
			}
			catch (...) {
				realloc_n(size()+1);
			}
					
			//��ʱ�������������˵�һ�ּ�capacity_!=end_����ʵ����pos�����Ѿ���Ч��,��Ҫ�ض�λ
			sequence_insert(distance+begin_, capacity_, value);//ע�ⲻ��end_����capacity_
			capacity_++;
		}
	}

	//���·���һ���СΪn�Ŀռ��ԭ����,n����С��ԭ�гߴ�
	template <class T>
	void vector<T>::realloc_n(size_type n) {
		MYSTL_DEBUG(n > size());
		iterator temp_begin = data_allocator::allocate(n);
		copyrange_fill(begin_, end_, temp_begin);
	    //capacity_����ԭ��״̬���䣬��ʱ����������û���ƶ���size()����ԭ��Ԫ������
		iterator temp_cap = temp_begin + size();
		iterator temp_end = temp_begin + n;
		swap_(begin_, temp_begin);
		swap_(capacity_, temp_cap);
		swap_(end_, temp_end);
		data_allocator::destroy_obj(temp_begin, temp_end);
		temp_begin = nullptr;
		temp_end = nullptr;
		temp_cap = nullptr;
	}

	//���·���һ���СΪn�Ŀռ��ԭ����,n������ԭ�гߴ�
	template <class T>
	void vector<T>::shrink_realloc_n(size_type n) {
		MYSTL_DEBUG(n <= size());
		iterator temp_begin = data_allocator::allocate(n);
		//ֻ�ܸ�������n��Ԫ��
		copyrange_fill(begin_, begin_+n, temp_begin);
		//ע��capacity_��realloc_n�������ͬ
		iterator temp_cap = temp_begin + n;
		iterator temp_end = temp_cap + n;
		swap_(begin_, temp_begin);
		swap_(capacity_, temp_cap);
		swap_(end_, temp_end);
		data_allocator::destroy_obj(temp_begin, temp_end);
		temp_begin = nullptr;
		temp_end = nullptr;
		temp_cap = nullptr;
	}

	//resize�����ռ��С������ռ��󣬶���Ĳ�������Ҳ������ʼ����
	template <class T>
	void vector<T>::resize(size_type n) {
		if (n > size()) {
			realloc_n(n);
		}
		else shrink_realloc_n(n);
	}

	//reserveֻ�ܽ��ռ�������ռ��󣬶���Ĳ������ﲻ����ʼ����
	template <class T>
	void vector<T>::reserve(size_type n) {
		realloc_n(n);
	}

	//�����Է�������Ŀռ�
	template <class T>
	void vector<T>::shrink_to_fit() {
		if(end_!=capacity_)
		{
			data_allocator::destroy_obj(capacity_, end_);
			end_ = capacity_;
		}
	}

	//push_back�൱����capacityλ�ò���һ��Ԫ��
	//��ʵ��һ��͵���������������˷�װ�õ�insert()
	template <class T>
	void vector<T>::push_back(const T& value) {
		insert(capacity_, value);
	}

	//�Ƚϲ���������
	template <class T>
	bool operator==(const vector<T>& vec1, const vector<T>& vec2)
	{
		/*��Ա�������������const���Σ������������޷�ʹ�ã�
		��Ϊʹ��const vector<t>&���Ͳ���ʱ����vector����Ϊconst����	
		��ʱ���ó�Ա����begin()�൱��const this->begin()��
		��begin()�Ķ�����û��const�ͻᱨ���޷�ת��*/
		return vec1.size() == vec2.size() &&
			lightstl::equal(vec1.begin(), vec1.end(), vec2.begin());
	}
	template <class T>
	bool operator<(const vector<T>& vec1, const vector<T>& vec2)
	{
		return lightstl::lexicographical_compare(vec1.begin(), vec1.end(), vec2.begin(), vec2.end());
	}

	template <class T>
	bool operator!=(const vector<T>& vec1, const vector<T>& vec2)
	{
		return !(vec1 == vec2);
	}

	template <class T>
	bool operator>(const vector<T>& vec1, const vector<T>& vec2)
	{
		return vec2 < vec1;
	}

	template <class T>
	bool operator<=(const vector<T>& vec1, const vector<T>& vec2)
	{
		return !(vec2 < vec1);
	}

	template <class T>
	bool operator>=(const vector<T>& vec1, const vector<T>& vec2)
	{
		return !(vec1 < vec2);
	}
}

