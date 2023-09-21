
#include<queue>
#include<stack>
#include<list>
#include<algorithm>
#include<stdexcept>

#include "Formula.hpp"


Formula::~Formula(){
    switch(this->connectiveType){
        case ConnectiveType::PRED:
            for(Term* arg : this->pred->args){
                delete arg;
            }
            delete this->pred;
            break;
        case ConnectiveType::UNARY:
            delete this->unary->arg;
            delete this->unary;
            break;
        case ConnectiveType::BINARY:
            delete this->binary->left;
            delete this->binary->right;
            delete this->binary;
            break;
        case ConnectiveType::QUANT:
            delete this->quantifier->arg;
            delete this->quantifier;
            break;
    }
}

Formula* Formula::copy() const{
    Formula* rv = new Formula;
    rv->type = this->type;
    rv->connectiveType = this->connectiveType;
    switch(this->connectiveType){
        case ConnectiveType::PRED:
            rv->pred = new Pred;
            rv->pred->name = this->pred->name;
            rv->pred->args = TermList();
            for(Term * arg : this->pred->args){
                rv->pred->args.push_back(arg->copy());
            }
            return rv;
        case ConnectiveType::UNARY:
            rv->unary = new UnaryConnective;
            rv->unary->arg = this->unary->arg->copy();
            return rv;
        case ConnectiveType::BINARY:
            rv->binary = new BinaryConnective;
            rv->binary->left = this->binary->left->copy(); 
            rv->binary->right = this->binary->right->copy();
            return rv;
        case ConnectiveType::QUANT:
            rv->quantifier = new Quantifier;
            rv->quantifier->var = this->quantifier->var;
            rv->quantifier->arg = this->quantifier->arg->copy();
            return rv;
    }
    throw std::runtime_error("Invalid connective type");
}

FormulaList Formula::subformulae() const{
    switch(this->connectiveType){
        case ConnectiveType::PRED:
            return FormulaList{};
        case ConnectiveType::UNARY:
            return FormulaList{this->unary->arg};
        case ConnectiveType::BINARY:
            return FormulaList{this->binary->left, this->binary->right};
        case ConnectiveType::QUANT:
            return FormulaList{this->quantifier->arg};
    }
    throw std::runtime_error("Invalid connective type");
}

FormulaList Formula::allSubformulae() const{
    FormulaList allFormula;
    //Queue based BFS over all nodes
    std::queue<const Formula*> next(std::deque<const Formula*>({this}));
    while(!next.empty()){ 
        for(Formula* subformula: next.front()->subformulae()){
            allFormula.push_back(subformula);
            next.push(subformula);
        }
        next.pop();
    }
    return allFormula;
}

FormulaList Formula::allFormulae() const{
    FormulaList allFormula = this->allSubformulae();
    allFormula.push_front((Formula*)this);
    return allFormula;
}


/**
 * Recursively traverses a formula tree in-order and returns the depth
 * @param base the tree to traverse and get the depth of
 * @param withTerms if true, will count the depth of term trees else treats predicates as leaves.
 * @return the depth of the tree
*/
size_t depthTraversal(const Formula* base, bool withTerms){
    switch(base->type){
        case Formula::Type::PRED:
            return !withTerms ? 1 : base->pred->depth();
        default:{
            size_t max = 0;
            for(Formula* arg : base->subformulae()){
                size_t argDepth = depthTraversal(arg, withTerms);
                if(argDepth > max){
                    max = argDepth;
                }
            }
            return 1 + max;
        }
    } 
}

size_t Formula::depth() const{
    return depthTraversal(this, false);
}

size_t Formula::depthWithTerms() const{
    return depthTraversal(this, true);
}

/**
 * Recursively performs an in-order traversal of the formula tree to get a vector of all predicates
 * in the order in which they appear in the formula.
 * @param base the formula to start the in order traversal at
 * @param predicates the list of predicates being built up in the order they appear within base.
*/
void inOrderPredicateTraversal(Formula* base, FormulaList& predicates){
    switch(base->connectiveType){
        case Formula::ConnectiveType::PRED:
            predicates.push_back(base);
            break;
        default:
            for(Formula* f : base->subformulae())
                inOrderPredicateTraversal(f, predicates);
    }
}

FormulaList Formula::allPredicates() const{
    FormulaList predicates;
    inOrderPredicateTraversal((Formula*)this, predicates);
    return predicates;
}

FormulaList Formula::allPropositions() const{
    FormulaList predicates = this->allPredicates();
    //filter for out predicates with args
    std::remove_if(predicates.begin(), predicates.end(), [](Formula* p){return !p->isProposition();});
    return predicates;
}

TermList Formula::allConstants() const{
    TermList rv;
    for(const Formula* p : this->allPredicates()){
        TermList constants = p->pred->allConstants();
        rv.splice(rv.end(), constants, constants.begin(), constants.end());
    }
    return rv;
}

