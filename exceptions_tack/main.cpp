#include <iostream>
#include <csetjmp>

const int MAX_BUFFERS_NUM = 1e5;
int last_env = -1;
bool is_flying = false;
std::jmp_buf env_buffers[MAX_BUFFERS_NUM];

#define TRY last_env++;                                                                          \
            int exception = setjmp(env_buffers[last_env]);                                       \
            if(exception == 0)

#define THROW(a) exception = a;                                                                  \
                 is_flying = true;                                                               \
                 std::longjmp(env_buffers[last_env], exception);

#define CATCH(b) else {                                                                          \
                     int checking_exception = b;                                                 \
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
    return 0;
}