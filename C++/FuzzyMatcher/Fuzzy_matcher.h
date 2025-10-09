#ifndef FUZZY_MATCHER_H
#define FUZZY_MATCHER_H

#include <vector>
#include <string>
#include <stack>

class FuzzyMatcher
{
std::vector<std::string> collection;  
std::string pattern; 
std::vector<std::string> matches;
std::stack<std::vector<std::string>> matches_stack;
void find_matches();
bool fuzzy_match(const std::string &subject);
void find_matches_push();
public:
    using Iterator = std::vector<std::string>::iterator;
    FuzzyMatcher(std::vector<std::string> collection);
    void push(char c);
    void pop();
    size_t size() const;
    std::vector<std::string> getmatches();
    Iterator begin();
    Iterator end();
};

#endif