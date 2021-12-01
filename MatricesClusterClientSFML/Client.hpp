#pragma once
#include <vector>
#include <SFML/Network.hpp>
#include "../ClusterMatrix/ClusterMatrix.hpp"

class Client
{
public:
	enum class RequestId : sf::Uint8
	{
		Set,         //(0, 1), matrix
		Mul,         //iy
		WaitAllGet,  //(0, 1, 2)

		IdCount
	};

	Client();
	~Client();

	void work();

private:
	void commandSet(sf::Packet& packet);
	void commandMul(sf::Packet& packet);
	void commandWaitAllGet(sf::Packet& packet);

	sf::TcpSocket socket;
	std::mutex matrix_mutex;
	PD::ClusterMatrix a; //0
	PD::ClusterMatrix b; //1
	PD::ClusterMatrix r; //2
};