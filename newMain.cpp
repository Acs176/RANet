#include <iostream>
#include <cstdint>
#include <memory>
#include <initializer_list>
#include <span>
#include <fstream>
#include <vector>
#include <sstream>
#include <random>
#include <time.h>
#include <list>
#include <cassert>
#include <random>
#include <cmath>
#include "Matrix.hpp"
#include "RANet.hpp"
#include <algorithm>
using InitL = std::initializer_list<uint32_t>;


struct solution{
    RANet net;
    std::size_t errors;

};

int get_random_int(int const min, int const max){
    static std::random_device dev;
    static std::mt19937 gen{dev()};     // si son static, solo se inicializan la primera vez que se llama a la funcion

    return std::uniform_int_distribution<int>{min,max}(gen);

}

double get_random_double(double const min, double const max){
    static std::random_device dev;
    static std::mt19937 gen{dev()};     // si son static, solo se inicializan la primera vez que se llama a la funcion

    return std::uniform_real_distribution<double>{min,max}(gen);

}

RANet trainGenetic(const Matrix& X, const Matrix& Y, std::size_t iterations, InitL net_parameters){

    int total_nets    = 1000;
    int cantidad_best = 100;

    std::vector<solution> solutions;
    std::vector<solution> best_solutions;
    for(std::size_t i=0; i<total_nets; i++){
        RANet new_net(net_parameters);
        solution s{new_net, X.rows()};
        solutions.push_back(s);
    }

    for(std::size_t i=0; i<iterations; i++){

        // calcular fitness 

        for(auto& s : solutions){
            auto H = s.net.predict(X);
            Matrix Error = H != sign(Y);
            auto score = sum_by_cols(Error);
            auto score_sum = sum_by_rows(score)[0,0];

            s.errors = score_sum;

        }


        // sort
        std::sort(solutions.begin(), solutions.end(),
        [](const auto& left, const auto& right){
            return left.errors < right.errors;
        }
        );

        if( i+1 >= iterations){
            return solutions[0].net;
        }

        // pillar mejores

        std::copy(
            solutions.begin(),
            solutions.begin() + cantidad_best,
            std::back_inserter(best_solutions)
        );
        solutions.clear();

        std::for_each(best_solutions.begin(), best_solutions.end(), [&](auto& s){
            for(auto& w : s.net.W_){
                w = w * get_random_double(0.95, 1.05);
            }
        });

        // volver a generar
        for(int new_i=0; new_i<total_nets; new_i++){
            solutions.push_back(solution{
                best_solutions[get_random_int(0, cantidad_best-1)].net,
                X.rows()
            });
        }
        best_solutions.clear();
    }

}

std::vector<std::vector<double>> readFile(std::string filename){
    std::vector<std::vector<double>> content;
    std::vector<double> row;
    std::string line, word;
    std::fstream file (filename, std::ios::in);

    if(file.is_open()){
        //std::cout<<"Hoal";
        while(getline(file, line)){
            row.clear();
            std::stringstream str(line);
            //row.push_back(1);
            while(getline(str, word, ';')){
                //sampleElement++;
                //std::cout << word << ' ';
                if(!word.empty()){
                    try{
                        double value = std::stod(word);
                        row.push_back(value);
                    }
                    catch(std::invalid_argument e){
                        std::cerr << "Exception!!! :: " << e.what() << '\n';
                    }

                }
            }

            content.push_back(row);
        }
    }
    file.close();
    std::cout << content.size() << "  " << content[0].size() << std::endl;
    //for(std::vector<double> row : content){
    //    for(double value : row){
    //        std::cout << value << " ";
    //    }
    //    std::cout << '\n';
    //}
    return content;
}

void test_net(){
    std::vector<std::vector<double>> a = readFile("reducedData.csv");
    Matrix X(a.size(), a[0].size()-3);
    Matrix Y(a.size(), 3);
    std::vector<double> max_X;
    std::vector<double> min_X;
    max_X.resize(a[0].size()-3);
    min_X.resize(a[0].size()-3);

    for(int i = 0; i<max_X.size(); i++){
        max_X[i] = -1;
        min_X[i] = 100000;
    }
    for(std::size_t i = 0; i<a.size(); i++){
        for(std::size_t j = 0; j<a[i].size(); j++){
            if(j<a[0].size()-3){
                X[i,j] = a[i][j];
                if(X[i,j] > max_X[j]){
                    max_X[j] = X[i,j];
                }
                if(X[i,j] < min_X[j]){
                    min_X[j] = X[i,j];
                }
            }
            else{
                Y[i,a[i].size()-j-1] = a[i][j];
            }
        }
    }

    for(std::size_t i = 0; i<X.rows(); i++){
        for(std::size_t j = 1; j<X.cols(); j++){
            X[i,j] = (X[i,j] - min_X[j])/(max_X[j]-min_X[j]);
        }
    }


    /// XOR
    // Matrix X {{1, -1,-1,-1},
    //           {1, -1,-1,1},
    //           {1, -1,1,-1},
    //           {1, -1,1,1},
    //           {1, 1,-1,-1},
    //           {1, 1,-1,1},
    //           {1, 1,1,-1},
    //           {1, 1,1,1}
    //           };

    // Matrix Y {{-1},
    //           {1},
    //           {1},
    //           {-1},
    //           {1},
    //           {-1},
    //           {-1},
    //           {-1}
    // };

    RANet red_tocha = trainGenetic(X,Y,5, {9,9,3});
    //net.outputWeights();
    // net.train(X, Y);
    // net.outputWeights();
    // net.train(X, Y);
    red_tocha.outputWeights();

    Matrix H = red_tocha.predict(X);
    std::cout << H;
    Matrix Error = H != sign(Y);
    auto score = sum_by_cols(Error);
    std::cout << score << '\n';
    //red_tocha.outputWeights();
}

int main(){

    test_net();
    
    return 0;
}