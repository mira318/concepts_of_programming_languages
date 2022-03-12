#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

const int MAX_COMMAND_LEN = 10;
const int MAX_ARG_LEN = 20;
const int MAX_ARGS_IN_COMMAND = 2;
const int USER_REGISTER_NUM = 7;
const int IP_REGISTER = 7;
const int REGISTER_NUM = 8;

char* memory_buffer;
int buffer_sz = 0;
int registers[REGISTER_NUM];

int read_string(std::ifstream& binary_input){}

int read_something(int* res){
    int val_type;
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
    if(address < 0  || address > buffer_sz){
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
    return  address;
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

int read_command(){
    int command_num;
    memcpy(&command_num, memory_buffer + registers[IP_REGISTER], sizeof(int));
    registers[IP_REGISTER] += sizeof(int);
    std::cout << "command_num = " << command_num << std::endl;
    switch(command_num){
        case 1:
            return move();
        case 11:
            return output_number();
        case 12:
            return move_instruction_pointer();
        case 13:
            return output_string();
        default:
            return -1;
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
        binary_input.close();
        return 0;
    }
    registers[IP_REGISTER] += sizeof(int);

    int arg_type;
    memcpy(&arg_type, memory_buffer + registers[IP_REGISTER], sizeof(int));
    if(arg_type != 2){
        std::cout << "ERROR: IP command takes an address" << std::endl;
        binary_input.close();
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
    return 1;
}

int main(){
    for(int i = 0; i < USER_REGISTER_NUM; ++i){
        registers[i] = 0;
    }

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
    while(registers[IP_REGISTER] != buffer_sz){
        // Надо будет переделать на STOP
        switch(read_command()){
            case 0:
                break;

            case -1:
                std::cout << "Unknown command" << std::endl;
                return -1;

            case -3:
                input.close();
                return -1;

            default:
                std::cout << "Unknown mistake occurred" << std::endl;
                return -1;
        }
    }
    return 0;
}

