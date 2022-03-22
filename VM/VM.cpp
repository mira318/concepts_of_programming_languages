#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

const int USER_REGISTER_NUM = 7;
const int BP_REGISTER = 13;
const int SP_REGISTER = 14;
const int IP_REGISTER = 15;
const int REGISTER_NUM = 16;
const int STACK_SIZE = 1e5;

char* memory_buffer;
int buffer_sz = 0;
int registers[REGISTER_NUM];
char stack[STACK_SIZE];

int read_something(int* res){
    int val_type;
    int val_stack_start = registers[SP_REGISTER] - 2 * sizeof(int);
    memcpy(&val_type, memory_buffer + registers[IP_REGISTER], sizeof(int));
    registers[IP_REGISTER] += sizeof(int);

    if(val_type <= 0 && val_type > 4){
        std::cout << "Unknown value type" << std::endl;
        return -1;
    }
    if(val_type == 4){
        std::cout << "Don't expect a string here" << std::endl;
        return -2;
    }

    memcpy(res, memory_buffer + registers[IP_REGISTER], sizeof(int));
    registers[IP_REGISTER] += sizeof(int);
    return val_type;
}

bool check_register(int reg_num){
    if(reg_num >= USER_REGISTER_NUM || reg_num < 0){
        return false;
    }
    return true;
}

bool check_address(int address){
    if(address < 0  || address >= buffer_sz){
	    return false;
    }
    return true;
}

int read_register(){
    int reg_num;
    if(read_something(&reg_num) != 1){
        std::cout << "Expected address, wrong value type" << std::endl;
        return -1;
    }
    if(!check_register(reg_num)){
        std::cout << "Wrong register number" << std::endl;
        return -2;
    }
    return reg_num;
}

int read_address(){
    int address;
    if(read_something(&address) != 2){
        std::cout << "Expected address, wrong value type" << std::endl;
        return -1;
    }
    if(!check_address(address)){
        std::cout << "Impossible address" << std::endl;
        return -2;
    }
    return address;
}

int read_number(){
    int number;
    if(read_something(&number) != 3){
        std::cout << "Expected number, wrong value type" << std::endl;
        return -1;
    }
    return  number;
}

int move() {
    // source -> destination
    int arg1, arg2;
    int val1_type = read_something(&arg1);
    int val2_type = read_something(&arg2);
    if (val1_type < 0 || val2_type < 0) {
        return -3;
    }
    switch (val1_type) {
        case 1:
            if (!check_register(arg1)) {
                std::cout << "Wrong register number in MOVE" << std::endl;
                return -3;
            }
            switch (val2_type) {
                case 1:
                    if (!check_register(arg2)) {
                        std::cout << "Wrong register number in MOVE" << std::endl;
                        return -3;
                    }
                    registers[arg2] = registers[arg1];
                    break;

                case 2:
                    if (!check_address(arg2)) {
                        std::cout << "Wrong address in MOVE" << std::endl;
                        return -3;
                    }
                    std::memcpy(memory_buffer + arg2, registers + arg1, sizeof(int));
                    break;

                default:
                    std::cout << "Incorrect second argument in MOVE" << std::endl;
                    return -3;
            }
            break;

        case 2:
            if(!check_address(arg1)){
                std::cout << "Wrong address in MOVE" << std::endl;
                return -3;
            }
            switch(val2_type){
                case 1:
                    if(!check_register(arg2)){
                        std::cout << "Wrong register number in MOVE" << std::endl;
                        return -3;
                    }
                    memcpy(registers + arg2, memory_buffer + arg1, sizeof(int));
                    break;

                case 2:
                    if(!check_address(arg2)){
                        std::cout << "Wrong address in MOVE" << std::endl;
                        return -3;
                    }
                    std::cout <<
                        "WARNING: it takes too much time to move from memory to memory, but I will do it." << std::endl;
                    memcpy(memory_buffer + arg2, memory_buffer + arg1, sizeof(int));
                    break;

                default:
                    std::cout << "Incorrect second argument in MOVE" << std::endl;

            }
            break;

        case 3:
            switch(val2_type){
                case 1:
                    if(!check_register(arg2)){
                        std::cout << "Wrong register number in MOVE" << std::endl;
                        return -3;
                    }
                    registers[arg2] = arg1;
                    break;

                case 2:
                    if(!check_address(arg2)){
                        std::cout << "Wrong address in MOVE" << std::endl;
                        return -3;
                    }
                    memcpy(memory_buffer + arg2, &arg1, sizeof(int));
                    break;

                default:
                    std::cout << "Incorrect second argument in MOVE" << std::endl;
                    return -3;
            }
            break;

        default:
            std::cout << "Incorrect first argument in MOVE" << std::endl;
            return -3;
    }
    return 0;
}

