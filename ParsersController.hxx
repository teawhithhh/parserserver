#ifndef PARSERCONTROLLER
#define PARSERCONTROLLER

#include "Parser.hxx"
#include <filesystem>
#include <memory>

enum class ParserFormat {yaml};

struct ParserInfo{
	ParserInfo(ParserFormat formatParser, std::filesystem::path pathToFile) : format(formatParser), pathFile(pathToFile) {};
	ParserFormat format;
	std::filesystem::path pathFile;
	bool operator==(const ParserInfo& other) const {
			return format == other.format && pathFile == other.pathFile;
	}
};

namespace std {
	template <>
	struct hash<ParserInfo> {
		std::size_t operator()(const ParserInfo& inf) const {
			std::size_t h1 = std::hash<int>{}(static_cast<int>(inf.format));
			std::size_t h2 = std::hash<std::filesystem::path>{}(inf.pathFile);
			return h1 ^ (h2 << 1);
		}
	};
}

class ParserFactory {
	public:
		static std::unique_ptr<Parser> createParser(const ParserInfo& inf);
};

class ParserController {
	public:
		ParserController(){};
		void write(const ParserInfo& inf, const Node& tree);
		Node read(const ParserInfo& inf);
		void addParser(const ParserInfo& inf);
		void deleteParser(const ParserInfo& inf);
	private:
		std::unordered_map<ParserInfo, std::unique_ptr<Parser>> parsers;
};

#endif
