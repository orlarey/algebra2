# Algebra Framework

[![CI](https://github.com/orlarey/algebra2/workflows/CI/badge.svg)](https://github.com/orlarey/algebra2/actions)

A C++17 framework demonstrating the algebraic approach to expression representation and evaluation.

## Philosophy

This project implements a generic algebraic framework where expressions are represented as abstract syntax trees and evaluated through different "algebras" - implementations that define how operations should be performed on specific data types.

### Core Concept

The fundamental idea is to separate **structure** from **interpretation**:

- **Structure**: Mathematical expressions are represented as trees using a generic `TreeAlgebra`
- **Interpretation**: Different algebras can evaluate the same expression tree in different ways

This separation allows the same mathematical expression to be:

- Evaluated numerically (using `DoubleAlgebra`)
- Converted to human-readable strings (using `StringAlgebra`)
- Analyzed for operator precedence (using `PriorityAlgebra`)

### Example

Consider the expression `abs(2 * (5 + 3)) / (8 - 1)`:

```cpp
TreeAlgebra treeAlg;
DoubleAlgebra doubleAlg;
StringAlgebra stringAlg;

// Build the expression tree once
auto expr = treeAlg.div(
    treeAlg.abs(
        treeAlg.mul(
            treeAlg.num(2.0),
            treeAlg.add(treeAlg.num(5.0), treeAlg.num(3.0))
        )
    ),
    treeAlg.sub(treeAlg.num(8.0), treeAlg.num(1.0))
);

// Evaluate in different ways
double result = (*expr)(doubleAlg);        // → 2.28571
auto text = (*expr)(stringAlg);            // → "abs(2 * (5 + 3)) / (8 - 1)"
```

## Architecture

### Base Algebra Interface

The `Algebra<T>` template class defines the interface that all algebras must implement:

```cpp
template<typename T>
class Algebra {
public:
    virtual T num(double value) const = 0;
    virtual T add(const T& a, const T& b) const = 0;
    virtual T sub(const T& a, const T& b) const = 0;
    virtual T mul(const T& a, const T& b) const = 0;
    virtual T div(const T& a, const T& b) const = 0;
    virtual T abs(const T& a) const = 0;
    
    // Generic dispatch methods
    T unary(UnaryOp op, const T& a) const;
    T binary(BinaryOp op, const T& a, const T& b) const;
};
```

### Expression Trees

The `Tree` class represents mathematical expressions as immutable abstract syntax trees. Each tree node contains either:
- A numeric value (leaf node)
- A unary operation and one child
- A binary operation and two children

### Hash-Consing Optimization

TreeAlgebra implements **hash-consing**, an optimization technique where structurally identical expressions share the same memory location. This provides:

- **Memory efficiency**: No duplicate expressions in memory
- **Fast equality testing**: Compare expressions by pointer equality (O(1))
- **Memoization opportunities**: Cache computation results by expression address

```cpp
auto expr1 = treeAlg.add(treeAlg.num(5.0), treeAlg.num(3.0));
auto expr2 = treeAlg.add(treeAlg.num(5.0), treeAlg.num(3.0));

assert(expr1.get() == expr2.get());  // Same pointer!
```

## Included Algebras

### DoubleAlgebra
Performs standard numerical computation with IEEE 754 double-precision floating-point numbers.

### StringAlgebra
Converts expressions to human-readable mathematical notation with intelligent parentheses placement based on operator precedence.

### PriorityAlgebra
Computes operator precedence values for correct parentheses placement in string representations.

## Key Features

### Type Safety
The generic algebra interface ensures that all algebras implement the required operations. Adding a new operation to the base interface automatically requires all derived algebras to implement it.

### Extensibility
New algebras can be easily added by inheriting from `Algebra<T>` and implementing the required methods. Examples could include:
- Symbolic differentiation algebra
- Code generation algebra
- LaTeX formatting algebra

### Performance
- Hash-consing eliminates duplicate expressions
- Header-only library with inline functions
- Efficient dispatch through virtual function tables

## Building

This project uses CMake and requires C++17:

```bash
mkdir build && cd build
cmake ..
make
```

### Running Tests

```bash
ctest --verbose
```

### Running the Demo

```bash
./main
```

## Project Structure

```
algebra/
├── algebra/           # Header-only library
│   ├── Algebra.hh            # Base algebra interface
│   ├── TreeAlgebra.hh        # Expression trees with hash-consing
│   ├── DoubleAlgebra.hh      # Numerical evaluation
│   ├── StringAlgebra.hh      # String representation
│   └── PriorityAlgebra.hh    # Operator precedence
├── tests/             # Unit tests
├── main.cpp           # Demonstration program
└── CMakeLists.txt     # Build configuration
```

## Design Patterns

This framework demonstrates several important design patterns:

- **Visitor Pattern**: The `operator()` method allows different algebras to "visit" and process expression trees
- **Strategy Pattern**: Different algebras represent different strategies for processing expressions  
- **Template Method Pattern**: The base `Algebra` class provides generic dispatch methods
- **Flyweight Pattern**: Hash-consing implements the flyweight pattern for memory efficiency

## Educational Value

This project illustrates advanced C++ concepts including:
- Template metaprogramming
- Virtual function dispatch optimization
- Memory management with shared_ptr
- Hash table implementation for structural sharing
- Separation of concerns in software architecture

The algebraic approach provides a clean, extensible foundation for building domain-specific languages, computer algebra systems, and expression evaluators.