# Fixpoint Log - Development of Recursive Variables in Algebra Framework

This document chronicles the design and implementation of recursive variables and fixpoint computation in the Algebra framework.

## Q1: What is the goal of adding variables to the tree algebra?

**Context**: We want to extend the tree algebra to support infinite or recursive structures using finite representations.

**Answer**: The idea is to introduce variable nodes `var(i)` where `i` is an integer index. These variables have a mutable definition field that can be changed, allowing us to describe infinite trees through recursion. The key insight is that variables act as "back-references" or "cycles" in what would otherwise be an infinite tree.

## Q2: How should variables be hashed in the hash-consing system?

**Context**: Hash-consing ensures structural sharing, but variables have mutable definitions.

**Answer**: Variables should be hashed only by their index `i`, not by their definition. This is crucial because:
- The definition is mutable and can change
- Two variables with the same index should be considered identical
- This prevents infinite recursion when hashing self-referential structures

## Q3: Should variables be a concept only in TreeAlgebra or in all algebras?

**Context**: We need to decide where the complexity of variables should reside.

**Answer**: Variables are a concept that belongs **only to TreeAlgebra**. Other algebras (DoubleAlgebra, StringAlgebra, etc.) don't need to know about variables. When evaluating a tree with variables:
- TreeAlgebra creates and manages variables via `var(int index)`
- When `Tree::operator()` encounters a `Var` node, it simply evaluates its definition
- Other algebras remain simple and unchanged

This encapsulation is elegant - the complexity stays in Tree/TreeAlgebra.

## Q4: How should we handle undefined variables?

**Context**: What happens when we evaluate a variable that has no definition?

**Answer**: For a framework focused on formalizing concepts, we assume variables are always defined before use. If not, it's a programming error. We use a simple runtime exception:
```cpp
throw std::runtime_error("Variable " + std::to_string(index) + " is not defined");
```

This keeps the design simple and focuses on the theoretical aspects rather than error recovery.

## Q5: How do we compute fixpoints for recursive variables?

**Context**: Variables can have recursive definitions like `var(0) = 1 + var(0)`. How do we evaluate these?

**Initial approach**: We considered adding to the base Algebra interface:
- `equals(T, T)` to detect convergence
- `bottom()` to provide an initial value

**Better approach**: After reflection, a more elegant solution emerged...

## Q6: How should StringAlgebra handle recursive definitions?

**Context**: StringAlgebra would produce infinitely growing strings for recursive definitions.

**Key insight**: Instead of trying to detect string equality, StringAlgebra could:
1. Use `bottom(i)` to create a placeholder name like "x" for variable i
2. Evaluate the definition to get something like "2*x+3"  
3. When asked `isFixpoint("2*x+3", i)`, it returns true - accepting the recursive equation

This is more flexible than testing equality - each algebra defines its own notion of "acceptable fixpoint".

## Q7: What methods should algebras provide for fixpoint computation?

**Context**: TreeAlgebra needs to orchestrate fixpoint computation across different algebras.

**Answer**: Add to the Algebra interface:
```cpp
// Create a placeholder/bottom value for variable i
virtual T bottom(int varIndex) const = 0;

// Test if 'value' is an acceptable fixpoint for variable varIndex
virtual bool isFixpoint(const T& value, int varIndex) const = 0;
```

This allows:
- **StringAlgebra**: Create symbolic names and accept recursive equations
- **DoubleAlgebra**: Use 0.0 or NaN as bottom, test numerical convergence
- **TreeAlgebra**: Build the syntactic fixpoint that other algebras interpret

## Q8: How does this relate to theoretical foundations?

**Context**: Is this approach grounded in theory?

**Answer**: Yes! This connects to several theoretical concepts:

1. **Kleene's fixpoint theorem**: The fixpoint is the limit of the ascending chain ⊥, F(⊥), F(F(⊥)), ...

2. **Initial Algebra Semantics (Goguen et al.)**: TreeAlgebra computes the syntactic fixpoint, then the unique homomorphism translates it to each semantic algebra

3. **Domain theory**: Each algebra defines its own partial order and notion of convergence

4. **Alpha-equivalence**: Two trees with variables are equivalent if they represent the same infinite structure

