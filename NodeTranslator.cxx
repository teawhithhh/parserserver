#include "NodeTranslator.hxx"
#include <stack>

std::string NodeTranslator::translateTJ(const Node& root) {
	nlohmann::json json_response;
	std::stack<std::pair<const Node*, nlohmann::json*>> stack;
	
	stack.push(std::make_pair(&root, &json_response));

	while (!stack.empty())
	{
		auto [currentNode, currentJson] = stack.top();
		stack.pop();

		for (size_t i{0}; i<currentNode->getChildrensCount(); ++i)
		{
			const Node& child = currentNode->getChild(i);

			auto [key, _] = child.getInfo();
			(*currentJson)[key] = nlohmann::json();
			stack.push(std::make_pair(&child, &(*currentJson)[key]));
		}

		auto [_, data] = currentNode->getInfo();
		if (data!="")
			(*currentJson)["data"] = data;
	}

	std::ostringstream oss;
	oss << json_response;
	return oss.str();
}

Node NodeTranslator::translateTHM(const nlohmann::json& json) {
	Node root("root");
	std::stack<std::pair<Node *, const nlohmann::json*>> stack;

	stack.push(std::make_pair(&root, &json));

	while (!stack.empty())
	{
		auto [currentNode, currentJson] = stack.top();
		stack.pop();

		for (auto it = currentJson->begin(); it!=currentJson->end(); ++it)
		{
			const std::string& key = it.key();
			const auto& value = it.value();

			if (key == "data")
			{
				currentNode->setInfo(std::make_pair(currentNode->getInfo().first, value.get<std::string>()));
			} else {
				currentNode->createChild(key, "");

				Node& child = const_cast<Node&>(currentNode->getChild(currentNode->getChildrensCount()-1));

				if (value.is_object()) {
					stack.push(std::make_pair(&child, &value));
				} else if (value.is_string())
				{
					child.setInfo(std::make_pair(key, value.get<std::string>()));
				}
			}
		}
	}
	return root;
}
