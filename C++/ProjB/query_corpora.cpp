#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include "query_corpora.h"
#include <span>
//Load corpus into a corpus object from a file
Corpus load_corpus(const std::string &filename){
    Corpus corpus;
    std::ifstream f(filename);
    if (!f) { 
        std::cerr << "Unable to open file" << std::endl;
    }
    std::string l;
    bool skip = true;
    int pos = 0;
    corpus.sentences.push_back(pos);
    while(std::getline(f,l)){
        //We've reached the end of a sentence, so we add the sentence to the corpus
        if(l.empty()){
            corpus.sentences.push_back(pos);
            continue;
        //Skip comment lines
        } else if(l[0] == '#'){
            continue;
        //For skipping the first line in the file
        } else if(skip){
            skip = false;
            continue;
        }
        //Get each word seperated by tab
        std::istringstream l_stream(l);
        std::string t_word, t_c5, t_lemma, t_pos;
        std::getline(l_stream, t_word, '\t');
        std::getline(l_stream, t_c5, '\t');
        std::getline(l_stream, t_lemma, '\t');
        std::getline(l_stream, t_pos, '\t');
        Token t = generate_token(corpus, t_word, t_c5, t_lemma, t_pos);
        corpus.tokens.push_back(t);
        pos++;
    }
    corpus.sentences.push_back(corpus.tokens.size());
    return corpus;
}

//Builds an index for an attribute
Index build_index(const std::vector<Token> &tokens, uint32_t Token::* attribute){
    Index index(tokens.size());
    for(int i = 0; i < (int)tokens.size() - 1; i++){
        index[i] = i;
    }
    std::stable_sort(index.begin(), index.end(), [attribute, &tokens](int a, int b) {
        return tokens[a].*attribute < tokens[b].*attribute;
    });
    return index;
}

//Builds indices for the all atributes
void build_indices(Corpus &corpus){
    corpus.lemma_index = build_index(corpus.tokens, &Token::lemma);
    corpus.c5_index = build_index(corpus.tokens, &Token::c5);
    corpus.word_index = build_index(corpus.tokens, &Token::word);
    corpus.pos_index = build_index(corpus.tokens, &Token::pos);
}

//Generates a token
Token generate_token(Corpus &corpus, std::string t_word, std::string t_c5, std::string t_lemma, std::string t_pos){
    Token t;
    auto index = corpus.string2index.find(t_word);
    if (index != corpus.string2index.end()){
        t.word = index->second;
    } else{
    
        int i = corpus.index2string.size();
        corpus.index2string.push_back(t_word);
        corpus.string2index.insert({t_word, i});
        t.word = i;
    }
    index = corpus.string2index.find(t_c5);
    if (index != corpus.string2index.end()){
        t.c5 = index->second;
    } else{
        int i = corpus.index2string.size();
        corpus.index2string.push_back(t_c5);
        corpus.string2index.insert({t_c5, i});
        t.c5 = i;
    }
    index = corpus.string2index.find(t_lemma);
    if (index != corpus.string2index.end()){
        t.lemma = index->second;
    } else{
        int i = corpus.index2string.size();
        corpus.index2string.push_back(t_lemma);
        corpus.string2index.insert({t_lemma, i});
        t.lemma = i;
    }
    index = corpus.string2index.find(t_pos);
    if (index != corpus.string2index.end()){
        t.pos = index->second;
    } else{
        int i = corpus.index2string.size();
        corpus.index2string.push_back(t_pos);
        corpus.string2index.insert({t_pos, i});
        t.pos = i;
    }
    return t;
}