The beauty is that each algebra defines its own notion of "acceptable fixpoint" - numerical convergence for DoubleAlgebra, symbolic equation for StringAlgebra, etc.

## Q9: Should bottom() take a variable index as parameter?

**Context**: When creating placeholder values, should algebras know which variable they're creating a bottom value for?

**Arguments for `bottom(int varIndex)`**:
- StringAlgebra could generate distinct names: "x" for var(0), "y" for var(1), etc.
- Easier debugging - we know which variable is being computed
- More flexibility for algebras that want variable-specific placeholders

**Arguments against (simpler approach)**:
- **Conceptual purity**: In domain theory, ⊥ (bottom) is the unique least element of a domain, not parameterized
- **Simplicity**: Algebras shouldn't need to know which variable they're computing
- **Separation of concerns**: Variable management is TreeAlgebra's responsibility

**Decision**: Use `bottom()` without parameters. Each algebra defines how to create bottom values:
- DoubleAlgebra: `0.0` or `NaN`
- StringAlgebra: Generate a **fresh variable name** each time (e.g., "x0", "x1", "x2", ...)
- TreeAlgebra: `nullptr` or special bottom tree

**Important clarification**: StringAlgebra's `bottom()` must generate a unique name on each call, not return a generic placeholder. This requires maintaining internal state (e.g., a counter) to ensure uniqueness. This allows proper handling of multiple variables in the same expression.

## Q10: What exactly happens when TreeAlgebra encounters a variable during evaluation?

**Context**: We need to formalize the algorithm for computing fixpoints when evaluating trees with recursive variables.

**Key questions to answer**:
1. How does TreeAlgebra detect that it needs to compute a fixpoint?
2. What's the iteration strategy?
3. How do we avoid infinite loops during the fixpoint computation itself?
4. Should TreeAlgebra maintain a environment/context during evaluation?

### The Challenge of Mutual Recursion

Consider this example:
```
var(0) = 1 + var(1)
var(1) = 2 * var(0)
```

When evaluating var(0):
1. We start computing var(0)
2. We encounter var(1), so we start computing var(1)
3. var(1) references var(0) - we're back where we started!

This suggests TreeAlgebra needs to maintain:
- **Visiting set**: Variables currently being computed (to detect cycles)
- **Memo table**: Already computed values (to avoid recomputation)
- **Recursion stack**: The chain of variables being evaluated

### Possible Approaches

**Approach 1: Single Variable Fixpoint**
- Treat each variable independently
- Problem: Doesn't handle mutual recursion correctly

**Approach 2: System of Equations**
- Collect all mutually recursive variables
- Solve them as a system
- Similar to how type inference handles mutually recursive types

**Approach 3: Lazy Evaluation with Memoization**
- Start with all unknowns as `bottom()`
- Evaluate on demand, memoizing results
- Iterate until convergence

### Key Insight Needed

The fundamental question: When we encounter var(j) while computing var(i), do we:
1. Use the current approximation of var(j)?
2. Fully compute var(j) first (risking infinite recursion)?
3. Detect the cycle and handle it specially?

## Q11: How to handle mutually recursive variables systematically?

**Context**: Variables can be mutually recursive, requiring a systematic approach to compute fixpoints.

**Proposed Algorithm**: Use strongly connected components (SCC) in the dependency graph.

1. **Build dependency graph**: var(i) → var(j) if var(i)'s definition uses var(j)
2. **Find SCCs**: Groups of mutually recursive variables
3. **Topological sort**: Process SCCs in dependency order
4. **For each SCC**:
   - Initialize all variables in the group to `bottom()`
   - Iterate:
     - For each var in the group, evaluate its definition
     - Check if all computed values are fixpoints (`isFixpoint()`)
     - If yes: done with this SCC
     - If no: update values and continue iterating
5. **Invariant**: Once an SCC's fixpoint is found, it never changes

**Example**:
```
var(0) = var(2) + 1    // SCC1: {0,1}
var(1) = var(0) * 2    // 
var(2) = 5             // SCC2: {2}
```
Process order: SCC2 first (var(2) = 5), then SCC1.

**Questions this raises**:

1. **Convergence**: How do we guarantee that the iteration for an SCC will converge?
   - Depends on the algebra and the operations
   - May need iteration limits

