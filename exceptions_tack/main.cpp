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

class Stack {
private:
    template <typename TYPE>
    class Stack_item{
    public:
        Stack_item<int>* prev = nullptr;
        bool is_try = false;
        TYPE* object_pointer = nullptr;

        Stack_item() {}
        Stack_item(bool is_try_in, TYPE* pointer): is_try(is_try_in), object_pointer(pointer) {}

        bool destruct() {
            Stack_item<TYPE> current_item = *(reinterpret_cast<Stack_item<TYPE>*>(this));
            std::cout << "called destruct, type = " << typeid(TYPE).name() << std::endl;
            if (current_item.is_try) {
                std::cout << "returned true" << std::endl;
                return true;
            }
            if (current_item.object_pointer == nullptr) {
                throw("Incorrect item in the stack");
            }
            std::cout << "called type destructor" << std::endl;
            current_item.object_pointer->~TYPE();
            return false;
        }

        void to_string() {
            std::cout << "Stack_item: prev = " << prev << ", is_try = " << is_try
            << ", TYPE = " << typeid(TYPE).name() << std::endl;
        }
    };
    Stack_item<int>* tail_of_stack = nullptr;

public:
    template<typename TYPE>
    void push(bool is_try, TYPE* pointer){
        std::cout << "Pushed type = " << typeid(TYPE).name() << std::endl;
        Stack_item<TYPE> next_item = Stack_item<TYPE>(is_try, pointer);
        if(tail_of_stack != nullptr) {
            next_item.prev = tail_of_stack;
        }
        tail_of_stack = reinterpret_cast<Stack_item<int> *>(&next_item);
    }

    bool pop() {
        if(tail_of_stack == nullptr) {
            throw("Stack call is empty, but the program attempt to get an item from it");
        }
        Stack_item<int>* tmp = tail_of_stack;
        tail_of_stack = tail_of_stack->prev;
        return tmp->destruct();
    }

    void to_string() {
        std::cout << "tail = " << tail_of_stack << std::endl;
        Stack_item<int>* cur = tail_of_stack;
        while(cur != nullptr) {
            cur->to_string();
            cur = cur->prev;
        }
    }

    ~Stack() {
        Stack_item<int>* cur = tail_of_stack;
        while(cur != nullptr) {
            cur->destruct();
            cur = cur->prev;
        }

    }
};
Stack var_stack;

#define TRY last_env++;                                                                          \
            exception = setjmp(env_buffers[last_env]);                                           \
            var_stack.push<int>(true, nullptr);                                                  \
            if(exception == 0) {

#define THROW(a) exception = a;                                                                  \
                 last_exception++;                                                               \
                 exception_stack[last_exception] = a;                                            \
                 std::cout << "Before while in try " << std::endl;                               \
                 var_stack.to_string();                                                          \
                 while(!var_stack.pop());                                                        \
                 std::cout << "After while in throw" << std::endl;                               \
                 var_stack.to_string();                                                                                \
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
                         std::longjmp(env_buffers[last_env], exception);                         \
                     }

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
        std::cout << "created a something:)" << std::endl;
    }
    ~MyCrocodile(){
        c.clear();
        std::cout << "destroying a crocodile" << std::endl;
    }
};

/*void b() {
    std::cout << "In void b before THROW" << std::endl;
    THROW(8)
    std::cout << "In void c after THROW" << std::endl;
}

void c(){
    std::cout << "In void c" << std::endl;
}

void a(){
    TRY
        b();
    CATCH(9)
        std::cout << "Catching 9" << std::endl;
    END_CATCH
    c();
}*/

int main() {
    TRY
        std::cout << "Stack in the start of try" << std::endl;
        var_stack.to_string();
        MyDouble my_double = MyDouble(1.23);
        MyCrocodile croc = MyCrocodile(6, "abacaba");
        var_stack.push<MyDouble>(false, &my_double);
        var_stack.push<MyCrocodile>(false, &croc);
        THROW(13)
    CATCH(13)
        std::cout << "Catching 13" << std::endl;
    END_CATCH












    /* Experiments */
    /*TRY
        TRY
            TRY
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
    std::cout << "Gone through experimental try-catch" << std::endl;*/

    /* Harder example */
    /*TRY
        TRY
            TRY
                TRY
                    THROW(11)
                CATCH(7)
                    std::cout << "Catching 7" << std::endl;
                END_CATCH
            CATCH(11)
                std::cout << "Catching 11" << std::endl;
                TRY
                    THROW(13)
                CATCH(13)
                    std::cout << "Catching inner 13" << std::endl;
                END_CATCH
                RETHROW
            END_CATCH
        CATCH(11)
            std::cout << "Catching rethrown 11" << std::endl;
            THROW(7)
        END_CATCH
    CATCH(7)
        std::cout << "Caught 7 as final" << std::endl;
    END_CATCH
    std::cout << "Gone through harder example, using END_CATCH" << std::endl;*/

    /*TRY
        a();
    CATCH(8)
        std::cout << "Catching 8" << std::endl;
    END_CATCH
    std::cout << "Done catching from function" << std::endl;
    return 0;*/
}