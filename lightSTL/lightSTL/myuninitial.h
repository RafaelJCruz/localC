#pragma once
//负责管理已经申请到的内存，对一段物理或者逻辑上连续的空间赋值
//只需要有迭代器指针即可，而迭代器可以脱离对象存在
#include "myconstruct.h"

namespace lightstl {
    typedef size_t size_type;

    //初始化的方法如下：在当前迭代器指向的位置构造和赋值对象，然后指针移动到下一个位置，如此循环
    //这个抽象对于任何数据类型都成立，各个类型需要自行重载++等运算
    template<class T, class Iter_begin>
    void fill_section(Iter_begin begin, size_type n, T value) {
        //从begin开始连续填入n个内嵌对象value
        auto cur = begin;
        try
        {
            for (; n > 0; --n, ++cur)
            {
                //使用值value初始化对象，使用引用代替形参
                construct(&*cur, value);
            }
        }
        catch (...)
        {
            std::cout << "fill_section_ERROR" << std::endl;
            for (; begin != cur; ++begin)
                //析构该对象
                destroy(&*begin);
        }
    }

  /*  左右指针是一类，beginptr是另一类（尽管两类可以相同）
    这是因为拷贝初始化传入值会设定为const常量，此时左右指针是常量指针，而beginptr是变量，类型不同仍能够操作
    这样允许不同类别的初始化,例如初始列表初始化
    但此设计是存在问题的*/
    template<class Iter,class Iter2>
    void copyrange_fill(Iter left,Iter right,Iter2 beginptr) {
        //左闭右开区间的拷贝
        auto cur = left;
        auto target = beginptr;
        //auto i = 0;
        try
        {
            for (; cur!=right; ++cur)
            {   
                //cur是另一个容器的指针，target才是要复制的位置
                //如果把二者混淆的话会出错
                auto value = *cur;
                construct(&*target,value);
                target++;
            //std::cout << i++;
            }
        }
        catch (...)
        {
            std::cout << "copyrange_fill_ERROR" << std::endl;
            //原错误是将beginptr误写为cur，两者类型不匹配导致无法重载!=
            //在编写vector时两指针类型相同能够掩盖这一错误，但在deque时出现了这个问题
            for (; target!= beginptr; --target)
                //析构该对象
                destroy(&*target);
        }
    }
}