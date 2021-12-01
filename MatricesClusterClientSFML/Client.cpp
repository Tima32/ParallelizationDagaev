#include <iostream>
#include <thread>
#include "Client.hpp"

using namespace std;
using namespace sf;

Client::Client()
{
}

Client::~Client()
{
}

void Client::work()
{
	connect:
	auto status = socket.connect("192.168.0.2", 32050);
	if (status)
	{
		cout << "Connect fail, try again." << endl;
		goto connect;
	}

	Packet thread_info;
	thread_info << size_t(thread::hardware_concurrency());
	status = socket.send(thread_info);
	cout << "Send status" << status << endl;
	if (status)
		goto connect;

	//Ожидание команд
	while (true)
	{
		Packet packet;
		if (socket.receive(packet))
			goto connect;

		sf::Uint8 id(-1);
		packet >> id;
		if (RequestId(id) == RequestId::Set)
		{
			commandSet(packet);
		}
		if (RequestId(id) == RequestId::Mul)
		{
			commandMul(packet);
		}
		if (RequestId(id) == RequestId::WaitAllGet)
		{
			commandWaitAllGet(packet);
		}
	}
}


//private
void Client::commandSet(sf::Packet& packet)
{
	sf::Uint64 matrix_num(-1);
	packet >> matrix_num;

	switch (matrix_num)
	{
	case(0):
		packet >> a;
		cout << "Matrix a:" << endl;
		cout << a << endl;
		break;
	case(1):
		packet >> b;
		cout << "Matrix b:" << endl;
		cout << b << endl;
		break;
	case(2):
		packet >> r;
		cout << "Matrix r:" << endl;
		cout << r << endl;
		break;
	default:
		break;
	}
}
void Client::commandMul(sf::Packet& packet)
{
	sf::Uint64 y;
	packet >> y;
	
	matrix_mutex.lock();
	{
		if (r.getSizeX() != b.getSizeX() || r.getSizeY() != a.getSizeY())
			r.setSize(b.getSizeX(), a.getSizeY());
	}
	matrix_mutex.unlock();

	PD::ClusterMatrix::pool.push_task([this, y]()
		{
			cout << "T!" << endl;
			auto res = a.mul(b, y);
			cout << "T&" << endl;
			matrix_mutex.lock();
			cout << "T*" << endl;
			r = r + res;
			matrix_mutex.unlock();
			cout << "T#" << endl;
		});
}
void Client::commandWaitAllGet(sf::Packet& packet)
{
	PD::ClusterMatrix::pool.wait_for_tasks();

	Packet answer;
	answer << r;
	socket.send(answer);
}