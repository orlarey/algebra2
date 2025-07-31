#include <iostream>
#include "../algebra/TreeAlgebra.hh"
#include "../algebra/DoubleAlgebra.hh"

int main() {
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    
    // Test using generic methods
    std::cout << "=== Testing generic unary/binary methods ===" << std::endl;
    
    // Build expression using generic methods
    auto a = treeAlg.num(10.0);
    auto b = treeAlg.num(5.0);
    
    // Test binary generic
    auto sum = treeAlg.binary(BinaryOp::Add, a, b);
    std::cout << "binary(Add, 10, 5) = " << (*sum)(doubleAlg) << std::endl;
    
    // Test unary generic
    auto negFive = treeAlg.num(-5.0);
    auto absNegFive = treeAlg.unary(UnaryOp::Abs, negFive);
    std::cout << "unary(Abs, -5) = " << (*absNegFive)(doubleAlg) << std::endl;
    
    // Complex expression using generic methods
    auto expr = treeAlg.binary(BinaryOp::Mul,
        treeAlg.binary(BinaryOp::Sub, a, b),
        treeAlg.unary(UnaryOp::Abs, treeAlg.num(-2.0))
    );
    std::cout << "(10 - 5) * abs(-2) = " << (*expr)(doubleAlg) << std::endl;
    
    return 0;
}