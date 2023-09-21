

#include<set>
#include<string>
#include<vector>
#include<format>

#include"Formula.hpp"
#include"verify.hpp"

VerifyResult verifyAssumption(
    const Formula* cur,
    const std::vector<Formula*>& parents,
    std::set<Formula*>& assumptions
){
    if(parents.size() != 0){
        return {false, std::format("Assumption has {}")}
    }
}