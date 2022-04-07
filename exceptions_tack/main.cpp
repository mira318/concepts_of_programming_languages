#include <iostream>
#include <csetjmp>

const int MAX_BUFFERS_NUM = 1e5;
const int MAX_EXCEPTIONS_STACKED = 1e5;
int last_env = -1;
int last_exception = -1;
std::jmp_buf env_buffers[MAX_BUFFERS_NUM];
int exception_stack[MAX_EXCEPTIONS_STACKED];
int exception = -1, checking_exception = -1;

#define TRY last_env++;                                                                          \
            exception = setjmp(env_buffers[last_env]);                                           \
            if(exception == 0) {

#define THROW(a) exception = a;                                                                  \
                 last_exception++;                                                                \
                 exception_stack[last_exception] = a;                                            \
                 std::longjmp(env_buffers[last_env], exception);

#define RETHROW if(last_exception < 0){                                                          \
                    std::cout << "Terminated: rethrowning was impossible" << std::endl;          \
                }                                                                                \
                std::longjmp(env_buffers[last_env], exception_stack[last_exception]);

#define CATCH(b)} else {                                                                          \
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
int main(){
    /* Experiments */
    TRY
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
    std::cout << "Gone through experimental try-catch" << std::endl;

    /* Harder example */
    TRY
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
    std::cout << "Gone through harder example, using END_CATCH" << std::endl;
    return 0;
}