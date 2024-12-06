#ifndef YAML_PARSER
#define YAML_PARSER

#include "Parser.hxx"
#include <filesystem>
#include <iostream>
#include <yaml-cpp/yaml.h>

struct Node
{
	std::string value;
	std::vector<std::unique_ptr<Node>> children;

	Node(std::string val) : value{val} {};
	void addChild(std::unique_ptr<Node> child) {
		children.push_back(std::move(child));
	}
};


class YamlParser : public Parser {
	public:
		YamlParser(std::filesystem::path path) : path_(std::move(path)) {}

		std::unordered_map<std::string, std::string> read() {
			std::unordered_map<std::string, std::string> data;
			YAML::Node config = YAML::LoadFile(path_.string());

			for (const auto pair : config)
			{
				
				std::ostringstream oss;
				oss << pair.first;
				std::string index = oss.str();
				oss.clear();
				oss << pair.second;
				std::string val = oss.str();
				data[index] = val;
			}

			return data;
		}

		void write(std::unordered_map<std::string, std::string>) {

		}
	private:
		std::filesystem::path path_;
};

#endif
