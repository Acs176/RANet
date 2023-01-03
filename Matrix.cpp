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
#include "Matrix.hpp"
#include <random>

template <typename T>
using initL = std::initializer_list<T>;


double get_random(double const min, double const max){
    static std::random_device dev;
    static std::mt19937 gen{dev()};     // si son static, solo se inicializan la primera vez que se llama a la funcion

    return std::uniform_real_distribution<double>{min,max}(gen);

}

//////////////////////////////////////
Matrix::Matrix(std::size_t rows, std::size_t cols)
    : h_{rows}, w_{cols}    
{
    if(rows == 0 || cols == 0)  throw std::out_of_range("Matrix:Matrix(rows,cols) - Trying to create a matrix with 0 rows or cols");

}
//////////////////////////////////////

Matrix::Matrix(initL<initL<double>> l)
    : Matrix( l.size()
            , (l.size()>0) ? l.begin()->size() : 0
            )

{
    int i = 0;
    for(auto& row : l){
        if(row.size() != w_)    throw std::out_of_range("Matrix : Matrix - Trying to create a Matrix with varying size of rows");
        for(auto& item : row){
            data_[i] = item;
            i++;
        }
    }
}
//////////////////////////////////////
Matrix::Matrix(const Matrix &m)
    : Matrix( m.h_
            , m.w_
            )

{
    // Recibe (source, donde termina lo que copio, a donde va)
    std::copy(m.data_.get(), m.data_.get() + m.size(), this->data_.get());

}
//////////////////////////////////////

Matrix::Matrix(std::vector<std::vector<double>> l)
    : Matrix( l.size()
        , l.begin()->size()
        )
{
    int i = 0;
    for(auto& row : l){
        if(row.size() != w_){
            std::cout << "ERROR - row.size() " << row.size() << " and w_ " << w_ << std::endl;
            //throw std::out_of_range("Matrix : Matrix - Trying to create a Matrix with varying size of rows");
        }                
        for(auto& item : row){
            data_[i] = item;
            i++;
        }
    }
}

//////////////////////////////////////
Matrix& Matrix::operator=(const Matrix &m){
    if(h_ == m.h_ && w_ == m.w_){
        for(std::size_t i=0; i < h_*w_; i++){
            data_[i] = m.data_[i];
        }
    }

    return *this;
}
//////////////////////////////////////
double& Matrix::operator[](std::size_t const row, std::size_t const col){
    return data_[row*w_ + col];
}
//////////////////////////////////////
double Matrix::operator[](std::size_t const row, std::size_t const col) const{
    return data_[row*w_ + col];
}
//////////////////////////////////////
Matrix Matrix::operator!=(Matrix const& MR) const{
    assert(MR.rows() == rows());
    assert(MR.cols() == cols());
    
    auto& ML = *this;
    Matrix Res(rows(), cols());

    for(std::size_t r=0; r<rows(); r++){
        for(std::size_t c=0; c<cols(); c++){
            if(ML[r,c] != MR[r,c])  {Res[r,c] = 1.0;}
            else                    {Res[r,c] = 0.0;}
        }
    }

    return Res;


}
//////////////////////////////////////
Matrix sum_by_cols(Matrix const& MR){
    Matrix Res(1, MR.cols());

    for(std::size_t r=0; r<MR.rows(); r++){
        for(std::size_t c=0; c<MR.cols(); c++){
            Res[0,c] += MR[r,c];
        }
        
    }
    return Res;
}
//////////////////////////////////////
void Matrix::fillWithRandoms(const double min, const double max){
    auto Matrix = *this;
    for(std::size_t row=0; row<rows(); row++){
        for(std::size_t col=0; col<cols(); col++){
            double random = get_random(min,max);
            std::cout << random << std::endl;
            data_[row*w_ + col] = random; 
        }
    }
}
//////////////////////////////////////

std::ostream& operator<<(std::ostream& out, Matrix const& M){
    out << "---------- MATRIX -----------\n";
    for(std::size_t r{}; r<M.h_; r++){
        for(std::size_t c{}; c<M.w_; c++){
            out << M[r,c] << ' ';
        }
        out << '\n';
    }
    return out;
}
//////////////////////////////////////
Matrix Matrix::operator*(const Matrix &MR) const{
    // MR = Matrix Right
    // columnas de la primera matriz == filas de la segunda
    if(cols() != MR.rows()) throw std::out_of_range("[Matrix::operator*] Cannot multiply because cols1 != rows2 ");
    Matrix Res(rows(), MR.cols());

    // 2x3      3x1     2x1
    //
    // 1 1 1    1 0     6  3
    // 2 2 2    2 1    12 6   
    //          3 2
    auto& ML = *this; // Matrix Left
    for(std::size_t r=0; r < ML.rows(); r++){
        for(std::size_t c=0; c < MR.cols(); c++){
            //Res[r,c]=0;
            for(std::size_t i=0; i < cols(); i++){
                Res[r,c] += ML[r,i] * MR[i,c]; 
            }

        }
    }

    return Res;
}
//////////////////////////////////////
Matrix sign(Matrix const& M){
    Matrix Res(M.rows(), M.cols());

    for(std::size_t r=0; r<M.rows(); r++){
        for(std::size_t c=0; c<M.cols(); c++){

            if(M[r,c] >=0)  {Res[r,c] =  1.0;}
            else            {Res[r,c] = -1.0;}
        }
    }

    return Res;
}