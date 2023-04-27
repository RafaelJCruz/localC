//复杂一点的算法
namespace lightstl {
	/*###############################################################################*/
	/*all_of()将检验(left,right)区间内的所有对象，
	所有对象经函数或一元运算符（仿函数）unary运算后如果为非零，则allof()判断为真
	例如向量v=｛1，2，3，4，5｝
	可以使用all_of(v.begin(),v.end(),[](int i){return i%2;})判断向量的元素是否全部为偶数*/
	/*###############################################################################*/
	template<class Iter, class unary_Predicate>
	bool all_of(Iter left, Iter right, unary_Predicate unary) {
		for (; left < right; left++) {
			if (!(unary(*left)))
				return false;
		}
		return true;
	}
	/*###############################################################################*/
	/*any_of()功能类似，但只要有一个元素符合要求就为真*/
	/*###############################################################################*/
	template<class Iter, class unary_Predicate>
	bool any_of(Iter left, Iter right, unary_Predicate unary) {
		for (; left < right; left++) {
			if ((unary(*left)))
				return true;
		}
		return false;
	}

	/*###############################################################################*/
	/*在区间内寻找值等于value的对象个数*/
	/*###############################################################################*/
	template<class T, class Iter>
	size_t count_value(Iter left, Iter right, const T& value) {
		size_t n = 0;
		for (; left < right; left++) {
			if (*(left) == value){
				n++;
			}
		}
		return n;
	}

	/*###############################################################################*/
	/*在区间内寻找第一个值为value的对象并返回迭代器*/
	/*###############################################################################*/
	template<class T, class Iter>
	Iter find_value(Iter left, Iter right, const T& value) {
		size_t n = 0;
		for (; left < right; left++) {
			if (*(left) == value){
				return left;
			}	
		}
		return nullptr;
	}

	/*###############################################################################*/
	/*upper_bound,广义二分查找上界，暂时不写了*/
	/*###############################################################################*/
}