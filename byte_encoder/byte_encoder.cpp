#include <iostream>
#include <fstream>
#include <map>
#include <string>

const int MAX_COMMAND_LEN = 10;
const int MAX_ARG_LEN = 20;
const int ARGS_IN_COMMAND = 2;

std::map<std::string, int> command_codes;
std::map<std::string, int> command_args_nums;
std::map<std::string, int> register_adds;

void fill_command_codes(){
    command_codes["MOV"] = 0;
    command_codes["JMP"] = 1;
    command_codes["JZ"] = 2;
    command_codes["JNZ"] = 3;
    command_codes["STOP"] = 4;
    command_codes["ADD"] = 5;
    command_codes["SUB"] = 6;
    command_codes["MUL"] = 7;
    command_codes["LOAD"] = 8;
    command_codes["SAFE"] = 9;
    command_codes["INP"] = 10;
    command_codes["OUT"] = 11;
    command_codes["IP"] = 12;
}

void fill_command_args(){
    command_args_nums["MOV"] = 2;
    command_args_nums["JMP"] = 1;
    command_args_nums["JZ"] = 2; // what to check and where to jump
    command_args_nums["JNZ"] = 2;
    command_args_nums["STOP"] = 0;
    command_args_nums["ADD"] = 2;
    command_args_nums["SUB"] = 2;
    command_args_nums["MUL"] = 2;
    command_args_nums["LOAD"] = 2;
    command_args_nums["SAFE"] = 2;
    command_args_nums["INP"] = 1;
    command_args_nums["OUT"] = 1;
    command_args_nums["IP"] = 1;
}

void fill_register_adds(){
    register_adds["AX"] = 0;
    register_adds["BX"] = 1;
    register_adds["CX"] = 2;
    register_adds["DX"] = 3;
    register_adds["EX"] = 4;
    register_adds["FX"] = 5;
    register_adds["SP"] = 6;
    register_adds["CP"] = 7;
}

bool get_number(std::string number_string, int* res){
    for(int i = 1; i < number_string.length(); ++i) {
        if(!std::isdigit(number_string[i])){
            return 0;
        }
    }
    int number;
    try{
        number = std::stoi(number_string);
    }catch(std::invalid_argument e){
        return false;
    }
    *res = number;
    return true;
}

bool write_to_output(std::ofstream& binary_output, const std::string current_arg){
    int number;
    int val_type = -1;
    if(current_arg.length() < 1){
        return 0;
    }

    if(register_adds.find(current_arg) != register_adds.end()){
        number = register_adds[current_arg];
        val_type = 0;
    }

    if(val_type == -1 && current_arg[0] == '#'){
        if(!get_number(&current_arg[1], &number)){
            return false;
        }
        val_type = 1;
    }

    if(val_type == -1){
        if(!get_number(current_arg, &number)){
            return false;
        }
        val_type = 2;
    }

    binary_output.write(reinterpret_cast<const char*>(&val_type), sizeof(int));
    binary_output.write(reinterpret_cast<const char*>(&number), sizeof(number));
    return true;
}

bool get_args(std::ofstream& binary_output, int args_num, std::string input_string, int start){
    int j = start;
    std::string current_arg;
    int t;
    for(int arg = 0; arg < args_num; ++arg){
        current_arg = "";
        while(j < input_string.length() && std::isblank(input_string[j])){
            j++;
        }
        t = 0;
        while(j < input_string.length() && !std::isblank(input_string[j])){
            if(input_string[j] == '\n'){
                return false;
            }
            current_arg.push_back(input_string[j]);
            j++;
            t++;
            if(t > MAX_ARG_LEN){
                return false;
            }
        }
        if(!write_to_output(binary_output, current_arg)){
            return false;
        }
    }
    int val_type = 0;
    int num = 0;
    for(int args = args_num; args < ARGS_IN_COMMAND; ++args){
        binary_output.write(reinterpret_cast<const char*>(&val_type), sizeof(int));
        binary_output.write(reinterpret_cast<const char*>(&num), sizeof(int));
    }
    return true;
}

int main(){
    fill_command_codes();
    fill_command_args();
    fill_register_adds();

    std::string filename;
    std::cin >> filename;
    std::ifstream input(filename, std::ios::in);
    if(!input.is_open()){
        std::cout << "Unable to open the input file" << std::endl;
        return -1;
    }

    std::ofstream output("../output.bin", std::ios::out | std::ios::binary | std::ios::trunc);
    if(!output.is_open()){
        std::cout << "Unable to open output file, check permissions." << std::endl;
        input.close();
        return -1;
    }

    std::string next_line;
    std::string command;
    int i;
    int line = 0;

    while(getline(input, next_line)){
        i = 0;
        command = "";
        while(i < next_line.length() && !std::isblank(next_line[i])) {
            command.push_back(next_line[i]);
            i++;
            if(i > MAX_COMMAND_LEN){
                std::cout << "Unknown command, line " << line << std::endl;
                input.close();
                output.close();
                return -1;
            }
        }

        if(command_codes.find(command) == command_codes.end()){
            std::cout << "Unknown command, line " << line << std::endl;
            input.close();
            output.close();
            return -1;
        }
        output.write(reinterpret_cast<const char *>(&command_codes[command]), sizeof(int));
        if(!get_args(output, command_args_nums[command], next_line, i)){
            std::cout << "Bad arguments in line " << line << std::endl;
            input.close();
            output.close();
            return -1;
        }
        line++;
    }
    std::string hello = "Привет";

    int cur_num;
    for(int i = 0; i < hello.length(); ++i){
        cur_num = int(hello[i]);
        output.write(reinterpret_cast<const char*>(&cur_num), sizeof(int));
    }
    input.close();
    output.close();
    return 0;
}