//Parses a query from a string
Query parse_query(const std::string &text, const Corpus &corpus){
    Query q;
    Clause c;
    bool clause_entered = false;
    //Loop through each char
    for(int i = 0; i < (int)text.size(); i++){
        if(clause_entered){
            Literal l;
            std::string s;
            //empty clause
            if(text[i] == ']'){
                c.clear();
                q.push_back(c);
                clause_entered = false;
            }else{
                //loop through attribute
                while(text[i] != '!' && text[i] != '='){
                    s += text[i];
                    i++;
                }
                if(attribute_is_valid(s)){
                    l.attribute = s;
                } else{
                    throw std::invalid_argument("Attribute " + s +" does not exist");
                }    
                if(text[i] == '!'){
                    l.is_equality = false;
                    i+=3;
                } else{
                    l.is_equality = true;
                    i+=2;
                }
                s.clear();
                //loop through value
                while(text[i]!= '"'){
                    s += text[i];
                    i++;
                }
                auto index = corpus.string2index.find(s);
                if (index != corpus.string2index.end()){
                    l.value = index->second;
                } else{
                    int i = corpus.index2string.size();
                    l.value = i;
                }
                i++;
                c.push_back(l);
                //loop through characters until we hit end of clause or get a new literal
                while(text[i] == ' '){
                    i++;
                }
                if(text[i] == ']'){
                    clause_entered = false;
                    q.push_back(c);
                }
                //this is here so we dont skip the first letter of next literal
                else{
                    i--;
                }
            }
        }

        if(text[i] == '[' && !clause_entered){
            c.clear();
            clause_entered = true;
        } else if(text[i] != ' ' && text[i] != ']'){
            throw std::invalid_argument("Only whitespace can exist outside clauses");
        }
    }
    return q;
}

