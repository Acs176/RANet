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
        lr = 0.1;

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
        

        // calcular deltas
        //std::cout << salidas.back().rows() << ", " << salidas.back().cols()<< ", " << derivada_tanh(vector_S.back()).rows() << ", " << derivada_tanh(vector_S.back()).cols() <<  '\n';
        Matrix deltaFinal = ((salidas.back() - y_train) * 2 ).elementProduct(derivada_tanh(vector_S.back()));
        //std::cout << deltaFinal.rows() << ", " << deltaFinal.cols() << '\n';
        vector_deltas.emplace_back(deltaFinal);

        auto it_deltas = vector_deltas.begin();
        auto it_S = vector_S.end() - 2;
        for(auto it{W_.end()-1}; it != W_.begin(); it--){
            Matrix weights_without_ones = removeRow((*it), 0);
            Matrix& delta_siguiente = *it_deltas;
            Matrix current_S = *it_S;
            //std::cout << weights_without_ones.rows() << ", " << weights_without_ones.cols()<< ", " << derivada_tanh(current_S).rows() << ", " << derivada_tanh(current_S).cols() <<  '\n';
            Matrix delta = (delta_siguiente * transpose(weights_without_ones) ).elementProduct(derivada_tanh(current_S));
            vector_deltas.emplace_back(delta);
            it_deltas = vector_deltas.end() - 1;
            it_S--;
        }

        // for(it_deltas = vector_deltas.end() - 1; it_deltas >= vector_deltas.begin(); it_deltas--){
        //     std::cout << "DELTAAA\n";
        //     std::cout << (*it_deltas) << '\n';
        // }
        // actualizar pesos

        it_deltas = vector_deltas.begin();
        auto it_salidas = salidas.end() - 2;
        for(auto it{W_.end()-1}; it >= W_.begin(); it--){
            Matrix& New_Weights = *it;
            Matrix& delta = *it_deltas;
            //std::cout << delta.rows() << ", " << delta.cols()<< ", " << it_salidas->rows() << ", " << it_salidas->cols() <<  '\n';
            New_Weights = New_Weights - ( ( transpose((*it_salidas)) * delta ) * lr );
            it_salidas--;
            it_deltas++;

        }

        
        
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
        //std::cout << "SALIDA ANTES DE SIGN\n";
        //std::cout << salidas.back() << '\n';
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
    std::vector<std::vector<double>> a = readFile("data.csv");
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

    //std::cout << X;
    //std::cout << Y;

    RANet net({9, 9, 3});

    //net.outputWeights();
    //net.train(X, Y);
    // net.train(X, Y);
    // net.outputWeights();
    // net.train(X, Y);
    // net.outputWeights();
    for(int i =0; i<1000; i++){
        net.train(X,Y);
    }

    Matrix H = net.predict(X);
    std::cout << H;
    Matrix Error = H != sign(Y);
    auto score = sum_by_cols(Error);
    std::cout << score << '\n';
    net.outputWeights();
}

int main(){


    test_net();
    
    return 0;
}