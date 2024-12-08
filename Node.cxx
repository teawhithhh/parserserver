#include "Node.hxx"
#include <spdlog/spdlog.h>
#include <stack>

Node::Node(const std::string& key, const std::string& data) : key{key}, data{data} {}

Node::Node(const YAML::Node& yamlNode, const std::string& parentKey)
	: key(parentKey), data(yamlNode.IsScalar() ? yamlNode.as<std::string>() : "")
{
	if (yamlNode.IsScalar())
		return;

	std::stack<std::pair<YAML::Node, Node*>> stack;
	stack.push(std::make_pair(yamlNode, this));

	while (!stack.empty())
	{
		auto [currentYamlNode, currentNode] = stack.top();
		stack.pop();

		// Проверяем на дочерние ветки
		if (currentYamlNode.IsMap()) {
			for (const auto& item : currentYamlNode) {
				auto childKey = item.first.as<std::string>();
				const auto& childValue = item.second;

				auto childNode = std::make_unique<Node>(childKey, childValue.IsScalar() ? childValue.as<std::string>() : "");
				Node* childNodePtr = childNode.get();
				currentNode->connectChild(std::move(childNode));

				if (!childValue.IsScalar()) {
					stack.push(std::make_pair(childValue, childNodePtr));
				}
			}
		} else if (currentYamlNode.IsSequence()) { // проверяем на последовательности (массивы)
			int index = 0;
			for (const auto& item : currentYamlNode)
			{
				std::string childKey = std::to_string(index++);

				auto childNode = std::make_unique<Node>(childKey, item.IsScalar() ? item.as<std::string>() : "");
				Node* childNodePtr = childNode.get();

				currentNode->connectChild(std::move(childNode));
				if (!item.IsScalar()) {
					stack.push(std::make_pair(item, childNodePtr));
				}
			}
		}
	}
}

size_t Node::getChildrensCount() const
{
	return childrens.size();
}

const Node& Node::getChild(size_t index) const {
	return *childrens.at(index);
}

const std::pair<std::string, std::string> Node::getInfo() const {
	return std::make_pair(key, data);
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

void Node::changeData (const std::string& key, const std::string& newData)
{
	if (key == this->key)
	{
		data = newData;
		return;
	}

	std::stack<std::vector<std::unique_ptr<Node>>::iterator> stack;

	for (auto it = childrens.begin(); it != childrens.end(); ++it)
	{
		stack.push(it);
	}

	while (!stack.empty())
	{
		auto it = stack.top();
		stack.pop();

		auto [keyChild, _] = (*it)->getInfo();
		if (keyChild == key)
		{
			(*it)->data = newData;
			return;
		}

		if (!(*it)->childrens.empty())
		{
			for (auto childIt = (*it)->childrens.begin(); childIt != (*it)->childrens.end(); ++childIt)
			{
				stack.push(childIt);
			}
		}
	}
}

void Node::deleteChild(const std::string& key)
{
	std::stack<std::pair<std::vector<std::unique_ptr<Node>>::iterator, std::vector<std::unique_ptr<Node>>*>> stack;

	for (auto it = childrens.begin(); it != childrens.end(); ++it)
	{
		stack.push(std::make_pair(it, &childrens));
	}

	while (!stack.empty())
	{
		auto [it, parentContainter] = stack.top();
		stack.pop();

		auto [keyChild, _] = (*it)->getInfo();

		if (keyChild == key)
		{
			parentContainter->erase(it);
			return;
		}

		if (!(*it)->childrens.empty())
		{
			for ( auto childIt = (*it)->childrens.begin(); childIt!=(*it)->childrens.end(); ++childIt)
			{
				stack.push(std::make_pair(childIt, &(*it)->childrens));
			}
		}
	}
}

void Node::createChild(const std::string& key, const std::string& data)
{
	std::unique_ptr<Node> child = std::make_unique<Node>(key, data);
	connectChild(std::move(child));
}

void Node::connectChild(std::unique_ptr<Node>&& child)
{
	childrens.push_back(std::move(child));
}