//Checks if an attribute is valid
bool attribute_is_valid(std::string &attr){
    return (attr == "word" || attr == "lemma" || attr == "pos" || attr == "c5");
}
//Looks through the index of the submitted value and finds all instances of value in it.
IndexSet index_lookup(const Corpus &corpus, const std::string &attribute, uint32_t value){
    auto &tokens = corpus.tokens;
    if(attribute == "lemma"){
        auto start = std::find_if(corpus.lemma_index.begin(), corpus.lemma_index.end(), [tokens, value](int a) {
            return tokens[a].lemma == value;});
        auto end =  std::find_if(start, corpus.lemma_index.end(), [tokens, value](int a) {
            return tokens[a].lemma != value;});
        size_t startpos =  std::distance(corpus.lemma_index.begin(), start);
        size_t endpos =  std::distance(corpus.lemma_index.begin(), end);
        IndexSet s;
        s.elems = std::span<const int>(corpus.lemma_index.begin() + startpos, endpos-startpos);
        return s;
    }else if(attribute == "c5"){
        auto start = std::find_if(corpus.c5_index.begin(), corpus.c5_index.end(), [tokens, value](int a) {
            return tokens[a].c5 == value;});
        auto end =  std::find_if(start, corpus.c5_index.end(), [tokens, value](int a) {
            return tokens[a].c5 != value;});
        size_t startpos =  std::distance(corpus.c5_index.begin(), start);
        size_t endpos =  std::distance(corpus.c5_index.begin(), end);

        IndexSet s;
        s.elems = std::span<const int>(corpus.c5_index.begin() + startpos, endpos-startpos);
        return s;
    } else if(attribute == "word"){
        auto start = std::find_if(corpus.word_index.begin(), corpus.word_index.end(), [tokens, value](int a) {
            return tokens[a].word == value;});
        auto end =  std::find_if(start, corpus.word_index.end(), [tokens, value](int a) {
            return tokens[a].word != value;});
        size_t startpos =  std::distance(corpus.word_index.begin(), start);
        size_t endpos =  std::distance(corpus.word_index.begin(), end);

        IndexSet s;
        s.elems = std::span<const int>(corpus.word_index.begin() + startpos, endpos-startpos);
        return s;
    } else{
        auto start = std::find_if(corpus.pos_index.begin(), corpus.pos_index.end(), [tokens, value](int a) {
            return tokens[a].pos == value;});
        auto end =  std::find_if(start, corpus.pos_index.end(), [tokens, value](int a) {
            return tokens[a].pos != value;});
        size_t startpos =  std::distance(corpus.pos_index.begin(), start);
        size_t endpos =  std::distance(corpus.pos_index.begin(), end);

        IndexSet s;
        s.elems = std::span<const int>(corpus.pos_index.begin() + startpos, endpos-startpos);
        return s;
    }
}
//Match function from older version
std::vector<Match> match_single(Corpus &corpus, const std::string &attr, const std::string &value){
    std::vector<Match> matches;
    IndexSet s = index_lookup(corpus, attr, corpus.string2index.find(value)->second);
    for(const int &t:s.elems){
        auto sentence = std::upper_bound(corpus.sentences.begin(), corpus.sentences.end(), t);
        int sentence_index = std::distance(corpus.sentences.begin(), sentence) - 1;
        Match m;
        m.sentence = sentence_index;
        m.len = 1;
        m.pos = t - corpus.sentences[sentence_index];
        matches.push_back(m);
    }
    return matches;
}
//Creates a match_set from a literal
MatchSet match_set(const Corpus &corpus, const Literal &literal, int shift){
    MatchSet m;
    m.complement = !literal.is_equality;
    IndexSet s = index_lookup(corpus, literal.attribute, literal.value);
    s.shift = shift;
    m.set = s;
    return m;
}
//Creates match sets for all literals in the clause and adds it to the matchset vector
void match_set(const Corpus &corpus, const Clause &clause, int shift, std::vector<MatchSet> &sets){
    if(clause.empty()){
        DenseSet d;
        d.first = 0;
        d.last = corpus.tokens.size() - shift;
        MatchSet m;
        m.complement = false;
        m.set = d;
        sets.push_back(m);
    }
    for(int i = 0; i < (int)clause.size(); i++){
        sets.push_back(match_set(corpus, clause[i], shift));
    }
}
//Returns a matchset from a query
MatchSet match_set(const Corpus &corpus, const Query &query){
    //Return empty query
    if(query.empty()){
        DenseSet d;
        d.first = 0;
        d.last = corpus.tokens.size();
        MatchSet m;
        m.complement = false;
        m.set = d;
        return m;
    }
    //Create matchsets for everything in the query
    std::vector<MatchSet> sets;
    for(int i = 0; i < (int)query.size(); i++){
        match_set(corpus, query[i], i, sets);
    }
    //Pick out all densesets
    std::vector<MatchSet> densesets;
    int i = 0;
    while(i < (int)sets.size()){
        if(std::holds_alternative<DenseSet>(sets[i].set)){
            densesets.push_back(sets[i]);
            sets.erase(sets.begin() + i);
        }else{
            i++;
        }
    }
    //Collapse all densesets into one
    MatchSet denseset;
    if(!densesets.empty()){
        denseset = densesets[0];
        for(int i = 1; i < (int)densesets.size(); i++){
            denseset = intersection(denseset, densesets[i]);
        }
    }
    //Sort all remaining sets from smallest to biggest and intersect them in that order
    MatchSet intersect;
    if(!sets.empty()){
        std::sort(sets.begin(), sets.end(), comp_size); 
        intersect = sets[0];
        for(int i = 1; i < (int)sets.size(); i++){
            intersect = intersection(sets[i], intersect);
        }
    }
    //Intersect the denseset if it exists
    if(!densesets.empty()){
        if(!sets.empty()){
            intersect = intersection(intersect, denseset);
        } else{
            return denseset;
        }
    }
    //If the final product is a complement, create a set from everything else
    if(intersect.complement){
        DenseSet d;
        d.first = 0;
        d.last = corpus.tokens.size() - 1;
        intersect.set = std::visit([&d](auto&& arg2){ return difference(d, arg2); }, intersect.set);
        intersect.complement = false;
    }
    return intersect;
}
//Compares sizes of two sets
bool comp_size(const MatchSet &A, const MatchSet &B){
    int size_A = std::visit([](auto&& arg) { return get_size(arg); }, A.set);
    int size_B = std::visit([](auto&& arg) { return get_size(arg); }, B.set);
    return size_A < size_B;
}
//Get size functions for all types of sets
int get_size(const IndexSet &A){
    return A.elems.size();
}
int get_size(const ExplicitSet &A){
    return A.elems.size();
}
int get_size(const DenseSet &A){
    return A.last - A.first + 1;
}
//Get all matches from a query
std::vector<Match> match2(const Corpus &corpus, const Query &query){
    std::vector<Match> matches;
    MatchSet m = match_set(corpus, query);
    int size = query.size();
    matches = std::visit([&corpus, &size](auto&& arg1){ return match2(corpus, arg1, size); }, m.set);
    return matches;
}
//Match functions for all types of sets
std::vector<Match> match2(const Corpus &corpus, const ExplicitSet &M, int size){
    std::vector<Match> matches;
    for(const int &t:M.elems){
        auto sentence = std::upper_bound(corpus.sentences.begin(), corpus.sentences.end(), t);
        int sentence_index = std::distance(corpus.sentences.begin(), sentence) - 1;
        Match m;
        m.sentence = sentence_index;
        m.len = size;
        m.pos = t - corpus.sentences[sentence_index];
        if(corpus.sentences[sentence_index] + m.pos + m.len <= corpus.sentences[sentence_index + 1]){
            matches.push_back(m);
        }
    }
    return matches;
}
std::vector<Match> match2(const Corpus &corpus, const IndexSet &M, int size){
    std::vector<Match> matches;
    for(const int &t:M.elems){
        auto sentence = std::upper_bound(corpus.sentences.begin(), corpus.sentences.end(), t);
        int sentence_index = std::distance(corpus.sentences.begin(), sentence) - 1;
        Match m;
        m.sentence = sentence_index;
        m.len = size;
        m.pos = t - corpus.sentences[sentence_index];
        if(corpus.sentences[sentence_index] + m.pos + m.len <= corpus.sentences[sentence_index + 1]){
            matches.push_back(m);
        }
    }
    return matches;
}
std::vector<Match> match2(const Corpus &corpus, const DenseSet &M, int size){
    std::vector<Match> matches;
    for(int i = M.first; i <= M.last; i++){
        auto sentence = std::upper_bound(corpus.sentences.begin(), corpus.sentences.end(), i);
        int sentence_index = std::distance(corpus.sentences.begin(), sentence) - 1;
        Match m;
        m.sentence = sentence_index;
        m.len = size;
        m.pos = i - corpus.sentences[sentence_index];
        if(corpus.sentences[sentence_index] + m.pos + m.len <= corpus.sentences[sentence_index + 1]){
            matches.push_back(m);
        }
    }
    return matches;
}
//Finds matches in the corpus from a query (from a old version)
std::vector<Match> match(const Corpus &corpus, const Query &query){
    std::vector<Match> matches;
        Match m;
        bool first_token = true;
        int pos = 0;
        int current_clause = 0;
        int index = 0;
        int sentence_length = corpus.sentences[index + 1] - corpus.sentences[index];
        for(Token t: corpus.tokens){
            if(pos >= sentence_length){
                index++;
                pos = 0;
                sentence_length = corpus.sentences[index + 1] - corpus.sentences[index];
            }
            if(query.empty()){
                m.sentence = index;
                m.len = 1;
                m.pos = pos;
                matches.push_back(m);
                pos++;
                continue;
            }
            std::unordered_map<std::string, std::uint32_t> tokenMap = {{"c5", t.c5},{"pos", t.pos},{"lemma", t.lemma}, {"word", t.word}};
            bool all_literals_matching = true;
            if(!query[current_clause].empty()){
                //Loops through each literal in a query until the end or until the literal dousnt match the token
                for(Literal l: query[current_clause]){
                    auto value = tokenMap.find(l.attribute);
                    if(l.is_equality){
                        all_literals_matching = (value->second == l.value);
                    } else{
                        all_literals_matching = !(value->second == l.value);
                    }
                    if(!all_literals_matching){
                        break;
                    }
                }
            }
            if(all_literals_matching){
                if(first_token){
                    m.pos = pos;
                    first_token = false;
                }         
                current_clause++;
                if(current_clause >= (int)query.size()){
                    m.sentence = index;
                    m.len = query.size();
                    matches.push_back(m);
                    current_clause = 0;
                }
            }else{
                first_token = true;
                current_clause = 0;
            }
            pos++;
        }
    
    return matches;
}

