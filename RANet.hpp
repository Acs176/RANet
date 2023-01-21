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
    public:
        using InitL = std::initializer_list<uint32_t>;
        RANet(InitL arch);
        Matrix predict(const Matrix& XT);
        void train(const Matrix& X_train, const Matrix& y_train);
        void outputWeights();
        void setWeights(Matrix X, size_t capa);
    private:
        std::vector<Matrix> W_{};
        double lr;
};