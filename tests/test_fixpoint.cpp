#include "algebra/TreeAlgebra.hh"
#include "algebra/DoubleAlgebra.hh"
#include "algebra/StringAlgebra.hh"
#include <iostream>
#include <cassert>

void test_simple_eval() {
    std::cout << "Testing simple eval method..." << std::endl;
    
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    
    // Create a simple expression: 2 + 3
    auto expr = treeAlg.add(treeAlg.num(2.0), treeAlg.num(3.0));
    
    // Test new eval method
    double result = treeAlg.eval(expr, doubleAlg);
    assert(result == 5.0);
    
    std::cout << "Simple eval test passed!" << std::endl;
}

void test_simple_variable_eval() {
    std::cout << "Testing simple variable with eval..." << std::endl;
    
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    
    // Create a variable var(0) = 42
    auto var0 = treeAlg.var(0);
    var0->setDefinition(treeAlg.num(42.0));
    
    // Test eval method with variable
    double result = treeAlg.eval(var0, doubleAlg);
    std::cout << "Result: " << result << " (expected 42.0)" << std::endl;
    
    // Also test the old method for comparison
    double oldResult = (*var0)(doubleAlg);
    std::cout << "Old method result: " << oldResult << std::endl;
    
    assert(result == 42.0);
    
    std::cout << "Simple variable eval test passed!" << std::endl;
}

void test_non_recursive_variable() {
    std::cout << "Testing non-recursive variable definition..." << std::endl;
    
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    StringAlgebra stringAlg;
    
    // Create var(0) = 2 * 3 + 1 = 7
    auto var0 = treeAlg.var(0);
    var0->setDefinition(
        treeAlg.add(
            treeAlg.mul(treeAlg.num(2.0), treeAlg.num(3.0)),
            treeAlg.num(1.0)
        )
    );
    
    // Test with DoubleAlgebra
    double result = treeAlg.eval(var0, doubleAlg);
    assert(result == 7.0);
    
    // Test with StringAlgebra
    auto strResult = treeAlg.eval(var0, stringAlg);
    std::cout << "String representation: " << strResult.first << std::endl;
    
    std::cout << "Non-recursive variable test passed!" << std::endl;
}

void test_simple_recursive_variable() {
    std::cout << "Testing simple recursive variable..." << std::endl;
    
    TreeAlgebra treeAlg;
    StringAlgebra stringAlg;
    
    // Create var(0) = var(0) + 1
    auto var0 = treeAlg.var(0);
    var0->setDefinition(treeAlg.add(var0, treeAlg.num(1.0)));
    
    // Test with StringAlgebra (should produce symbolic equation)
    auto strResult = treeAlg.eval(var0, stringAlg);
    std::cout << "Recursive equation: x = " << strResult.first << std::endl;
    
    std::cout << "Simple recursive variable test passed!" << std::endl;
}

void test_semantic_non_recursive_variables() {
    std::cout << "Testing semantic evaluation with non-recursive variables..." << std::endl;
    
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    
    // Create non-recursive variables:
    // var(0) = 10.0
    // var(1) = var(0) + 5.0 = 15.0
    // expr = var(1) * 2.0 = 30.0
    auto var0 = treeAlg.var(0);
    auto var1 = treeAlg.var(1);
    
    var0->setDefinition(treeAlg.num(10.0));
    var1->setDefinition(treeAlg.add(var0, treeAlg.num(5.0)));
    
    auto expr = treeAlg.mul(var1, treeAlg.num(2.0));
    
    // Test semantic evaluation
    double result0 = treeAlg.eval(var0, doubleAlg);
    double result1 = treeAlg.eval(var1, doubleAlg);
    double resultExpr = treeAlg.eval(expr, doubleAlg);
    
    std::cout << "var(0) = " << result0 << " (expected 10.0)" << std::endl;
    std::cout << "var(1) = " << result1 << " (expected 15.0)" << std::endl;
    std::cout << "expr = " << resultExpr << " (expected 30.0)" << std::endl;
    
    assert(result0 == 10.0);
    assert(result1 == 15.0);
    assert(resultExpr == 30.0);
    
    std::cout << "Semantic non-recursive variables test passed!" << std::endl;
}

