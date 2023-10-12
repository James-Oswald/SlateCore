
#include<set>
#include<string>
#include<vector>

#include "Formula.hpp"

struct VerifyResult {
    bool verified; //True if verified
    std::string errMsg; //What went wrong with verification?
    int depth; //How deep did the function get
};

VerifyResult verifyAssumption(
    const Formula* cur, 
    const std::vector<Formula*>& parents,
    std::set<Formula*>& assumptions
);

VerifyResult verifyOrIntro(
    const Formula* cur,
    const std::vector<Formula*>& parents,
    std::set<Formula*>& assumptions
);

VerifyResult verifyOrElim(
    const Formula* cur,
    const std::vector<Formula*>& parents,
    std::set<Formula*>& assumptions
);

