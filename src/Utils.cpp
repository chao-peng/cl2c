#include "Utils.h"
#include "Constants.h"

#include <iostream>
#include <list>
#include <set>
#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cctype>

int CL2CUtils::toInteger(std::string s){
    int power = 1;
    int result = 0;
    char c;
    for (int i = s.length() - 1; i >= 0; --i){
        c = s.at(i);
        if (!isdigit(c)) return 0;
        result = result + power * (c - '0');
        power *= 10;
    }
    return result;
}

void CL2CUtils::makeNumberValid(int& number, int low, int high, bool inclusive = true){
    if (!inclusive){
        low = low + 1;
        high = high - 1;
    }
    if (number < low) number = low;
    if (number > high) number = high;
}

std::list<int> CL2CUtils::retrieveNumbers(std::string inputString, int totalNumber){
    inputString = inputString + ',';
    std::list<int> numberList;
    std::string part = "";
    size_t i = 0;
    bool expectingSecondNumber = false;
    bool isNegative = false;
    int first, second;
    char c;
    while(i < inputString.length()){
        c = inputString.at(i);

        if (c == '-'){
            isNegative = true;
            ++i;
            continue;
        } else if (c == ':'){
            if (part.empty()){
                ++i;
                continue;
            }
            first = toInteger(part);
            if (isNegative) {
                first = totalNumber - first + 1;
                isNegative = false;
            }
                makeNumberValid(first, 1, totalNumber);
                expectingSecondNumber = true;
                part = "";
        } else if (c == ','){
            if (part.empty()){
                ++i;
                continue;
            }
            if (expectingSecondNumber){
                expectingSecondNumber = false;
                second = toInteger(part);
                if (isNegative) {
                    second = totalNumber - second + 1;
                    isNegative = false;
                }
                makeNumberValid(first, 1, totalNumber);
                makeNumberValid(second, 1, totalNumber);
                part = "";
                if (first == second) {
                    numberList.push_back(first);
                } else {
                    if (first > second) { //swap without introducing a new variable;
                        first = first + second;
                        second = first - second;
                        first = first - second;
                    }
            
                    while (first <= second){
                        numberList.push_back(first);
                        ++first;
                    }
                }
            } else {
                first = toInteger(part);
                if (isNegative) {
                    first = totalNumber - first + 1;
                    isNegative = false;
                }
                makeNumberValid(first, 1, totalNumber);
                numberList.push_back(first);
                part = "";
            }
        } else if (isdigit(c)){
            part = part + c;
        }
        ++i;
    }
    numberList.sort();
    numberList.unique();
    return numberList;
}

int CL2CUtils::getNumLines(std::string fileName){
    std::ifstream inFile(fileName);
    int i = std::count(std::istreambuf_iterator<char>(inFile), std::istreambuf_iterator<char>(), '\n');
    inFile.close();
    return i;
}

bool CL2CUtils::hasQuality(const unsigned int& quality, const unsigned int& qualityIdentifier){
    return ((quality & qualityIdentifier) == qualityIdentifier)? true: false;
}

std::string CL2CUtils::colourString(const std::string& str, const char* const colour){
    return colour + str + output_colour::KNRM;
}

int CL2CUtils::alert(const std::string& message, const char* const colour, const int& msg_type){
    switch(msg_type){
        case (message_type::STD):
            std::cout << colourString(message, colour) << std::endl;
            return error_code::STATUS_OK;
        case (message_type::ERR):
            std::cerr << colourString(message, colour) << std::endl;
            return error_code::STATUS_OK;
        case (message_type::LOG):
            std::cout << colourString(message, colour) << std::endl;
            return error_code::STATUS_OK;
        default:
            return error_code::STATUS_ERR;
    }
}

int CL2CUtils::alert(const std::stringstream& messagestream, const char* const colour, const int& msg_type){
    return alert(messagestream.str(), colour, msg_type);
}

int CL2CUtils::runCommand(const std::string& command){
    return system(command.c_str());
}