2. **Efficiency**: Building the dependency graph requires analyzing all variable definitions
   - When to do this? Lazily or eagerly?
   - Cache the SCC decomposition?

3. **Dynamic definitions**: What if variable definitions change after we've computed SCCs?
   - Invalidate and recompute?
   - Track dependencies?

4. **Bottom values in iteration**: When iterating within an SCC, do we:
   - Use the values from the previous iteration?
   - Reset to bottom each time?
   - Something else?

5. **Implementation complexity**: This is significantly more complex than single-variable fixpoints
   - Is it worth it for the initial implementation?
   - Could we start simpler and extend later?

## Q12: How to discover and solve SCCs during evaluation?

**Context**: Rather than pre-computing the dependency graph, we want to discover SCCs dynamically during evaluation.

**The Algorithm**: Maintain a stack of SCCs (sets of mutually recursive variables) during evaluation.

**Data structures**:
- `Stack<Set<VarIndex>>`: Stack of SCCs being computed
- `Map<VarIndex, T>`: Current values for all variables
- `Set<VarIndex>`: Already fully computed variables

**Algorithm steps**:

1. **Encountering a new variable var(i)**:
   - Push new SCC {i} onto stack
   - Set value[i] = bottom()
   - Evaluate var(i)'s definition

2. **During evaluation of a definition**:
   - **No variables encountered**: 
     - Simple case, no fixpoint needed
     - Mark var(i) as done with computed value
   - **Encounter var(i) (self-reference)**:
     - Return bottom() and continue
     - Will need fixpoint iteration
   - **Encounter var(j) already on stack in SCC at position k**:
     - Pop and merge all SCCs from position k to top
     - Push merged SCC back
     - Continue evaluation

3. **After evaluating all definitions in current SCC**:
   - Check if all variables reached fixpoint
   - If yes: mark all as done
   - If no: update values and iterate

**Example trace** for `var(0) = 1 + var(1), var(1) = 2 * var(0)`:
1. Evaluate var(0): push SCC {0}, value[0] = ⊥
2. Encounter var(1): push SCC {1}, value[1] = ⊥  
3. Encounter var(0) (on stack!): merge → SCC {0,1}
4. Complete evaluation: value[0] = 1 + ⊥, value[1] = 2 * ⊥
5. Not fixpoint, iterate with new values...

**Key insight**: This discovers exactly the minimal SCCs needed, in the right order, without pre-analysis!

**Questions**:
1. How to efficiently check "is var(j) on the stack and in which SCC?"
2. When merging SCCs, do we restart evaluation or continue?
3. How to handle nested evaluation calls cleanly?

## Q15: Complete formal specification of the fixpoint algorithm

**Context**: Final formalization with clear notation and detailed comments.