//Same as other match but it takes a query string instead of query object (from old version)
std::vector<Match> match(const Corpus &corpus, const std::string &query_string){
    Query q = parse_query(query_string, corpus);
    return match(corpus, q);
}

//Returns a set that is the intersection of two sets
MatchSet intersection(const MatchSet &A, const MatchSet &B){
    MatchSet m;
    //If both sets are complements, get the union of them and set that as a complement
    if(A.complement && B.complement){
        m.complement = true;
        m.set = std::visit([](auto&& arg1, auto&& arg2) -> std::variant<DenseSet, IndexSet, ExplicitSet>{ return set_union(arg1, arg2); }, A.set, B.set);
        return m;                        
    } 
    //If one of the sets is a complement, get the difference between them instead
    else if (A.complement && !std::holds_alternative<DenseSet>(B.set)){
        m.complement = false;
        m.set = std::visit([](auto&& arg1, auto&& arg2) -> std::variant<DenseSet, IndexSet, ExplicitSet>{ return difference(arg1, arg2); }, B.set, A.set);
        return m;
    } else if (B.complement && !std::holds_alternative<DenseSet>(A.set)){
        m.complement = false;
        m.set = std::visit([](auto&& arg1, auto&& arg2) -> std::variant<DenseSet, IndexSet, ExplicitSet>{ return difference(arg1, arg2); }, A.set, B.set);
        return m;
    } else{
        m.set = std::visit([](auto&& arg1, auto&& arg2) -> std::variant<DenseSet, IndexSet, ExplicitSet>{ return intersection(arg1, arg2); }, A.set, B.set);
        m.complement = (A.complement && B.complement);
        return m;
    }
}
//Functions for returning the differenxe for different combinations of sets
ExplicitSet difference(const IndexSet &A, const IndexSet &B){
    ExplicitSet C;
    if(A.elems.size() < B.elems.size() / 10){
        for(int x:A.elems){
            int t = x - A.shift;
            if(!std::binary_search(B.elems.begin(), B.elems.end(), t + B.shift)){
                C.elems.push_back(t);
            }
        }
        return C;
    } 
    int p = 0;
    int q = 0;
    while(p < (int)A.elems.size() && q < (int)B.elems.size()){
        if(A.elems[p] - A.shift < B.elems[q] - B.shift){
            C.elems.push_back(A.elems[p] - A.shift);
            p++;
        } else if(A.elems[p] - A.shift > B.elems[q] - B.shift){
            q++;
        } else{
            p++;
            q++;
        }
    }
    while(p < (int)A.elems.size()){
        C.elems.push_back(A.elems[p] - A.shift);
        p++;
    }
    return C;
}

