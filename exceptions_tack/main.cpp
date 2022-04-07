#include <iostream>
#include <csetjmp>

const int MAX_BUFFERS_NUM = 1e5;
int last_env = -1;
std::jmp_buf env_buffers[MAX_BUFFERS_NUM];
int exception = -1, checking_exception = -1;

#define TRY last_env++;                                                                          \
            exception = setjmp(env_buffers[last_env]);                                           \
            if(exception == 0)

#define THROW(a) exception = a;                                                                  \
                 std::longjmp(env_buffers[last_env], exception);

#define RETHROW std::longjmp(env_buffers[last_env], exception);

#define CATCH(b) else {                                                                          \
                     checking_exception = b;                                                     \
                     last_env--;                                                                 \
                     if(exception != checking_exception) {                                       \
                         if(last_env < 0){                                                       \
                             std::cout << "Terminated with exception " << exception << std::endl;\
                             exit(1);                                                            \
                         }                                                                       \
                         std::longjmp(env_buffers[last_env], exception);                         \
                     }
int main(){
    /* How it should be ... */
    last_env++;
    int ideal_exception = setjmp(env_buffers[last_env]);
    if(ideal_exception == 0) { // TRY {
        last_env++;
        int ideal_exception = setjmp(env_buffers[last_env]);
        if(ideal_exception == 0) { // TRY {
            last_env++;
            int ideal_exception = setjmp(env_buffers[last_env]);
            if(ideal_exception == 0) {
                //THROW
                ideal_exception = 9;
                std::longjmp(env_buffers[last_env], ideal_exception);
            } else { // } CATCH {
                int ideal_checking_exception = 9;
                last_env--;
                if (ideal_exception != ideal_checking_exception) {
                    if (last_env < 0) {
                        std::cout << "Terminated with exception " << ideal_exception << std::endl;
                        exit(1);
                    }
                    std::longjmp(env_buffers[last_env], ideal_exception);
                }
                std::cout << "Catching 9" << std::endl;
            } // } from CATCH
        } else { // } CATCH {
            int ideal_checking_exception = 7;
            last_env--;
            if(ideal_exception != ideal_checking_exception) {
                if (last_env < 0) {
                    std::cout << "Terminated with exception " << ideal_exception << std::endl;
                    exit(1);
                }
                std::longjmp(env_buffers[last_env], ideal_exception);
            }
            std::cout << "Catching 7" << std::endl;
        } // } from CATCH
    } else { // } CATCH {
        int ideal_checking_exception = 5;
        last_env--;
        if(ideal_exception != ideal_checking_exception) {
            if (last_env < 0) {
                std::cout << "Terminated with exception " << ideal_exception << std::endl;
                exit(1);
            }
            std::longjmp(env_buffers[last_env], ideal_exception);
        }
        std::cout << "Catching 5" << std::endl;
    } // } from CATCH
    std::cout << "Gone through ideal try-catch block" << std::endl;

    /* Experiments */
    TRY{
        TRY{
            TRY{
                THROW(5)
            } CATCH(9)
                std::cout << "Catching 9" << std::endl;
            }
        } CATCH(7)
            std::cout << "Catching 7" << std::endl;
        }
    } CATCH(5)
        std::cout << "Catching 5" << std::endl;
    }
    std::cout << "Gone through experimental try-catch" << std::endl;

    TRY {
        TRY {
            TRY {
                TRY {
                    THROW(11)

                } CATCH(7)
                    std::cout << "Catching 7" << std::endl;
                }

            } CATCH(11)
                std::cout << "Catching 11" << std::endl;
                TRY {
                    THROW(13)

                } CATCH(13)
                    std::cout << "Catching inner 13" << std::endl;
                }

                RETHROW
            }

        } CATCH(11)
            std::cout << "Catching rethrown 11" << std::endl;
            THROW(7)

        }

    } CATCH(7)
        std::cout << "Caught 7 as final" << std::endl;
    }
    std::cout << "Gone through harder example" << std::endl;
    return 0;
}