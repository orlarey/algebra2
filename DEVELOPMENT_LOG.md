# Algebra Framework Development Log

This document chronicles the complete development process of the Algebra Framework, a C++17 project demonstrating algebraic approaches to expression evaluation with advanced optimizations like hash-consing.

## Project Overview

**Goal**: Implement a small programming language based on algebraic concepts, where an algebra is a generic C++ class containing a list of operations.

**Core Operations**:

- `num(double) -> T`
- `add(T,T) -> T`
- `sub(T,T) -> T`
- `mul(T,T) -> T`
- `div(T,T) -> T`
- `abs(T) -> T` (added later)

## Development Timeline

### Phase 1: Basic Algebra Interface

**Objective**: Create the generic algebra template class.

```cpp
template<typename T>
class Algebra {
public:
    virtual ~Algebra() = default;
    
    virtual T num(double value) const = 0;
    virtual T add(const T& a, const T& b) const = 0;
    virtual T sub(const T& a, const T& b) const = 0;
    virtual T mul(const T& a, const T& b) const = 0;
    virtual T div(const T& a, const T& b) const = 0;
};
```

**Key Decision**: Used pure virtual methods to ensure type safety - all derived classes must implement every operation.

### Phase 2: Concrete Implementation with DoubleAlgebra

**Objective**: Create a concrete algebra implementation for numerical computation.

```cpp
class DoubleAlgebra : public Algebra<double> {
public:
    double num(double value) const override { return value; }
    double add(const double& a, const double& b) const override { return a + b; }
    double sub(const double& a, const double& b) const override { return a - b; }
    double mul(const double& a, const double& b) const override { return a * b; }
    double div(const double& a, const double& b) const override { return a / b; }
};
```

**Testing**: Created `main.cpp` with basic arithmetic tests. All operations worked correctly.

### Phase 3: TreeAlgebra - Expression Tree Implementation

**Objective**: Create an algebra that builds expression trees instead of computing values.

**Key Principle**: The same expression tree can be evaluated by different algebras:

- Build once with `TreeAlgebra`
- Evaluate with `DoubleAlgebra` for numbers
- Evaluate with other algebras for different interpretations

```cpp
class Tree {
    enum class Op { Num, Add, Sub, Mul, Div };
    Op op;
    std::variant<double, std::pair<std::shared_ptr<Tree>, std::shared_ptr<Tree>>> data;
    
    template<typename T>
    T operator()(const Algebra<T>& algebra) const;
};
```

**Design Approach**: The `operator()` method allows trees to evaluate themselves using any algebra - this implements the visitor pattern.

### Phase 4: StringAlgebra with Operator Precedence

**Objective**: Convert expression trees to human-readable mathematical notation.

**Challenge**: Minimize parentheses by respecting operator precedence.

**Solution**: Each algebra operation returns `std::pair<std::string, int>` where:

- `first`: The string representation
- `second`: The precedence level

```cpp
std::pair<std::string, int> mul(const std::pair<std::string, int>& a, 
                                const std::pair<std::string, int>& b) const override {
    std::string left = a.second < 50 ? "(" + a.first + ")" : a.first;
    std::string right = b.second < 50 ? "(" + b.first + ")" : b.first;
    return {left + " * " + right, 50};
}
```

**Result**: Clean mathematical expressions like `2 + 3 * 4` without unnecessary parentheses.

### Phase 5: Adding Unary Operations

**Objective**: Extend the algebra to support unary operations like absolute value.

**Implementation**:

- Added `abs(const T& a)` to base `Algebra` class
- Extended `Tree` variant to handle unary operations
- Updated all concrete algebras

**Key Learning**: The type system enforced consistency - forgot to implement `abs` in any algebra resulted in compilation errors.

### Phase 6: Generic Operation Dispatch

**Objective**: Structure the algebra for extensibility and avoid code duplication.

**Approach**: Created enum-based operation dispatch:

