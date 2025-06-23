#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include <string>
#include <map>

class ConfigParser {
public:
    ConfigParser(const std::string &filename);
    std::string getValue(const std::string &section, const std::string &key) const;

private:
    void parseConfigFile(const std::string &filename);
    std::map<std::string, std::map<std::string, std::string>> configMap;
};

#endif // CONFIGPARSER_H