TermList Formula::allFunctions() const{
    TermList rv;
    for(const Formula* p : this->allPredicates()){
        TermList functions = p->pred->allFunctions();
        rv.splice(rv.end(), functions, functions.begin(), functions.end());
    }
    return rv;
}

FormulaList Formula::allQuantified() const{
    FormulaList rv;
    if(this->connectiveType == ConnectiveType::QUANT){
        rv.push_back((Formula*)this);
    }
    for(Formula* f : this->allSubformulae()){
        if(f->connectiveType == ConnectiveType::QUANT){
            rv.push_back(f);
        }
    }
    return rv;
}

bool Formula::isProposition() const{
    return this->type == Type::PRED && this->pred->args.size() == 0;
}

/**
 * Recursive in-order traversal function for finding a list of items (Predicates, Constants, or Functions)
 * of that are bound by quantifiers.
 * @tparam ItemType the type of item that the quantifier formula will be associated with, Term* in the case
 * of quantifying over terms and functions and Formula* in the case of quantifying over Predicates
 * @param base The formula to start checking from
 * @param quantifierStack An iterable stack of quantifier formulae
 * @param boundObjVars the vector or Term* Formula* pairs the result is written to.
 * @param baseCase a function mapping Predicates to a list of items with names to check if they are bound
 * @param itemName a function mapping an item to its name to check against the quantifier's bound name
*/
template<typename ItemType>
void inOrderQuantifierTraversal(Formula* base, 
                                std::list<Formula*>& quantifierStack,
                                std::list<std::pair<ItemType, Formula*>>& boundObjVars,
                                std::list<ItemType> (*baseCase)(Formula*),
                                std::string (*itemName)(ItemType)){
    switch(base->connectiveType){
        //Base Case 1: the formula is a Predicate check if we associate with anything and leave
        case Formula::ConnectiveType::PRED:
            for(ItemType arg : baseCase(base)){
                for(Formula* quantifierFormula : quantifierStack){
                    if(quantifierFormula->quantifier->var == itemName(arg)){
                        boundObjVars.push_back(std::make_pair(arg, quantifierFormula));
                    }
                }
            }
            break;
        //Recursive case 1: The formula is a quantifier, push onto the quantifier stack to update binding order
        //and traverse the subformula
        case Formula::ConnectiveType::QUANT:
            //Push the current quantifier onto the list of bound vars we're looking for
            quantifierStack.push_front(base);
            inOrderQuantifierTraversal(base->quantifier->arg, quantifierStack, boundObjVars, baseCase, itemName);
            //Pop the quantifier since we've traversed all its inner vars
            quantifierStack.pop_front();
            break;
        //Recursive case 2: For any other formula type recurse on all subformulae
        default:
            for(Formula* subformula : base->subformulae()){
                inOrderQuantifierTraversal(subformula, quantifierStack, boundObjVars, baseCase, itemName);
            }
    }
}

std::list<std::pair<Term*, Formula*>> Formula::boundTermVariables() const{
    std::list<std::pair<Term*, Formula*>> rv;
    std::list<Formula*> quantifierStack;
    auto getConstants = [](Formula* f){return f->pred->allConstants();};
    auto getTermName = [](Term* o){return o->name;};
    inOrderQuantifierTraversal<Term*>((Formula*)this, quantifierStack, rv, getConstants, getTermName);
    return rv;
}

std::list<std::pair<Term*, Formula*>> Formula::boundFunctionVariables() const{
    std::list<std::pair<Term*, Formula*>> rv;
    std::list<Formula*> quantifierStack;
    auto getFunctions = [](Formula* f){return f->pred->allFunctions();};
    auto getTermName = [](Term* o){return o->name;};
    inOrderQuantifierTraversal<Term*>((Formula*)this, quantifierStack, rv, getFunctions, getTermName);
    return rv;
}

std::list<std::pair<Formula*, Formula*>> Formula::boundPredicateVariables() const{
    std::list<std::pair<Formula*, Formula*>> rv;
    std::list<Formula*> quantifierStack;
    auto getPredicates = [](Formula* p){return std::list<Formula*>{p};};
    auto getPredicateName = [](Formula* p){return p->pred->name;};
    inOrderQuantifierTraversal<Formula*>((Formula*)this, quantifierStack, rv, getPredicates, getPredicateName);
    return rv;
}

std::unordered_set<std::string> Formula::identifiers() const{
    switch (this->connectiveType){
        case ConnectiveType::PRED:{
            std::unordered_set<std::string> rv;
            this->pred->applyToAsTerm([&rv](Term* t){rv = t->identifiers();});
            return rv;
        }
        default:{
            std::unordered_set<std::string> rv;
            for(Formula* subformula : this->subformulae()){
                rv.merge(subformula->identifiers());
            }
            return rv;
        }
    }
}

// Formula Class testers -----------------------------------------------------------------------------------------------