int move_instruction_pointer(){
    int address = read_address();
    if(address < 0){
        return -3;
    }
    registers[IP_REGISTER] = address;
    return 0;
}

int jump_if_zero(bool zero){
    int val1_type;
    int arg1, arg2;
    val1_type = read_something(&arg1);
    arg2 = read_address();
    if(val1_type < 0){
        return -3;
    }
    switch(val1_type) {
        case 1:
            if(!check_register(arg1)){
                std::cout << "Wrong register number in JZ" << std::endl;
                return -3;
            }
            if((registers[arg1] == 0) == zero){
                registers[IP_REGISTER] = arg2;
            }
            break;

        case 2:
            if(!check_address(arg1)){
                std::cout << "Wrong address in JZ" << std::endl;
                return -3;
            }
            int val;
            memcpy(&val, memory_buffer + arg1, sizeof(int));
            if((val == 0) == zero){
                registers[IP_REGISTER] = arg2;
            }
            break;

        case 3:
            if((arg1 == 0) == zero){
                registers[IP_REGISTER] = arg2;
            }
            break;

        default:
            std::cout << "Incorrect first argument in JZ" << std::endl;
            return -3;
    }
    return 0;
}

int add(){
    int val1_type, val2_type;
    int arg1, arg2;
    val1_type = read_something(&arg1);
    val2_type = read_something(&arg2);
    switch(val1_type){
        case 1:
            if(!check_register(arg1)){
                std::cout << "Wrong register number in ADD" << std::endl;
                return -3;
            }
            switch(val2_type){
                case 1:
                    if(!check_register(arg2)){
                        std::cout << "Wrong register number in ADD" << std::endl;
                        return -3;
                    }
                    registers[arg1] += registers[arg2];
                    break;
                case 2:
                    if(!check_address(arg2)){
                        std::cout << "Impossible address in ADD" << std::endl;
                        return -3;
                    }
                    int val2;
                    memcpy(&val2, memory_buffer + arg2, sizeof(int));
                    registers[arg1] += val2;
                    break;
                case 3:
                    registers[arg1] += arg2;
                    break;
                default:
                    std::cout << "Incorrect second argument in ADD" << std::endl;
                    return -3;
            }
            break;

        case 2:
            if(!check_address(arg1)){
                std::cout << "Wrong address in ADD" << std::endl;
                return -3;
            }
            int val1;
            int ans;
            memcpy(&val1, memory_buffer + arg1, sizeof(int));
            switch(val2_type){
                case 1:
                    if(!check_register(arg2)){
                        std::cout << "Wrong register number in ADD" << std::endl;
                        return -3;
                    }
                    ans = val1 + registers[arg2];
                    break;

                case 2:
                    if(!check_address(arg2)){
                        std::cout << "Impossible address in ADD" << std::endl;
                        return -3;
                    }
                    std::cout << "ADD 2 numbers from memory is too long, but I will do it." << std::endl;
                    int val2;
                    memcpy(&val2, memory_buffer + arg2, sizeof(int));
                    ans = val1 + val2;
                    break;

                case 3:
                    ans = val1 + arg2;
                    break;

                default:
                    std::cout << "Incorrect second argument in ADD" << std::endl;
                    return -3;
            }
            memcpy(memory_buffer + arg1, &ans, sizeof(int));
            break;

        default:
            std::cout << "Incorrect first argument in ADD";
            return -3;
    }
    return 0;
}

