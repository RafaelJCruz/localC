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

//deque��һ����װ�õ�˫�˶��У�Ҳ��stack�ĵײ�����
//deque���ܷḻ�����ݽ϶ࣻ��Ҫ���ר���ĵ�����
//��ʱʵ�����¼���������push,pop,insert��front,back��erase����Щ���ɹ�stackʹ��
namespace lightstl {
	//�漰��������������
	template <class T>
	struct deque_buf_size
	{
		/*sizeof()����ֵ��λΪ�ֽ�
		���ǵ��ڴ�й©���գ�һ�β��ܷ���̫��
		static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16;*/
		static constexpr size_t value = 256 / sizeof(T);
	};

	/*ר����������Ӧ��ʵ�����¹��ܣ�
	���س�ʼ���͸�ֵ����
	ʵ�ֶ�����deque�ķ��ʣ���ͨ������++��--��+=��-=��ʵ�֣���ײ��߼��ǻ�����֮����л�
	(����deque�������˵�����Ա����������������л�������)
	���رȽϲ�����*/
	template <class T, class Ref, class Ptr>
	struct deque_iterator {
	public:
		//û�õ�myiter��������������ʱ�ѹ�
		//ʵ�����Ķ���
		typedef deque_iterator<T, T&, T*>             iterator;
		typedef deque_iterator<T, const T&, const T*> const_iterator;
		typedef deque_iterator                        self;

		typedef T            value_type;
		typedef Ptr          pointer;
		typedef Ref          reference;
		typedef size_t       size_type;
		typedef ptrdiff_t    difference_type;//����������Ϊlong_int,��¼ָ�����
		typedef T* value_pointer;//һ��ָ�룬ָ��Ԫ�أ����ǵ�����
		typedef T** map_pointer;//�п���ָ��

		//ÿ������Ļ�������С
		static const size_type buffer_size = deque_buf_size<T>::value;
		//Ϊʵ���߼��ϵ�˳����ʣ���������Ҫ֪������Ϣ���£�
		value_pointer cur;//��������ǰָ���Ԫ��
		value_pointer first;//�ֱ�ָ�򻺳�������
		value_pointer last;
		//�п���ָ�룬��ָ��Ķ�����һ�����������׵�ַ
		//����ָ����ǵ�ǰ���ڻ��������׵�ַ
		map_pointer node;

		//�������ĳ�ʼ��
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

		//��ֵ�����
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

		//ͨ���п����л�����һ����������ͬʱ��������ָ��
		void set_node(map_pointer new_node) {
			//ԭ���������»�����
			node = new_node;
			first = *new_node;
			last = first + buffer_size;
			cur = first;
		}

		//���ص�Ŀ���������ʹ�õ���������������һ��ָ��Ԫ�صļ�ָ�룬ʵ����Զ�����
		reference operator*()const { return *cur; }
		value_pointer   operator->() const { return cur; }
		reference operator[](difference_type n) const { return *(*this + n); }

		//++��--����ʵ�ֵ������ķ���
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

