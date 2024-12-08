#ifndef NODE
#define NODE

#include <yaml-cpp/yaml.h>

class Node {
	public:
		Node(const std::string& key, const std::string& data = "") ;
		Node(const YAML::Node& yamlNode, const std::string& parentKey = "root");

		const std::pair<std::string, std::string> getInfo() const;
		size_t getChildrensCount() const;
		const Node& getChild(size_t index) const;

		void printTree();
		void changeData(const std::string& key, const std::string& newData);
		void deleteChild(const std::string& key);
		void createChild(const std::string& key, const std::string& data);
		void connectChild(std::unique_ptr<Node>&& child);
	private:
		std::string key;
		std::string data;
		std::vector<std::unique_ptr<Node>> childrens;
};

#endif
