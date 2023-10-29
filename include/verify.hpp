
#include<set>
#include<string>
#include<vector>
#include<optional>

#include "Formula.hpp"
#include "ProofGraph.hpp"

/**
 * Verify a proof node
 * @return an optional string, if none, returns true, else contains
 *         the error string.
*/
std::optional<std::string> verify(ProofNode& node);
