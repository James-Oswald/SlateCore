#include<iostream>
#include<memory>
#include<cassert>

#include "Formula.hpp"

//For the sake of testing
using pFormula = std::unique_ptr<Formula>;

int main(){
    pFormula f01 (Prop("A"));
    std::cout<<toSExpression(f01.get())<<std::endl;
    std::cout<<toFirstOrderTPTP("f01", "hypothesis", f01.get())<<std::endl;
    assert(f01->depth() == 1);
    assert(f01->depthWithTerms() == 1);
    assert(f01->isPropositional());
    assert(f01->isZerothOrder());
    assert(f01->isFirstOrder());
    assert(f01->isSecondOrder());
    assert(f01->allFunctions().size() == 0);
    assert(f01->allConstants().size() == 0);
    assert(f01->allPredicates().size() == 1);
    assert(f01->isProposition());
    assert(f01->boundTermVariables().size() == 0);
    assert(f01->boundFunctionVariables().size() == 0);
    assert(f01->boundPredicateVariables().size() == 0);

    pFormula f02 (And(Pred("eq", {Func("S", {Const("1")}), Const("2")}), Pred("eq", {Func("S", {Const("2")}), Const("3")})));
    std::cout<<toSExpression(f02.get())<<std::endl;
    std::cout<<toFirstOrderTPTP("f02", "hypothesis", f02.get())<<std::endl;
    assert(f02->depth() == 2);
    assert(f02->depthWithTerms() == 4);
    assert(!f02->isPropositional());
    assert(f02->isZerothOrder());
    assert(f02->isFirstOrder());
    assert(f02->isSecondOrder());
    assert(f02->allFunctions().size() == 2);
    assert(f02->allConstants().size() == 4);
    assert(f02->allPredicates().size() == 2);
    assert(!f02->isProposition());
    assert(f02->boundTermVariables().size() == 0);
    assert(f02->boundFunctionVariables().size() == 0);
    assert(f02->boundPredicateVariables().size() == 0);

    pFormula f1 (Exists("x", Forall("y", Pred("eq", {Var("x"), Var("y")}))));
    std::cout<<toSExpression(f1.get())<<std::endl;
    std::cout<<toFirstOrderTPTP("f1", "hypothesis", f1.get())<<std::endl;
    assert(toSExpression(f1.get()) == "(Exists (Forall (eq x y)))");
    assert(f1->depth() == 3);
    assert(f1->depthWithTerms() == 4);
    assert(f1->quantifier->var == "x");
    assert(f1->quantifier->arg->quantifier->var == "y");
    assert(f1->boundTermVariables().size() == 2);
    assert(f1->boundFunctionVariables().size() == 0);
    assert(f1->boundPredicateVariables().size() == 0);
    assert(!f1->isPropositional());
    assert(!f1->isZerothOrder());
    assert(f1->isFirstOrder());
    assert(f1->isSecondOrder());
    assert(f1->allFunctions().size() == 0);
    assert(f1->allConstants().size() == 2);
    assert(f1->allPredicates().size() == 1);
    assert(!f1->isProposition());

    //Mathmatical Induction in 2nd order logic
    pFormula f2 (
        Forall("P",
            If(
                And(
                    Pred("P", {Const("0")}), //base case
                    Forall("n",              //Inductive step
                        If(
                            Pred("P", {Var("n")}),
                            Pred("P", {Func("add", {Var("n"), Const("1")})})
                        )
                    )
                ),
                Forall("n", Pred("P", {Var("n")}))
            )
        )
    );
    std::cout<<toSExpression(f2.get())<<std::endl;
    //std::cout<<toFirstOrderTPTP("f2", "hypothesis", f2.get())<<std::endl;
    assert(toSExpression(f2.get()) == 
          "(Forall (If (And (P 0) (Forall (If (P n) (P (add n 1))))) (Forall (P n))))");
    assert(f2->depth() == 6);
    assert(f2->depthWithTerms() == 8);
    assert(f2->quantifier->var == "P");
    assert(f2->boundTermVariables().size() == 3);
    assert(f2->boundFunctionVariables().size() == 0);
    assert(f2->boundPredicateVariables().size() == 4);
    assert(!f2->isPropositional());
    assert(!f2->isZerothOrder());
    assert(!f2->isFirstOrder());
    assert(f2->isSecondOrder());
    assert(f2->allFunctions().size() == 1);
    assert(f2->allConstants().size() == 5);
    assert(f2->allPredicates().size() == 4);
    assert(!f2->isProposition());

    pFormula f3 (Iff(Not(And(Prop("A"), Prop("B"))), Or(Not(Prop("A")), Not(Prop("B")))));
    std::cout<<toSExpression(f3.get())<<std::endl;
    std::cout<<toFirstOrderTPTP("f3", "hypothesis", f3.get())<<std::endl;
    assert(f3->depth() == 4);
    assert(f3->depthWithTerms() == 4);
    assert(f3->isPropositional());
    assert(f3->isZerothOrder());
    assert(f3->isFirstOrder());
    assert(f3->isSecondOrder());
    assert(f3->allFunctions().size() == 0);
    assert(f3->allConstants().size() == 0);
    assert(f3->allPredicates().size() == 4);
    assert(!f3->isProposition());
}