		//˫Ŀ�����
		//������������룬const�ؼ��ֽ���ʹ��
		difference_type operator-(const self& x) const {
			//�������ֱ�Ӽ̳�myiter�ľ�����㷽�����󽵵�����
			return static_cast<difference_type> (buffer_size) * (node - x.node) +
				(cur - first) - (x.cur - x.first);
		}
		//�������ƶ�
		//+=������������ģ����ǲ�Ӱ��ʹ�ã��ݲ��޸�
		self& operator+=(difference_type n) {
			if (n < 0)
				return *this -= (-n);
			const auto total_interval = n + (cur - first);//��Ե�ǰ��������λ��
			const auto node_num = total_interval / buffer_size;//��Խ�Ļ�������
			const auto elem_num = total_interval % buffer_size;//ǰ���ƶ���Ԫ����
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
			/*����-=�������΢����һЩ������͵����
			-=����--����ɣ�������ʧ����*/
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

		//���رȽϲ�����
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

	/*deque��һϵ����ɢ�Ļ��������ɣ����������˵Ŀռ���ɶ�̬����
	deque���ڽṹ�ϵĲ�ͬ�����vector��˵��Ҫ������ƿռ�����ά���ķ��������Գ�Ա
	��Ҫ��Ƴ�Աָ��mapָ��һ��ռ䣬�ÿռ䱣��ָ��dequeÿ����������ָ�룬��Ϊ�п���
	��deque�Ŀռ䶯̬����ʱ�������µĻ���������ָ���¼��*(map);
	��mapҲ������ʱ����Чvector���ڴ��п����¿ռ�*/
	template <class T>
	class deque {
	public:
		//���õ����������ܹ����п���ֱ�ӽ�����ϵ
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

		//ÿ�������������ɵ�Ԫ������
		static const size_type buffer_size = deque_buf_size<T>::value;
	private:
		iterator			begin_;
		iterator			end_;//�������Ϊ�ֱ�ָ����е�һ�������һ��Ԫ��,Ҳָ����е�һ����������һ������
		map_pointer			map_;//�п���ָ��
		map_pointer         map_last;//ָ���п���ĩβ����
		size_type			map_size;//��¼�п����ߴ�(������ʼ���ĺ�δ��ʼ����ָ������) 

	public:
		//�������������
		deque() {
			fill_init(0, value_type());
		}

		explicit deque(size_type n) {
			fill_init(n, value_type());
		}

		deque(size_type n, const T& value) {
			fill_init(n, value);
		}

		//��������ʼ��
		deque(iterator first, iterator last)
		{
			copy_init(first, last, begin());
		}

		//������ʼ��,����ֵ������;����������ʼ���Ķ����������
		deque(const deque& otherDeq) {
			copy_init(otherDeq.begin(), otherDeq.end(), begin());
		}

		//�б��ʼ��
		deque(std::initializer_list<value_type> ilist) {
			copy_init(ilist.begin(), ilist.end(), begin());
		}

		//��������
		~deque() {
			//���������Ĳ����������ͷŻ��������ͷ��п�����ָ�����
			clear_buffer();
			//����п���
			map_allocator::deallocate(map_, map_size);
			map_ = nullptr;
			std::cout << "Object recycled." << std::endl;
		}
		//��ʼ����������������Ϊ˽�г�Ա
	private:
		void fill_init(size_type n, const T& value);
		template <class Iter, class Iter2>
		void copy_init(Iter left, Iter right, Iter2 start);
		void map_init(size_type elem_num);
		map_pointer create_map(size_type mapSize);
		void create_buffer(map_pointer start, map_pointer end);
		void clear_buffer();

	public:
		//�������
		bool      empty()    const noexcept { return begin() == end(); }
		size_type size()     const noexcept { return end_ - begin_; }//sizeΪԪ������
		size_type max_size() const noexcept { return static_cast<size_type>(-1); }
		/*void      resize(size_type new_size) { resize(new_size, value_type()); }
		void      resize(size_type new_size, const value_type& value);
		void      shrink_to_fit() noexcept;*/


		//����Ԫ�����
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

		//���ù���
		void push_front(const value_type& value);
		void push_back(const value_type& value);
		value_type pop_front();
		value_type pop_back();

	private:
		//׷��n��������
		void require_buffer_back(size_type n);
		//�����п���
		void reset_map(size_type n);


	};

	//����ʼ������������n��Ԫ�صĿռ䲢��ȫ����value���
	template <class T>
	void deque<T>::fill_init(size_type n, const T& value) {
		//��ʼ���п���������������
		MYSTL_DEBUG(n != 0);
		map_init(n);
		//�������ĳ�ʼ��������vector����ͷ��ʼ
		fill_section(begin_, n, value);
	}

	//������ʼ����ʹ�����н��п�����ֵ
	template <class T>
	template <class Iter, class Iter2 >
	void deque<T>::copy_init(Iter left, Iter right, Iter2 start) {
		Iter lptr = left;
		Iter rptr = right;
		const size_type dist = right - left;
		map_init(dist);
		copyrange_fill(left, right, begin_);
	}

	//�п�������������ʼ��
	template <class T>
	void deque<T>::map_init(size_type n) {
		//ȷ�����뻺��������
		const size_type nodeNum = n / buffer_size + 1;  // ��Ҫ����Ļ���������
		map_size = lightstl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), nodeNum + 2);//���ú������п����������ֵ
		try {
			map_ = create_map(map_size);
			map_last = map_ + map_size-1;//ָ�����һ��������
			//destroy_obj(map_, map_ + map_size);
		}
		catch (...) {
			map_ = nullptr;
			map_last = nullptr;
			map_size = 0;
		}
		//������Ϊ�п���ָ�����뻺��������ʼ���п��������n����λ�Է�������������
		map_pointer nstart = map_ + (map_size - nodeNum) / 2;
		map_pointer nfinish = nstart + nodeNum - 1;
		try {
			create_buffer(nstart, nfinish);
		}
		catch (...) {
			//һ���������������п���
			map_allocator::deallocate(map_, map_size);
			map_ = nullptr;
			map_size = 0;
			throw;
		}
		//�ռ䴦����������������õ�����ָ��
		begin_.set_node(nstart);
		end_.set_node(nfinish);
		begin_.cur = begin_.first;
		end_.cur = end_.first + (n % buffer_size);
	}

