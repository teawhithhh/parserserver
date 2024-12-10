#ifndef SERVER_HXX
#define SERVER_HXX

#include "crow.h"
#include "ParsersController.hxx"

class IRequestHandler {
	public:
		virtual ~IRequestHandler() = default;
		virtual crow::response handleRequest(const crow::request& req) = 0;
};

class GetRequestHandler : public IRequestHandler {
	using ParserInfoMap = std::unordered_map<std::string, ParserInfo>;
	public:
		explicit GetRequestHandler(ParserInfoMap& sList, ParserController& cntrl) : settingsList{sList}, controller{cntrl} {};
		crow::response handleRequest(const crow::request& req) override;
	private:
		ParserController& controller;
		const ParserInfoMap& settingsList;
};

class PushRequestHandler : public IRequestHandler {
	using ParserInfoMap = std::unordered_map<std::string, ParserInfo>;
	public:
		explicit PushRequestHandler(ParserInfoMap& sList, ParserController& cntrl) : settingsList{sList}, controller{cntrl} {};
		crow::response handleRequest(const crow::request& req) override;
	private:
		ParserController& controller;
		const ParserInfoMap& settingsList;
};

class RouteManager {
	public:
		void registryRoute(const std::string& route, std::unique_ptr<IRequestHandler> handler);
		void applyRoutes(crow::SimpleApp& app);
	private:
		std::unordered_map<std::string, std::unique_ptr<IRequestHandler>> routes; 
};

class TcpServer {
public:
	TcpServer(int port);

	void setupRoutes();
	void run();

private:
	int m_port;
	crow::SimpleApp m_app;
	RouteManager m_manager;

	ParserController m_controller;
	std::unordered_map<std::string, ParserInfo> m_settingsList;

	void setupSettingsList();
	void setupController();
};

#endif
