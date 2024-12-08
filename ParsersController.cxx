#include "ParsersController.hxx"
#include "YamlParser.hxx"

std::unique_ptr<Parser> ParserFactory::createParser(ParserInfo inf)
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

void ParserController::write(ParserInfo inf, const Node& tree)
{
	parsers[inf]->write(tree);
}

Node ParserController::read(ParserInfo inf)
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
