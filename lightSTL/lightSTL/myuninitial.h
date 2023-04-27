#pragma once
//��������Ѿ����뵽���ڴ棬��һ����������߼��������Ŀռ丳ֵ
//ֻ��Ҫ�е�����ָ�뼴�ɣ�����������������������
#include "myconstruct.h"

namespace lightstl {
    typedef size_t size_type;

    //��ʼ���ķ������£��ڵ�ǰ������ָ���λ�ù���͸�ֵ����Ȼ��ָ���ƶ�����һ��λ�ã����ѭ��
    //�����������κ��������Ͷ�����������������Ҫ��������++������
    template<class T, class Iter_begin>
    void fill_section(Iter_begin begin, size_type n, T value) {
        //��begin��ʼ��������n����Ƕ����value
        auto cur = begin;
        try
        {
            for (; n > 0; --n, ++cur)
            {
                //ʹ��ֵvalue��ʼ������ʹ�����ô����β�
                construct(&*cur, value);
            }
        }
        catch (...)
        {
            std::cout << "fill_section_ERROR" << std::endl;
            for (; begin != cur; ++begin)
                //�����ö���
                destroy(&*begin);
        }
    }

  /*  ����ָ����һ�࣬beginptr����һ�ࣨ�������������ͬ��
    ������Ϊ������ʼ������ֵ���趨Ϊconst��������ʱ����ָ���ǳ���ָ�룬��beginptr�Ǳ��������Ͳ�ͬ���ܹ�����
    ��������ͬ���ĳ�ʼ��,�����ʼ�б��ʼ��
    ��������Ǵ��������*/
    template<class Iter,class Iter2>
    void copyrange_fill(Iter left,Iter right,Iter2 beginptr) {
        //����ҿ�����Ŀ���
        auto cur = left;
        auto target = beginptr;
        //auto i = 0;
        try
        {
            for (; cur!=right; ++cur)
            {   
                //cur����һ��������ָ�룬target����Ҫ���Ƶ�λ��
                //����Ѷ��߻����Ļ������
                auto value = *cur;
                construct(&*target,value);
                target++;
            //std::cout << i++;
            }
        }
        catch (...)
        {
            std::cout << "copyrange_fill_ERROR" << std::endl;
            //ԭ�����ǽ�beginptr��дΪcur���������Ͳ�ƥ�䵼���޷�����!=
            //�ڱ�дvectorʱ��ָ��������ͬ�ܹ��ڸ���һ���󣬵���dequeʱ�������������
            for (; target!= beginptr; --target)
                //�����ö���
                destroy(&*target);
        }
    }
}