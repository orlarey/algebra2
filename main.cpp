#include <iostream>
#include "algebra/TreeAlgebra.hh"
#include "algebra/DoubleAlgebra.hh"
#include "algebra/StringAlgebra.hh"

int main() {
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    StringAlgebra stringAlg;
    
    std::cout << "=== Synthetic Test: Complex Expression with Hash-Consing ===" << std::endl;
    
    // Build a complex expression: abs(2 * (5 + 3)) / (8 - 1)
    std::cout << "\nBuilding first complex expression..." << std::endl;
    auto expr1 = treeAlg.div(
        treeAlg.abs(
            treeAlg.mul(
                treeAlg.num(2.0),
                treeAlg.add(treeAlg.num(5.0), treeAlg.num(3.0))
            )
        ),
        treeAlg.sub(treeAlg.num(8.0), treeAlg.num(1.0))
    );
    
    std::cout << "First expression pointer: " << expr1.get() << std::endl;
    
    // Build the EXACT same expression again
    std::cout << "\nBuilding second identical expression..." << std::endl;
    auto expr2 = treeAlg.div(
        treeAlg.abs(
            treeAlg.mul(
                treeAlg.num(2.0),
                treeAlg.add(treeAlg.num(5.0), treeAlg.num(3.0))
            )
        ),
        treeAlg.sub(treeAlg.num(8.0), treeAlg.num(1.0))
    );
    
    std::cout << "Second expression pointer: " << expr2.get() << std::endl;
    
    // Verify hash-consing works
    std::cout << "\n=== Hash-Consing Verification ===" << std::endl;
    std::cout << "Pointers are identical: " << (expr1.get() == expr2.get() ? "YES ✓" : "NO ✗") << std::endl;
    
    // Evaluate with DoubleAlgebra
    std::cout << "\n=== Numerical Evaluation ===" << std::endl;
    double result1 = (*expr1)(doubleAlg);
    double result2 = (*expr2)(doubleAlg);
    std::cout << "expr1 result: " << result1 << std::endl;
    std::cout << "expr2 result: " << result2 << std::endl;
    std::cout << "Results are equal: " << (result1 == result2 ? "YES ✓" : "NO ✗") << std::endl;
    
    // Generate string representation with StringAlgebra
    std::cout << "\n=== String Representation ===" << std::endl;
    auto str1 = (*expr1)(stringAlg);
    auto str2 = (*expr2)(stringAlg);
    std::cout << "expr1 string: " << str1.first << std::endl;
    std::cout << "expr2 string: " << str2.first << std::endl;
    std::cout << "String representations are equal: " << (str1.first == str2.first ? "YES ✓" : "NO ✗") << std::endl;
    
    // Manual calculation verification
    std::cout << "\n=== Manual Verification ===" << std::endl;
    std::cout << "Expected: abs(2 * (5 + 3)) / (8 - 1) = abs(2 * 8) / 7 = abs(16) / 7 = 16 / 7 ≈ " << (16.0/7.0) << std::endl;
    std::cout << "Computed: " << result1 << std::endl;
    std::cout << "Match: " << (std::abs(result1 - 16.0/7.0) < 1e-10 ? "YES ✓" : "NO ✗") << std::endl;
    
    return 0;
}