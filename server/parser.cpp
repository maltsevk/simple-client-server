#include "parser.h"

#include <regex>
#include <set>

Parser::Parser(const std::string& str) :
    m_str(str)
{}

std::string Parser::handle() const
{
    std::string str = m_str;
    std::smatch num_match;
    std::multiset<int> numbers;

    while (std::regex_search(str, num_match, std::regex("\\d+")))
    {
        int number;

        try
        {
            number = std::stoi(num_match[0]);
        }
        catch(const std::out_of_range& ex)
        {
            return std::string("The message contains a very large number");
        }

        numbers.insert(number);
        str = num_match.suffix().str();
    }

    if (numbers.empty())
        return std::string("There are no numbers in the message");

    std::string result;

    for (const auto& number : numbers)
        result += std::to_string(number) + " ";

    result += "\n";
    result += std::to_string(std::accumulate(numbers.begin(), numbers.end(), 0));

    return result;
}