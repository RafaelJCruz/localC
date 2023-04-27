#include "myalloc.h"
//基础的无差别通用算法
//在这些算法中，所有的顺序容器在逻辑上都是线性连续的对象序列
//迭代器必须能够建立逻辑结构和物理结构的映射以保证这一点
namespace lightstl {
	/*###############################################################################*/
	/*max函数*/
	/*###############################################################################*/
	template <class T>
	const T& max(const T& a, const T& b)
	{
		return a < b ? b : a;
	}

	// 重载版本使用函数对象 comp 代替比较操作
	template <class T, class Compare>
	const T& max(const T& a, const T& b, Compare comp)
	{
		return comp(a, b) ? b : a;
	}

	/*###############################################################################*/
	/*min函数*/
	/*###############################################################################*/
	template <class T>
	const T& min(const T& a, const T& b)
	{
		return a > b ? b : a;
	}
	
	//一样的
	template <class T, class Compare>
	const T& min(const T& a, const T& b, Compare comp)
	{
		return comp(a, b) ? a : b;
	}


	/*###############################################################################*/
	/*顺序容器擦除，擦除一个单位的内容并将后继内容赋值给前面,直到界限为止*/
	/*###############################################################################*/
	
	//end_ptr为容器最后一个元素后一位的迭代器
	template<class Iter>
	Iter sequence_erase(Iter ptr,Iter end_ptr) {
		MYSTL_DEBUG(ptr < end_ptr);
		auto pos = ptr;
		for (; pos != (end_ptr - 1); pos++) {
			*(pos) = *(pos + 1);
		};
		return pos;
	}
	/*###############################################################################*/
	/*顺序容器插入，将插入目标位置以后的元素前移，可能申请不到内存*/
	/*###############################################################################*/
	//end_ptr为区间结束位置的迭代器
	template<class T,class Iter>
	void  sequence_insert(Iter ptr, Iter end_ptr,const T& value) {
		for (; end_ptr !=ptr; end_ptr--) {
			*(end_ptr) = *(end_ptr - 1);
		};
		*(ptr) = value;
	}

	/*###############################################################################*/
	/*swap_交换任意两个对象，包括指针，类型必须一致*/
	/*###############################################################################*/
	template <class T>
	//和各个容器的swap区分，传入时必须使用引用
	void swap_(T& a, T& b) {
		auto temp = a;
		auto temp2 = b;
		a = temp2;
		b = temp;
	}

	/*###############################################################################*/
	/*equal判断两个对象在区间内是否相等*/
	/*###############################################################################*/
	template <class Iter1, class Iter2>
	bool equal(Iter1 first1, Iter1 last1, Iter2 first2)
	{
		for (; first1 != last1; ++first1, ++first2)
		{
			if (*first1 != *first2)
				return false;
		}
		return true;
	}

	/*****************************************************************************************/
	 /*lexicographical_compare,仿照STL实现
	 以字典序排列对两个序列进行比较，当在某个位置双方元素不等时，依次执行如下比较：
	 (1)如果该位置都不为空，比较该元素即可决定大小，如果相同则
	 (2)有一个序列在此处没有元素，则判定它较小
	 (3)如果都没有元素则判定第一个序列较小 false*/
	/*****************************************************************************************/
	template <class Iter1, class Iter2>
	bool lexicographical_compare(Iter1 first1, Iter1 last1,
		Iter2 first2, Iter2 last2)
	{
		for (; first1 != last1 && first2 != last2; ++first1, ++first2)
		{
			//第一个序列元素较小，则该序列判定为较小
			return *first1 < first2;
		}
		//较短的序列判定为较小
		return first1 == last1 && first2 != last2;
	}



}