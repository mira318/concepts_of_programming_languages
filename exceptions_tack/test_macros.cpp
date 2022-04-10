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

class MyCrocodile{
    int a;
    std::string b;
    std::vector<int> c;

public:
    MyCrocodile(int a_t, std::string b_t): a(a_t), b(b_t){
        for(int i = 0; i < 10; ++i){
            c.push_back(i);
        }
        std::cout << "created a crocodile, a = " << a << std::endl;
    }
    ~MyCrocodile(){
        c.clear();
        std::cout << "destroying a crocodile, a = " << a << std::endl;
    }
};

void b() {
    MyDouble in_b = MyDouble(3.0);
    REGISTER(MyDouble, in_b);
    THROW(8)
}

void c(){
    MyCrocodile last_croc = MyCrocodile(4, "they will leave forever");
    REGISTER(MyCrocodile, last_croc);
    std::cout << "In void c" << std::endl;
}

void a(){
    TRY
        MyCrocodile try_in_a = MyCrocodile(2, "a");
        REGISTER(MyCrocodile, try_in_a);
        b();
    CATCH(9)
        std::cout << "Catching 9" << std::endl;
    END_CATCH
    c();
}

void test_in_function(){
    TRY
        MyDouble first_try = MyDouble(1.0);
        REGISTER(MyDouble, first_try);
        a();
    CATCH(8)
        std::cout << "Catching 8" << std::endl;
    END_CATCH
    std::cout << "Done catching from function" << std::endl;

}

void test_simple_inner(){
    TRY
        MyCrocodile crocodile = MyCrocodile(1, "a");
        REGISTER(MyCrocodile, crocodile)
        TRY
            MyCrocodile crocodil = MyCrocodile(2, "aba");
            REGISTER(MyCrocodile, crocodil)
            TRY
                MyCrocodile crocodi = MyCrocodile(3, "abacaba");
                REGISTER(MyCrocodile, crocodi)
                THROW(5)
            CATCH(9)
                std::cout << "Catching 9" << std::endl;
            END_CATCH
        CATCH(7)
            std::cout << "Catching 7" << std::endl;
        END_CATCH
    CATCH(5)
        std::cout << "Catching 5" << std::endl;
    END_CATCH
    std::cout << "Gone through experimental try-catch" << std::endl;
}

void test_hard_inner(){
    MyDouble forever_stack_double = MyDouble(0.111);
    REGISTER(MyDouble, forever_stack_double)
    TRY
        MyCrocodile crocodile = MyCrocodile(10, "abacabadaba");
        REGISTER(MyCrocodile, crocodile);
        TRY
            MyDouble second_try_double = MyDouble(2.2222);
            REGISTER(MyDouble, second_try_double);
            TRY
                MyCrocodile crocod = MyCrocodile(9, "abacaba");
                REGISTER(MyCrocodile, crocod);
                MyDouble third_try_double = MyDouble(3.03);
                REGISTER(MyDouble, third_try_double);
                TRY
                    MyDouble inside_double = MyDouble(4.04);
                    REGISTER(MyDouble, inside_double);
                    THROW(11)
                CATCH(7)
                    MyCrocodile extinct_animal = MyCrocodile(8, "aba");
                    REGISTER(MyCrocodile, extinct_animal);
                    std::cout << "Catching 7" << std::endl;
                END_CATCH
            CATCH(11)
                MyDouble alive_double = MyDouble(6.66);
                REGISTER(MyDouble, alive_double);
                std::cout << "Catching 11" << std::endl;
                TRY
                    MyCrocodile strange_croc = MyCrocodile(7, "a");
                    REGISTER(MyCrocodile, strange_croc);
                    int another_variable = 42;
                    REGISTER(int, another_variable);
                    THROW(13)
                CATCH(13)
                    std::string stringtype_variable = "Morax need mora";
                    REGISTER(std::string, stringtype_variable);
                    std::cout << "Catching inner 13" << std::endl;
                    MyDouble have_mora = MyDouble(0.001);
                    REGISTER(MyDouble, have_mora);
                END_CATCH
                MyCrocodile inside_another_try = MyCrocodile(23, "time");
                REGISTER(MyCrocodile, inside_another_try);
                RETHROW
            END_CATCH
        CATCH(11)
            std::cout << "Catching rethrown 11" << std::endl;
            MyDouble yet_another_double = MyDouble(321.464);
            REGISTER(MyDouble, yet_another_double)
            std::string precatious = "don't try to make it shorter";
            REGISTER(std::string, precatious)
            RETHROW
        END_CATCH
    CATCH(11)
        MyDouble another_double_end = MyDouble(99.9);
        REGISTER(MyDouble, another_double_end);
        std::cout << "Caught 7 as final" << std::endl;
    END_CATCH
    std::cout << "Gone through harder example" << std::endl;
}

int main(){
    test_simple_inner();
    test_hard_inner();
    test_in_function();
    return 0;
}

