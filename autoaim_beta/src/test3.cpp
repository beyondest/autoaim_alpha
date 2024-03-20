#include <iostream>





template<typename T>
void forwardFunc(T x)
{
    x++;
    std::cout << "forwardFunc: " << x << std::endl;
    std::cout << "forwardFunc: " << typeid(x).name() << std::endl;
    
    std::cout << "forwardFunc: " << &x << std::endl;
    
}


template<typename T>
void forwardFunc2(T&& x)
{
    x++;
    std::cout << "forwardFunc2: " << x << std::endl;
    std::cout << "forwardFunc2: " << typeid(x).name() << std::endl;
    std::cout << "forwardFunc2: " << &x << std::endl;

}


void add(float* a)
{
    (*a) ++;
    std::cout << "add: " << *a << std::endl;
    std::cout << "address: "<< a << std::endl;
}

int main()
{

    int m = 32;
    decltype(m) n = 42;
    std::cout << "m: " << m << std::endl;
    std::cout << "n: " << n << std::endl;
    std::cout << "type: " << typeid(m).name() << std::endl;
    std::cout << "type: " << typeid(n).name() << std::endl;
    int&& x = 10;
    std::cout << "address: " << &x << std::endl;
    std::cout << "value: " << x << std::endl;
    float f = 3.14;
    std::cout << "address: " << &f << std::endl;
    
    forwardFunc(f);
    std::cout << "after forwardFunc f: " << f << std::endl;
    forwardFunc(3.14);
    std::cout << "after forwardFunc 3.14: " << 3.14 << std::endl;
    forwardFunc2(f);
    std::cout << "after forwardFunc2 f: " << f << std::endl;
    forwardFunc2(3.14);
    std::cout << "after forwardFunc2 3.14: " << 3.14 << std::endl;
    add(&f);
    std::cout << "after add: " << f << std::endl;
    std::cout << "address after add: " << &f << std::endl;
    return 0;
}   


