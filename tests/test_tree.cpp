#include <iostream>
#include "../algebra/TreeAlgebra.hh"
#include "../algebra/DoubleAlgebra.hh"

int main() {
    TreeAlgebra treeAlg;
    DoubleAlgebra doubleAlg;
    
    // Build expression tree: (10 + 5) * 2
    auto ten = treeAlg.num(10.0);
    auto five = treeAlg.num(5.0);
    auto two = treeAlg.num(2.0);
    auto sum = treeAlg.add(ten, five);
    auto result = treeAlg.mul(sum, two);
    
    // Evaluate the tree using DoubleAlgebra
    double value = (*result)(doubleAlg);
    std::cout << "(10 + 5) * 2 = " << value << std::endl;
    
    // Build another expression: (20 / 4) - 3
    auto twenty = treeAlg.num(20.0);
    auto four = treeAlg.num(4.0);
    auto three = treeAlg.num(3.0);
    auto quotient = treeAlg.div(twenty, four);
    auto diff = treeAlg.sub(quotient, three);
    
    double value2 = (*diff)(doubleAlg);
    std::cout << "(20 / 4) - 3 = " << value2 << std::endl;
    
    // Build complex expression: ((8 + 2) * 3) / (15 - 5)
    auto expr1 = treeAlg.div(
        treeAlg.mul(
            treeAlg.add(treeAlg.num(8.0), treeAlg.num(2.0)),
            treeAlg.num(3.0)
        ),
        treeAlg.sub(treeAlg.num(15.0), treeAlg.num(5.0))
    );
    
    double value3 = (*expr1)(doubleAlg);
    std::cout << "((8 + 2) * 3) / (15 - 5) = " << value3 << std::endl;
    
    return 0;
}