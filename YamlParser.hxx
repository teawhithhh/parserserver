#ifndef YAML_PARSER
#define YAML_PARSER

#include "Parser.hxx"
#include <filesystem>
#include <stack>
#include <yaml-cpp/yaml.h>
#include <spdlog/spdlog.h>
#include "Node.hxx"
#include <fstream>
#include "nlohmann/json.hpp"

class YamlParser : public Parser {
public:
  YamlParser(std::filesystem::path path) : path(path) {
		spdlog::info("YamlParser initialized with path: {}", path.string());
	}

	Node read() override {
		spdlog::info("Starting to read YAML file: {}", path.string()); 
		YAML::Node cfg;

		try {
			cfg = YAML::LoadFile(path.string());
			spdlog::info("YAML file successfully loaded.");
		} catch (const std::exception& e) {
			spdlog::error("Failed to load YAML file: {}", e.what());
			throw std::runtime_error("Failed to load YAML file");
		}

		Node cfgRoot(cfg);
		spdlog::info("Node object created from YAML root.");
		return cfgRoot;
  }

  void write(const Node& root) override {
		spdlog::info("Starting to write YAML data, file: {}", path.string());

		YAML::Node yamlRoot(convertNode(root));

		std::filesystem::path pth("C:\\Users\\240821\\electron\\electron-app\\cxx-back\\outconfig.yaml");

		spdlog::info("Attempting to write to file: {}", path.string());
		std::ofstream file(pth);
		if (!file.is_open()) {
			spdlog::error("Unable to open file: {}", path.string());
			throw std::runtime_error("Unable to open file");
		}

		spdlog::info("File opened successfully for writing.");
		file << yamlRoot;

		file.close();
		spdlog::info("File successfully written and closed.");
    // Метод для записи данных в YAML (пока не реализован)
  }

private:

	YAML::Node convertNode(const Node& root) {
		spdlog::info("Converting Node to YAML::Node format...");

		YAML::Node yamlNode;

		// Если у узла есть данные (ключ-значение), то создаем соответствующий узел
		if (!root.getInfo().second.empty()) {
			yamlNode = root.getInfo().second;
		}

		// Если узел имеет дочерние элементы, нужно обработать их как карту (Map)
		if (root.getChildrensCount() > 0) {
			for (size_t i = 0; i < root.getChildrensCount(); ++i) {
				const Node& childNode = root.getChild(i);
				// Преобразуем каждого ребенка в новый узел
				yamlNode[root.getChild(i).getInfo().first] = convertNode(childNode);
			}
		}

		spdlog::info("Node successfully converted to YAML::Node.");
		return yamlNode;
	}


  std::filesystem::path path;
};

#endif
