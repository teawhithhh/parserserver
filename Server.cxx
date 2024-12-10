#include "Server.hxx"
#include <spdlog/spdlog.h>
#include "NodeTranslator.hxx"

TcpServer::TcpServer(int port) : m_port(port), m_controller(), m_settingsList() {}

crow::response GetRequestHandler::handleRequest(const crow::request& req)
{
	auto json_request = crow::json::load(req.body);
	if (!json_request) {
		return crow::response(400, R"({"status":"error","message":"Invalid JSON"})");
	}
	
	if (json_request.has("config") && json_request["config"] == "glazewm"){
		try {
			ParserInfo glazewm(ParserFormat::yaml, settingsList["glazewm"]);
			Node glazewmConf = controller.read(glazewm);

			return crow::response(200, NodeTranslator::translateTJ(glazewmConf));
		} catch (const std::exception& e) {
			return crow::response(500, R"({"status":"error","message":"Internal server error"})");
		}	
	}

	return crow::response(400, R"({"status":"error","error":"config not exists"})");
}

crow::response PushRequestHandler::handleRequest(const crow::request& req)
{
	try {
		nlohmann::json json_request = nlohmann::json::parse(req.body);
		Node root = NodeTranslator::translateTHM(json_request);

		spdlog::info("send write command");
		controller.write(ParserInfo(ParserFormat::yaml, settingsList["glazewm"]), root);

		crow::json::wvalue json_response;
		json_response["status"] = "success";
		return crow::response(json_response);
	} catch (const std::exception& e) {
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
	ParserInfo glazewm(ParserFormat::yaml, m_settingsList["glazewm"]);
	m_controller.addParser(glazewm);
}

void TcpServer::setupSettingsList()
{
	m_settingsList["glazewm"] = "C:/Users/240821/testconfig.yaml";
	m_settingsList["zebar"] = "";
}