/**
 * @brief tests if a formula only contains the given connectives
 * @param connectives the set of connectives to check if the formula contains
 * @param formula the formula to test
 * @return true iff formula contains only propositional connectives
*/
bool onlyConnectives(std::unordered_set<Formula::Type> connectives, const Formula * formula){
    for(Formula* subformula : formula->allFormulae()){
        if(subformula->type != Formula::Type::PRED && 
           connectives.find(subformula->type) == connectives.end()){
            return false;
        }
    }
    return true;
}

const std::unordered_set<Formula::Type> PropositionalConnectives = 
{Formula::Type::NOT, Formula::Type::AND, Formula::Type::OR, Formula::Type::IF, Formula::Type::IFF};

const std::unordered_set<Formula::Type> BaseConnectives = 
{Formula::Type::NOT, Formula::Type::AND, Formula::Type::OR,
 Formula::Type::IF, Formula::Type::IFF, Formula::Type::EXISTS, Formula::Type::FORALL};

bool Formula::isPropositional() const{
    
    //Make sure we only use propositional connectives
    if(!onlyConnectives(PropositionalConnectives, this)){
        return false;
    }
    
    //Make sure all predicates are propositions
    for(Formula * predicate : this->allPredicates()){
        if(!predicate->isProposition()){
            return false;
        }
    }

    return true;
}


bool Formula::isZerothOrder() const{
    return onlyConnectives(PropositionalConnectives, this);
}

bool Formula::isFirstOrder() const{
    //Make sure we only use base connectives (Prop + quantifiers)
    if(!onlyConnectives(BaseConnectives, this)){
        return false;
    }

    //Make sure we don't quantify over predicates or formulae
    if(this->boundPredicateVariables().size() > 0 || this->boundFunctionVariables().size() > 0){
        return false;
    }

    return true;
}


bool Formula::isSecondOrder() const{
    //Make sure we only use base connectives (Prop + quantifiers)
    if(!onlyConnectives(BaseConnectives, this)){
        return false;
    }

    return true;
}

//Construction Helpers =================================================================================================


Formula* Prop(std::string name){
    Formula* rv = new Formula;
    rv->type = Formula::Type::PRED;
    rv->connectiveType = Formula::ConnectiveType::PRED;
    rv->pred = new Formula::Pred;
    rv->pred->name = std::move(name);
    rv->pred->args = TermList();
    return rv;
}

Formula* Pred(std::string name, TermList args){
    Formula* rv = new Formula;
    rv->type = Formula::Type::PRED;
    rv->connectiveType = Formula::ConnectiveType::PRED;
    rv->pred = new Formula::Pred;
    rv->pred->name = std::move(name);
    rv->pred->args = std::move(args);
    return rv;
}

Formula* Not(Formula* arg){
    Formula* rv = new Formula;
    rv->type = Formula::Type::NOT;
    rv->connectiveType = Formula::ConnectiveType::UNARY;
    rv->unary = new Formula::UnaryConnective;
    rv->unary->arg = arg;
    return rv;
}

Formula* And(Formula* left, Formula* right){
    Formula* rv = new Formula;
    rv->type = Formula::Type::AND;
    rv->connectiveType = Formula::ConnectiveType::BINARY;
    rv->binary = new Formula::BinaryConnective;
    rv->binary->left = left;
    rv->binary->right = right;
    return rv;
}

Formula* Or(Formula* left, Formula* right){
    Formula* rv = new Formula;
    rv->type = Formula::Type::OR;
    rv->connectiveType = Formula::ConnectiveType::BINARY;
    rv->binary = new Formula::BinaryConnective;
    rv->binary->left = left;
    rv->binary->right = right;
    return rv;
}

Formula* If(Formula* left, Formula* right){
    Formula* rv = new Formula;
    rv->type = Formula::Type::IF;
    rv->connectiveType = Formula::ConnectiveType::BINARY;
    rv->binary = new Formula::BinaryConnective;
    rv->binary->left = left;
    rv->binary->right = right;
    return rv;
}

Formula* Iff(Formula* left, Formula* right){
    Formula* rv = new Formula;
    rv->type = Formula::Type::IFF;
    rv->connectiveType = Formula::ConnectiveType::BINARY;
    rv->binary = new Formula::BinaryConnective;
    rv->binary->left = left;
    rv->binary->right = right;
    return rv;
}

Formula* Forall(std::string varName, Formula* arg){
    Formula* rv = new Formula;
    rv->type = Formula::Type::FORALL;
    rv->connectiveType = Formula::ConnectiveType::QUANT;
    rv->quantifier = new Formula::Quantifier;
    rv->quantifier->var = std::move(varName);
    rv->quantifier->arg = arg;
    return rv;
}

Formula* Exists(std::string varName, Formula* arg){
    Formula* rv = new Formula;
    rv->type = Formula::Type::EXISTS;
    rv->connectiveType = Formula::ConnectiveType::QUANT;
    rv->quantifier = new Formula::Quantifier;
    rv->quantifier->var = std::move(varName);
    rv->quantifier->arg = arg;
    return rv;
}