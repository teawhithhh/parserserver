#ifndef NODE_TRANSLATOR
#define NODE_TRANSLATOR

#include "Node.hxx"
#include "nlohmann/json.hpp"
#include <string>

class NodeTranslator{
	public:
		static std::string translateTJ(const Node& root);
		static Node translateTHM(const nlohmann::json& json);
};

#endif
