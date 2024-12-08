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
		Node(const std::string& key, const std::string& data) : key{key}, data{data} {}

		const std::pair<std::string, std::string> getInfo() const {
			return std::make_pair(key, data);
		}

		void printTree()
		{
			spdlog::info("{} : {}", key, data);

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
					tabSpace++;
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

		Node root{"root", "zxc"};
		root.createChild("ghoul1", "SanyaKing");
		root.createChild("ghoul2", "AndreiImression");

		std::unique_ptr<Node> skvadMaksima = std::make_unique<Node>("admin", "Maksim");
		skvadMaksima->createChild("strimer", "Artem");

		root.connectChild(std::move(skvadMaksima));
		root.printTree();
		return data;
  }

  void write(std::unordered_map<std::string, std::string>) override {
    // Метод для записи данных в YAML (пока не реализован)
  }

private:
  std::filesystem::path path_;
};

#endif

