
#include "Term.hpp"

Term::~Term(){
    for(Term* arg : args){
        delete arg;
    }
}

Term* Term::copy() const{
    Term* rv = new Term;
    rv->name = this->name;
    rv->args = TermList();
    for(Term* arg : this->args){
        rv->args.push_back(arg->copy());
    }
    return rv;
}

/**
 * Returns true iff the terms share the same name and, in the event they have arguments,
 * if they share the same arguments.
 * @return if the terms are semantically equivelent 
*/
bool Term::operator==(const Term& term){
    //Name and argument size are the same.
    if(this->name != term.name || this->args.size() != term.args.size())
        return false;
    //Arguments are the same
    TermList::const_iterator itr1 = this->args.begin();
    TermList::const_iterator itr2 = term.args.begin();
    for(; itr1 == this->args.end(); itr1++, itr2++){
        if(!(**itr1 == **itr2)){
            return false;
        }
    }
    return true;
}

TermList Term::subconstants() const{
    TermList rv;
    for(Term* arg : this->args){
        if(arg->args.size() == 0){
            rv.push_back(arg);
        }
    }
    return rv;
}


void inOrderConstantTraversal(Term* base, TermList& constants){
    for(Term* arg : base->args){
        if(arg->args.size() == 0){
            constants.push_back(arg);
        }else if(arg->args.size() > 0){
            inOrderConstantTraversal(arg, constants);
        }
    }
}

TermList Term::allSubconstants() const {
    TermList rv;
    inOrderConstantTraversal((Term *)this, rv);
    return rv;
}

TermList Term::allConstants() const{
    if(this->args.size() == 0){
        return {(Term*) this};
    }else{
        return this->allSubconstants();
    }
}

void inOrderFunctionTraversal(Term* base, TermList& functions){
    for(Term* arg : base->args){
        if(arg->args.size() != 0){
            functions.push_back(arg);
        }
        inOrderFunctionTraversal(arg, functions);
    }
}

TermList Term::allSubfunctions() const {
    TermList rv;
    inOrderFunctionTraversal((Term*)this, rv);
    return rv;
}

TermList Term::allFunctions() const {
    TermList rv = this->allSubfunctions();
    if(this->args.size() > 0){
        rv.push_front((Term*)this);
    }
    return rv;
}

size_t Term::depth() const{
    size_t termDepth = 0;
    for(Term* arg : this->args){
        size_t argDepth = arg->depth();
        if(argDepth > termDepth){
            termDepth = argDepth;
        }
    }
    return 1 + termDepth;
}

std::unordered_set<std::string> Term::identifiers() const{
    std::unordered_set<std::string> rv = {this->name};
    for(Term* arg : args){
        rv.merge(arg->identifiers());
    }
    return rv;
}

// Construction Helpers ================================================================================================

Term* Var(std::string name){
    Term* rv = new Term;
    rv->name = std::move(name);
    rv->args = TermList();
    return rv;
}

Term* Const(std::string name){
    Term* rv = new Term;
    rv->name = std::move(name);
    rv->args = TermList();
    return rv;
}

Term* Func(std::string name, TermList args){
    Term* rv = new Term;
    rv->name = std::move(name);
    rv->args = std::move(args);
    return rv;
}