int sub(){
    int val1_type, val2_type;
    int arg1, arg2;
    val1_type = read_something(&arg1);
    val2_type = read_something(&arg2);
    switch(val1_type){
        case 1:
            if(!check_register(arg1)){
                std::cout << "Wrong register number in SUB" << std::endl;
                return -3;
            }
            switch(val2_type){
                case 1:
                    if(!check_register(arg2)){
                        std::cout << "Wrong register number in SUB" << std::endl;
                        return -3;
                    }
                    registers[arg1] -= registers[arg2];
                    break;
                case 2:
                    if(!check_address(arg2)){
                        std::cout << "Impossible address in SUB" << std::endl;
                        return -3;
                    }
                    int val2;
                    memcpy(&val2, memory_buffer + arg2, sizeof(int));
                    registers[arg1] -= val2;
                    break;
                case 3:
                    registers[arg1] -= arg2;
                    break;
                default:
                    std::cout << "Incorrect second argument in SUB" << std::endl;
                    return -3;
            }
            break;

        case 2:
            if(!check_address(arg1)){
                std::cout << "Wrong address in SUB" << std::endl;
                return -3;
            }
            int val1;
            int ans;
            memcpy(&val1, memory_buffer + arg1, sizeof(int));
            switch(val2_type){
                case 1:
                    if(!check_register(arg2)){
                        std::cout << "Wrong register number in SUB" << std::endl;
                        return -3;
                    }
                    ans = val1 - registers[arg2];
                    break;

                case 2:
                    if(!check_address(arg2)){
                        std::cout << "Impossible address in SUB" << std::endl;
                        return -3;
                    }
                    int val2;
                    std::cout << "WARNING:SUB 2 numbers from memory is too long, but I will do it." << std::endl;
                    memcpy(&val2, memory_buffer + arg2, sizeof(int));
                    ans = val1 - val2;
                    break;

                case 3:
                    ans = val1 - arg2;
                    break;

                default:
                    std::cout << "Incorrect second argument in SUB" << std::endl;
                    return -3;
            }
            memcpy(memory_buffer + arg1, &ans, sizeof(int));
            break;

        default:
            std::cout << "Incorrect first argument in SUB";
            return -3;
    }
    return 0;
}

int input_number(){
    int arg;
    int number;
    int arg_type = read_something(&arg);
    if(arg_type < 0){
        return -3;
    }

    switch(arg_type){
        case 1:
            if(!check_register(arg)){
                std::cout << "Wrong register number in INP" << std::endl;
                return -3;
            }
            std::cin >> number;
            registers[arg] = number;
            break;

        case 2:
            if(!check_address(arg)){
                std::cout << "Wrong address in INP" << std::endl;
                return -3;
            }
            std::cin >> number;
            memcpy(memory_buffer + arg, &number, sizeof(int));
            break;

        default:
            std::cout << "Incorrect argument in INP" << std::endl;
            return -3;
    }

    registers[IP_REGISTER] += 2 * sizeof(int);
    return 0;
}

int output_number(){
    int arg;
    int val_type = read_something(&arg);
    switch(val_type){
        case 1:
            if(!check_register(arg)){
                std::cout << "Wrong register number in OUT" << std::endl;
                return -3;
            }
            std::cout << registers[arg] << std::endl;
            break;

        case 2:
            if(!check_address(arg)){
                std::cout << "Incorrect address in OUT" << std::endl;
                return -3;
            }
            int res;
            memcpy(&res, memory_buffer + arg, sizeof(int));
            std::cout << res << std::endl;
            break;

        case 3:
            std::cout << arg << std::endl;
            break;

        default:
            std::cout << "Incorrect argument in OUT" << std::endl;
            return -3;
    }
    registers[IP_REGISTER] += 2 * sizeof(int);
    return 0;
}

int output_string(){
    int address = read_address();
    if(address < 0){
        return -3;
    }
    int i = address;
    while(memory_buffer[i] != '\0'){
        std::cout << memory_buffer[i];
        ++i;
    }
    std::cout << std::endl;
    registers[IP_REGISTER] += 2 * sizeof(int);
    return 0;
}

