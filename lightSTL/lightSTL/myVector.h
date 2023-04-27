#pragma once
#include "myalgorithm.h"
#include "myutil.h"
#include "myiter.h"
#include "myexcept.h"
#include "myalloc.h"
#include "myuninitial.h"

//截止到现在，原作lightstl还没有分离，如果出现了奇怪的重载问题或者错误，记得去除源代码文件
namespace lightstl {

	template <class T>
	class vector {
	public:
		//区别数据分配器和分配器类
		typedef lightstl::myallocator<T>             myallocator;
		typedef lightstl::myallocator<T>             data_allocator;

		typedef typename myallocator::value_type  value_type;
		typedef typename myallocator::reference  reference;

		typedef value_type*                       iterator;
		typedef size_t                            size_type;
		/*vector无需定义专属迭代器，使用指针即可完成
		由于指针自增会考虑到自身指向的数据类型，因此不必重载指针++运算
		三个指针负责头尾和最后一个元素的下一个位置*/
	private:
		iterator begin_;
		iterator capacity_;
		//注意end的位置在最后一个元素的后一个位置，其本身指向的内存是没有分配的
		iterator end_;
		//最小初始化空间，这里暂时不用了，后续检测shrink_to_fit再用
		size_type min_init_space = 1;

	public:
		//无参数初始化
		vector() noexcept
		{
			try_init();
		}

		/*带参数初始化,允许分配n个某种类型的单位空间，使用默认值来初始化
		size_t是一个无符号整数，该类型只依赖于机器而非编译器，因此相比int可以提高可移植性
		应当把size_t重命名为size_type以方便理解
		应当拒绝隐式转换构造*/
		explicit vector(size_type n) {
			/*应当使用默认值来初始化，如何使用类的默认值？
			对于一个自定义类T来说，它有自己的默认构造函数T()
			对于原生数据类型例如int来说，它有自己的默认构造方式int()
			例如int a=int();则a的值取默认值0*/
			fill_init(n, value_type());
		}

		//使用类型值来初始化vector向量，示例：vector(10,5)
		//const修饰参数防止意外改动，const修饰引用则允许传入右值
		vector(size_type n, const value_type& value) {
			fill_init(n, value);
		}

		//迭代器初始化，预先检验类型
		template <class Iter, typename std::enable_if<std::is_same<Iter,iterator>::value,int>::type = 0>
		vector(Iter first, Iter last)
		{
			//指针保护
			MYSTL_DEBUG(!(last < first));
			range_init(first, last);
		}

        //拷贝初始化
		vector(const vector& otherVec) {
			range_init(otherVec.begin(), otherVec.end());
		}

		//移动构造函数,作浅拷贝使用
		vector(vector&& rhs) noexcept
			:begin_(rhs.begin()),
			end_(rhs.end()),
			capacity_(rhs.capacity())
		{
			rhs.begin_ = nullptr;
			rhs.end_ = nullptr;
			rhs.capacity_ = nullptr;
		}

		//initializer_list也是一种模板，用于传递可变数量的参数（类似可变参数）
		//使用的时候将任意个参数传入即可，实现列表初始化
		vector(std::initializer_list<value_type> ilist)
		{
			range_init(ilist.begin(), ilist.end());
		}

		//析构
		~vector()
		{
			//利用指针释放空间
			destroy_and_recover(begin_, end_, capacity_ - begin_);
			//释放指针
			begin_ = end_ = capacity_ = nullptr;
		}

		//迭代器操作
		iterator begin() const noexcept
		{ return begin_; }
		const iterator cbegin() const noexcept 
		{return begin_;}
		iterator end() const noexcept
		{ return end_; }
		const iterator cend()const noexcept
		{return end_;}

		//容量相关
		bool empty()  const 
		{ return begin_ == end_; }
		size_type size() const 
		{return static_cast<size_type>(end_ - begin_);}
		size_type capacity() const 
		{return static_cast<size_type>(capacity_ - begin_);}
		//预留空间
		void reserve(size_type n);
		//放弃多余的容量
		void shrink_to_fit();

