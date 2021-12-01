#pragma once
#include <vector>
#include <SFML/Network.hpp>

class Server
{
public:
	enum class RequestId : sf::Uint8
	{
		Set,         //(0, 1)
		Mul,         //iy
		WaitAllGet,  //(0, 1, 2)

		IdCount
	};

	Server(sf::Uint16 port = 32050);
	~Server();

	void work();

private:
	void selectorWork();
	void disconected(size_t i);

	void testMul();

	sf::TcpListener listener;
	sf::SocketSelector selector;
	sf::Thread* selector_thread{nullptr};
	std::vector<sf::TcpSocket*> cluster;
	std::vector<size_t> cluster_threads;
	sf::Mutex sluster_mutex;
};