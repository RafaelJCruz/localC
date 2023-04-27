#pragma once
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

#include "myalgorithm.h"
#include "myutil.h"
#include "myiter.h"
#include "myexcept.h"
#include "myalloc.h"
#include "myuninitial.h"

//deque是一个封装好的双端队列，也是stack的底层容器
//deque功能丰富，内容较多；需要设计专属的迭代器
//暂时实现如下几个方法：push,pop,insert，front,back，erase，这些即可供stack使用
namespace lightstl {
	//涉及到缓冲区的申请
	template <class T>
	struct deque_buf_size
	{
		/*sizeof()返回值单位为字节
		考虑到内存泄漏风险，一次不能分配太多
		static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;*/
		static constexpr size_t value = 256 / sizeof(T);
	};

	/*专属迭代器，应当实现如下功能：
	重载初始化和赋值操作
	实现对整个deque的访问，这通过重载++，--，+=，-=来实现，其底层逻辑是缓冲区之间的切换
	(对于deque类对象来说，其成员方法并不关心如何切换缓冲区)
	重载比较操作符*/
	template <class T, class Ref, class Ptr>
	struct deque_iterator {
	public:
		//没用到myiter里的诸多特征，暂时脱钩
		//实例化的对象
		typedef deque_iterator<T, T&, T*>             iterator;
		typedef deque_iterator<T, const T&, const T*> const_iterator;
		typedef deque_iterator                        self;

		typedef T            value_type;
		typedef Ptr          pointer;
		typedef Ref          reference;
		typedef size_t       size_type;
		typedef ptrdiff_t    difference_type;//该数据类型为long_int,记录指针距离
		typedef T* value_pointer;//一级指针，指向元素，不是迭代器
		typedef T** map_pointer;//中控器指针

		//每次申请的缓冲区大小
		static const size_type buffer_size = deque_buf_size<T>::value;
		//为实现逻辑上的顺序访问，迭代器需要知道的信息如下：
		value_pointer cur;//迭代器当前指向的元素
		value_pointer first;//分别指向缓冲区两端
		value_pointer last;
		//中控器指针，其指向的对象是一个缓冲区的首地址
		//这里指向的是当前所在缓冲区的首地址
		map_pointer node;

		//迭代器的初始化
		deque_iterator() :cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

		deque_iterator(value_pointer value_ptr, map_pointer mp_ptr)
			:cur(value_ptr), first(*mp_ptr), last(*mp_ptr + buffer_size), node(mp_ptr) {}

		deque_iterator(const iterator& otherIter)
			:cur(otherIter.cur), first(otherIter.first), last(otherIter.last), node(otherIter.node) {}

		deque_iterator(iterator&& otherIter)
			:cur(otherIter.cur), first(otherIter.first), last(otherIter.last), node(otherIter.node)
		{
			otherIter.cur = nullptr;
			otherIter.first = nullptr;
			otherIter.last = nullptr;
			otherIter.node = nullptr;
		}

		//赋值运算符
		self& operator=(const iterator& otherIter)
		{
			if (this != &otherIter)
			{
				cur = otherIter.cur;
				first = otherIter.first;
				last = otherIter.last;
				node = otherIter.node;
			}
			return *this;
		}

		//通过中控器切换到另一个缓冲区，同时设置区间指针
		void set_node(map_pointer new_node) {
			//原缓冲区到新缓冲区
			node = new_node;
			first = *new_node;
			last = first + buffer_size;
			cur = first;
		}

		//重载单目运算符，这使得迭代器看起来像是一个指向元素的简单指针，实际上远非如此
		reference operator*()const { return *cur; }
		value_pointer   operator->() const { return cur; }
		reference operator[](difference_type n) const { return *(*this + n); }

		//++，--负责实现迭代器的访问
		self& operator ++() {
			++cur;
			if (cur == last) {
				set_node(node + 1);
			}
			return *this;
		}

		self operator ++(int) {
			return ++ * this;
		}

		self& operator --() {
			--cur;
			if (cur == last) {
				set_node(node - 1);
			}
			return *this;
		}
		self operator --(int) {
			return -- * this;
		}

