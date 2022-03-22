#include <iostream>
#include <fstream>
#include <map>
#include <string>

const int MAX_COMMAND_LEN = 10;
const int MAX_ARG_LEN = 20;
const int MAX_ARGS_IN_COMMAND = 2;
const int MAX_FUNCTION_NAME_LENGTH = 100;
const int CHAR_SIZE = 1;
int next_address = 0;

std::map<std::string, int> command_codes;
std::map<std::string, int> command_args_nums;
std::map<std::string, int> register_adds;
std::map<std::string, int> func_addresses;

void fill_command_codes(){
    command_codes["MOVE"] = 1;
    command_codes["JZ"] = 2;
    command_codes["JNZ"] = 3;
    command_codes["STOP"] = 4;
    command_codes["ADD"] = 5;
    command_codes["SUB"] = 6;
    //command_codes["MUL"] = 7;
    //command_codes["LOAD"] = 8;
    //command_codes["SAFE"] = 9;
    command_codes["INP"] = 10;
    command_codes["OUT"] = 11;
    command_codes["IP"] = 12;
    command_codes["OUTS"] = 13;
    command_codes["CALL"] = 14;
    command_codes["RET"] = 15;
}

void fill_command_args(){
    command_args_nums["MOVE"] = 2;
    command_args_nums["JZ"] = 2; // what to check and where to jump
    command_args_nums["JNZ"] = 2;
    command_args_nums["STOP"] = 0;
    command_args_nums["ADD"] = 2;
    command_args_nums["SUB"] = 2;
    //command_args_nums["MUL"] = 2;
    //command_args_nums["LOAD"] = 2;
    //command_args_nums["SAFE"] = 2;
    command_args_nums["INP"] = 1;
    command_args_nums["OUT"] = 1;
    command_args_nums["IP"] = 1;
    command_args_nums["OUTS"] = 1;
    command_args_nums["CALL"] = 1;
    command_args_nums["RET"] = 0;
}

void fill_register_adds(){
    register_adds["AX"] = 0;
    register_adds["BX"] = 1;
    register_adds["CX"] = 2;
    register_adds["DX"] = 3;
    register_adds["EX"] = 4;
    register_adds["FX"] = 5;
    register_adds["AD"] = 6;
    register_adds["BD"] = 7;
    register_adds["CD"] = 8;
    register_adds["ED"] = 9;
    register_adds["FD"] = 10;
    register_adds["CA"] = 11;
    register_adds["CB"] = 12;
    register_adds["BP"] = 13;
    register_adds["SP"] = 14;
    register_adds["IP"] = 15;
}

bool get_number(std::string number_string, int* res){
    for(int i = 0; i < number_string.length(); ++i) {
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
        val_type = 1;
    }

    if(val_type == -1 && current_arg[0] == '#'){
        if(!get_number(&current_arg[1], &number)){
            return false;
        }
        val_type = 2;
    }

    if(val_type == -1){
        if(!get_number(current_arg, &number)){
            return false;
        }
        val_type = 3;
    }

    binary_output.write(reinterpret_cast<const char*>(&val_type), sizeof(int));
    binary_output.write(reinterpret_cast<const char*>(&number), sizeof(int));
    next_address += 2 * sizeof(int);
    return true;
}

bool get_args(std::ofstream& binary_output, int args_num, const std::string& input_string, int start){
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
    int number = 0;
    for(int i = args_num; i < MAX_ARGS_IN_COMMAND; ++i){
        binary_output.write(reinterpret_cast<const char*>(&val_type), sizeof(int));
        binary_output.write(reinterpret_cast<const char*>(&number), sizeof(int));
        next_address += 2 * sizeof(int);
    }
    return true;
}

bool get_function_name(std::string& input, int line){
    int i = 0;
    std::string func_name = "";
    while(i < input.length() && !std::isblank(input[i]) && input[i] != ':') {
        func_name.push_back(input[i]);
        i++;
        if(i > MAX_FUNCTION_NAME_LENGTH){
            std::cout << "Too long function name" << line << std::endl;
            return false;
        }
    }
    if(input[i] != ':'){
        std::cout << "Function name should be followed by : in definition" << std::endl;
        return false;
    }
    func_addresses[func_name] = next_address;
    return true;
}

bool get_function_call(std::ofstream& binary_output, std::string input){
    int i = 0;
    std::string func_name = "";

    while(i < input.length() && !std::isblank(input[i])) {
        func_name.push_back(input[i]);
        i++;
        if(i > MAX_FUNCTION_NAME_LENGTH){
            return false;
        }
    }
    if(func_addresses.find(func_name) != func_addresses.end()){
        int val_type = 2;
        int func_address = func_addresses[func_name];
        int blank = 0;
        binary_output.write(reinterpret_cast<const char*>(&val_type), sizeof(int));
        binary_output.write(reinterpret_cast<const char*>(&func_address), sizeof(int));

        binary_output.write(reinterpret_cast<const char*>(&blank), sizeof(int));
        binary_output.write(reinterpret_cast<const char*>(&blank), sizeof(int));

        next_address += 4 * sizeof(int);
        return true;
    } else {
        return false;
    }
}

void output_as_char(std::ofstream& binary_output, std::string chars){
    for(int i = 0; i < chars.length(); ++i){
        binary_output.write(reinterpret_cast<const char*>(&chars[i]), sizeof(char));
        next_address += CHAR_SIZE;
    }
}

int output_as_number(std::ofstream& binary_output, std::string number_string){
    int res;
    if(!get_number(number_string, &res)){
        return -1;
    }
    binary_output.write(reinterpret_cast<const char*>(&res), sizeof(int));
    next_address += sizeof(int);
    return 0;
}

int main(){
    fill_command_codes();
    fill_command_args();
    fill_register_adds();

    std::string input_filename;
    std::cout << "Write input file name" << std::endl;
    std::cin >> input_filename;
    std::ifstream input(input_filename, std::ios::in);
    if(!input.is_open()){
        std::cout << "Unable to open the input file" << std::endl;
        return -1;
    }

    std::string output_filename;
    std::cout << "Write output file name" << std::endl;
    std::cin >> output_filename;
    std::ofstream output(output_filename, std::ios::out | std::ios::binary | std::ios::trunc);
    if(!output.is_open()){
        std::cout << "Unable to open output file." << std::endl;
        input.close();
        return -1;
    }

    std::string next_line;
    std::string command;
    int i;
    int line = 0;

    while(getline(input, next_line)){
        if(next_line == ""){
            line++;
            continue;
        }

        if(next_line[0] == '%'){
            output_as_char(output, &next_line[1]);
            line++;
            continue;
        }

        if(next_line[0] == '$'){
            if(output_as_number(output, &next_line[1]) < 0){
                std::cout << "Unable to make a number from this string, line " << line << std::endl;
                return -1;
            }
            line++;
            continue;
        }

        if(next_line[0] == '.'){
            if(!get_function_name(next_line, line)){
                input.close();
                output.close();
                return -1;
            }
            line++;
            continue;
        }

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
        output.write(reinterpret_cast<const char*>(&command_codes[command]), sizeof(int));
        next_address += sizeof(int);
        if(command_codes[command] == 14){
            if(!get_function_call(output, &next_line[i + 1])){
                std::cout << "Unknown function name in call" << std::endl;
                input.close();
                output.close();
                return -1;
            }
        } else {
            if(!get_args(output, command_args_nums[command], next_line, i)){
                std::cout << "Bad arguments in line " << line << std::endl;
                input.close();
                output.close();
                return -1;
            }
        }

        line++;
    }

    input.close();
    output.close();
    return 0;
}