```cpp
enum class UnaryOp { Abs };
enum class BinaryOp { Add, Sub, Mul, Div };

template<typename T>
class Algebra {
protected:
    UnaryMethod fUnaryOps[static_cast<int>(UnaryOp::COUNT)];
    BinaryMethod fBinaryOps[static_cast<int>(BinaryOp::COUNT)];
    
public:
    T unary(UnaryOp op, const T& a) const {
        return (this->*fUnaryOps[static_cast<int>(op)])(a);
    }
    
    T binary(BinaryOp op, const T& a, const T& b) const {
        return (this->*fBinaryOps[static_cast<int>(op)])(a, b);
    }
};
```

**Benefits**:

- O(1) dispatch via function pointer arrays
- Easy to add new operations (just extend enums)
- Derived classes remain simple - only implement virtual methods

### Phase 7: Hash-Consing Optimization

**Objective**: Implement hash-consing for memory efficiency and fast equality testing.

**Concept**: Structurally identical expressions should share the same memory location.

**Implementation Strategy**:

1. Make `Tree` constructors private
2. `TreeAlgebra` becomes friend class and sole creator of trees
3. Implement `intern()` method with hash table lookup
4. All tree creation goes through `intern()`

```cpp
class Tree {
private:
    Tree(double value);  // Private constructors
    Tree(BinaryOp op, std::shared_ptr<Tree> left, std::shared_ptr<Tree> right);
    friend class TreeAlgebra;
};

class TreeAlgebra {
private:
    mutable std::unordered_set<std::shared_ptr<Tree>, TreeHash, TreeEqual> fTrees;
    
    std::shared_ptr<Tree> intern(std::shared_ptr<Tree> candidate) const {
        auto it = fTrees.find(candidate);
        return it != fTrees.end() ? *it : (fTrees.insert(candidate), candidate);
    }
};
```

**Results**:

- Identical expressions like `add(num(5), num(3))` called twice return the same pointer
- Memory usage reduced for complex expressions with repeated subexpressions
- Equality testing becomes O(1) pointer comparison

### Phase 8: Code Organization and Standards

**File Naming Convention Changes**:

- Extensions: `.hpp` → `.hh`
- File names match class names: `tree_algebra.hpp` → `TreeAlgebra.hh`
- Field naming: All class members prefixed with `f` (e.g., `fType`, `fData`)

**Directory Structure**:

```
algebra/
├── algebra/              # Header-only library
│   ├── Algebra.hh
│   ├── TreeAlgebra.hh
│   ├── DoubleAlgebra.hh
│   ├── StringAlgebra.hh
│   └── PriorityAlgebra.hh
├── tests/                # Unit tests
├── main.cpp              # Demo program
└── CMakeLists.txt        # Build system
```

### Phase 9: CMake Integration

**Objective**: Professional build system with IDE support.

**CMake Features Implemented**:

- Header-only library configuration
- Automatic test discovery and execution
- C++17 standard enforcement
- VSCode/IDE integration ready

```cmake
# algebra/CMakeLists.txt - Header-only library
add_library(algebra INTERFACE)
target_include_directories(algebra INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
)

# tests/CMakeLists.txt - Automated testing
function(add_algebra_test test_name)
    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name} algebra)
    add_test(NAME ${test_name} COMMAND ${test_name})
endfunction()
```

**Test Results**: All 5 test suites pass (100% success rate):

- `test_tree`: Basic TreeAlgebra functionality
- `test_hashcons`: Hash-consing verification
- `test_abs`: Unary operations
- `test_string`: String representation with precedence
- `test_generic`: Generic dispatch methods

### Phase 10: Documentation and Git Repository

**README.md**: Comprehensive documentation covering:

- Philosophy of algebraic approach
- Architecture explanation with code examples
- Hash-consing optimization details
- Build instructions and usage
- Design patterns analysis
- Educational value discussion

**Git Repository Setup**:

- Proper `.gitignore` for C++/CMake projects
- Descriptive commit messages
- Clean project structure ready for GitHub
- 16 files, 996 lines of code committed

## Technical Approaches

### 1. Separation of Structure and Interpretation

**Strategy**: Expression trees are built once but can be interpreted multiple ways.

```cpp
auto expr = treeAlg.div(treeAlg.abs(...), treeAlg.sub(...));
double result = (*expr)(doubleAlg);        // Numerical evaluation
auto text = (*expr)(stringAlg);            // String representation
```