		//双目运算符
		//计算迭代器距离，const关键字谨慎使用
		difference_type operator-(const self& x) const {
			//这里如果直接继承myiter的距离计算方法会大大降低性能
			return static_cast<difference_type> (buffer_size) * (node - x.node) +
				(cur - first) - (x.cur - x.first);
		}
		//迭代器移动
		//+=处理是有问题的，但是不影响使用，暂不修改
		self& operator+=(difference_type n) {
			if (n < 0)
				return *this -= (-n);
			const auto total_interval = n + (cur - first);//相对当前区起点的总位移
			const auto node_num = total_interval / buffer_size;//跨越的缓冲区数
			const auto elem_num = total_interval % buffer_size;//前进移动的元素数
			set_node(node + node_num);
			cur += elem_num;
			return *this;
		}
		self operator+(difference_type n) const {
			self tmp = *this;
			return tmp += n;
		}
		self& operator-=(difference_type n) {
			if (n < 0)
				return *this += (-n);
			/*由于-=设计上稍微复杂一些，这里偷懒了
			-=套用--来完成，但是损失性能*/
			for (; n > 0; n--) {
				--* this;
			}
			return *this;
		}
		self operator-(difference_type n) const
		{
			self tmp = *this;
			return tmp -= n;
		}

		//重载比较操作符
		bool operator==(const self& rhs) const { return cur == rhs.cur; }
		bool operator< (const self& rhs) const
		{
			return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node);
		}
		bool operator!=(const self& rhs) const { return !(*this == rhs); }
		bool operator> (const self& rhs) const { return rhs < *this; }
		bool operator<=(const self& rhs) const { return !(rhs < *this); }
		bool operator>=(const self& rhs) const { return !(*this < rhs); }
	};

	/*deque由一系列离散的缓冲区构成，且左右两端的空间均可动态增长
	deque由于结构上的不同，相比vector来说需要额外设计空间管理和维护的方法和属性成员
	需要设计成员指针map指向一块空间，该空间保存指向deque每个缓冲区的指针，称为中控器
	当deque的空间动态增长时，申请新的缓冲区并将指针记录在*(map);
	当map也不够用时，仿效vector在内存中开辟新空间*/
	template <class T>
	class deque {
	public:
		//内置迭代器必须能够和中控器直接建立联系
		typedef lightstl::myallocator<T>             myallocator_type;
		typedef lightstl::myallocator<T>             data_allocator;
		typedef lightstl::myallocator<T*>				map_allocator;

		typedef deque_iterator<T, T&, T*>					 iterator;
		typedef deque_iterator<T, const T&, const T*>  const_iterator;


		typedef typename myallocator_type::value_type	   value_type;
		typedef typename myallocator_type::reference        reference;
		typedef typename myallocator_type::const_reference const_reference;
		typedef typename myallocator_type::pointer            pointer;

		typedef size_t                                      size_type;
		typedef pointer* map_pointer;

		//每个缓冲区能容纳的元素数量
		static const size_type buffer_size = deque_buf_size<T>::value;
	private:
		iterator			begin_;
		iterator			end_;//对外表现为分别指向队列第一个和最后一个元素,也指向队列第一个区间和最后一个区间
		map_pointer			map_;//中控器指针
		map_pointer         map_last;//指向中控器末尾区间
		size_type			map_size;//记录中控器尺寸(包括初始化的和未初始化的指针数量) 

	public:
		//构造和析构函数
		deque() {
			fill_init(0, value_type());
		}

		explicit deque(size_type n) {
			fill_init(n, value_type());
		}

		deque(size_type n, const T& value) {
			fill_init(n, value);
		}

		//迭代器初始化
		deque(iterator first, iterator last)
		{
			copy_init(first, last, begin());
		}

		//拷贝初始化,左右值均接受;析构拷贝初始化的对象存在问题
		deque(const deque& otherDeq) {
			copy_init(otherDeq.begin(), otherDeq.end(), begin());
		}

		//列表初始化
		deque(std::initializer_list<value_type> ilist) {
			copy_init(ilist.begin(), ilist.end(), begin());
		}

		//析构函数
		~deque() {
			//析构函数的操作：依次释放缓冲区，释放中控器，指针清空
			clear_buffer();
			//清空中控器
			map_allocator::deallocate(map_, map_size);
			map_ = nullptr;
			std::cout << "Object recycled." << std::endl;
		}
		//初始化辅助方法，设置为私有成员
	private:
		void fill_init(size_type n, const T& value);
		template <class Iter, class Iter2>
		void copy_init(Iter left, Iter right, Iter2 start);
		void map_init(size_type elem_num);
		map_pointer create_map(size_type mapSize);
		void create_buffer(map_pointer start, map_pointer end);
		void clear_buffer();

	public:
		//容量相关
		bool      empty()    const noexcept { return begin() == end(); }
		size_type size()     const noexcept { return end_ - begin_; }//size为元素数量
		size_type max_size() const noexcept { return static_cast<size_type>(-1); }
		/*void      resize(size_type new_size) { resize(new_size, value_type()); }
		void      resize(size_type new_size, const value_type& value);
		void      shrink_to_fit() noexcept;*/


		//访问元素相关
		iterator begin()   noexcept { return  begin_; }
		const_iterator begin() const noexcept { return begin_; }
		iterator end()   noexcept { return end_; }
		const_iterator end() const noexcept { return  end_; }
		reference operator[](size_type n) {
			return *(begin_ + n);
		}
		const_reference operator[](size_type n) const
		{
			MYSTL_DEBUG(n < size());
			return begin_[n];
		}
		reference       at(size_type n)
		{
			THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() subscript out of range");
			return (*this)[n];
		}
		const_reference at(size_type n) const
		{
			THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() subscript out of range");
			return (*this)[n];
		}
		reference       front()
		{
			MYSTL_DEBUG(!empty());
			return *begin();
		}
		const_reference front() const
		{
			MYSTL_DEBUG(!empty());
			return *begin();
		}
		reference       back()
		{
			MYSTL_DEBUG(!empty());
			return *(end() - 1);
		}
		const_reference back() const
		{
			MYSTL_DEBUG(!empty());
			return *(end() - 1);
		}

		//常用功能
		void push_front(const value_type& value);
		void push_back(const value_type& value);
		value_type pop_front();
		value_type pop_back();

	private:
		//追加n个缓冲区
		void require_buffer_back(size_type n);
		//重置中控器
		void reset_map(size_type n);


	};

	//填充初始化，申请容纳n个元素的空间并且全部用value填充
	template <class T>
	void deque<T>::fill_init(size_type n, const T& value) {
		//初始化中控器及下属缓冲区
		MYSTL_DEBUG(n != 0);
		map_init(n);
		//缓冲区的初始化则类似vector，从头开始
		fill_section(begin_, n, value);
	}

	//拷贝初始化，使用序列进行拷贝赋值
	template <class T>
	template <class Iter, class Iter2 >
	void deque<T>::copy_init(Iter left, Iter right, Iter2 start) {
		Iter lptr = left;
		Iter rptr = right;
		const size_type dist = right - left;
		map_init(dist);
		copyrange_fill(left, right, begin_);
	}

	//中控器及缓冲区初始化
	template <class T>
	void deque<T>::map_init(size_type n) {
		//确定申请缓冲区数量
		const size_type nodeNum = n / buffer_size + 1;  // 需要分配的缓冲区个数
		map_size = lightstl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), nodeNum + 2);//设置和限制中控器容量最低值
		try {
			map_ = create_map(map_size);
			map_last = map_ + map_size-1;//指向最后一个缓冲区
			//destroy_obj(map_, map_ + map_size);
		}
		catch (...) {
			map_ = nullptr;
			map_last = nullptr;
			map_size = 0;
		}
		//接下来为中控器指针申请缓冲区，初始化中控器中央的n个单位以方便向两端扩充
		map_pointer nstart = map_ + (map_size - nodeNum) / 2;
		map_pointer nfinish = nstart + nodeNum - 1;
		try {
			create_buffer(nstart, nfinish);
		}
		catch (...) {
			//一旦报错，回收整个中控器
			map_allocator::deallocate(map_, map_size);
			map_ = nullptr;
			map_size = 0;
			throw;
		}
		//空间处理就绪，接下来设置迭代器指向
		begin_.set_node(nstart);
		end_.set_node(nfinish);
		begin_.cur = begin_.first;
		end_.cur = end_.first + (n % buffer_size);
	}

	//在内存中开辟一块区域给中控器
	template <class T>
	typename deque<T>::map_pointer deque<T>::create_map(size_type mapSize) {
		map_pointer mp = nullptr;
		mp = map_allocator::allocate(mapSize);
		for (size_type i = 0; i < mapSize; ++i)
		{
			//*(map+i)是类型为T*的指针，初始化为空
			*(mp + i) = nullptr;
		}
		return mp;
	}

	//为中控器指定区间初始化,注意：该区间是闭区间
	template <class T>
	void deque<T>::create_buffer(map_pointer nstart, map_pointer nfinish) {
		map_pointer cur;
		try
		{
			for (cur = nstart; cur <= nfinish; ++cur)
			{
				//将申请到的空间地址装入中控器指定位置
				*cur = data_allocator::allocate(buffer_size);
			}
		}
		catch (...)
		{
			while (cur != nstart)
			{
				--cur;
				data_allocator::deallocate(*cur, buffer_size);
				*cur = nullptr;
			}
			throw;
		}
	}

	//清空中控器指向的全部区间
	template <class T>
	void deque<T>::clear_buffer() {
		//这里destroy_obj可以无视容器差别直接清除，原作者的思路则是逐个缓冲区清除，似乎麻烦了一些
		data_allocator::destroy_obj(begin_.cur, buffer_size);
		std::cout << "Buffer recycled." << std::endl;
	}

	//在队尾插入元素
	//该功能及底层逻辑尚未经过测试，暂不使用
	template <class T>
	void deque<T>::push_back(const value_type& value) {
		/*分两种种情况：
			队尾指针在最后一个缓冲区内部，直接插入即可
			队尾指针在最后一个缓冲区结尾，那么需要申请新的缓冲区
			第二种情况下，如果中控器空间耗尽，那么需要重新开辟中控器*/
		if (end_.cur != end_.last) {
			*(++end_.cur) = value;
		}
		else {
			require_buffer_back(1);
			*(++end_.cur) = value;
		}

		
	}

	//追加n个缓冲区
	template <class T>
	void deque<T>::require_buffer_back(size_type n) {
		map_pointer curEnd = end_.node;
		if ((map_last - curEnd) >= n) {
			//此时中控器队尾剩余空间充足，可以直接挂载
			create_buffer(curEnd,curEnd+n-1);
		}
		else {
			//重置中控器,尺寸加倍
			try {
				reset_map(2*n);
			}
			catch (...) {
				reset_map(n + 1);
			}
		}
			
	}

	//重置中继器尺寸为n
	template<class T>
	void deque<T>::reset_map(size_type new_size) {
		//先初始化新的map，然后拷贝赋值，再和map_交换，最后释放旧的
		//申请空间
		map_pointer tmp_map = create_map(new_size);
		//拷贝中控器内容,从新中控器中间开始
		//缓冲区数量
		size_type node_num =end_.node- begin_.node;
		map_pointer nstart = tmp_map + (new_size - node_num) / 2;
		map_pointer nfinish = nstart + node_num - 1;
		copyrange_fill(begin_.node, end_.node + 1, nstart);
		//交换指针
		swap_(tmp_map, map_);
		//回收旧的map
		map_allocator::deallocate(map_, map_size);
		//重设迭代器及其他变量
		begin_.set_node(nstart);
		end_.set_node(nfinish);
		begin_.cur= begin_.cur = begin_.first;
		end_.cur = end_.first + (size() % buffer_size);
		map_size = new_size;
	}


}











