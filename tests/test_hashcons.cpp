#include <iostream>
#include "../algebra/TreeAlgebra.hh"
#include "../algebra/DoubleAlgebra.hh"

int main() {
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    
    std::cout << "=== Testing Hash-Consing ===" << std::endl;
    
    // Test 1: Same number values should share the same Tree
    auto num1a = treeAlg.num(5.0);
    auto num1b = treeAlg.num(5.0);
    
    std::cout << "num(5.0) first call:  " << num1a.get() << std::endl;
    std::cout << "num(5.0) second call: " << num1b.get() << std::endl;
    std::cout << "Same pointer? " << (num1a.get() == num1b.get() ? "YES" : "NO") << std::endl;
    
    // Test 2: Different numbers should have different Trees
    auto num2 = treeAlg.num(3.0);
    std::cout << "num(3.0): " << num2.get() << std::endl;
    std::cout << "Different from num(5.0)? " << (num1a.get() != num2.get() ? "YES" : "NO") << std::endl;
    
    // Test 3: Same binary operations should share the same Tree
    auto add1a = treeAlg.add(num1a, num2);  // 5 + 3
    auto add1b = treeAlg.add(num1b, num2);  // 5 + 3 (same operation)
    
    std::cout << "\nadd(5,3) first call:  " << add1a.get() << std::endl;
    std::cout << "add(5,3) second call: " << add1b.get() << std::endl;
    std::cout << "Same pointer? " << (add1a.get() == add1b.get() ? "YES" : "NO") << std::endl;
    
    // Test 4: Different operations should have different Trees
    auto add2 = treeAlg.add(num2, num1a);  // 3 + 5 (different order)
    std::cout << "add(3,5): " << add2.get() << std::endl;
    std::cout << "Different from add(5,3)? " << (add1a.get() != add2.get() ? "YES" : "NO") << std::endl;
    
    // Test 5: Complex expressions
    auto expr1a = treeAlg.mul(add1a, num2);  // (5 + 3) * 3
    auto expr1b = treeAlg.mul(add1b, num2);  // (5 + 3) * 3 (same)
    
    std::cout << "\nComplex expr first:  " << expr1a.get() << std::endl;
    std::cout << "Complex expr second: " << expr1b.get() << std::endl;
    std::cout << "Same pointer? " << (expr1a.get() == expr1b.get() ? "YES" : "NO") << std::endl;
    
    // Test 6: Unary operations
    auto abs1a = treeAlg.abs(num1a);
    auto abs1b = treeAlg.abs(num1b);
    
    std::cout << "\nabs(5) first:  " << abs1a.get() << std::endl;
    std::cout << "abs(5) second: " << abs1b.get() << std::endl;
    std::cout << "Same pointer? " << (abs1a.get() == abs1b.get() ? "YES" : "NO") << std::endl;
    
    // Verify that evaluation still works correctly
    std::cout << "\n=== Verification that evaluation still works ===" << std::endl;
    std::cout << "expr1a evaluation: " << (*expr1a)(doubleAlg) << std::endl;
    std::cout << "expr1b evaluation: " << (*expr1b)(doubleAlg) << std::endl;
    
    return 0;
}