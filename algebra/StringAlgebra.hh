#ifndef STRING_ALGEBRA_HH
#define STRING_ALGEBRA_HH

#include "Algebra.hh"
#include <string>
#include <sstream>
#include <utility>

class StringAlgebra : public Algebra<std::pair<std::string, int>> {
private:
    mutable int fVarCounter = 0;  // Counter for generating unique variable names
    
public:
    std::pair<std::string, int> num(double value) const override {
        std::ostringstream oss;
        oss << value;
        return {oss.str(), 100}; // highest priority
    }
    
    std::pair<std::string, int> add(const std::pair<std::string, int>& a, 
                                    const std::pair<std::string, int>& b) const override {
        std::string result = a.first + " + " + b.first;
        return {result, 10}; // lowest priority
    }
    
    std::pair<std::string, int> sub(const std::pair<std::string, int>& a, 
                                    const std::pair<std::string, int>& b) const override {
        std::string left = a.first;
        std::string right = b.second <= 10 ? "(" + b.first + ")" : b.first;
        std::string result = left + " - " + right;
        return {result, 10}; // lowest priority
    }
    
    std::pair<std::string, int> mul(const std::pair<std::string, int>& a, 
                                    const std::pair<std::string, int>& b) const override {
        std::string left = a.second < 50 ? "(" + a.first + ")" : a.first;
        std::string right = b.second < 50 ? "(" + b.first + ")" : b.first;
        std::string result = left + " * " + right;
        return {result, 50}; // medium priority
    }
    
    std::pair<std::string, int> div(const std::pair<std::string, int>& a, 
                                    const std::pair<std::string, int>& b) const override {
        std::string left = a.second < 50 ? "(" + a.first + ")" : a.first;
        std::string right = b.second <= 50 ? "(" + b.first + ")" : b.first;
        std::string result = left + " / " + right;
        return {result, 50}; // medium priority
    }
    
    std::pair<std::string, int> abs(const std::pair<std::string, int>& a) const override {
        std::string result = "abs(" + a.first + ")";
        return {result, 100}; // highest priority (like a function call)
    }
    
    std::pair<std::string, int> bottom() const override {
        std::ostringstream oss;
        oss << "x" << (++fVarCounter);  // Generate unique variable name
        return {oss.str(), 100};  // Variables have highest priority
    }
    
    bool isEquivalent(const std::pair<std::string, int>& a, 
                      const std::pair<std::string, int>& b) const override {
        // For symbolic algebra, we trust TreeAlgebra's fixpoint algorithm
        // and always accept equivalence (could be more sophisticated)
        return true;
    }
};

#endif