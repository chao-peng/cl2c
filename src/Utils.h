#ifndef CL2C_UTILS_HEADER_
#define CL2C_UTILS_HEADER_

#include <string>
#include <list>
#include <fstream>
#include <map>
#include <sstream>

#include "Constants.h"

class CL2CUtils{
public:

    static int toInteger(std::string s);
    static void makeNumberValid(int& number, int low, int high, bool inclusive);
    static std::list<int> retrieveNumbers(std::string inputString, int totalNumber);
    static int getNumLines(std::string fileName);
    static bool hasQuality(const unsigned int& quality, const unsigned int& qualityIdentifier);
    static std::string colourString(const std::string& str, const char* const colour);
    static int alert(const std::string& message, const char* const colour = output_colour::KNRM, const int& msg_type = message_type::STD);
    static int alert(const std::stringstream& messagestream, const char* const colour = output_colour::KNRM, const int& msg_type = message_type::STD);
    static int runCommand(const std::string& command);
};

#endif