void test_semantic_complex_variables() {
    std::cout << "Testing semantic evaluation with complex non-recursive variables..." << std::endl;
    
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    
    // Create a chain of variables:
    // var(0) = 3.0
    // var(1) = var(0) * 2.0 = 6.0
    // var(2) = var(1) + var(0) = 9.0
    // expr = (var(2) - var(1)) * var(0) = (9-6)*3 = 9.0
    auto var0 = treeAlg.var(0);
    auto var1 = treeAlg.var(1);
    auto var2 = treeAlg.var(2);
    
    var0->setDefinition(treeAlg.num(3.0));
    var1->setDefinition(treeAlg.mul(var0, treeAlg.num(2.0)));
    var2->setDefinition(treeAlg.add(var1, var0));
    
    auto expr = treeAlg.mul(
        treeAlg.sub(var2, var1),
        var0
    );
    
    double result = treeAlg.eval(expr, doubleAlg);
    std::cout << "Complex expression = " << result << " (expected 9.0)" << std::endl;
    
    assert(result == 9.0);
    
    std::cout << "Semantic complex variables test passed!" << std::endl;
}

void test_mutual_recursion() {
    std::cout << "Testing mutually recursive variables..." << std::endl;
    
    TreeAlgebra treeAlg;
    StringAlgebra stringAlg;
    
    // Create mutually recursive variables:
    // var(0) = var(1) + 1
    // var(1) = var(0) * 2
    auto var0 = treeAlg.var(0);
    auto var1 = treeAlg.var(1);
    
    var0->setDefinition(treeAlg.add(var1, treeAlg.num(1.0)));
    var1->setDefinition(treeAlg.mul(var0, treeAlg.num(2.0)));
    
    // Test with StringAlgebra
    auto str0 = treeAlg.eval(var0, stringAlg);
    auto str1 = treeAlg.eval(var1, stringAlg);
    
    std::cout << "var(0): x0 = " << str0.first << std::endl;
    std::cout << "var(1): x1 = " << str1.first << std::endl;
    
    std::cout << "Mutual recursion test passed!" << std::endl;
}