```
Notation:
  For state σ = ⟨S, V, Θ, Δ⟩:
  - S(σ) denotes the stack component
  - V(σ) denotes the variable values
  - Θ(σ) denotes the hypothetical memoization
  - Δ(σ) denotes the definitive memoization
  
  For algebra A:
  - A⟦num⟧(n) denotes A.num(n)
  - A⟦op⟧(v₁, v₂) denotes A.op(v₁, v₂)
  - A⟦⊥⟧ denotes A.bottom()

Types:
  𝕍 = infinite set of variables
  𝕋 = set of all trees
  SCC = P(𝕍)
  T = algebra domain type
  Algebra(T) = algebra over domain T
  
Note: 𝕍 ⊂ 𝕋 (variables are trees)

State = ⟨S, V, Θ, Δ⟩ where:
  S : Seq(SCC)                          // Stack of SCCs being computed
  V : 𝕍 → T                             // Current variable values
  Θ : SCC → (𝕋 → T)                    // Hypothetical memoization per SCC
  Δ : 𝕋 → T                            // Definitive memoization

eval : 𝕋 × State × Algebra(T) → (T × SCC × State)
eval(t, σ, A) = 
  // If we've already computed this tree definitively, return cached result
  // This tree depends on no variables currently being computed
  case t ∈ dom(Δ(σ)):
    ⟨Δ(σ)(t), ∅, σ⟩
    
  // If this tree was computed in current top SCC iteration, return cached result
  // This tree depends on variables in the top SCC
  case S(σ) ≠ [] ∧ t ∈ dom(Θ(σ)(top(S(σ)))):
    ⟨Θ(σ)(top(S(σ)))(t), top(S(σ)), σ⟩
    
  // Evaluate a numeric constant - never depends on variables
  case t = num(n):
    ⟨A⟦num⟧(n), ∅, σ⟩
    
  // Evaluate a binary operation - dependencies are union of children's dependencies
  case t = op(t₁, t₂):
    let ⟨v₁, D₁, σ₁⟩ = eval(t₁, σ, A)     // Evaluate left child
    let ⟨v₂, D₂, σ₂⟩ = eval(t₂, σ₁, A)    // Evaluate right child
    let v = A⟦op⟧(v₁, v₂)                  // Apply operation in algebra
    let D = D₁ ∪ D₂                        // Union dependencies
    let σ' = memoize(t, v, D, σ₂)          // Cache result appropriately
    in ⟨v, D, σ'⟩
    
  // Evaluate a variable - may trigger fixpoint computation
  case t = v where v ∈ 𝕍:
    evalVar(v, σ, A)

evalVar : 𝕍 × State × Algebra(T) → (T × SCC × State)
evalVar(v, σ, A) =
  // Variable already computed in a previous SCC - return definitive value
  case ¬onStack(v, S(σ)) ∧ v ∈ dom(V(σ)):
    ⟨V(σ)(v), ∅, σ⟩
    
  // Variable is on stack - found a cycle! Merge SCCs and return current approximation
  case onStack(v, S(σ)):
    let k = position(v, S(σ))              // Find which SCC contains v
    let σ' = merge(k, σ)                   // Merge all SCCs from k to top
    in ⟨V(σ')(v), top(S(σ')), σ'⟩         // Return current value, depends on merged SCC
    
  // New variable - start computing its fixpoint
  otherwise:
    let G = {v}                             // Create new SCC with just v
    let σ₁ = ⟨S(σ) :: G,                   // Push new SCC on stack
              V(σ)[v ↦ A⟦⊥⟧],              // Initialize v to bottom
              Θ(σ)[G ↦ ∅],                  // Create empty hypothetical memo
              Δ(σ)⟩
    let ⟨val, D, σ₂⟩ = eval(def(v), σ₁, A) // Evaluate v's definition
    let σ₃ = σ₂ with V[v ↦ val]            // Update v's value
    
    // If we're back at the same SCC, compute fixpoint
    case top(S(σ₃)) = G:
      fixpoint(G, σ₃, A)
    // Otherwise, we merged with another SCC, continue with that
    otherwise:
      ⟨val, top(S(σ₃)), σ₃⟩

fixpoint : SCC × State × Algebra(T) → (T × SCC × State)
fixpoint(G, σ, A) =
  // Clean hypothetical memo: keep only variable entries, discard sub-expressions
  let σ₀ = clean(G, σ)
  // Iterate until all variables in G reach their fixpoints
  let ⟨σₙ, converged⟩ = iterate(G, σ₀, A, 0)
  
  case converged:
    // Success! Move everything to definitive and pop stack
    let σ' = promote(G, σₙ)
    in ⟨V(σ')(choose(G)), ∅, σ'⟩
  otherwise:
    error("Fixpoint did not converge")

iterate : SCC × State × Algebra(T) × ℕ → (State × Bool)
iterate(G, σ, A, n) =
  // Safety check: avoid infinite iteration
  case n > MAX_ITER: ⟨σ, false⟩
  
  otherwise:
    // Compute new value for each variable in the SCC
    let vals = {v ↦ π₁(eval(def(v), σ, A)) | v ∈ G}
    // Update all variables with new values
    let σ' = σ with V updated by vals
    
    // Check if all variables reached their fixpoints
    case ∀v ∈ G. A.isFixpoint(vals(v), v):
      ⟨σ', true⟩                           // Converged!
    otherwise:
      iterate(G, σ', A, n+1)               // Continue iterating

Helper functions:

memoize(t, val, D, σ) =
  case D = ∅: σ with Δ[t ↦ val]           // No dependencies -> definitive
  case D = top(S(σ)): σ with Θ[D][t ↦ val] // Depends on top SCC -> hypothetical
  otherwise: σ                             // Should not happen

merge(k, σ) =
  let G = ⋃{S(σ)[j] | k ≤ j < |S(σ)|}     // Union all SCCs from k to top
  let S' = S(σ)[0..k] :: G                 // Replace with single merged SCC
  in ⟨S', V(σ), Θ(σ) with merged entries, Δ(σ)⟩

promote(G, σ) =
  let Δ' = Δ(σ) ∪ Θ(σ)(G)                 // Move hypothetical to definitive
  in ⟨tail(S(σ)), V(σ), Θ(σ) \ {G}, Δ'⟩

clean(G, σ) =
  σ with Θ[G] keeping only variable entries

Predicates:
  onStack(v, S) = ∃G ∈ S. v ∈ G
  position(v, S) = min{k | v ∈ S[k]}
```

