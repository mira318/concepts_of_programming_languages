#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

const int MAX_COMMAND_LEN = 10;
const int MAX_ARG_LEN = 20;
const int MAX_ARGS_IN_COMMAND = 2;
const int REGISTER_NUM = 7;

char* memory_buffer;
int buffer_sz = 0;
int registers[REGISTER_NUM];

int read_string(std::ifstream& binary_input){}

int read_something(std::ifstream& binary_input, int* res){
    int val_type;
    binary_input.read(reinterpret_cast<char*>(&val_type), sizeof(int));
    if(val_type <= 0 && val_type > 4){
        std::cout << "Unknown value type" << std::endl;
        return -1;
    }
    if(val_type == 4){
        std::cout << "Don't expect a string here" << std::endl;
        return -2;
    }
    binary_input.read(reinterpret_cast<char*>(res), sizeof(int));
    return val_type;
}

bool check_register(int reg_num){
    if(reg_num > REGISTER_NUM || reg_num < 0){
        std::cout << "Wrong register value" << std::endl;
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

int read_register(std::ifstream& binary_input){
    int reg_num;
    if(read_something(binary_input, &reg_num) != 1){
        std::cout << "Expected address, wrong value type" << std::endl;
        return -1;
    }
    if(!check_register(reg_num)){
        return -1;
    }
    return reg_num;
}


int read_address(std::ifstream& binary_input){
    int address;
    if(read_something(binary_input, &address) != 2){
        std::cout << "Expected address, wrong value type" << std::endl;
        return -1;
    }
    return  address;
}

int read_number(std::ifstream& binary_input){
    int number;
    if(read_something(binary_input, &number) != 3){
        std::cout << "Expected address, wrong value type" << std::endl;
        return -1;
    }
    return  number;
}
int move(std::ifstream& binary_input) {
    // source -> destination
    int arg1, arg2;
    int val1_type = read_something(binary_input, &arg1);
    int val2_type = read_something(binary_input, &arg2);
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


int move_instruction_pointer(std::ifstream& binary_input){
    int address = read_address(binary_input);
    if(!check_address(address)){
        std::cout << "Incorrect address in IP" << std::endl;
        return -3;
    }
    binary_input.seekg(address, std::ios::beg);
    return 0;
}

int output_number(std::ifstream& binary_input){
    int arg;
    int val_type = read_something(binary_input, &arg);
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
    binary_input.seekg(2 * sizeof(int), std::ios::cur);
    return 0;
}

int output_string(std::ifstream& binary_input){
    int address = read_address(binary_input);
    if(!check_address(address)){
        std::cout << "Incorrect address in OUTS" << std::endl;
        return -3;
    }
    int i = address;
    while(memory_buffer[i] != '\0'){
        std::cout << memory_buffer[i];
        ++i;
    }
    std::cout << std::endl;
    binary_input.seekg(2 * sizeof(int), std::ios::cur);
    return 0;
}

int read_command(std::ifstream& binary_input){
    int command_num;
    binary_input.read(reinterpret_cast<char*>(&command_num), sizeof(int));
    // std::cout << "command_num = " << command_num << std::endl;
    switch(command_num){
        case 1:
            return move(binary_input);
        case 11:
            return output_number(binary_input);
        case 12:
            return move_instruction_pointer(binary_input);
        case 13:
            return output_string(binary_input);
        default:
            return -1;
    }

}

bool read_first_command(std::ifstream& binary_input, std::string& filename){
    int command_num;
    binary_input.read(reinterpret_cast<char*>(&command_num), sizeof(int));
    if(command_num != 12){
        std::cout << "ERROR: Any program should start with IP command." << std::endl;
        binary_input.close();
        return 0;
    }

    int arg_type;
    binary_input.read(reinterpret_cast<char*>(&arg_type), sizeof(int));
    if(arg_type != 2){
        std::cout << "ERROR: IP command takes an address" << std::endl;
        binary_input.close();
        return 0;
    }

    int command_start_address;
    binary_input.read(reinterpret_cast<char*>(&command_start_address), sizeof(int));

    binary_input.seekg(0, std::ios::end);
    buffer_sz = binary_input.tellg();
    memory_buffer = new char[buffer_sz];
    std::ifstream memory_input(filename, std::ios::in | std::ios::binary);

    if(!memory_input.read(reinterpret_cast<char*>(memory_buffer), buffer_sz)){
        std::cout << "ERROR: Can't read memory from file" << std::endl;
        memory_input.close();
        binary_input.close();
        return 0;
    }
    memory_input.close();

    // Seekg will fail if the address is bad
    binary_input.seekg(command_start_address, std::ios::beg);
    return 1;
}

int main(){
    for(int i = 0; i < REGISTER_NUM; ++i){
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
    while(input.peek() != EOF){
        switch(read_command(input)){
            case 0:
                break;

            case -1:
                std::cout << "Unknown command" << std::endl;
                input.close();
                return -1;

            case -3:
                input.close();
                return -1;

            default:
                std::cout << "Unknown mistake occurred" << std::endl;
                input.close();
                return -1;
        }
    }
    input.close();
    return 0;
}