void test_alpha_equivalence() {
    std::cout << "Testing alpha-equivalence..." << std::endl;
    
    TreeAlgebra treeAlg;
    
    // Test 1: Identity - same tree should be alpha-equivalent to itself
    auto tree1 = treeAlg.add(treeAlg.num(2.0), treeAlg.num(3.0));
    assert(treeAlg.alphaEquivalent(tree1, tree1));
    std::cout << "✓ Identity test passed" << std::endl;
    
    // Test 2: Structural equivalence - different instances, same structure
    auto tree2a = treeAlg.add(treeAlg.num(2.0), treeAlg.num(3.0));
    auto tree2b = treeAlg.add(treeAlg.num(2.0), treeAlg.num(3.0));
    assert(treeAlg.alphaEquivalent(tree2a, tree2b));
    std::cout << "✓ Structural equivalence test passed" << std::endl;
    
    // Test 3: Different structures should not be equivalent
    auto tree3a = treeAlg.add(treeAlg.num(2.0), treeAlg.num(3.0));
    auto tree3b = treeAlg.mul(treeAlg.num(2.0), treeAlg.num(3.0));
    assert(!treeAlg.alphaEquivalent(tree3a, tree3b));
    std::cout << "✓ Different structures test passed" << std::endl;
    
    // Test 4: Simple variable equivalence - same variable references
    auto var1 = treeAlg.var(1);
    auto var2 = treeAlg.var(2);
    
    var1->setDefinition(treeAlg.num(42.0));
    var2->setDefinition(treeAlg.num(42.0));
    
    auto expr4a = treeAlg.add(var1, treeAlg.num(1.0));
    auto expr4b = treeAlg.add(var2, treeAlg.num(1.0));
    
    assert(treeAlg.alphaEquivalent(expr4a, expr4b));
    std::cout << "✓ Simple variable equivalence test passed" << std::endl;
    
    // Test 5: Variables with different definitions should not be equivalent
    var1->setDefinition(treeAlg.num(42.0));
    var2->setDefinition(treeAlg.num(100.0));
    
    assert(!treeAlg.alphaEquivalent(expr4a, expr4b));
    std::cout << "✓ Different variable definitions test passed" << std::endl;
    
    // Test 6: Recursive variables - same recursive pattern
    auto recVar1 = treeAlg.var(10);
    auto recVar2 = treeAlg.var(20);
    
    recVar1->setDefinition(treeAlg.add(recVar1, treeAlg.num(1.0)));
    recVar2->setDefinition(treeAlg.add(recVar2, treeAlg.num(1.0)));
    
    assert(treeAlg.alphaEquivalent(recVar1, recVar2));
    std::cout << "✓ Recursive variable equivalence test passed" << std::endl;
    
    // Test 7: Mutual recursion - same pattern with different variable names
    // System 1: x = y + 1, y = x * 2
    auto mutVar1_x = treeAlg.var(30);
    auto mutVar1_y = treeAlg.var(31);
    
    mutVar1_x->setDefinition(treeAlg.add(mutVar1_y, treeAlg.num(1.0)));  // x = y + 1
    mutVar1_y->setDefinition(treeAlg.mul(mutVar1_x, treeAlg.num(2.0)));  // y = x * 2
    
    // System 2: a = b + 1, b = a * 2 (same pattern, different names)
    auto mutVar2_a = treeAlg.var(40);
    auto mutVar2_b = treeAlg.var(41);
    
    mutVar2_a->setDefinition(treeAlg.add(mutVar2_b, treeAlg.num(1.0)));  // a = b + 1
    mutVar2_b->setDefinition(treeAlg.mul(mutVar2_a, treeAlg.num(2.0)));  // b = a * 2
    
    // Test equivalence of the systems
    assert(treeAlg.alphaEquivalent(mutVar1_x, mutVar2_a));  // x ≡α a
    assert(treeAlg.alphaEquivalent(mutVar1_y, mutVar2_b));  // y ≡α b
    std::cout << "✓ Mutual recursion equivalence test passed" << std::endl;
    
    // Test 8: Mutual recursion with different structure should not be equivalent
    // System 3: u = v + 2, v = u * 3 (different constants)
    auto mutVar3_u = treeAlg.var(50);
    auto mutVar3_v = treeAlg.var(51);
    
    mutVar3_u->setDefinition(treeAlg.add(mutVar3_v, treeAlg.num(2.0)));  // u = v + 2 (different!)
    mutVar3_v->setDefinition(treeAlg.mul(mutVar3_u, treeAlg.num(3.0)));  // v = u * 3 (different!)
    
    assert(!treeAlg.alphaEquivalent(mutVar1_x, mutVar3_u));  // Different patterns
    std::cout << "✓ Different mutual recursion patterns test passed" << std::endl;
    
    // Test 9: Complex mutual recursion with shared sub-expressions
    // System 4: p = q + (r * 5), q = p - r, r = 7
    auto mutVar4_p = treeAlg.var(60);
    auto mutVar4_q = treeAlg.var(61);
    auto mutVar4_r = treeAlg.var(62);
    
    mutVar4_r->setDefinition(treeAlg.num(7.0));  // r = 7 (base case)
    mutVar4_p->setDefinition(treeAlg.add(mutVar4_q, treeAlg.mul(mutVar4_r, treeAlg.num(5.0))));  // p = q + (r * 5)
    mutVar4_q->setDefinition(treeAlg.sub(mutVar4_p, mutVar4_r));  // q = p - r
    
    // System 5: same pattern with different variable names
    auto mutVar5_x = treeAlg.var(70);
    auto mutVar5_y = treeAlg.var(71);
    auto mutVar5_z = treeAlg.var(72);
    
    mutVar5_z->setDefinition(treeAlg.num(7.0));  // z = 7
    mutVar5_x->setDefinition(treeAlg.add(mutVar5_y, treeAlg.mul(mutVar5_z, treeAlg.num(5.0))));  // x = y + (z * 5)
    mutVar5_y->setDefinition(treeAlg.sub(mutVar5_x, mutVar5_z));  // y = x - z
    
    assert(treeAlg.alphaEquivalent(mutVar4_p, mutVar5_x));
    assert(treeAlg.alphaEquivalent(mutVar4_q, mutVar5_y));
    assert(treeAlg.alphaEquivalent(mutVar4_r, mutVar5_z));
    std::cout << "✓ Complex mutual recursion with shared expressions test passed" << std::endl;
    
    std::cout << "Alpha-equivalence test passed!" << std::endl;
}

