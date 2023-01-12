#pragma once

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

template <typename T>
using initL = std::initializer_list<T>;

struct Matrix{

    Matrix(std::size_t rows, std::size_t cols);
    Matrix(initL<initL<double>> l);
    Matrix(const Matrix &m);
    Matrix(std::vector<std::vector<double>> l);

    // nodiscard hace que salte error si hay un objeto creado con una de estas operaciones QUE NO SE USA
    [[nodiscard]] Matrix& operator=(const Matrix &m);
    [[nodiscard]] Matrix operator*(const Matrix &m) const;
    [[nodiscard]] double& operator[](std::size_t const row, std::size_t const col);
    [[nodiscard]] double operator[](std::size_t const row, std::size_t const col) const;
    [[nodiscard]] Matrix operator!=(const Matrix &MR) const;
    friend std::ostream& operator<<(std::ostream& out, Matrix const& M);
    friend Matrix sign(Matrix const& M);
    friend Matrix relu(Matrix const& M);
    friend Matrix tanh(Matrix const& M);
    friend Matrix derivada_tanh(Matrix const& M);
    friend Matrix derivada_error(Matrix const& Y, Matrix const& OUT);
    friend Matrix sum_by_cols(Matrix const& M);
    void fillWithRandoms(double const min, double const max);
    auto cols() const {return w_;}
    auto rows() const {return h_;}
    friend Matrix add_column_of_ones(Matrix const& M);
    std::size_t size() const {return w_*h_;}
    private:
        std::size_t const h_{}, w_{};
        std::unique_ptr<double[]> data_ {new double[h_*w_]{0}}; // el {0} asegura que se inicializan todos a 0
};