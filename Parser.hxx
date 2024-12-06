#ifndef PARSER_ABSTR
#define PARSER_ABSTR

#include <string>
#include <unordered_map>

class Parser{
	public:
		virtual std::unordered_map<std::string, std::string> read() = 0;
		virtual void write(std::unordered_map<std::string, std::string>) = 0;

		virtual ~Parser() = default;
};

#endif