void test_grand_alpha_equivalence() {
    std::cout << "Testing GRAND alpha-equivalence: t ≡α t(TreeAlgebra)..." << std::endl;
    
    TreeAlgebra treeAlg;
    
    // Create a complex tree with mutual recursion
    // System: x = y + z + 1, y = x * 2 - z, z = x + y - 3
    auto var_x = treeAlg.var(100);
    auto var_y = treeAlg.var(101); 
    auto var_z = treeAlg.var(102);
    
    // Define the mutual recursion system
    var_x->setDefinition(
        treeAlg.add(
            treeAlg.add(var_y, var_z), 
            treeAlg.num(1.0)
        )
    ); // x = y + z + 1
    
    var_y->setDefinition(
        treeAlg.sub(
            treeAlg.mul(var_x, treeAlg.num(2.0)), 
            var_z
        )
    ); // y = x * 2 - z
    
    var_z->setDefinition(
        treeAlg.sub(
            treeAlg.add(var_x, var_y), 
            treeAlg.num(3.0)
        )
    ); // z = x + y - 3
    
    // Create a complex expression using these variables
    auto t = treeAlg.mul(
        treeAlg.add(var_x, var_y),
        treeAlg.sub(var_z, treeAlg.num(5.0))
    ); // t = (x + y) * (z - 5)
    
    std::cout << "Created complex tree with mutual recursion system" << std::endl;
    
    // THE GRAND TEST: Evaluate t with TreeAlgebra to get t'
    std::cout << "Evaluating t with TreeAlgebra..." << std::endl;
    auto t_prime = treeAlg.eval(t, treeAlg);  // t' = eval(t, TreeAlgebra)
    
    std::cout << "Testing fundamental property: t ≡α t'..." << std::endl;
    
    // THE FUNDAMENTAL TEST: t should be alpha-equivalent to t'
    bool are_equivalent = treeAlg.alphaEquivalent(t, t_prime);
    
    if (are_equivalent) {
        std::cout << "🎉 SUCCESS: t ≡α t(TreeAlgebra) - Fundamental property verified!" << std::endl;
    } else {
        std::cout << "❌ FAILURE: t ≢α t(TreeAlgebra) - Need to fix implementation" << std::endl;
    }
    
    assert(are_equivalent);
    std::cout << "Grand alpha-equivalence test passed!" << std::endl;
}

void test_string_algebra_complex() {
    std::cout << "Testing StringAlgebra on complex mutual recursion system..." << std::endl;
    
    TreeAlgebra treeAlg;
    StringAlgebra stringAlg;
    
    // Create the same complex tree with mutual recursion as in the grand test
    // System: x = y + z + 1, y = x * 2 - z, z = x + y - 3
    auto var_x = treeAlg.var(200);
    auto var_y = treeAlg.var(201); 
    auto var_z = treeAlg.var(202);
    
    // Define the mutual recursion system
    var_x->setDefinition(
        treeAlg.add(
            treeAlg.add(var_y, var_z), 
            treeAlg.num(1.0)
        )
    ); // x = y + z + 1
    
    var_y->setDefinition(
        treeAlg.sub(
            treeAlg.mul(var_x, treeAlg.num(2.0)), 
            var_z
        )
    ); // y = x * 2 - z
    
    var_z->setDefinition(
        treeAlg.sub(
            treeAlg.add(var_x, var_y), 
            treeAlg.num(3.0)
        )
    ); // z = x + y - 3
    
    // Create the same complex expression
    auto t = treeAlg.mul(
        treeAlg.add(var_x, var_y),
        treeAlg.sub(var_z, treeAlg.num(5.0))
    ); // t = (x + y) * (z - 5)
    
    std::cout << "Evaluating complex tree with StringAlgebra..." << std::endl;
    
    try {
        // Test evaluation with StringAlgebra
        auto result = treeAlg.eval(t, stringAlg);
        
        std::cout << "StringAlgebra result: " << result.first << std::endl;
        std::cout << "StringAlgebra complex recursion test passed!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "StringAlgebra evaluation failed: " << e.what() << std::endl;
        std::cout << "Need to investigate and fix StringAlgebra implementation..." << std::endl;
        throw; // Re-throw to fail the test
    }
}

int main() {
    test_simple_eval();
    test_simple_variable_eval();
    test_non_recursive_variable();
    test_semantic_non_recursive_variables();
    test_semantic_complex_variables();
    test_simple_recursive_variable();
    test_mutual_recursion();
    test_alpha_equivalence();
    test_grand_alpha_equivalence();
    test_string_algebra_complex();
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}