
#include "Formula.hpp"

void Formula::Pred::applyToAsTerm(std::function<void(Term*)> termContext) const{
    Term* dummy = new Term;
    dummy->name = this->name;
    dummy->args = this->args;
    termContext(dummy);
    //Don't call the destructor on dummy, just delete the pointer
    operator delete(dummy); 
}

bool Formula::Pred::operator==(const Pred& other) const{
    if(this->name != other.name || this->args.size() != other.args.size())
        return false;
    //Arguments are the same
    TermList::const_iterator itr1 = this->args.begin();
    TermList::const_iterator itr2 = other.args.begin();
    for(; itr1 != this->args.end(); itr1++, itr2++){
        if(!(**itr1 == **itr2)){
            return false;
        }
    }
    return true;
}

TermList Formula::Pred::allConstants() const{
    TermList rv;
    for(Term* arg : this->args){
        //.splice(rv.end(),...) functions as a concat
        rv.splice(rv.end(), arg->allConstants());
    }
    return rv;
}

TermList Formula::Pred::allFunctions() const{
    TermList rv;
    for(Term* arg : this->args){
        //.splice(rv.end(),...) functions as a concat
        rv.splice(rv.end(), arg->allFunctions());
    }
    return rv;
}

//Code reuse
size_t Formula::Pred::depth() const{
    size_t termDepth = 0;
    for(Term* arg : this->args){
        size_t argDepth = arg->depth();
        if(argDepth > termDepth){
            termDepth = argDepth;
        }
    }
    return 1 + termDepth;
}