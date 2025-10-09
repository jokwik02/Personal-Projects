#include "query_corpora.h"
#include <iostream>
int main(){
    std::string input;
    std::cout << "Loading corpus..." << std::endl;
    Corpus c = load_corpus("bnc-05M.csv");
    build_indices(c);
    std::cout << "Enter query (or nothing to quit): ";
    std::getline(std::cin, input);
    while(!input.empty()){
        try{
            Query q = parse_query(input, c);
            std::vector<Match> matches = match2(c,q);
            if(matches.empty()){
                std::cout << "No matches found" << std::endl;
            }
            int count = 0;
            for(Match m:matches){
                count++;
                int pos = 0;
                int length = 1;
                bool start = false;
                for(int i = c.sentences[m.sentence]; i < c.sentences[m.sentence + 1]; i++){
                    if(pos == m.pos){
                        start = true;
                    }
                    if(start && length <= m.len){
                        std::cout << "\033[36m" << c.index2string[c.tokens[i].word] << " ";
                        length++;
                    } else{
                        std::cout << "\033[37m" << c.index2string[c.tokens[i].word] << " ";
                    }
                    pos++;
                }
                std::cout << std::endl;
                if(count >= 10){break;}
            }
        } catch(const std::invalid_argument &e){
            std::cerr << "Error: " << e.what() << std::endl;
        }
        std::cout << "\033[37m" << "Enter query: ";
        std::getline(std::cin, input);
    }
} 