### 2. Hash-Consing for Expression Trees

**Method**: Automatic structural sharing with guaranteed uniqueness.

```cpp
auto expr1 = treeAlg.add(treeAlg.num(5.0), treeAlg.num(3.0));
auto expr2 = treeAlg.add(treeAlg.num(5.0), treeAlg.num(3.0));
assert(expr1.get() == expr2.get());  // Same pointer!
```

### 3. Generic Operation Dispatch

**Technique**: Function pointer arrays for O(1) dispatch while maintaining extensibility.

Benefits over traditional switch statements:

- Better performance for many operations
- Easier to extend (just add to enum)
- Cleaner separation of concerns

### 4. Type-Safe Extensibility

**Principle**: Adding new operations forces all algebras to implement them.

Adding `abs()` operation:

1. Add to base `Algebra` class → Compilation error in all derived classes
2. Implement in each algebra → System consistent again
3. No runtime surprises or missing method errors

## Design Patterns Demonstrated

1. **Visitor Pattern**: Trees accept algebras as visitors
2. **Strategy Pattern**: Different algebras are different evaluation strategies  
3. **Template Method Pattern**: Base class provides generic dispatch
4. **Flyweight Pattern**: Hash-consing implements flyweight for trees
5. **Factory Pattern**: TreeAlgebra controls tree creation through intern()

## Performance Characteristics

- **Tree Construction**: O(log n) due to hash table lookup in intern()
- **Expression Evaluation**: O(n) where n is number of nodes
- **Memory Usage**: Optimal due to structural sharing
- **Equality Testing**: O(1) for trees (pointer comparison)

## Educational Value

This project demonstrates advanced C++ concepts:

- **Template Metaprogramming**: Generic algebra interface
- **Memory Management**: Smart pointers with structural sharing
- **Virtual Function Optimization**: Function pointer dispatch tables
- **Hash Table Implementation**: Custom hash and equality functors
- **Software Architecture**: Clean separation of concerns
- **Modern C++**: C++17 features, std::variant, structured bindings

## Extensibility Examples

The framework easily supports new algebras:

```cpp
// LaTeX algebra for mathematical typesetting
class LaTeXAlgebra : public Algebra<std::string> {
    std::string add(const std::string& a, const std::string& b) const override {
        return a + " + " + b;
    }
    std::string abs(const std::string& a) const override {
        return "\\left|" + a + "\\right|";
    }
    // ... other operations
};

// Code generation algebra
class CppCodeAlgebra : public Algebra<std::string> {
    std::string add(const std::string& a, const std::string& b) const override {
        return "(" + a + " + " + b + ")";
    }
    // ... generate C++ expressions
};
```

## Testing Strategy

Comprehensive test suite covering:

1. **Basic Functionality**: Each algebra works correctly
2. **Hash-Consing**: Structural sharing verification
3. **String Representation**: Correct precedence handling
4. **Generic Dispatch**: Unary/binary method routing
5. **Integration**: Complex expressions work end-to-end

All tests automated with CMake/CTest integration.

## Future Enhancements

Potential extensions to explore:

1. **More Operations**: trigonometric functions, logarithms, power
2. **Symbolic Differentiation**: Algebra that computes derivatives
3. **Optimization**: Algebraic simplification (e.g., x + 0 = x)
4. **Parallel Evaluation**: Multi-threaded expression evaluation
5. **Serialization**: Save/load expression trees
6. **Domain-Specific Languages**: Parser to create trees from text

## Conclusion

This project successfully demonstrates how algebraic thinking can create highly flexible and extensible software architectures. The separation of structure (trees) from interpretation (algebras) provides a powerful foundation for expression manipulation systems.

Key achievements:

- ✅ Clean, extensible architecture
- ✅ Advanced C++ techniques (hash-consing, generic dispatch)
- ✅ Comprehensive testing and documentation
- ✅ Professional build system and project structure
- ✅ Educational value for software design principles

The framework serves as an excellent foundation for building domain-specific languages, computer algebra systems, or any application requiring flexible expression evaluation.

---

*Development completed in a single session, demonstrating rapid prototyping capabilities and iterative refinement of software architecture.*