		//下标访问相关
		reference operator[](size_type n) {
			//保护：下标不能越界
			MYSTL_DEBUG(n < size());
			return *(begin_ + n);
		}
		reference at(size_type n)
		{
			THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() subscript out of range");
			return (*this)[n];
		}
		reference front() {//获取第一个元素
			MYSTL_DEBUG(!empty());
			return *begin_;
		}
		reference back() {//获取最后一个
			MYSTL_DEBUG(!empty());
			return *(end_ - 1);
		}

		//常用方法
		void swap(vector& otherVec);
		void erase(iterator ptr);
		void print();
		size_type count(const T& value);
		iterator find(const T& value);
		void insert(iterator pos, const T& value);
		void resize(size_type n);
		void push_back(const T& value);
		

		//赋值运算符的不同重载
		vector& operator=(const vector& otherVec) {
			//拷贝初始化一个对象
			vector<T> tempVect(otherVec);
			swap(this, tempVect);
			//回收临时的对象
			~tempVect();
			return *this;
		}

		//vector& operator=(vector&& otherVec) {
		//	//浅拷贝，只交换指针
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
			//列表初始化
			vector<T> tempVect(ilist.begin(),ilist.end());
			swap(this, tempVect);
			~tempVect();
			return *this;
		}


	private:
		void init_space(size_type n, size_type cap);
		//初始化系列方法
		//不允许抛出错误，如果代码出错了只会报编译器错误，可能妨碍排查
		void try_init() noexcept;
		void fill_init(size_type n, const value_type& value);
		//多个模板类型的声明
		template<class Iter>
		void range_init(Iter left, Iter right);

		//析构中释放空间的操作
		void destroy_and_recover(iterator left, iterator right, size_type n);

