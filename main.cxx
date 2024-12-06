#include "ParsersController.hxx"
#include "spdlog/spdlog.h"
#include "Server.hxx"

int main()
{
/*
сделай завтра что-бы фронт отсылал джсонку а в джсонке каждое поле с настройков называлось путем к файлу в котором оно находиться
*/
	ParserController controller;

	ParserInfo testParser(ParserFormat::yaml, "C:/Users/240821/electron/electron-app/cxx-back/testconfig.yaml");
	controller.addParser(testParser);
	auto config = controller.read(testParser);
	for (const auto& [key, val] : config)
	{
		std::cout << key << " : " << val << std::endl;
	}

	/* TcpServer server(8080); */

	/* server.setupRoutes(); // Настройка маршрутов */
	/* server.run();         // Запуск сервера */

	return 0;
}
