#include "Node.hxx"
#include <spdlog/spdlog.h>
#include <stack>

Node::Node(const std::string& key, const std::string& data) : key{key}, data{data}
{
	spdlog::debug("Node created with key: {}, data: {}", key, data);
}

Node::Node(const YAML::Node& yamlNode, const std::string& parentKey)
	: key(parentKey), data(yamlNode.IsScalar() ? yamlNode.as<std::string>() : "")
{
	spdlog::debug("Node creating from YAML with parentKey: {}...", parentKey);
	if (yamlNode.IsScalar()) 
	{
		spdlog::debug("YAML Node is a scalar with value: {}", data);
		return;
	}

	std::stack<std::pair<YAML::Node, Node*>> stack;
	stack.push(std::make_pair(yamlNode, this));

	while (!stack.empty())
	{
		auto [currentYamlNode, currentNode] = stack.top();
		stack.pop();

		// Проверяем на дочерние ветки
		if (currentYamlNode.IsMap()) {
			spdlog::debug("Processing a map node...");
			for (const auto& item : currentYamlNode) {
				auto childKey = item.first.as<std::string>();
				const auto& childValue = item.second;

				auto childNode = std::make_unique<Node>(childKey, childValue.IsScalar() ? childValue.as<std::string>() : "");
				Node* childNodePtr = childNode.get();
				currentNode->connectChild(std::move(childNode));

				if (!childValue.IsScalar()) {
					spdlog::debug("Node with key '{}' has a child node, pushing to stack.", childKey);
					stack.push(std::make_pair(childValue, childNodePtr));
				}
			}
		} else if (currentYamlNode.IsSequence()) { // проверяем на последовательности (массивы)
			spdlog::debug("Processing a sequence Node...");
			int index = 0;
			for (const auto& item : currentYamlNode)
			{
				std::string childKey = std::to_string(index++);

				auto childNode = std::make_unique<Node>(childKey, item.IsScalar() ? item.as<std::string>() : "");
				Node* childNodePtr = childNode.get();

				currentNode->connectChild(std::move(childNode));
				if (!item.IsScalar()) {
					spdlog::debug("Node with index '{}', has a child node, pushing to stack.", childKey);
					stack.push(std::make_pair(item, childNodePtr));
				}
			}
		}
	}
}

size_t Node::getChildrensCount() const
{
	spdlog::debug("Getting children count: {}", childrens.size());
	return childrens.size();
}

const Node& Node::getChild(size_t index) const {
	try {
		spdlog::debug("Getting child at index: {} (Node: {})", index, key);
		return *childrens.at(index);
	} catch (std::exception& e)
	{
		spdlog::error("Out of range, error: {}", e.what());
		throw std::out_of_range("Node getChild out of range");
	}
}

const std::pair<std::string, std::string> Node::getInfo() const {
	spdlog::debug("Getting info for node with key: {}, data: {}", key, data);
	return std::make_pair(key, data);
}

void Node::setInfo(const std::pair<std::string, std::string>& info)
{
	this->key = info.first;
	this->data = info.second;
}

void Node::printTree()
{
	spdlog::info("{}: {}", key, data);

	std::stack<std::pair<std::vector<std::unique_ptr<Node>>::iterator, int>> stack;
	for (auto it = childrens.begin(); it!= childrens.end(); ++it)
	{
		stack.push(std::make_pair(it, 1));
	}

	while(!stack.empty())
	{
		auto [it, tabSpace] = stack.top();
		stack.pop();

		auto [keyChild, dataChild] = (*it)->getInfo();

		std::string spaces(tabSpace * 2, ' ');

		spdlog::info("{}{}: {}", spaces, keyChild, dataChild);

		for (auto childIt = (*it)->childrens.begin(); childIt!=(*it)->childrens.end(); ++childIt)
		{
			stack.push(std::make_pair(childIt, tabSpace + 1));
		}
	}
}

void Node::deleteChild(const std::string& key)
{
	if (key.empty())
	{
		spdlog::error("Key cannot be empty (Node deleteChild)");
		throw std::invalid_argument("Key cannot be empty (Node deleteChild)");
	}

	spdlog::debug("Deleting child with key: {}", key);

	for (auto it = childrens.begin(); it != childrens.end(); ++it)
	{
		if ((*it)->getInfo().first == key)
		{
			spdlog::debug("Child found and deleted with key: {}", key);
			childrens.erase(it);
			return;
		}
	}
	
	spdlog::error("Child not found, key: {} (deleteChild Node)", key);
	throw std::invalid_argument("Child not found (deleteChild Node)");
	/* std::stack<std::pair<std::vector<std::unique_ptr<Node>>::iterator, std::vector<std::unique_ptr<Node>>*>> stack; */

	/* for (auto it = childrens.begin(); it != childrens.end(); ++it) */
	/* { */
	/* 	stack.push(std::make_pair(it, &childrens)); */
	/* } */

	/* while (!stack.empty()) */
	/* { */
	/* 	auto [it, parentContainter] = stack.top(); */
	/* 	stack.pop(); */

	/* 	auto [keyChild, _] = (*it)->getInfo(); */

	/* 	if (keyChild == key) */
	/* 	{ */
	/* 		parentContainter->erase(it); */
	/* 		return; */
	/* 	} */

	/* 	if (!(*it)->childrens.empty()) */
	/* 	{ */
	/* 		for ( auto childIt = (*it)->childrens.begin(); childIt!=(*it)->childrens.end(); ++childIt) */
	/* 		{ */
	/* 			stack.push(std::make_pair(childIt, &(*it)->childrens)); */
	/* 		} */
	/* 	} */
	/* } */
}

void Node::createChild(const std::string& key, const std::string& data)
{
	spdlog::debug("Creating child node with key: {}, data: {}", key, data);
	std::unique_ptr<Node> child = std::make_unique<Node>(key, data);
	connectChild(std::move(child));
}

void Node::connectChild(std::unique_ptr<Node>&& child)
{
	spdlog::debug("Connecting child node with key: {}", child->key);
	childrens.push_back(std::move(child));
}