		//重新分配，该方法将大小为n的空间分配给原容器
		void realloc_n(size_type n);
		//该方法处理n小于size的情况（向量缩短）
		void shrink_realloc_n(size_type n);
	};

	template<class T>
	void vector<T>::init_space(size_type size, size_type cap) {
		//该方法申请分配空间，大小至少为16个单位
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
	//void vector<T>::try_init是将成员函数写在外面的固定格式
	template <class T>
	void vector<T>::try_init() noexcept {
		//默认初始化，尝试分配16个单位给实例化的对象
		try {
			begin_ = data_allocator::allocate(min_init_space);
			end_ = begin_;//虽然分配到了空间，但是还是空的
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
		//static_cast强制类型转换，该操作申请一个空间且大小至少为16
		const size_type init_size = max(static_cast<size_type>(min_init_space), n);
		init_space(init_size, n);
		//接下来用value填充该空间
		fill_section(begin_, n, value);
	}

	//析构的善后工作
	//当vector在内存移动时，也需要此函数善后
	template<class T>
	inline void vector<T>::destroy_and_recover(iterator left, iterator right, size_type n)
	{
		//从左指针到右指针的空间内全部析构
		data_allocator::destroy_obj(left, right);
		//原空间重新分配
		std::cout << "Object recycled." << std::endl;
	}

	//序列初始化，使用序列两端的指针初始化vector对象
	//支持迭代器初始化，list初始化，vector初始化
	template<class T>
	template<class Iter>
	void vector<T>::range_init(Iter left, Iter right) {
		const size_type len = distance(left, right);
		const size_type init_size = max(static_cast<size_type>(min_init_space),len);
		init_space(init_size, len);
		//把内容从另一个容器中复制到本容器的区间,千万别搞错
		copyrange_fill(left,right,begin_);
	}

	/*和另一个容器交换，直接交换指针即可,该方法有问题，运行有时可以通过，有时报内存错误
	经排查该函数可能没有错误，错误在std::cout造成的内存泄漏*/
	template<class T>
	void vector<T>::swap(vector<T>& otherVec) {
		//if(this!=otherVec)还没有重载运算符，等会再写，解决后应当删除这条注释
		{
			swap_(begin_, otherVec.begin_);
			swap_(end_, otherVec.end_);
			swap_(capacity_, otherVec.capacity_);
		}
	}

	template<class T>
	void vector<T>::erase(iterator ptr ) {
		/*用后一个对象的值给前一个对象赋值
		  回收最后一个对象
		  暂时没有重载=运算符，因此不支持内嵌vetcor,解决后应当删除这条注释*、
		/*auto pos = ptr;
		for (; pos != (end_-1); pos++) {
			*(pos) = *(pos + 1);
		}*///出于泛型编程考虑，该段被封装到algo.base供顺序容器使用
		auto pos=sequence_erase(ptr, end_);
		data_allocator::destroy_obj(++pos);
		end_--;
		capacity_--;
		pos = nullptr;
	}

	//这个方法没有限制T的类型，存在很大的问题，仅限测试时使用
	template <class T>
	void vector<T>::print() {
		auto ptr = begin_;
		for (; ptr!=end_; ptr++) {
			std::cout << *(ptr);
		}
		std::cout << std::endl;
		ptr = nullptr;
	}

	//count(),查找与键值value相同的对象数量
	//const修饰引用同时允许左值和右值传入
	template <class T>
	size_type vector<T>::count(const T& value) {
		return lightstl::count_value(begin_, end_, value);
	}

	//find()查找第一个键值为value的迭代器位置
	//必须写typename vector<T>::iterator作为返回类型以和iterator类区别开
	template<class T>
	typename vector<T>::iterator vector<T>::find(const T& value)
	{
		return lightstl::find_value(begin_, end_, value);
	}

	/*insert负责插入元素，如果capacity<size, 那么将插入位置以后的元素向前移一个位置
	如果capacity=size，那么需要申请新的空间*/
	template <class T> 
	void vector<T>::insert(iterator pos, const T& value) {
		if (capacity_ != end_) {//使用！=判断会出错
			sequence_insert(pos, capacity_,value);//注意不是end_而是capacity_
			capacity_++;
		}
		else {
			//先追加申请空间是更好的办法
			//只需申请一块大小为size()+1个单位的空间并使用辅助指针指向它
			//然后依次复制原容器的值，使用copyrange_fill填充之
			//再将此空间的指针与原对象指针交换，最后释放原空间并回收辅助指针
			auto distance = pos - begin_;
			//加倍申请策略,如果失败就改为线性+1
			try {
				realloc_n(2*size());	
			}
			catch (...) {
				realloc_n(size()+1);
			}
					
			//此时情况看起来变成了第一种即capacity_!=end_，但实际上pos参数已经无效了,需要重定位
			sequence_insert(distance+begin_, capacity_, value);//注意不是end_而是capacity_
			capacity_++;
		}
	}

	//重新分配一块大小为n的空间给原容器,n不会小于原有尺寸
	template <class T>
	void vector<T>::realloc_n(size_type n) {
		MYSTL_DEBUG(n > size());
		iterator temp_begin = data_allocator::allocate(n);
		copyrange_fill(begin_, end_, temp_begin);
	    //capacity_保持原有状态不变，此时三个迭代器没有移动，size()仍是原有元素数量
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

	//重新分配一块大小为n的空间给原容器,n不大于原有尺寸
	template <class T>
	void vector<T>::shrink_realloc_n(size_type n) {
		MYSTL_DEBUG(n <= size());
		iterator temp_begin = data_allocator::allocate(n);
		//只能复制至多n个元素
		copyrange_fill(begin_, begin_+n, temp_begin);
		//注意capacity_和realloc_n的情况不同
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

	//resize允许将空间变小，如果空间变大，多余的部分这里也不做初始化了
	template <class T>
	void vector<T>::resize(size_type n) {
		if (n > size()) {
			realloc_n(n);
		}
		else shrink_realloc_n(n);
	}

	//reserve只能将空间变大，如果空间变大，多余的部分这里不做初始化了
	template <class T>
	void vector<T>::reserve(size_type n) {
		realloc_n(n);
	}

	//收缩以放弃多余的空间
	template <class T>
	void vector<T>::shrink_to_fit() {
		if(end_!=capacity_)
		{
			data_allocator::destroy_obj(capacity_, end_);
			end_ = capacity_;
		}
	}

	//push_back相当于在capacity位置插入一个元素
	//其实是一个偷懒的做法，借用了封装好的insert()
	template <class T>
	void vector<T>::push_back(const T& value) {
		insert(capacity_, value);
	}

	//比较操作符重载
	template <class T>
	bool operator==(const vector<T>& vec1, const vector<T>& vec2)
	{
		/*成员方法定义必须用const修饰，否则在这里无法使用，
		因为使用const vector<t>&类型参数时，该vector对象为const类型	
		此时调用成员函数begin()相当于const this->begin()，
		而begin()的定义中没有const就会报错无法转换*/
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

