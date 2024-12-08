#ifndef PARSER_ABSTR
#define PARSER_ABSTR

#include "Node.hxx"

class Parser{
	public:
		virtual Node read() = 0;
		virtual void write(const Node&) = 0;

		virtual ~Parser() = default;
};

#endif
