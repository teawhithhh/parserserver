#include "Server.hxx"
#include <spdlog/spdlog.h>
#include "NodeTranslator.hxx"

TcpServer::TcpServer(int port) : m_port(port), m_controller(), m_settingsList() {}

crow::response GetRequestHandler::handleRequest(const crow::request& req)
{
	nlohmann::json json_request;

	try {
		json_request = nlohmann::json::parse(req.body);
	} catch (nlohmann::json::parse_error& e) {
		spdlog::warn("Error in parsing json request.");
		return crow::response(400, R"({"status":"error","message":"Invalid JSON"})");
	}

	if (!json_request.contains("config"))
	{
		spdlog::warn("Json request not contains \"config\" field.");
		return crow::response(400, R"({"status":"error", "message":"json not contains "config" field.})");
	}
	
	if (json_request["config"] == "glazewm"){
		try {
			Node glazewmConf = controller.read(settingsList.at("glazewm"));
			return crow::response(200, NodeTranslator::translateTJ(glazewmConf));
		} catch (const std::exception& e) {
			spdlog::warn("The internal server error, Error: ", e.what());
			return crow::response(500, R"({"status":"error","message":"Internal server error"})");
		}	
	}

	spdlog::warn("The specified config does not exist.");
	return crow::response(400, R"({"status":"error","error":"config not exists"})");
}

crow::response PushRequestHandler::handleRequest(const crow::request& req)
{
	try {
		nlohmann::json json_request;
		try {
			json_request = nlohmann::json::parse(req.body);
		} catch (nlohmann::json::parse_error& e) {
			std::ostringstream oss;
			spdlog::warn("Error in json at byte: {}", e.byte);
			return crow::response(400, R"({"status":"error","message":"Error in json"})");
		}

		Node root = NodeTranslator::translateTHM(json_request);
		controller.write(settingsList.at("glazewm"), root);

		spdlog::info("Configuration been successfully changed.");
		return crow::response(200, R"({"status":"success"})");
	} catch (const std::exception& e) {
		spdlog::warn("Internal server error, Error: {}", e.what());
		return crow::response(500, R"({"status":"error","message":"Internal server error"})");
	}
}

void RouteManager::registryRoute(const std::string& route, std::unique_ptr<IRequestHandler> handler)
{
	routes[route] = std::move(handler);
}


void RouteManager::applyRoutes(crow::SimpleApp& app) {
	for (auto& [route, handler] : routes) {
		app.route_dynamic(route)
			.methods("POST"_method)
			([handler = handler.get()](const crow::request& req) {
				return handler->handleRequest(req);
			});
	}
}

void TcpServer::setupRoutes()
{
	m_manager.registryRoute("/send_settings", std::make_unique<PushRequestHandler>(m_settingsList, m_controller));
	m_manager.registryRoute("/get_settings", std::make_unique<GetRequestHandler>(m_settingsList, m_controller));
	m_manager.applyRoutes(m_app);
}

void TcpServer::run()
{
	setupController();
	m_app.port(m_port).multithreaded().run();
}

void TcpServer::setupController()
{
	setupSettingsList();
	m_controller.addParser(m_settingsList["glazewm"]);
}

void TcpServer::setupSettingsList()
{
	m_settingsList["glazewm"] = ParserInfo(ParserFormat::yaml, "C:/Users/240821/testconfig.yaml");
}
