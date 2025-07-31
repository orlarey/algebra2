#include <iostream>
#include "../algebra/DoubleAlgebra.hh"
#include "../algebra/TreeAlgebra.hh"
#include "../algebra/StringAlgebra.hh"

int main() {
    DoubleAlgebra doubleAlg;
    TreeAlgebra treeAlg;
    StringAlgebra stringAlg;
    
    // Test 1: Simple abs
    std::cout << "=== Test 1: abs(-5) ===" << std::endl;
    double val1 = doubleAlg.abs(doubleAlg.num(-5.0));
    std::cout << "DoubleAlgebra: abs(-5) = " << val1 << std::endl;
    
    auto tree1 = treeAlg.abs(treeAlg.num(-5.0));
    double treeVal1 = (*tree1)(doubleAlg);
    std::cout << "TreeAlgebra evaluated: " << treeVal1 << std::endl;
    
    auto str1 = (*tree1)(stringAlg);
    std::cout << "StringAlgebra: " << str1.first << std::endl;
    
    // Test 2: abs of expression
    std::cout << "\n=== Test 2: abs(3 - 8) ===" << std::endl;
    auto expr = treeAlg.sub(treeAlg.num(3.0), treeAlg.num(8.0));
    auto absExpr = treeAlg.abs(expr);
    
    double treeVal2 = (*absExpr)(doubleAlg);
    std::cout << "TreeAlgebra evaluated: " << treeVal2 << std::endl;
    
    auto str2 = (*absExpr)(stringAlg);
    std::cout << "StringAlgebra: " << str2.first << std::endl;
    
    // Test 3: Complex expression with abs
    std::cout << "\n=== Test 3: abs(2 * -3) + 4 ===" << std::endl;
    auto complexExpr = treeAlg.add(
        treeAlg.abs(
            treeAlg.mul(treeAlg.num(2.0), treeAlg.num(-3.0))
        ),
        treeAlg.num(4.0)
    );
    
    double treeVal3 = (*complexExpr)(doubleAlg);
    std::cout << "TreeAlgebra evaluated: " << treeVal3 << std::endl;
    
    auto str3 = (*complexExpr)(stringAlg);
    std::cout << "StringAlgebra: " << str3.first << std::endl;
    
    return 0;
}