#include "Fuzzy_matcher.h"
#include <string.h>
#include <iostream>
#include <cctype>
using Iterator = std::vector<std::string>::iterator;
//Constructor for fuzzymatcher
FuzzyMatcher::FuzzyMatcher(std::vector<std::string> collection) : collection(std::move(collection)){
    this->matches_stack.push(this->matches);
}
//Pushes a character onto the pattern
void FuzzyMatcher::push(char c){
    this->pattern += c;
    find_matches_push();
    this->matches_stack.push(this->matches);
}
//Removes a character from the pattern
void FuzzyMatcher::pop(){
    if(!this->pattern.empty()){
        this->pattern.pop_back();
        this->matches_stack.pop();
        this->matches = this->matches_stack.top();
    }
}
//Finds words in the collection that matches the pattern
void FuzzyMatcher::find_matches(){
    this->matches.clear();
    if(pattern.empty()){
        return;
    }
    for(std::string s: this->collection){
        if(fuzzy_match(s)){
            this->matches.push_back(s);
        }
    }
}
//Finds word in the previous matches that matches the pattern
void FuzzyMatcher::find_matches_push(){
    if(matches.empty()){
        find_matches();
    }else{
        std::vector<std::string> matches;
        for(std::string s: this->matches){
            if(fuzzy_match(s)){
                matches.push_back(s);
            }
        }
        this->matches = std::move(matches);
    }

}
//Returns if a word matches a pattern "fuzzily"
bool FuzzyMatcher::fuzzy_match(const std::string &subject){
    int count = 0;
    for(char c: subject){
        if(std::tolower(c) == std::tolower(this->pattern[count])){
            if(count != (int)this->pattern.size()){
                count++;
            }
        }
    }
    return count >= (int)this->pattern.size();
}
size_t FuzzyMatcher::size()const{
    return this->matches.size();
}

Iterator FuzzyMatcher::begin(){
    return this->matches.begin();
}
Iterator FuzzyMatcher::end(){
    return this->matches.end();
}