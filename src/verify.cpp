

#include<set>
#include<string>
#include<vector>

#include"Formula.hpp"
#include"verify.hpp"

VerifyResult verifyAssumption(
    const Formula* cur,
    const std::vector<Formula*>& parents,
    std::set<Formula*>& assumptions
){
    size_t numParents = parents.size();
    if(numParents != 0){
        return { false, "Assumption has " + std::to_string(numParents) +\
               " parents but is expected to have no parents.", 0};
    }
}