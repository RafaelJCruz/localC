//����һ����㷨
namespace lightstl {
	/*###############################################################################*/
	/*all_of()������(left,right)�����ڵ����ж���
	���ж��󾭺�����һԪ��������º�����unary��������Ϊ���㣬��allof()�ж�Ϊ��
	��������v=��1��2��3��4��5��
	����ʹ��all_of(v.begin(),v.end(),[](int i){return i%2;})�ж�������Ԫ���Ƿ�ȫ��Ϊż��*/
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
	/*any_of()�������ƣ���ֻҪ��һ��Ԫ�ط���Ҫ���Ϊ��*/
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
	/*��������Ѱ��ֵ����value�Ķ������*/
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
	/*��������Ѱ�ҵ�һ��ֵΪvalue�Ķ��󲢷��ص�����*/
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
	/*upper_bound,������ֲ����Ͻ磬��ʱ��д��*/
	/*###############################################################################*/
}