ExplicitSet difference(const ExplicitSet &A, const IndexSet &B){
    ExplicitSet C;
    if(A.elems.size() < B.elems.size() / 10){
        for(int x:A.elems){
            if(!std::binary_search(B.elems.begin(), B.elems.end(), x + B.shift)){
                C.elems.push_back(x);
            }
        }
        return C;
    } 
    int p = 0;
    int q = 0;
    while(p < (int)A.elems.size() && q < (int)B.elems.size()){
        if(A.elems[p] < B.elems[q] - B.shift){
            C.elems.push_back(A.elems[p]);
            p++;
        } else if(A.elems[p] > B.elems[q] - B.shift){
            q++;
        } else{
            p++;
            q++;
        }
    }
    while(p < (int)A.elems.size()){
        C.elems.push_back(A.elems[p]);
        p++;
    }
    return C;
}
ExplicitSet difference(const ExplicitSet &A, const ExplicitSet &B){
    ExplicitSet C;
    if(A.elems.size() < B.elems.size() / 10){
        for(int x:A.elems){
            if(!std::binary_search(B.elems.begin(), B.elems.end(),x)){
                C.elems.push_back(x);
            }
        }
        return C;
    } 
    int p = 0;
    int q = 0;
    while(p < (int)A.elems.size() && q < (int)B.elems.size()){
        if(A.elems[p] < B.elems[q]){
            C.elems.push_back(A.elems[p]);
            p++;
        } else if(A.elems[p] > B.elems[q]){
            q++;
        } else{
            p++;
            q++;
        }
    }
    while(p < (int)A.elems.size()){
        C.elems.push_back(A.elems[p]);
        p++;
    }
    return C;
}

