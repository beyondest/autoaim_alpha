#include <iostream>
class A
{
private:
    /* data */
    mutable int a = 0;
public:
    A(/* args */){};
    ~A(){};
    void operator()(const int a_) const
    {
        this->set_a(a_);
    }
    void set_a(int a_) const
    {
        this->a = a_;
        std::cout << "set_a: " << a_ << std::endl;

        std::cout << "this->a: " << this->a << std::endl;
    }
};




int main()
{
    
    std::cout << "Hello World!" << std::endl;
    A a;
    a(10);
    a(20);
    return 0;
}