## Q14: Important observation about fixpoint criteria

**Context**: The fixpoint test doesn't have to be strict equality.

**Key insight**: For StringAlgebra, when we have `var(0) = 2 * var(0) + 3`, the fixpoint is reached when:
- Variable representation: `"x"`  
- Definition evaluation: `"2 * x + 3"`

These are not equal, but they are **equivalent** in the sense that `"x"` represents the solution to the equation `x = 2 * x + 3`.

**Generalization**: The `isFixpoint()` method can implement:
- **Equality**: For converging numerical values (DoubleAlgebra)
- **Equivalence**: For symbolic representations (StringAlgebra)
- **Structural equivalence**: For TreeAlgebra (alpha-equivalence)

This flexibility allows each algebra to define what "reaching a fixpoint" means in its domain. The algorithm doesn't need to know the specific criteria, just that the algebra can determine when to stop iterating.

**Note for implementation**: This is why we have `isFixpoint(value, varIndex)` rather than testing equality directly.

## Q16: Implementation Progress Log

**Context**: Documenting the step-by-step implementation of the fixpoint algorithm.

### Phase 1: Interface and Basic Implementation ✅

**Step 1: Extended base Algebra interface**
- Added `virtual T bottom() const = 0;`
- Added `virtual bool isEquivalent(const T& a, const T& b) const = 0;`
- Rejected initial `isFixpoint(value, varIndex)` approach to maintain algebra abstraction

**Step 2: Implemented fixpoint methods in concrete algebras**
- **DoubleAlgebra**: `bottom() = 0.0`, `isEquivalent() = (a == b)`
- **StringAlgebra**: `bottom()` generates unique names "x1", "x2", etc., `isEquivalent()` returns `true` (trusts TreeAlgebra)
- **TreeAlgebra**: `bottom()` creates fresh variables, `isEquivalent()` returns `true` (TODO: alpha-equivalence)
- **Removed PriorityAlgebra**: Unused in codebase

**Step 3: Added evaluation state structures**
```cpp
template<typename T>
struct SCCFrame {
    std::set<Tree*> scc;                      // Variables in this SCC
    std::map<Tree*, T> hypotheticalMemo;      // Hypothetical memoization
};

template<typename T>
struct EvaluationState {
    std::vector<SCCFrame<T>> sccStack;        // Stack of SCCs
    std::map<Tree*, T> variableValues;        // Current variable values
    std::map<Tree*, T> definitiveMemo;        // Definitive memoization
    
    std::optional<size_t> findSCCPosition(Tree* var) const;
    bool isOnStack(Tree* var) const;
};
```

**Verification**: ✅ All existing tests pass, compilation successful

### Phase 2: Core Algorithm Implementation (In Progress)

**Design Decision: Functional vs. Imperative State Management**

We chose the **imperative approach** with mutable state references:
- **Performance**: In-place modification, no copying of large structures
- **Simplicity**: More idiomatic C++, closer to typical algorithms
- **Memory efficiency**: Single state instance in memory
- **Natural fit**: Algorithm is inherently stateful (stack, memoization)

Rejected functional approach (immutable state) due to performance overhead and complexity.

**Auxiliary Functions Implementation** ✅

Implemented all auxiliary functions with imperative (mutable state) approach:
- `memoize()`: Cache results based on dependencies
- `checkDefinitiveMemo()`, `checkHypotheticalMemo()`: Check cached values
- `merge()`: Merge SCCs when cycles detected
- `promote()`: Move SCC from hypothetical to definitive
- `clean()`: Clean hypothetical memo keeping only variables
- Helper functions: `getDefinition()`, `hasTopSCC()`

