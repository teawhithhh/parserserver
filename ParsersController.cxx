#include "ParsersController.hxx"
#include "YamlParser.hxx"

std::unique_ptr<Parser> ParserFactory::createParser(const ParserInfo& inf)
{
	switch(inf.format)
	{
		case ParserFormat::yaml:
			return std::move(std::make_unique<YamlParser>(YamlParser(inf.pathFile)));
			break;
		default:
			spdlog::error("formatter not exists");
			throw std::invalid_argument("formatter not exists");
	}
}

void ParserController::write(const ParserInfo& inf, const Node& tree)
{
	parsers[inf]->write(tree);
}

Node ParserController::read(const ParserInfo& inf)
{
	return parsers[inf]->read();
}

void ParserController::addParser(const ParserInfo& inf)
{
	parsers[inf] = ParserFactory::createParser(inf);
}

void ParserController::deleteParser(const ParserInfo& inf)
{
	parsers.erase(inf);
}
