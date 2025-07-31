#include <iostream>
#include "../algebra/TreeAlgebra.hh"
#include "../algebra/StringAlgebra.hh"

int main() {
    TreeAlgebra treeAlg;
    StringAlgebra stringAlg;
    
    // Test 1: 2 + 3 * 4 (should not have parentheses around 3 * 4)
    auto expr1 = treeAlg.add(
        treeAlg.num(2.0),
        treeAlg.mul(treeAlg.num(3.0), treeAlg.num(4.0))
    );
    auto result1 = (*expr1)(stringAlg);
    std::cout << "2 + 3 * 4 = " << result1.first << std::endl;
    
    // Test 2: (2 + 3) * 4 (should have parentheses around 2 + 3)
    auto expr2 = treeAlg.mul(
        treeAlg.add(treeAlg.num(2.0), treeAlg.num(3.0)),
        treeAlg.num(4.0)
    );
    auto result2 = (*expr2)(stringAlg);
    std::cout << "(2 + 3) * 4 = " << result2.first << std::endl;
    
    // Test 3: 10 - 5 - 2 (right associativity test)
    auto expr3 = treeAlg.sub(
        treeAlg.num(10.0),
        treeAlg.sub(treeAlg.num(5.0), treeAlg.num(2.0))
    );
    auto result3 = (*expr3)(stringAlg);
    std::cout << "10 - (5 - 2) = " << result3.first << std::endl;
    
    // Test 4: 20 / 4 / 2 (right associativity test)
    auto expr4 = treeAlg.div(
        treeAlg.num(20.0),
        treeAlg.div(treeAlg.num(4.0), treeAlg.num(2.0))
    );
    auto result4 = (*expr4)(stringAlg);
    std::cout << "20 / (4 / 2) = " << result4.first << std::endl;
    
    // Test 5: Complex expression
    auto expr5 = treeAlg.add(
        treeAlg.mul(treeAlg.num(2.0), treeAlg.num(3.0)),
        treeAlg.div(
            treeAlg.add(treeAlg.num(8.0), treeAlg.num(2.0)),
            treeAlg.num(5.0)
        )
    );
    auto result5 = (*expr5)(stringAlg);
    std::cout << "2 * 3 + (8 + 2) / 5 = " << result5.first << std::endl;
    
    return 0;
}