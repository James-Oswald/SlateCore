
#include<cassert>
#include<optional>

#include"verify.hpp"

int main(){
    ProofNode* A = newProofNode("A", "Assumption", {});
    ProofNode* B = newProofNode("B", "Assumption", {});
    ProofNode* AB = newProofNode("(and A B)", "AndIntro", {A, B});
    ProofNode* AC = newProofNode("(or C A)", "OrIntro", {A});
    ProofNode* ABC = newProofNode("(and B (or C A))", "AndIntro", {A, AC});
    assert(verify(A) == std::nullopt);
    assert(verify(B) == std::nullopt);
    assert(verify(AB) == std::nullopt);
    assert(verify(ABC) == std::nullopt);
}