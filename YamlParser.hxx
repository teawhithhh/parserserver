#ifndef YAML_PARSER
#define YAML_PARSER

#include "Parser.hxx"
#include <filesystem>
#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <string>
#include <spdlog/spdlog.h>
#include "Node.hxx"

class YamlParser : public Parser {
public:
  YamlParser(std::filesystem::path path) : path_(std::move(path)) {}

	Node read() override {
		std::unordered_map<std::string, std::string> data;

		YAML::Node cfg = YAML::LoadFile(path_.string());

		Node cfgRoot(cfg);
		return cfgRoot;
  }

  void write(const Node&) override {
    // Метод для записи данных в YAML (пока не реализован)
  }

private:
  std::filesystem::path path_;
};

#endif
