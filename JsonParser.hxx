#include "Parser.hxx"
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

using json = nlohmann::json;

class JsonParser : public Parser {
	public:
		JsonParser(std::filesystem::path path) : path_(std::move(path)) {}

		void write(std::unordered_map<std::string, std::string> tbl) override {
			spdlog::info("Writing to JSON.");
			json j_new;

			for (const auto& [key, value] : tbl) {
				j_new[key] = value;
			}

			std::ofstream file(path_);
			if (file.is_open()) {
				file << j_new.dump(4);  // Запись с отступами
				file.close();  // Явно закрыть файл, если он не будет закрыт автоматически
				spdlog::info("File written succesfully.");
			} else {
				/* spdlog::error("Could not open file for writing: {}", path_); */
				std::cerr << "Could not open file for writing: " << path_ << std::endl;
			}
		}

		std::unordered_map<std::string, std::string> read() override {
			std::cout << "Reading from JSON file: " << path_ << std::endl;
			std::ifstream file(path_);
			if (!file.is_open()) {
				std::cerr << "Could not open file for reading: " << path_ << std::endl;
				return {};
			}

			json j;
			try {
				file >> j;  // Чтение в объект JSON
			} catch (const json::exception& e) {
				std::cerr << "Error reading JSON: " << e.what() << std::endl;
				return {};
			}

			std::unordered_map<std::string, std::string> tbl;
			for (auto& [key, value] : j.items()) {
				if (value.is_string()) {
					tbl[key] = value.get<std::string>();
				}
			}
			return tbl;
		}

	private:
		std::filesystem::path path_;
};
