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

#include "Matrix.hpp"

struct RANet{
    using InitL = std::initializer_list<uint32_t>;

    RANet(InitL arch){
        assert(arch.size() >= 2);
        W_.reserve(arch.size() -1);

        uint32_t inputs = *(arch.begin());
        assert(inputs > 0);
        for(auto it{arch.begin() +1}; it != arch.end(); ++it){
            uint32_t outputs = *it;
            assert(outputs > 0);
            Matrix& W_Matrix = W_.emplace_back(inputs +1, outputs); // construye implicitamente una Matrix(inputs +1, outputs)
            W_Matrix.fillWithRandoms(-inputs, inputs); // entre el numero maximo de entradas
            inputs = outputs;
        }
    }

    void train(const Matrix& X_train, const Matrix& y_train){
        // forwardfeeding
        // ##
        // backpropagation (con derivadas y demas)
        // ##
        // gradient descent
    }

    Matrix predict(const Matrix& XT, const Matrix& YT){
        Matrix X = XT;
        Matrix LastOutput(YT.rows(), YT.cols());
        for(auto it{W_.begin()}; it != W_.end(); it++){
            auto S = X*(*it);                               // ESTA MAL, PILLA SIEMPRE EL X DE FUERA
            auto THETA = relu(S);                           // CONVIENE APLICAR FUNCION DE ACTIVACION POR CAPA
            auto X = add_column_of_ones(THETA);              // add column if output is not last
            if(it+1 == W_.end()){
                LastOutput = THETA;
                std::cout << "Last output aqui\n";
            }
                
            std::cout << "X OUTPUT\n";
            std::cout << X;    
        }
        // output se compara con Y
        std::cout << LastOutput << '\n';
        return LastOutput;
    }
    void outputWeights(){
        std::cout << "WEIGHTS IN THE NEURAL NET\n:";
        std::cout << "#######################\n";
        for(auto it = W_.begin(); it != W_.end(); ++it){
            std::cout << *it << '\n';
            std::cout << "#######################\n";
        }
    }
private:
    std::vector<Matrix> W_{};


};

void test_net(){
    Matrix X {          // lleva implicita la columna de unos para w0
        {1, -1, -1},
        {1,  1, -1},
        {1, -1,  1},
        {1,  1,  1}
    };
    Matrix Y {
        {-1},
        {1},
        {1},
        {-1}
    };

    RANet net({2,2,1});
    net.outputWeights();
    Matrix H = net.predict(X, Y);

    Matrix Error = H != Y;
    auto score = sum_by_cols(Error);
    std::cout << score << '\n';
}

int main(){ 
    //RANet net({2,2,1});
    //net.outputWeights();
    test_net();
    
    return 0;
}