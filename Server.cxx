#include "Server.hxx"
#include <spdlog/spdlog.h>

TcpServer::TcpServer(int port) : m_port(port), m_controller(), m_settingsList() {}

crow::json::wvalue HashMapTranslator::translateTJ(const std::unordered_map<std::string, std::string>& tbl) {
	crow::json::wvalue json_response;

	for (const auto& pair : tbl) {
		json_response[pair.first] = pair.second;
	}

	return json_response;
}

std::unordered_map<std::string, std::string> HashMapTranslator::translateTHM(const crow::json::wvalue& json) {
	std::unordered_map<std::string, std::string> result;

	try {
		auto json_request = crow::json::load(json.dump());
		if (!json_request) {
				throw std::invalid_argument("Input JSON is not a valid object");
		}

		for (auto& it : json_request) {
				result[it.key()] = it.s();
		}
	} catch (const std::exception& e) {
		throw std::invalid_argument("Invalid JSON format: " + std::string(e.what()));
	}

	return result;
}

crow::response GetRequestHandler::handleRequest(const crow::request& req)
{
	try {
		auto json_request = crow::json::load(req.body);
		if (!json_request) {
			return crow::response(400, R"({"status":"error","message":"Invalid JSON"})");
		}

		crow::json::wvalue json_response;
		ParserInfo glazewm(ParserFormat::yaml, settingsList["glazewm"]);
		std::unordered_map<std::string, std::string> glazewmConf = controller.read(glazewm);
		spdlog::info("glazewm controller created");
		glazewmConf["status"] = "success";
		controller.write(glazewm, glazewmConf);
		/* json_response["status"] = "success"; */
		return crow::response(HashMapTranslator::translateTJ(glazewmConf));
	} catch (const std::exception& e) {
		return crow::response(500, R"({"status":"error","message":"Internal server error"})");
	}
}

crow::response PushRequestHandler::handleRequest(const crow::request& req)
{
	try {
		auto json_request = crow::json::load(req.body);
		if (!json_request) {
			return crow::response(400, R"({"status":"error","message":"Invalid JSON"})");
		}

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
