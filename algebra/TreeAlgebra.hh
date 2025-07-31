#ifndef TREE_ALGEBRA_HH
#define TREE_ALGEBRA_HH

#include "Algebra.hh"
#include <memory>
#include <variant>
#include <tuple>
#include <unordered_set>
#include <functional>

class Tree {
public:
    enum class NodeType { Num, Unary, Binary };
    
private:
    
    NodeType fType;
    std::variant<
        double,                                                              // For Num
        std::pair<UnaryOp, std::shared_ptr<Tree>>,                         // For Unary
        std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>> // For Binary
    > fData;
    
    // Private constructors - only TreeAlgebra can create Trees
    Tree(double value) : fType(NodeType::Num), fData(value) {}
    
    Tree(UnaryOp op, std::shared_ptr<Tree> operand) 
        : fType(NodeType::Unary), fData(std::make_pair(op, operand)) {}
    
    Tree(BinaryOp op, std::shared_ptr<Tree> left, std::shared_ptr<Tree> right) 
        : fType(NodeType::Binary), fData(std::make_tuple(op, left, right)) {}
    
    friend class TreeAlgebra;
    
public:
    // Getters for hash-consing
    NodeType getType() const { return fType; }
    
    double getValue() const { 
        return std::get<double>(fData); 
    }
    
    UnaryOp getUnaryOp() const { 
        return std::get<std::pair<UnaryOp, std::shared_ptr<Tree>>>(fData).first; 
    }
    
    std::shared_ptr<Tree> getOperand() const { 
        return std::get<std::pair<UnaryOp, std::shared_ptr<Tree>>>(fData).second; 
    }
    
    BinaryOp getBinaryOp() const { 
        return std::get<0>(std::get<std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>>>(fData)); 
    }
    
    std::shared_ptr<Tree> getLeft() const { 
        return std::get<1>(std::get<std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>>>(fData)); 
    }
    
    std::shared_ptr<Tree> getRight() const { 
        return std::get<2>(std::get<std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>>>(fData)); 
    }
    
    // Evaluation operator
    template<typename T>
    T operator()(const Algebra<T>& algebra) const {
        switch(fType) {
            case NodeType::Num:
                return algebra.num(std::get<double>(fData));
            case NodeType::Unary: {
                auto& [op, operand] = std::get<std::pair<UnaryOp, std::shared_ptr<Tree>>>(fData);
                return algebra.unary(op, (*operand)(algebra));
            }
            case NodeType::Binary: {
                auto& [op, left, right] = std::get<std::tuple<BinaryOp, std::shared_ptr<Tree>, std::shared_ptr<Tree>>>(fData);
                return algebra.binary(op, (*left)(algebra), (*right)(algebra));
            }
        }
    }
};

// Hash and equality functors for hash-consing
struct TreeHash {
    size_t operator()(const std::shared_ptr<Tree>& t) const {
        size_t h = 0;
        
        switch(t->getType()) {
            case Tree::NodeType::Num:
                h = std::hash<double>{}(t->getValue());
                break;
                
            case Tree::NodeType::Unary:
                h = std::hash<int>{}(static_cast<int>(t->getUnaryOp()));
                h ^= std::hash<void*>{}(t->getOperand().get()) + 0x9e3779b9 + (h << 6) + (h >> 2);
                break;
                
            case Tree::NodeType::Binary:
                h = std::hash<int>{}(static_cast<int>(t->getBinaryOp()));
                h ^= std::hash<void*>{}(t->getLeft().get()) + 0x9e3779b9 + (h << 6) + (h >> 2);
                h ^= std::hash<void*>{}(t->getRight().get()) + 0x517cc1b7 + (h << 6) + (h >> 2);
                break;
        }
        return h;
    }
};

struct TreeEqual {
    bool operator()(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const {
        if (a->getType() != b->getType()) return false;
        
        switch(a->getType()) {
            case Tree::NodeType::Num:
                return a->getValue() == b->getValue();
                
            case Tree::NodeType::Unary:
                return a->getUnaryOp() == b->getUnaryOp() && 
                       a->getOperand() == b->getOperand();  // Pointer equality (hash-consing invariant)
                
            case Tree::NodeType::Binary:
                return a->getBinaryOp() == b->getBinaryOp() && 
                       a->getLeft() == b->getLeft() &&      // Pointer equality
                       a->getRight() == b->getRight();      // Pointer equality
        }
        return false;
    }
};

class TreeAlgebra : public Algebra<std::shared_ptr<Tree>> {
private:
    // Hash-consing table
    mutable std::unordered_set<std::shared_ptr<Tree>, TreeHash, TreeEqual> fTrees;
    
    // Intern method for hash-consing
    std::shared_ptr<Tree> intern(std::shared_ptr<Tree> candidate) const {
        auto it = fTrees.find(candidate);
        if (it != fTrees.end()) {
            // Found existing tree, return it
            return *it;
        } else {
            // New tree, add to table and return
            fTrees.insert(candidate);
            return candidate;
        }
    }
    
public:
    std::shared_ptr<Tree> num(double value) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(value));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> add(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(BinaryOp::Add, a, b));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> sub(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(BinaryOp::Sub, a, b));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> mul(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(BinaryOp::Mul, a, b));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> div(const std::shared_ptr<Tree>& a, const std::shared_ptr<Tree>& b) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(BinaryOp::Div, a, b));
        return intern(candidate);
    }
    
    std::shared_ptr<Tree> abs(const std::shared_ptr<Tree>& a) const override {
        auto candidate = std::shared_ptr<Tree>(new Tree(UnaryOp::Abs, a));
        return intern(candidate);
    }
};

#endif