**API Design for eval() method**

**Goal**: Simple API `eval(tree, algebra) -> T` hiding complexity

**State Management Decision**: 
- **Permanent state**: `thread_local std::map<Tree*, T> definitiveMemo` (persists between calls)
- **Temporary state**: `TemporaryState<T>` struct (created per call)
  - `sccStack`: Pile of SCCs being computed
  - `currentValues`: Variable values during fixpoint computation

**Final API**:
```cpp
template<typename T>
struct Hypotheses {
    std::vector<SCCFrame<T>> sccStack;
    std::map<Tree*, T> hypotheticalValues;
    
    std::optional<size_t> findSCCPosition(Tree* var) const;
    bool isOnStack(Tree* var) const;
};

template<typename T>
T eval(const std::shared_ptr<Tree>& tree, const Algebra<T>& algebra) const {
    static thread_local std::map<Tree*, T> definitiveMemo;  // Definitive facts
    Hypotheses<T> hypotheses;                               // Current hypotheses
    return evalInternal(tree, definitiveMemo, hypotheses, algebra);
}
```

**Rationale**: 
- Variables are trees → covered by `definitiveMemo`, no separate variable storage needed
- No `PermanentState` struct → direct `thread_local` map simpler
- **Conceptual clarity**: `definitiveMemo` (permanent facts) vs. `Hypotheses` (assumptions being tested)
- Clean separation: proven results vs. conjectures under verification

**Next Step**: Implement `evalInternal()` method following formal specification

### Phase 3: Testing and Validation (Pending)

**Test Cases to Implement**:
- Simple recursive variables: `var(0) = 1 + var(0)`
- Mutually recursive: `var(0) = 1 + var(1), var(1) = 2 * var(0)`
- Alpha-equivalence verification
- Convergence vs. divergence cases

## Q17: Final Implementation Status

**Context**: Implementation completed and tested.

### Key Implementation Decisions

1. **Cache Management**: The definitive memoization cache is `static thread_local` to persist across calls within a thread. For testing, we clear it at the beginning of each `eval()` call to avoid interference between tests.

2. **Simple Variable Optimization**: When a variable definition has no dependencies (non-recursive), we promote it directly to definitive memoization without going through fixpoint iteration.

3. **Debug Support**: Added conditional compilation (`if constexpr`) for debug output specific to certain types, avoiding compilation errors for types that don't support `operator<<`.

### Testing Results

**Non-recursive variables**: ✅ Working correctly
- Simple variables (var(0) = 42) evaluate correctly
- Complex expressions without recursion work as expected

**Recursive variables**: ✅ Working with caveats
- Simple recursion (var(0) = var(0) + 1) produces symbolic representations
- Mutual recursion correctly identifies SCCs and evaluates them together
- StringAlgebra produces new variable names for each `bottom()` call, which is semantically correct but may not be the most readable output

### Known Issues and Future Improvements

1. **StringAlgebra naming**: Currently generates fresh names (x1, x2, ...) for each `bottom()` call. Could be improved to reuse variable names within the same SCC.

2. **Performance**: The cache is cleared on each `eval()` call for correctness. In production, maintaining the cache across calls would improve performance for repeated evaluations.

3. **Alpha-equivalence**: The `isEquivalent()` method in TreeAlgebra currently returns `true` always. A proper implementation of alpha-equivalence checking would be beneficial.

4. **Convergence for DoubleAlgebra**: Currently uses simple equality. Could add epsilon-based comparison for numerical stability.

### API Achievement

Successfully achieved the simple API goal:
```cpp
template<typename T>
T eval(const std::shared_ptr<Tree>& tree, const Algebra<T>& algebra) const
```

The complexity of fixpoint computation, SCC detection, and memoization is completely hidden from the user.

## Implementation Summary

### Core Components Implemented

1. **Algebra Interface Extensions**:
   - `bottom()`: Creates initial values for fixpoint iteration
   - `isEquivalent()`: Tests convergence

2. **State Management**:
   - `Hypotheses<T>`: Temporary state for testing fixpoint hypotheses
   - `SCCFrame<T>`: Represents a strongly connected component
   - Definitive memoization: Permanent cache for proven results

