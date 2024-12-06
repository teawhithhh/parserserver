#include "ParsersController.hxx"
#include "JsonParser.hxx"
#include "YamlParser.hxx"

std::unique_ptr<Parser> ParserFactory::createParser(ParserInfo inf)
{
	switch(inf.format)
	{
		case ParserFormat::json:
			return std::move(std::make_unique<JsonParser>(JsonParser(inf.pathFile)));
			break;
		case ParserFormat::yaml:
			return std::move(std::make_unique<YamlParser>(YamlParser(inf.pathFile)));
			break;
		default:
			spdlog::error("formatter dont exists");
			throw std::invalid_argument("formatter dont exists");
	}
}

void ParserController::write(ParserInfo inf, std::unordered_map<std::string, std::string> tbl)
{
	parsers[inf]->write(tbl);
}

std::unordered_map<std::string, std::string> ParserController::read(ParserInfo inf)
{
	return parsers[inf]->read();
}

void ParserController::addParser(ParserInfo inf)
{
	parsers[inf] = ParserFactory::createParser(inf);
}

void ParserController::deleteParser(ParserInfo inf)
{
	parsers.erase(inf);
}