ExplicitSet difference(const IndexSet &A, const ExplicitSet &B){
    ExplicitSet C;
    if(A.elems.size() < B.elems.size() / 10){
        for(int x:A.elems){
            int t = x - A.shift;
            if(!std::binary_search(B.elems.begin(), B.elems.end(), t)){
                C.elems.push_back(t);
            }
        }
        return C;
    } 
    int p = 0;
    int q = 0;
    while(p < (int)A.elems.size() && q < (int)B.elems.size()){
        if(A.elems[p] - A.shift < B.elems[q]){
            C.elems.push_back(A.elems[p] - A.shift);
            p++;
        } else if(A.elems[p] - A.shift > B.elems[q]){
            q++;
        } else{
            p++;
            q++;
        }
    }
    while(p < (int)A.elems.size()){
        C.elems.push_back(A.elems[p] - A.shift);
        p++;
    }
    return C;
}
ExplicitSet difference(const DenseSet &A, const ExplicitSet &B){
    ExplicitSet C;
    int p = A.first;
    int q = 0;
    while(p <= A.last && q < (int)B.elems.size()){
        if(p < B.elems[q]){
            C.elems.push_back(p);
            p++;
        } else if(p > B.elems[q]){
            q++;
        } else{
            p++;
            q++;
        }
    }
    while(p < A.last){
        C.elems.push_back(p);
        p++;
    }
    return C;
}
ExplicitSet difference(const DenseSet &A, const IndexSet &B){
    ExplicitSet C;
    int p = A.first;
    int q = 0;
    while(p <= A.last && q < (int)B.elems.size()){
        if(p < B.elems[q] - B.shift){
            C.elems.push_back(p);
            p++;
        } else if(p > B.elems[q] - B.shift){
            q++;
        } else{
            p++;
            q++;
        }
    }
    while(p < A.last){
        C.elems.push_back(p);
        p++;
    }
    return C;
}
template <typename T1, typename T2>
ExplicitSet difference(const T1&, const T2&) {
    return ExplicitSet{}; 
}

