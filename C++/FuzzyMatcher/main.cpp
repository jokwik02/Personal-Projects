#include <iostream>
#include "Fuzzy_matcher.h"
#include <x86intrin.h>
#include <unistd.h>
#include <cmath>
#include <random>
void warmup();
double mean(std::vector<double> v);
double stddev(std::vector<double> v, double mean);
int main()
{   
    std::vector<double> H_vector;
    std::vector<double> U_vector;
    std::vector<double> G_vector;
    std::vector<double> O_vector;
    std::vector<std::string> names;
    std::vector<std::string> some_names = {
        "Alice", "Bengt", "Lovisa", "Anna", "Patrik", "Viola",
        "Birger", "Frida", "Gudrun", "Simon", "Annika", "Linnea",
        "Johan", "Hannes", "Jonatan", "Joakim", "Algot", "Theodor",
        "Hugo", "Rickard", "Lars", "Arvid"
    };

    // Random number generator setup
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, some_names.size() - 1);

    // Generate 1000 random names and add to the vector
    for (int i = 0; i < 1000; ++i) {
        std::string randomName = some_names[dist(gen)];
        names.push_back(std::move(randomName));
    }
    warmup();
    //Calculates how many microseconds per tick
    auto s = _rdtsc();
    sleep(1);
    auto e = _rdtsc();
    auto d = e - s;
    double period = (1.0 / d) * 1e6;
    FuzzyMatcher fuzzy{std::move(names)};

    //Measures the time for pushing h onto the pattern
    for(int i=0;i<100;i++){
        auto x = _rdtsc();
        fuzzy.push('h');
        auto y = _rdtsc();
        auto l = y - x;
        H_vector.push_back(l*period);
        fuzzy.pop();
    }
    fuzzy.push('h');

    //Measures the time for pushing u onto the pattern
    for(int i=0;i<100;i++){
        auto x = _rdtsc();
        fuzzy.push('u');
        auto y = _rdtsc();
        auto l = y - x;
        U_vector.push_back(l*period);
        fuzzy.pop();
    }
    fuzzy.push('u');

    //Measures the time for pushing g onto the pattern
    for(int i=0;i<100;i++){
        auto x = _rdtsc();
        fuzzy.push('g');
        auto y = _rdtsc();
        auto l = y - x;
        G_vector.push_back(l*period);
        fuzzy.pop();
    }
    fuzzy.push('g');

    //Measures the time for pushing o onto the pattern
    for(int i=0;i<100;i++){
        auto x = _rdtsc();
        fuzzy.push('o');
        auto y = _rdtsc();
        auto l = y - x;
        O_vector.push_back(l*period);
        fuzzy.pop();
    }
    fuzzy.push('o');

    //Calculate mean and standard deviation for all measurements
    double m = mean(H_vector);
    double sd = stddev(H_vector, m);
    std::cout << "H mean: " << m << " stddev: " << sd <<std::endl;

    m = mean(U_vector);
    sd = stddev(U_vector, m);
    std::cout << "U mean: " << m << " stddev: " << sd <<std::endl;

    m = mean(G_vector);
    sd = stddev(G_vector, m);
    std::cout << "G mean: " << m << " stddev: " << sd <<std::endl;

    m = mean(O_vector);
    sd = stddev(O_vector, m);
    std::cout << "O mean: " << m << " stddev: " << sd <<std::endl;

}

//Runs through the entire code once for warmup
void warmup(){
    auto s = _rdtsc();
    sleep(1);
    auto e = _rdtsc();
    auto d = e - s;
    double period = (1.0 / d) * 1e6;
    std::vector<double> H_vector;
    std::vector<double> U_vector;
    std::vector<double> G_vector;
    std::vector<double> O_vector;
    std::vector<std::string> names;
    std::vector<std::string> some_names = {
        "Alice", "Bengt", "Lovisa", "Anna", "Patrik", "Viola",
        "Birger", "Frida", "Gudrun", "Simon", "Annika", "Linnea",
        "Johan", "Milou", "Jonatan", "Joakim", "Algot", "Theodor",
         "Hugo", "Rickard", "Hinok", "Arvid"
    };

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, some_names.size() - 1);

    for (int i = 0; i < 1000; ++i) {
        std::string randomName = some_names[dist(gen)];
        names.push_back(std::move(randomName));
    }
    FuzzyMatcher fuzzy{std::move(names)};
    for(int i=0;i<1000;i++){
        auto x = _rdtsc();
        fuzzy.push('h');
        auto y = _rdtsc();
        auto l = y - x;
        H_vector.push_back(l*period);
        fuzzy.pop();
    }
        fuzzy.push('h');
    for(int i=0;i<1000;i++){
        auto x = _rdtsc();
        fuzzy.push('u');
        auto y = _rdtsc();
        auto l = y - x;
        U_vector.push_back(l*period);
        fuzzy.pop();
    }
    fuzzy.push('u');
    for(int i=0;i<1000;i++){
        auto x = _rdtsc();
        fuzzy.push('g');
        auto y = _rdtsc();
        auto l = y - x;
        G_vector.push_back(l*period);
        fuzzy.pop();
    }
    fuzzy.push('g');

    for(int i=0;i<1000;i++){
        auto x = _rdtsc();
        fuzzy.push('o');
        auto y = _rdtsc();
        auto l = y - x;
        O_vector.push_back(l*period);
        fuzzy.pop();
    }
    fuzzy.push('o');
}

//Calculates the mean of all values in a vector
double mean(std::vector<double> v){
    double sum = 0;
    for(double d : v){
        sum += d;
    }
    return sum/v.size();
}

//Calculates the standard deviation
double stddev(std::vector<double> v, double mean){
    double sum = 0;
    for (double d : v) {
        sum += (d - mean) * (d - mean);
    }
    return sqrt(sum / v.size());
}

