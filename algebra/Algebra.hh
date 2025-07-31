#ifndef ALGEBRA_HH
#define ALGEBRA_HH

enum class UnaryOp {
    Abs = 0,
    // Add new unary operations here
    COUNT
};

enum class BinaryOp {
    Add = 0,
    Sub = 1,
    Mul = 2,
    Div = 3,
    // Add new binary operations here
    COUNT
};

template<typename T>
class Algebra {
protected:
    using UnaryMethod = T (Algebra<T>::*)(const T&) const;
    using BinaryMethod = T (Algebra<T>::*)(const T&, const T&) const;
    
    UnaryMethod fUnaryOps[static_cast<int>(UnaryOp::COUNT)];
    BinaryMethod fBinaryOps[static_cast<int>(BinaryOp::COUNT)];
    
    Algebra() {
        // Initialize unary operations table
        fUnaryOps[static_cast<int>(UnaryOp::Abs)] = &Algebra<T>::abs;
        
        // Initialize binary operations table
        fBinaryOps[static_cast<int>(BinaryOp::Add)] = &Algebra<T>::add;
        fBinaryOps[static_cast<int>(BinaryOp::Sub)] = &Algebra<T>::sub;
        fBinaryOps[static_cast<int>(BinaryOp::Mul)] = &Algebra<T>::mul;
        fBinaryOps[static_cast<int>(BinaryOp::Div)] = &Algebra<T>::div;
    }
    
public:
    virtual ~Algebra() = default;
    
    // Generic methods
    T unary(UnaryOp op, const T& a) const {
        return (this->*fUnaryOps[static_cast<int>(op)])(a);
    }
    
    T binary(BinaryOp op, const T& a, const T& b) const {
        return (this->*fBinaryOps[static_cast<int>(op)])(a, b);
    }
    
    // Specific methods (to be implemented by derived classes)
    virtual T num(double value) const = 0;
    virtual T add(const T& a, const T& b) const = 0;
    virtual T sub(const T& a, const T& b) const = 0;
    virtual T mul(const T& a, const T& b) const = 0;
    virtual T div(const T& a, const T& b) const = 0;
    virtual T abs(const T& a) const = 0;
};

#endif