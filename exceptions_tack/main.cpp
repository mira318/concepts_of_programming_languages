#include <iostream>
#include <csetjmp>
#include <vector>

const int MAX_BUFFERS_NUM = 1e5;
const int MAX_EXCEPTIONS_STACKED = 1e5;
int last_env = -1;
int last_exception = -1;
std::jmp_buf env_buffers[MAX_BUFFERS_NUM];

int exception_stack[MAX_EXCEPTIONS_STACKED];
int exception = -1, checking_exception = -1;
bool is_flying = false;

class Stack {
private:
    class Stack_item {
    public:
        bool is_try = false;
        Stack_item* prev = nullptr;

        virtual bool destruct() {
            return false;
        }
        virtual void to_string() {
            std::cout << "basic item, is_try = " << is_try << std::endl;
        }
    };

    template <typename TYPE>
    class StackItem: public Stack_item{
    public:
        TYPE* object_pointer = nullptr;

        StackItem(bool is_try_in, TYPE* pointer) {
            is_try = is_try_in;
            object_pointer = pointer;
        }

        bool destruct() {
            if (is_try) {
                return true;
            }
            if (object_pointer == nullptr) {
                throw("Incorrect item in the stack");
            }
            object_pointer->~TYPE();
            return false;
        }

        void to_string() {
            std::cout << "StackItem: prev = " << prev << ", is_try = " << is_try
            << ", TYPE = " << typeid(TYPE).name() << std::endl;
        }
    };

public:
    Stack_item* tail_of_stack = nullptr;

public:
    template<typename TYPE>
    void push(bool is_try, TYPE* pointer){

        StackItem<TYPE>* next_item = new StackItem<TYPE>(is_try, pointer);

        if(tail_of_stack != nullptr) {
            next_item->prev = tail_of_stack;
        }
        tail_of_stack = next_item;
    }

    bool pop() {
        if(tail_of_stack == nullptr) {
            throw("Stack is empty, but the program attempt to get an item from it");
        }
        Stack_item* tmp = tail_of_stack;
        tail_of_stack = tail_of_stack->prev;
        return tmp->destruct();
    }

    void to_string() {
        std::cout << "tail = " << tail_of_stack << std::endl;
        Stack_item* cur = tail_of_stack;
        while(cur != nullptr) {
            cur->to_string();
            cur = cur->prev;
        }
    }

    ~Stack() {
        Stack_item* cur = tail_of_stack;
        while(cur != nullptr) {
            cur->destruct();
            cur = cur->prev;
        }
    }
};

Stack var_stack;

#define REGISTER(TYPE, object) var_stack.push<TYPE>(false, &object);

#define TRY if(!is_flying){                                                                      \
                last_env++;                                                                      \
                var_stack.push<int>(true, nullptr);                                              \
            }                                                                                    \
            exception = setjmp(env_buffers[last_env]);                                           \
            if(exception == 0) {

#define THROW(a) if(is_flying){                                                                  \
                     std::cout << "Terminated due to second exception call " << a << std::endl;  \
                     exit(1);                                                                    \
                 }                                                                               \
                 exception = a;                                                                  \
                 last_exception++;                                                               \
                 exception_stack[last_exception] = a;                                            \
                 is_flying = true;                                                               \
                 while(!var_stack.pop());                                                        \
                 std::longjmp(env_buffers[last_env], exception);

#define RETHROW if(last_exception < 0){                                                          \
                    std::cout << "Terminated: rethrowning was impossible" << std::endl;          \
                }                                                                                \
                while(!var_stack.pop());                                                         \
                std::longjmp(env_buffers[last_env], exception_stack[last_exception]);

#define CATCH(b)} else {                                                                         \
                     checking_exception = b;                                                     \
                     last_env--;                                                                 \
                     if(exception != checking_exception) {                                       \
                         if(last_env < 0){                                                       \
                             std::cout << "Terminated with exception " << exception << std::endl;\
                             exit(1);                                                            \
                         }                                                                       \
                         while(!var_stack.pop());                                                \
                         std::longjmp(env_buffers[last_env], exception);                         \
                     } else {                                                                    \
                         is_flying = false;                                                      \
                     }                                                                           \

#define END_CATCH last_exception--;                                                              \
                 }


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
            THROW(4);
        CATCH(4);
            std::cout << "caught 4 in destructor" << std::endl;
        END_CATCH
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

int main() {
    // todo: don't delete it!
    /* bad rethrowing when unwinding
    /*TRY
        MyDouble my_double = MyDouble(1.23);
        MyBadCrocodile croc = MyBadCrocodile(7, "abacaba");
        REGISTER(MyDouble, my_double);
        REGISTER(MyBadCrocodile, croc);
        THROW(13)
    CATCH(13)
        std::cout << "Catching 13" << std::endl;
    END_CATCH*/
    
    /* Experiments */
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

    /* Harder example */
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

    TRY
        MyDouble first_try = MyDouble(1.0);
        REGISTER(MyDouble, first_try);
        a();
    CATCH(8)
        std::cout << "Catching 8" << std::endl;
    END_CATCH
    std::cout << "Done catching from function" << std::endl;
    return 0;
}