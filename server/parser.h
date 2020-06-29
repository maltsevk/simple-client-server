#ifndef PARSER_H
#define PARSER_H

#include <string>

class Parser
{
public:
    Parser(const std::string& str);
    ~Parser() = default;

    std::string handle() const;

private:
    std::string m_str;
};

#endif // PARSER_H