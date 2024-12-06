#ifndef SERVER_HXX
#define SERVER_HXX

#include "crow.h"
#include "ParsersController.hxx"
#include "nlohmann/json.hpp"

class HashMapTranslator{
	public:
		static crow::json::wvalue translateTJ(const std::unordered_map<std::string, std::string>& tbl);
		static std::unordered_map<std::string, std::string> translateTHM(const crow::json::wvalue& json);
};

class IRequestHandler {
	public:
		virtual ~IRequestHandler() = default;
		virtual crow::response handleRequest(const crow::request& req) = 0;
};

class GetRequestHandler : public IRequestHandler {
	public:
		explicit GetRequestHandler(std::unordered_map<std::string, std::filesystem::path>& sList, ParserController& cntrl) : settingsList{sList}, controller{cntrl} {};
		crow::response handleRequest(const crow::request& req) override;
	private:
		ParserController& controller;
		std::unordered_map<std::string, std::filesystem::path>& settingsList;
};

class PushRequestHandler : public IRequestHandler {
	public:
		explicit PushRequestHandler(std::unordered_map<std::string, std::filesystem::path>& sList, ParserController& cntrl) : settingsList{sList}, controller{cntrl} {};
		crow::response handleRequest(const crow::request& req) override;
	private:
		ParserController& controller;
		std::unordered_map<std::string, std::filesystem::path>& settingsList;
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

	void setupSettingsList();
	void setupController();

	ParserController m_controller;
	std::unordered_map<std::string, std::filesystem::path> m_settingsList;

	void initializeSettingsList();
};

#endif
