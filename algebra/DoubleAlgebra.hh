#ifndef DOUBLE_ALGEBRA_HH
#define DOUBLE_ALGEBRA_HH

#include "Algebra.hh"
#include <cmath>

class DoubleAlgebra : public Algebra<double> {
public:
    double num(double value) const override {
        return value;
    }
    
    double add(const double& a, const double& b) const override {
        return a + b;
    }
    
    double sub(const double& a, const double& b) const override {
        return a - b;
    }
    
    double mul(const double& a, const double& b) const override {
        return a * b;
    }
    
    double div(const double& a, const double& b) const override {
        return a / b;
    }
    
    double abs(const double& a) const override {
        return std::abs(a);
    }
    
    double bottom() const override {
        return 0.0;  // Use 0.0 as bottom value for numerical computation
    }
    
    bool isEquivalent(const double& a, const double& b) const override {
        return a == b;  // Simple equality for doubles (could add tolerance)
    }
};

#endif