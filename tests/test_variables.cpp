#include "algebra/TreeAlgebra.hh"
#include "algebra/DoubleAlgebra.hh"
#include "algebra/StringAlgebra.hh"
#include <iostream>
#include <cassert>

void test_simple_variable() {
    std::cout << "Testing simple variable..." << std::endl;
    
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    
    // Create a variable var(0)
    auto var0 = treeAlg.var(0);
    
    // Define var(0) = 42
    var0->setDefinition(treeAlg.num(42.0));
    
    // Evaluate the variable
    double result = (*var0)(doubleAlg);
    assert(result == 42.0);
    
    std::cout << "Simple variable test passed!" << std::endl;
}

void test_recursive_definition() {
    std::cout << "Testing recursive definition..." << std::endl;
    
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    
    // Create var(0) that refers to itself: var(0) = var(0) + 1
    auto var0 = treeAlg.var(0);
    var0->setDefinition(treeAlg.add(var0, treeAlg.num(1.0)));
    
    // This should create an infinite loop if evaluated
    // For now, we'll just verify that the definition is set correctly
    assert(var0->getDefinition() != nullptr);
    
    std::cout << "Recursive definition test passed!" << std::endl;
}

void test_variable_in_expression() {
    std::cout << "Testing variable in expression..." << std::endl;
    
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    StringAlgebra stringAlg;
    
    // Create var(0) = 5
    auto var0 = treeAlg.var(0);
    var0->setDefinition(treeAlg.num(5.0));
    
    // Create expression: 2 * var(0) + 3
    auto expr = treeAlg.add(
        treeAlg.mul(treeAlg.num(2.0), var0),
        treeAlg.num(3.0)
    );
    
    // Evaluate numerically: 2 * 5 + 3 = 13
    double result = (*expr)(doubleAlg);
    assert(result == 13.0);
    
    // Get string representation
    auto str = (*expr)(stringAlg);
    std::cout << "Expression with variable: " << str.first << " = " << result << std::endl;
    
    std::cout << "Variable in expression test passed!" << std::endl;
}

void test_hash_consing_with_variables() {
    std::cout << "Testing hash-consing with variables..." << std::endl;
    
    TreeAlgebra treeAlg;
    
    // Create two variables with the same index
    auto var0_a = treeAlg.var(0);
    auto var0_b = treeAlg.var(0);
    
    // They should be the same object due to hash-consing
    assert(var0_a.get() == var0_b.get());
    
    // Create variables with different indices
    auto var1 = treeAlg.var(1);
    assert(var0_a.get() != var1.get());
    
    std::cout << "Hash-consing with variables test passed!" << std::endl;
}

void test_multiple_variables() {
    std::cout << "Testing multiple variables..." << std::endl;
    
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    
    // Create var(0) = 10 and var(1) = 20
    auto var0 = treeAlg.var(0);
    auto var1 = treeAlg.var(1);
    
    var0->setDefinition(treeAlg.num(10.0));
    var1->setDefinition(treeAlg.num(20.0));
    
    // Create expression: var(0) + var(1)
    auto expr = treeAlg.add(var0, var1);
    
    // Evaluate: 10 + 20 = 30
    double result = (*expr)(doubleAlg);
    assert(result == 30.0);
    
    // Change var(0)'s definition
    var0->setDefinition(treeAlg.num(15.0));
    
    // Re-evaluate: 15 + 20 = 35
    result = (*expr)(doubleAlg);
    assert(result == 35.0);
    
    std::cout << "Multiple variables test passed!" << std::endl;
}

int main() {
    test_simple_variable();
    test_recursive_definition();
    test_variable_in_expression();
    test_hash_consing_with_variables();
    test_multiple_variables();
    
    std::cout << "\nAll variable tests passed!" << std::endl;
    return 0;
}