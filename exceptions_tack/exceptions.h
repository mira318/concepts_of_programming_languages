#include <iostream>
#include <csetjmp>
#include <map>

const int MAX_BUFFERS_NUM = 1e5;
const int MAX_EXCEPTIONS_STACKED = 1e5;
int last_env = -1;
int last_exception = -1;
std::jmp_buf env_buffers[MAX_BUFFERS_NUM];

std::map<std::string, int> exceptions_types;
std::string exception_stack[MAX_EXCEPTIONS_STACKED];
int exception = -1, checking_exception = -1;
int pre_exception = -1;
std::string last_thrown_exception;
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

int get_exception_num(std::string user_exception_string) {
    if(exceptions_types.find(user_exception_string) == exceptions_types.end()){
        return exceptions_types["UNKNOWN_EXCEPTION"];
    }
    return exceptions_types[user_exception_string];
}

bool compare_exceptions(){

}
#define REGISTER(TYPE, object) var_stack.push<TYPE>(false, &object);

#define TRY if(!is_flying){                                                                      \
                last_env++;                                                                      \
                var_stack.push<int>(true, nullptr);                                              \
            }                                                                                    \
            pre_exception = setjmp(env_buffers[last_env]);                                       \
            if(!is_flying){                                                                      \
                exception = pre_exception;                                                       \
            }                                                                                    \
            if(pre_exception == 0) {

#define THROW(a) if(is_flying){                                                                              \
                     std::cout << "Terminated due to second exception call " << a << std::endl;              \
                     exit(1);                                                                                \
                 }                                                                                           \
                 last_trown_exception = a;                                                                   \
                 exception = get_exception_num(a);                                                           \
                 last_exception++;                                                                           \
                 exception_stack[last_exception] = a                                                         \
                 is_flying = true;                                                                           \
                 while(!var_stack.pop());                                                                    \
                 std::longjmp(env_buffers[last_env], exception);

#define RETHROW if(last_exception < 0){                                                                      \
                    std::cout << "Terminated: rethrowning was impossible" << std::endl;                      \
                }                                                                                            \
                last_thrown_exception = exception_stack[last_exception];                                     \
                while(!var_stack.pop());                                                                     \
                std::longjmp(env_buffers[last_env], get_exception_num(exception_stack[last_exception]));

#define CATCH(b)} else {                                                                                     \
                     checking_exception = get_exception_num(b);                                              \
                     last_env--;                                                                             \
                     if(!compare(exception, checking_exception)) {                                           \
                         if(last_env < 0){                                                                   \
                             std::cout << "Terminated with exception " << last_thrown_exception << std::endl;\
                             exit(1);                                                                        \
                         }                                                                                   \
                         while(!var_stack.pop());                                                            \
                         std::longjmp(env_buffers[last_env], exception);                                     \
                     } else {                                                                                \
                         is_flying = false;                                                                  \
                     }                                                                                       \

#define END_CATCH last_exception--;                                                              \
                 }

void construct_map(){
    exceptions_types["RUNTIME_EXCEPTION"] = 1;
    exceptions_types["NO_SUCH_ELEMENT_EXCEPTION"] = 2;
    exceptions_types["INDEX_OUT_OF_BOUNDS_EXCEPTION"] = 3;
    exceptions_types["ARITHMETIC_EXCEPTION"] = 4;
    exceptions_types["CLASS_CAST_EXCEPTION"] = 5;
    exceptions_types["NULL_POINTER_EXCEPTION"] = 6;
    exceptions_types["ILLEGAL_ARGUMENT_EXCEPTION"] = 7;
    exceptions_types["EOF_EXCEPTION"] = 8;
    exceptions_types["FILE_NOT_FOUNDED_EXCEPTION"] = 9;
    exceptions_types["NO_SUCH_FIELD_EXCEPTION"] = 10;
    exceptions_types["ILLEGAL_ACCESS_EXCEPTION"] = 11;
    exceptions_types["CLASS_NOT_FOUNDED_EXCEPTION"] = 12;
    exceptions_types["NO_SUCH_METHOD_EXCEPTION"] = 13;
    exceptions_types["UNKNOWN_EXCEPTION"] = 14;
}

/*int main(){
    construct_map();
    return 0;
}*/