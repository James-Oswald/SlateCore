#include<string>

#include<rapidjson/document.h>
#include<rapidjson/schema.h>

#include"ProofGraph.hpp"
#include"settings.hpp"

extern std::string proofGraphSchema;

void throwFailure(const std::string& message){
    throw std::runtime_error("Proof Json Parse Error: Error reading " \
    "json file: " + message);
}

ProofNode* newProofNode(
    std::string formulaExpr, std::string justification, 
    std::vector<ProofNode*> parents
){
    ProofNode* p = new ProofNode;
    p->formula = fromSExpressionString(formulaExpr);
    p->justification = JUSTIFICATION_STRING_MAP.at(justification);
    p->parents = parents;
    p->assumptions = {};
    return p;
}

ProofGraph* newProofGraph(std::string jsonProofGraph){
    rapidjson::Document schemaDoc;
    schemaDoc.Parse(proofGraphSchema.c_str());
    if(schemaDoc.HasParseError()){
        throw std::runtime_error("Proof Graph Schema JSON is invalid");
    }
    rapidjson::Document proofGraphDoc;
    proofGraphDoc.Parse(jsonProofGraph.c_str());
    if(proofGraphDoc.HasParseError()){
        throw std::runtime_error("Proof Graph JSON is malformed");
    }
    rapidjson::SchemaDocument d = rapidjson::SchemaDocument(schemaDoc);
    rapidjson::SchemaValidator validator(d);
    if(!proofGraphDoc.Accept(validator)){
        throw std::runtime_error("Proof Graph JSON does not conform to schema");
    }

    const rapidjson::Value& nodes = proofGraphDoc["nodes"];
    const rapidjson::Value& links = proofGraphDoc["links"];
    
    ProofGraph* graph = new ProofGraph;
    graph->assumptions = std::set<ProofNode*>();
    graph->nodes = std::unordered_map<size_t, ProofNode*>();

    //Create the nodes and add them to the list of all nodes
    for (rapidjson::Value::ConstValueIterator itr = nodes.Begin(); itr != nodes.End(); itr++){
        const rapidjson::Value& json_node = *itr;
        ProofNode* node = new ProofNode;
        node->formula = fromSExpression(sExpression(json_node["formula"].GetString()));
        node->justification = JUSTIFICATION_STRING_MAP.at(json_node["justification"].GetString());
        node->parents = std::vector<ProofNode*>();
        node->assumptions = std::set<ProofNode*>();
        node->children = std::list<ProofNode*>();
        graph->nodes[json_node["id"].GetUint()] = node;
    }

    //Use links to connect the nodes parents and children
    for(rapidjson::Value::ConstValueIterator itr = links.Begin(); itr != links.End(); itr++){
        const rapidjson::Value& json_link = *itr;
        size_t from = json_link["from"].GetUint();
        size_t to = json_link["to"].GetUint();
        ProofNode* from_p = graph->nodes[from];
        ProofNode* to_p = graph->nodes[to];
        from_p->children.push_back(to_p);
        to_p->parents.push_back(from_p);
    }

    //Add all nodes with no parents to the assumption set
    for(const auto [_, node] :  graph->nodes){
        if(node->parents.size() == 0){
            graph->assumptions.insert(node);
        }
    }

    return graph;
}