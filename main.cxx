#include "Server.hxx"

int main()
{
	TcpServer server(8080);

	server.setupRoutes(); // Настройка маршрутов
	server.run();         // Запуск сервера

	return 0;
}
