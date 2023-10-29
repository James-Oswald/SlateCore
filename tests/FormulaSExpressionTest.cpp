
#include<cassert>

#include"Formula.hpp"

int main(){
    std::string f1 = "(forall P (if (and (P 0) (forall n (If (P n) (P (add n 1))))) (forall n (P n))))";
    assert(toSExpression(fromSExpressionString(f1)) == f1);
}