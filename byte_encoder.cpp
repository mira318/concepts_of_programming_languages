#include <iostream>
#include <fstream>
#include <map>

std::map<std::string, std::string> command_codes;
std::map<std::string, int> command_args_nums;

void fill_command_codes(){
    command_codes["MOV"] = "00";
    command_codes["JMP"] = "01";
    command_codes["JZ"] = "02";
    command_codes["JNZ"] = "03";
    command_codes["STOP"] = "04";
    command_codes["ADD"] = "05";
    command_codes["SUB"] = "06";
    command_codes["MUL"] = "07";
    command_codes["LOAD"] = "08";
    command_codes["SAFE"] = "09";
    command_codes["INP"] = "0A";
    command_codes["OUT"] = "0B";
}

void fill_command_args(){
    command_args_nums["MOV"] = 2;
    command_args_nums["JMP"] = 2; // 2 args, like a twice long world for address
    command_args_nums["JZ"] = 2;
    command_args_nums["JNZ"] = 2;
    command_args_nums["STOP"] = 0;
    command_args_nums["ADD"] = 2;
    command_args_nums["SUB"] = 2;
    command_args_nums["MUL"] = 2;
    command_args_nums["LOAD"] = 3;
    command_args_nums["SAFE"] = 3;
    command_args_nums["INP"] = 1;
    command_args_nums["OUT"] = 1;
}

bool write_to_output(std::ofstream& binary_output, std::string current_arg){
    std::cout << "get arg: " << current_arg << std::endl;
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
        if(input_string[j] == '\n'){
            return false;
        }
        t = 0;
        while(j < input_string.length() && !std::isblank(input_string[j])){
            if(input_string[j] == '\n'){
                return false;
            }
            current_arg.push_back(input_string[j]);
            j++;
            t++;
            if(t > 10){
                return false;
            }
        }
        if(!write_to_output(binary_output, current_arg)){
            return false;
        }
    }
    return true;
}

int main(){
    fill_command_codes();
    fill_command_args();
    std::string filename;
    std::cin >> filename;
    std::ifstream input(filename, std::ios::in);
    if(!input.is_open()){
        std::cout << "Unable to open the input file" << std::endl;
        return -1;
    }
    std::ofstream output("output.bin", std::ios::out | std::ios::binary | std::ios::trunc);
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
            if(i > 10){
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
        output << command_codes[command] << " ";
        if(!get_args(output, command_args_nums[command], next_line, i)){
            std::cout << "Bad arguments in line " << line << std::endl;
            return -1;
        }
        line++;
    }
    input.close();
    output.close();
    return 0;
}