int call(){
    int address = read_address();
    if(address < 0){
        return -3;
    }

    registers[BP_REGISTER] = registers[SP_REGISTER];
    int address_type = 2;
    memcpy(stack + registers[SP_REGISTER], &address_type, sizeof(int));
    registers[SP_REGISTER] += sizeof(int);

    int next_address_after_call = registers[IP_REGISTER] + 2 * sizeof(int);
    memcpy(stack + registers[SP_REGISTER], &next_address_after_call, sizeof(int));
    std::cout << "pushed address after call = " << next_address_after_call << std::endl;
    registers[SP_REGISTER] += sizeof(int);

    registers[IP_REGISTER] = address;
    return 0;
}

int ret(){
    int val_type;
    int back_address;
    memcpy(&val_type, stack + registers[BP_REGISTER], sizeof(int));
    if(val_type != 2){
        return -4;
    }
    registers[BP_REGISTER] += sizeof(int);
    memcpy(&back_address, stack + registers[BP_REGISTER], sizeof(int));
    if(!check_address(back_address)){
        return -4;
    }
    registers[BP_REGISTER] -= 3 * sizeof(int);
    registers[IP_REGISTER] = back_address;
    return 0;
}

int read_command(){
    int command_num;
    memcpy(&command_num, memory_buffer + registers[IP_REGISTER], sizeof(int));
    registers[IP_REGISTER] += sizeof(int);
    //std::cout << "command_num = " << command_num << std::endl;
    switch(command_num){
        case 1:
            return move();
        case 2:
            return jump_if_zero(true);
        case 3:
            return jump_if_zero(false);
        case 4:
            return -1;
        case 5:
            return add();
        case 6:
            return sub();
        case 10:
            return input_number();
        case 11:
            return output_number();
        case 12:
            return move_instruction_pointer();
        case 13:
            return output_string();
        case 14:
            return call();
        case 15:
            return ret();
        default:
            return -2;
    }
}

bool read_first_command(std::ifstream& binary_input, std::string& filename){
    binary_input.seekg(0, std::ios::end);
    buffer_sz = binary_input.tellg();
    binary_input.seekg(0, std::ios::beg);
    memory_buffer = new char[buffer_sz];

    if(!binary_input.read(reinterpret_cast<char*>(memory_buffer), buffer_sz)){
        std::cout << "ERROR: Can't read memory from file" << std::endl;
        binary_input.close();
        return 0;
    }
    binary_input.close();
    registers[IP_REGISTER] = 0;

    int command_num;
    memcpy(&command_num, memory_buffer + registers[IP_REGISTER], sizeof(int));
    if(command_num != 12){
        std::cout << "ERROR: Any program should start with IP command." << std::endl;
        return 0;
    }
    registers[IP_REGISTER] += sizeof(int);

    int arg_type;
    memcpy(&arg_type, memory_buffer + registers[IP_REGISTER], sizeof(int));
    if(arg_type != 2){
        std::cout << "ERROR: IP command takes an address" << std::endl;
        return 0;
    }
    registers[IP_REGISTER] += sizeof(int);

    int command_start_address;
    memcpy(&command_start_address, memory_buffer + registers[IP_REGISTER], sizeof(int));
    if(!check_address(command_start_address)){
        std::cout << "Incorrect IP address" << std::endl;
        return 0;
    }
    registers[IP_REGISTER] = command_start_address;
    registers[SP_REGISTER] = 0;
    registers[BP_REGISTER] = 0;
    return 1;
}

int main(){
    for(int i = 0; i < USER_REGISTER_NUM; ++i){
        registers[i] = 0;
    }

    std::cout << "Write assembly program file name" << std::endl;
    std::string filename;
    std::cin >> filename;
    std::ifstream input(filename, std::ios::in | std::ios::binary);
    if(!input.is_open()){
        std::cout << "Unable to open the input file" << std::endl;
        return -1;
    }
    if(!read_first_command(input, filename)){
        return -1;
    }
    bool stopped = false;
    while(!stopped){
        switch(read_command()){
            case 0:
                break;

            case -1:
                stopped = true;
                break;

            case -2:
                std::cout << "Unknown command" << std::endl;
                return -1;

            case -3:
                return -1;

            case -4:
                std::cout << "Missing back pointer in call stack" << std::endl;
                return -1;

            default:
                std::cout << "Unknown mistake occurred" << std::endl;
                return -1;
        }
    }
    return 0;
}