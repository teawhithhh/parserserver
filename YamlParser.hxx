#ifndef YAML_PARSER
#define YAML_PARSER

#include "Parser.hxx"
#include <filesystem>
#include <iostream>
#include <stack>
#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <memory>
#include <vector>
#include <sstream>
#include <string>
#include <spdlog/spdlog.h>

class Node {
	public:
		Node(const std::string& key, const std::string& data = "") : key{key}, data{data} {}

		Node(const YAML::Node& yamlNode, const std::string& parentKey = "root") 
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
				} else if (currentYamlNode.IsSequence()) {
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

		const std::pair<std::string, std::string> getInfo() const {
			return std::make_pair(key, data);
		}

		void printTree()
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

		void changeData(const std::string& key, const std::string& newData)
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

		void deleteChild(const std::string& key)
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

		void createChild(const std::string& key, const std::string& data) {
			std::unique_ptr<Node> child = std::make_unique<Node>(key, data);
			connectChild(std::move(child));
		}

		void connectChild(std::unique_ptr<Node>&& child) {
			childrens.push_back(std::move(child));
		}
	private:
		std::string key;
		std::string data;
		std::vector<std::unique_ptr<Node>> childrens;
};

class YamlParser : public Parser {
public:
  YamlParser(std::filesystem::path path) : path_(std::move(path)) {}

	std::unordered_map<std::string, std::string> read() override {
		std::unordered_map<std::string, std::string> data;

		Node root{"root"};
		root.createChild("ghoul1", "SanyaKing");
		root.createChild("ghoul2", "AndreiImression");

		std::unique_ptr<Node> skvadMaksima = std::make_unique<Node>("Maksim squad");
		skvadMaksima->createChild("admin", "Maksim");
		skvadMaksima->createChild("strimer", "Artem");
		
		std::unique_ptr<Node> yarik = std::make_unique<Node>("yarikSquad");
		yarik->createChild("rak", "yarik");
		yarik->createChild("ghoul", "spletnya");

		skvadMaksima->connectChild(std::move(yarik));

		root.connectChild(std::move(skvadMaksima));
		root.printTree();
		root.deleteChild("Maksim squad");
		spdlog::info("----");
		root.printTree();
		spdlog::info("----");
		root.changeData("ghoul2", "zxcursed");
		root.printTree();
		spdlog::info("----");

		YAML::Node cfg = YAML::LoadFile(path_.string());
		Node secondRoot(cfg);
		secondRoot.printTree();
		return data;
  }

  void write(std::unordered_map<std::string, std::string>) override {
    // Метод для записи данных в YAML (пока не реализован)
  }

private:
  std::filesystem::path path_;
};

#endif
