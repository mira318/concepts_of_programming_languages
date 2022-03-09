# include <iostream>
#include <fstream>
const int MAX_COMMAND_LEN = 10;
const int MAX_ARG_LEN = 20;
const int ARGS_IN_COMMAND = 2;
char* memory_buffer;

int read_address(std::ifstream& binary_input){
    int val_type;
    binary_input.read(reinterpret_cast<char*>(&val_type), sizeof(int));
    if(val_type != 1){
        return -1;
    }
    int address;
    binary_input.read(reinterpret_cast<char*>(&address), sizeof(int));
    return address;
}

int move_instruction_pointer(std::ifstream& binary_input){
    int address = read_address(binary_input);
    if(address < 0){
        return -2;
    }
    binary_input.seekg(address, std::ios::beg);
    return 0;
}

int output_string(std::ifstream& binary_input){
    int address = read_address(binary_input);
    if(address < 0){
        return -2;
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
    switch(command_num){
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
    if(arg_type != 1){
        std::cout << "ERROR: IP command takes an address" << std::endl;
        binary_input.close();
        return 0;
    }

    int command_start_address;
    binary_input.read(reinterpret_cast<char*>(&command_start_address), sizeof(int));

    binary_input.seekg(0, std::ios::end);
    int buffer_sz = binary_input.tellg();
    memory_buffer = new char[buffer_sz];
    std::ifstream memory_input(filename, std::ios::in | std::ios::binary);

    if(!memory_input.read(memory_buffer, buffer_sz)){
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

            case -2:
                std::cout << "Unknown address" << std::endl;
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

