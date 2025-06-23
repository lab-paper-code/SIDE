#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "ConfigParser.hpp"

ConfigParser::ConfigParser(const std::string &filename) {
    parseConfigFile(filename);
}

void ConfigParser::parseConfigFile(const std::string &filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open config file: " + filename);
    }

    std::string line, currentSection;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            // Skip empty lines and comments
            continue;
        }
        if (line[0] == '[' && line.back() == ']') {
            // New section
            currentSection = line.substr(1, line.size() - 2);
        } else {
            std::istringstream iss(line);
            std::string key, value;
            if (std::getline(iss, key, '=') && std::getline(iss, value)) {
                configMap[currentSection][key] = value;
            }
        }
    }
}

std::string ConfigParser::getValue(const std::string &section, const std::string &key) const {
    auto itSection = configMap.find(section);
    if (itSection != configMap.end()) {
        auto itKey = itSection->second.find(key);
        if (itKey != itSection->second.end()) {
            return itKey->second;
        } else {
            throw std::runtime_error("Key '" + key + "' not found in section '" + section + "'");
        }
    } else {
        throw std::runtime_error("Section '" + section + "' not found in config file");
    }
}