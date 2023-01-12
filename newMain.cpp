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

struct RANet{
    using InitL = std::initializer_list<uint32_t>;

    RANet(InitL arch){
        lr = 0.05;

        assert(arch.size() >= 2);
        W_.reserve(arch.size() -1);

        uint32_t inputs = *(arch.begin());
        assert(inputs > 0);
        for(auto it{arch.begin() +1}; it != arch.end(); ++it){
            uint32_t outputs = *it;
            assert(outputs > 0);
            Matrix& W_Matrix = W_.emplace_back(inputs +1, outputs); // construye implicitamente una Matrix(inputs +1, outputs)
            W_Matrix.fillWithRandoms(-1, 1); // entre el numero maximo de entradas
            inputs = outputs;
        }
    }

    void train(const Matrix& X_train, const Matrix& y_train){

        //crear vector de X
        std::vector<Matrix> salidas;
        std::vector<Matrix> vector_S;

        salidas.push_back(X_train);
        for(auto it{W_.begin()}; it != W_.end(); it++){
            if(it+1 == W_.end())
                salidas.emplace_back(X_train.rows(), it->cols());
            else
                salidas.emplace_back(X_train.rows(), it->cols()+1);
        }
        assert(salidas.size() == (W_.size() + 1));

        for(std::size_t i=0; i<W_.size(); i++){

            auto S = salidas[i] * W_[i];
            vector_S.emplace_back(S);                               
            auto THETA = tanh(S);                          
            if(i+1 >= W_.size())
                salidas[i+1] = THETA;
            else
                salidas[i+1] = add_column_of_ones(THETA);              // add column if output is not last
        }

        // ##
        // backpropagation (con derivadas y demas)
        // ##

        std::vector<Matrix> vector_deltas;

        // capa final

        Matrix& last_W = *(W_.end()-1);
        Matrix last_S = *(salidas.end() - 1);
        Matrix delta_final(last_W.rows(), last_W.cols());
        for(std::size_t r=0; r<last_W.rows(); r++){
            for(std::size_t c=0; c<last_W.cols(); c++){
                std::cout << "LAST S\n";
                std::cout << last_S[0,c];
                delta_final[r,c] = 2 * (tanh(last_S[0,c]) - y_train[0,c]) * (1 - pow(tanh(last_S[0,c]), 2));
                last_W[r,c] = last_W[r,c] - lr * delta_final[r,c] * (*(salidas.end()-2))[0,c];
                
            }
        }
        
        vector_deltas.push_back(delta_final);
        //resto de capas
        auto delta_it = vector_deltas.begin();
        auto salidas_it = salidas.end() - 2;

        for(auto it{W_.end()-2}; it>=W_.begin(); it--, delta_it++, salidas_it--){
            Matrix delta_k(it->rows(), it->cols());
            for(std::size_t r=0; r<it->rows(); r++){
                for(std::size_t c=0; c<it->cols(); c++){
                    double delta = 0;
                    Matrix delta_matrix = *delta_it;
                    for(std::size_t r_2=0; r_2<(it+1)->rows(); r_2++){
                        for(std::size_t c_2=0; c_2<(it+1)->cols(); c_2++){
                            delta += delta_matrix[r_2,c_2] * (*(it+1))[r_2,c_2] * (1 - pow(tanh((*salidas_it)[0,c_2]), 2));
                    
                        }
                    }
                    delta_k[r,c] = delta;
                    (*it)[r,c] = (*it)[r,c] - lr*delta * (*(salidas_it-1))[0,c];
                    
                }
            }
            vector_deltas.push_back(delta_k);
            
        }

        // gradient descent
    }
    

    // NECESITA UN VISTAZO GORDO EL TEMA DE LA X
    Matrix predict(const Matrix& XT){

        //crear vector de X
        std::vector<Matrix> salidas;


        salidas.push_back(XT);
        for(auto it{W_.begin()}; it != W_.end(); it++){
            if(it+1 == W_.end())
                salidas.emplace_back(XT.rows(), it->cols());
            else
                salidas.emplace_back(XT.rows(), it->cols()+1);
        }
        assert(salidas.size() == (W_.size() + 1));

        for(std::size_t i=0; i<W_.size(); i++){

            auto S = salidas[i] * W_[i];                               // ESTA MAL, PILLA SIEMPRE EL X DE FUERA
            auto THETA = tanh(S);                           // CONVIENE APLICAR FUNCION DE ACTIVACION POR CAPA
            if(i+1 >= W_.size())
                salidas[i+1] = THETA;
            else
                salidas[i+1] = add_column_of_ones(THETA);              // add column if output is not last
        }
        // output se compara con Y
        std::cout << "SALIDA ANTES DE SIGN\n";
        std::cout << salidas.back() << '\n';
        return sign(salidas.back());
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
    double lr;


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

    RANet net({2, 2,2, 1});
    net.train(X, Y);
    net.outputWeights();
    net.train(X, Y);
    net.outputWeights();
    net.train(X, Y);
    net.outputWeights();
    for(int i =0; i<50; i++){
        net.train(X,Y);
    }

    net.outputWeights();
    Matrix H = net.predict(X);

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