#ifndef PRIORITY_ALGEBRA_HH
#define PRIORITY_ALGEBRA_HH

#include "Algebra.hh"

class PriorityAlgebra : public Algebra<int> {
public:
    // num has highest priority (no parentheses needed)
    int num(double value) const override {
        return 100;
    }
    
    // mul and div have medium priority
    int mul(const int& a, const int& b) const override {
        return 50;
    }
    
    int div(const int& a, const int& b) const override {
        return 50;
    }
    
    // add and sub have lowest priority
    int add(const int& a, const int& b) const override {
        return 10;
    }
    
    int sub(const int& a, const int& b) const override {
        return 10;
    }
    
    // abs has highest priority (like num)
    int abs(const int& a) const override {
        return 100;
    }
};

#endif