	//���ڴ��п���һ��������п���
	template <class T>
	typename deque<T>::map_pointer deque<T>::create_map(size_type mapSize) {
		map_pointer mp = nullptr;
		mp = map_allocator::allocate(mapSize);
		for (size_type i = 0; i < mapSize; ++i)
		{
			//*(map+i)������ΪT*��ָ�룬��ʼ��Ϊ��
			*(mp + i) = nullptr;
		}
		return mp;
	}

	//Ϊ�п���ָ�������ʼ��,ע�⣺�������Ǳ�����
	template <class T>
	void deque<T>::create_buffer(map_pointer nstart, map_pointer nfinish) {
		map_pointer cur;
		try
		{
			for (cur = nstart; cur <= nfinish; ++cur)
			{
				//�����뵽�Ŀռ��ַװ���п���ָ��λ��
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

	//����п���ָ���ȫ������
	template <class T>
	void deque<T>::clear_buffer() {
		//����destroy_obj���������������ֱ�������ԭ���ߵ�˼·�������������������ƺ��鷳��һЩ
		data_allocator::destroy_obj(begin_.cur, buffer_size);
		std::cout << "Buffer recycled." << std::endl;
	}

	//�ڶ�β����Ԫ��
	//�ù��ܼ��ײ��߼���δ�������ԣ��ݲ�ʹ��
	template <class T>
	void deque<T>::push_back(const value_type& value) {
		/*�������������
			��βָ�������һ���������ڲ���ֱ�Ӳ��뼴��
			��βָ�������һ����������β����ô��Ҫ�����µĻ�����
			�ڶ�������£�����п����ռ�ľ�����ô��Ҫ���¿����п���*/
		if (end_.cur != end_.last) {
			*(++end_.cur) = value;
		}
		else {
			require_buffer_back(1);
			*(++end_.cur) = value;
		}

		
	}

	//׷��n��������
	template <class T>
	void deque<T>::require_buffer_back(size_type n) {
		map_pointer curEnd = end_.node;
		if ((map_last - curEnd) >= n) {
			//��ʱ�п�����βʣ��ռ���㣬����ֱ�ӹ���
			create_buffer(curEnd,curEnd+n-1);
		}
		else {
			//�����п���,�ߴ�ӱ�
			try {
				reset_map(2*n);
			}
			catch (...) {
				reset_map(n + 1);
			}
		}
			
	}

	//�����м����ߴ�Ϊn
	template<class T>
	void deque<T>::reset_map(size_type new_size) {
		//�ȳ�ʼ���µ�map��Ȼ�󿽱���ֵ���ٺ�map_����������ͷžɵ�
		//����ռ�
		map_pointer tmp_map = create_map(new_size);
		//�����п�������,�����п����м俪ʼ
		//����������
		size_type node_num =end_.node- begin_.node;
		map_pointer nstart = tmp_map + (new_size - node_num) / 2;
		map_pointer nfinish = nstart + node_num - 1;
		copyrange_fill(begin_.node, end_.node + 1, nstart);
		//����ָ��
		swap_(tmp_map, map_);
		//���վɵ�map
		map_allocator::deallocate(map_, map_size);
		//�������������������
		begin_.set_node(nstart);
		end_.set_node(nfinish);
		begin_.cur= begin_.cur = begin_.first;
		end_.cur = end_.first + (size() % buffer_size);
		map_size = new_size;
	}


}