3. **Evaluation Algorithm**:
   - `eval()`: Public API
   - `evalInternal()`: Core recursive evaluation with dependency tracking
   - `evalVar()`: Variable-specific evaluation with SCC detection
   - `fixpoint()`: Orchestrates fixpoint computation for an SCC
   - `iterate()`: Iterates until convergence

4. **Auxiliary Functions**:
   - `memoize()`: Smart caching based on dependencies
   - `merge()`: Merges SCCs when cycles are detected
   - `promote()`: Moves hypothetical results to definitive
   - `clean()`: Removes non-variable entries from hypothetical cache

### Test Coverage

- ✅ Simple arithmetic expressions
- ✅ Non-recursive variables
- ✅ Variables in complex expressions
- ✅ Simple recursive variables
- ✅ Mutually recursive variables
- ✅ Multiple algebra interpretations (Double, String)

## Conclusion

The fixpoint computation implementation is complete and functional. The system correctly:
1. Detects and handles recursive variable definitions
2. Identifies strongly connected components dynamically
3. Computes fixpoints through iteration
4. Maintains separation between proven facts and hypotheses
5. Provides a clean, simple API that hides all complexity

The implementation successfully demonstrates the theoretical concepts of initial algebra semantics, fixpoint computation, and the elegance of separating structure (trees) from interpretation (algebras).

## Q18: Amélioration de la représentation des équivalences pour StringAlgebra

**Context**: Discussion sur l'amélioration de la représentation des systèmes récursifs dans StringAlgebra.

### Problème identifié

Avec l'implémentation actuelle, StringAlgebra produit des résultats comme `"2 + 5 * (x1 + 3)"` pour des systèmes récursifs, ce qui :
- Cache la structure récursive originale
- Utilise des noms de variables générés qui changent à chaque évaluation
- Ne montre pas explicitement le système d'équations sous-jacent

### Représentation souhaitée

Pour le système :
```
var(0) = var(1) + 3
var(1) = 2 + 5 * var(0)
```

Au lieu de : `"2 + 5 * (x1 + 3)"`
On voudrait : `"x0 = 2 + 5 * (x1 = x0 + 3)"`

Cette notation :
- Préserve la structure récursive visible
- Montre explicitement les équivalences/définitions
- Reste cohérente avec les noms générés par StringAlgebra

### Approche proposée : Méthode `equation()`

**Idée** : TreeAlgebra demande à StringAlgebra de représenter une équivalence explicitement.

**Extension de l'interface Algebra** :
```cpp
// Nouvelle méthode dans Algebra.hh
virtual T equation(const std::string& varName, const T& definition) const = 0;
```

**Implémentation StringAlgebra** :
```cpp
std::string equation(const std::string& varName, const std::string& definition) const override {
    return varName + " = " + definition;
}
```

**Utilisation par TreeAlgebra** :
- Détecter quand un résultat représente une équivalence plutôt qu'une valeur simple
- Appeler `algebra.equation("x0", "2 + 5 * (x1 = x0 + 3)")`
- Permettre la composition d'équations imbriquées

### Avantages de cette approche

1. **Séparation des préoccupations** : TreeAlgebra gère la logique récursive, StringAlgebra gère uniquement la représentation
2. **Extensibilité** : Autres algèbres peuvent implémenter `equation()` selon leurs besoins
3. **Encapsulation préservée** : StringAlgebra n'a pas besoin de connaître les concepts de variables TreeAlgebra
4. **Cohérence architecturale** : Respecte le principe que chaque algèbre définit sa propre représentation

### Défis d'implémentation

- TreeAlgebra doit détecter quand il construit une équivalence vs. une valeur simple
- Gestion de la composition d'équations imbriquées
- Maintien de la cohérence des noms de variables dans les équations complexes

### Alternative considérée mais rejetée

Une approche où StringAlgebra maintiendrait une table de définitions interne a été considérée mais rejetée car elle violerait l'encapsulation et compliquerait l'interface.

### Statut

**Proposition documentée** - Non implémentée dans la version actuelle, mais architecture définie pour une future extension.

Cette amélioration permettrait d'obtenir des représentations beaucoup plus expressives des systèmes récursifs tout en maintenant l'élégance architecturale du framework.

---

*Implementation completed successfully. Future enhancement for equation representation documented.*