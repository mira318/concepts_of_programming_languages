#include "exceptions.h"
#include <vector>

class MyDouble{
    double value;
public:
    MyDouble(double val): value(val){
        std::cout << "Created MyDouble from " << val << std::endl;
    }
    ~MyDouble(){
        std::cout << "Destroying MyDouble value = " << value << std::endl;
    }
};

class MyBadCrocodile{
    int a;
    std::string b;
    std::vector<int> c;

public:
    MyBadCrocodile(int a_t, std::string b_t): a(a_t), b(b_t){
        for(int i = 0; i < 10; ++i){
            c.push_back(i);
        }
        std::cout << "created a dangerous animal, a = " << a << std::endl;
    }
    ~MyBadCrocodile(){
        TRY
           std::cout << "destroying a crocodile, a = " << a << std::endl;
           c.clear();
        THROW("ARITHMETIC_EXCEPTION")
        CATCH("ARITHMETIC_EXCEPTION")
           std::cout << "caught arithmetic exception in destructor" << std::endl;
        END_CATCH
    }
};

int main(){
    INIT
    TRY
        MyDouble my_double = MyDouble(1.23);
        MyBadCrocodile croc = MyBadCrocodile(7, "abacaba");
        REGISTER(MyDouble, my_double);
        REGISTER(MyBadCrocodile, croc);
    THROW("NO_SUCH_METHOD_EXCEPTION")
    CATCH("NO_SUCH_METHOD_EXCEPTION")
        std::cout << "Catching 13" << std::endl;
    END_CATCH
}