//Functions for gettng the intersections for all combinations of sets
ExplicitSet intersection(const IndexSet &A, const IndexSet &B){
    ExplicitSet C;
    if(A.elems.size() < B.elems.size() / 10){
        for(int x:A.elems){
            int t = x - A.shift;
            if(std::binary_search(B.elems.begin(), B.elems.end(), t + B.shift)){
                C.elems.push_back(t);
            }
        }
        return C;
    } else if(B.elems.size() < A.elems.size() / 10){
        for(int x:B.elems){
            int t = x - B.shift;
            if(std::binary_search(A.elems.begin(), A.elems.end(), t + A.shift)){
                C.elems.push_back(t);
            }
        }
        return C;
    }
    int p = 0;
    int q = 0;
    while(p < (int)A.elems.size() && q < (int)B.elems.size()){
        if(A.elems[p] - A.shift < B.elems[q] - B.shift){
            p++;
        } else if(A.elems[p] - A.shift > B.elems[q] - B.shift){
            q++;
        } else{
            C.elems.push_back(A.elems[p] - A.shift);
            p++;
            q++;
        }
    }
    return C;
}
ExplicitSet intersection(const IndexSet &A, const DenseSet &B){
    std::vector<int> shifted;
    for(int i: A.elems){
        if(i - A.shift <= B.last && i - A.shift >= B.first){
            shifted.push_back(i-A.shift);
        }
    }
    return ExplicitSet{shifted};
}
ExplicitSet intersection(const IndexSet &A, const ExplicitSet &B){
    ExplicitSet C;
    if(A.elems.size() < B.elems.size() / 10){
        for(int x:A.elems){
            int t = x - A.shift;
            if(std::binary_search(B.elems.begin(), B.elems.end(), t)){
                C.elems.push_back(t);
            }
        }
        return C;
    } else if(B.elems.size() < A.elems.size() / 10){
        for(int x:B.elems){
            if(std::binary_search(A.elems.begin(), A.elems.end(), x + A.shift)){
                C.elems.push_back(x);
            }
        }
        return C;
    }
    int p = 0;
    int q = 0;
    while(p < (int)A.elems.size() && q < (int)B.elems.size()){
        if(A.elems[p] - A.shift < B.elems[q]){
            p++;
        } else if(A.elems[p] - A.shift > B.elems[q]){
            q++;
        } else{
            C.elems.push_back(A.elems[p] - A.shift);
            p++;
            q++;
        }
    }
    return C;
}
DenseSet intersection(const DenseSet &A, const DenseSet &B){
    DenseSet D;
    D.first = std::max(A.first, B.first);
    D.last = std::min(A.last, B.last);
    return D;
}
ExplicitSet intersection(const DenseSet &A, const ExplicitSet &B){
    std::vector<int> v;
    for (int i : B.elems){
        if(i > A.last){
            return ExplicitSet{v};
        }
        v.push_back(i);
    }
    return ExplicitSet{v};
}
ExplicitSet intersection(const ExplicitSet &A, const ExplicitSet &B){
    ExplicitSet C;
    if(A.elems.size() < B.elems.size() / 10){
        for(int x:A.elems){
            if(std::binary_search(B.elems.begin(), B.elems.end(), x)){
                C.elems.push_back(x);
            }
        }
        return C;
    } else if(B.elems.size() < A.elems.size() / 10){
        for(int x:B.elems){
            if(std::binary_search(A.elems.begin(), A.elems.end(), x)){
                C.elems.push_back(x);
            }
        }
        return C;
    }
    int p = 0;
    int q = 0;
    while(p < (int)A.elems.size() && q < (int)B.elems.size()){
        if(A.elems[p] < B.elems[q]){
            p++;
        } else if(A.elems[p] > B.elems[q]){
            q++;
        } else{
            C.elems.push_back(A.elems[p]);
            p++;
            q++;
        }
    }
    return C;
}
ExplicitSet intersection(const DenseSet &A, const IndexSet &B){
    return intersection(B, A);
}
ExplicitSet intersection(const ExplicitSet &A, const IndexSet &B){
    return intersection(B, A);
}
ExplicitSet intersection(const ExplicitSet &A, const DenseSet &B){
    return intersection(B, A);
}
ExplicitSet set_union(const IndexSet &A, const IndexSet &B){
    ExplicitSet C;
    int p = 0;
    int q = 0;
    while(p < (int)A.elems.size() && q < (int)B.elems.size()){
        if(A.elems[p] - A.shift < B.elems[q] - B.shift){
            C.elems.push_back(A.elems[p] - A.shift);
            p++;
        } else if(A.elems[p] - A.shift > B.elems[q] - B.shift){
            C.elems.push_back(B.elems[q] - B.shift);
            q++;
        } else{
            C.elems.push_back(A.elems[p] - A.shift);
            p++;
            q++;
        }
    }
    while(p < (int)A.elems.size()){
        C.elems.push_back(A.elems[p] - A.shift);
        p++;
    }
    while(q < (int)B.elems.size()){
        C.elems.push_back(B.elems[q] - B.shift);
        q++;
    }
    return C;
}
//Functions for getting the union of all combinations of sets
ExplicitSet set_union(const ExplicitSet &A, const IndexSet &B){
    ExplicitSet C;
    int p = 0;
    int q = 0;
    while(p < (int)A.elems.size() && q < (int)B.elems.size()){
        if(A.elems[p] < B.elems[q] - B.shift){
            C.elems.push_back(A.elems[p]);
            p++;
        } else if(A.elems[p] > B.elems[q] - B.shift){
            C.elems.push_back(B.elems[q] - B.shift);
            q++;
        } else{
            C.elems.push_back(A.elems[p]);
            p++;
            q++;
        }
    }
    while(p < (int)A.elems.size()){
        C.elems.push_back(A.elems[p]);
        p++;
    }
    while(q < (int)B.elems.size()){
        C.elems.push_back(B.elems[q] - B.shift);
        q++;
    }
    return C;
}
ExplicitSet set_union(const IndexSet &A, const ExplicitSet &B){
    return set_union(B,A);
}
ExplicitSet set_union(const ExplicitSet &A, const ExplicitSet &B){
    ExplicitSet C;
    int p = 0;
    int q = 0;
    while(p < (int)A.elems.size() && q < (int)B.elems.size()){
        if(A.elems[p] < B.elems[q]){
            C.elems.push_back(A.elems[p]);
            p++;
        } else if(A.elems[p] > B.elems[q]){
            C.elems.push_back(B.elems[q]);
            q++;
        } else{
            C.elems.push_back(A.elems[p]);
            p++;
            q++;
        }
    }
    while(p < (int)A.elems.size()){
        C.elems.push_back(A.elems[p]);
        p++;
    }
    while(q < (int)B.elems.size()){
        C.elems.push_back(B.elems[q]);
        q++;
    }
    return C;
}
template <typename T1, typename T2>
ExplicitSet set_union(const T1&, const T2&) {
    return ExplicitSet{}; 
}