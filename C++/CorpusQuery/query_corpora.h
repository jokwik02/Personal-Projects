#ifndef QUERY_CORPORA_H
#define QUERY_CORPORA_H
#include <span>
#include <vector>
#include <string>
#include <map>
#include <variant>
#include <cstdint>
struct Token
{
    uint32_t word;
    uint32_t c5;
    uint32_t lemma;
    uint32_t pos;
};
struct Literal
{
    std::string attribute; 
    uint32_t value;
    bool is_equality;
};
using Sentence = std::vector<Token>;
struct Match
{
    int sentence;
    int pos;
    int len;
};
using Index = std::vector<int>;
struct Corpus
{
    std::vector<Token> tokens;
    std::vector<int> sentences;
    std::vector<std::string> index2string;        
    std::map<std::string, uint32_t> string2index; 
    Index word_index;
    Index c5_index;    
    Index lemma_index; 
    Index pos_index; 
};;
using Clause = std::vector<Literal>;
using Query = std::vector<Clause>;
struct IndexSet
{
    std::span<const int> elems;
    int shift;
};
struct DenseSet
{
    int first;
    int last;
};
struct ExplicitSet
{
    std::vector<int> elems;
};
struct MatchSet
{
    std::variant<DenseSet, IndexSet, ExplicitSet> set;
    bool complement; 
};
Corpus load_corpus(const std::string &filename);
Index build_index(const std::vector<Token> &tokens, uint32_t Token::* attribute);
void build_indices(Corpus &corpus);
Query parse_query(const std::string &text, const Corpus &corpus);
bool attribute_is_valid(std::string &attr);
IndexSet index_lookup(const Corpus &corpus, const std::string &attribute, uint32_t value);
MatchSet intersection(const MatchSet &A, const MatchSet &B);
ExplicitSet intersection(const IndexSet &A, const IndexSet &B);
ExplicitSet intersection(const IndexSet &A, const DenseSet &B);
ExplicitSet intersection(const IndexSet &A, const ExplicitSet &B);
DenseSet intersection(const DenseSet &A, const DenseSet &B);
ExplicitSet intersection(const DenseSet &A, const ExplicitSet &B);
ExplicitSet intersection(const ExplicitSet &A, const ExplicitSet &B);
ExplicitSet intersection(const DenseSet &A, const IndexSet &B);
ExplicitSet intersection(const ExplicitSet &A, const IndexSet &B);
ExplicitSet intersection(const ExplicitSet &A, const DenseSet &B);
ExplicitSet difference(const IndexSet &A, const IndexSet &B);
ExplicitSet difference(const ExplicitSet &A, const IndexSet &B);
ExplicitSet difference(const ExplicitSet &A, const ExplicitSet &B);
ExplicitSet difference(const IndexSet &A, const ExplicitSet &B);
ExplicitSet difference(const DenseSet &A, const ExplicitSet &B);
ExplicitSet difference(const DenseSet &A, const IndexSet &B);
template <typename T1, typename T2>
ExplicitSet difference(const T1&, const T2&);
ExplicitSet set_union(const IndexSet &A, const IndexSet &B);
ExplicitSet set_union(const IndexSet &A, const ExplicitSet &B);
ExplicitSet set_union(const ExplicitSet &A, const ExplicitSet &B);
ExplicitSet set_union(const ExplicitSet &A, const IndexSet &B);
template <typename T1, typename T2>
ExplicitSet set_union(const T1&, const T2&);
std::span<const int> shift(const IndexSet &s);
std::vector<Match> match(const Corpus &corpus, const Query &query);
std::vector<Match> match(const Corpus &corpus, const std::string &query_string);
std::vector<Match> match_single(const Corpus &corpus, const std::string &attr, const std::string &value);
MatchSet match_set(const Corpus &corpus, const Literal &literal, int shift);
void match_set(const Corpus &corpus, const Clause &clause, int shift, std::vector<MatchSet> &sets);
MatchSet match_set(const Corpus &corpus, const Query &query);
bool comp_size(const MatchSet &A, const MatchSet &B);
int get_size(const IndexSet &A);
int get_size(const ExplicitSet &A);
int get_size(const DenseSet &A);
std::vector<Match> match2(const Corpus &corpus, const Query &query);
std::vector<Match> match2(const Corpus &corpus, const ExplicitSet &M, int size);
std::vector<Match> match2(const Corpus &corpus, const IndexSet &M, int size);
std::vector<Match> match2(const Corpus &corpus, const DenseSet &M, int size);
Token generate_token(Corpus &corpus, std::string t_word, std::string t_c5, std::string t_lemma, std::string t_pos);
#endif