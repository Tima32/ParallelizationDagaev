#include <iostream>
#include <thread>
#include "Server.hpp"
#include "../ClusterMatrix/ClusterMatrix.hpp"

using namespace std;
using namespace sf;

Server::Server(sf::Uint16 port)
{
	auto listener_status = listener.listen(32050);
	cout << "<Server::Server>Info: listener status " << size_t(listener_status) << endl;
}
Server::~Server()
{
	for (auto c : cluster)
		delete c;
}

void Server::work()
{
	selector_thread = new Thread(&Server::selectorWork, this);
	selector_thread->launch();
	while (true)
	{
		TcpSocket* socket = new TcpSocket;
		auto status = listener.accept(*socket);
		cout << "Status accept: " << status << endl;
		sluster_mutex.lock();
		cluster.push_back(socket);
		cluster_threads.push_back(0);
		selector.add(*socket);
		std::cout << "New client connected: " << socket->getRemoteAddress() << std::endl;
		sluster_mutex.unlock();

		if (cluster.size() == 1)
		{
			this_thread::sleep_for(1s);
			testMul();
		}
	}
}

//private
void Server::selectorWork()
{
	cout << "Selector started" << endl;

	while (1)
	{
		auto status = selector.wait();
		//cout << "Selector wait status " << status << endl;

		sluster_mutex.lock();
		for (size_t i = 0; i < cluster.size(); ++i)
		{
			if (!selector.isReady(*cluster[i]))
				continue;
			if (cluster_threads[i] != 0)
				continue;

			cout << "Client id: " << i << endl;
			Packet pak;
			auto status = cluster[i]->receive(pak);
			if (status == Socket::Disconnected)
			{
				disconected(i);
				--i;
				continue;
			}
			if (cluster_threads[i] == 0)
			{
				pak >> cluster_threads[i];
				cout << "Client threads: " << cluster_threads[i] << endl;
			}

			// Send an answer
			/*std::string message = "Welcome, client";
			cluster[i]->send(message.c_str(), message.size() + 1);*/
		}
		sluster_mutex.unlock();
		this_thread::sleep_for(10ms);
	}

	cout << "Selector stoped" << endl;
}
void Server::disconected(size_t i)
{
	selector.remove(*cluster[i]);
	delete cluster[i];
	cluster.erase(cluster.begin() + i);
	cluster_threads.erase(cluster_threads.begin() + i);
	cout << "Desconection: " << i << endl;
}

void Server::testMul()
{
	PD::ClusterMatrix a(3, 3);
	PD::ClusterMatrix b(3, 3);

	a[0][0] = 1;
	a[0][1] = 2;
	a[0][2] = 3;
	a[1][0] = 4;
	a[1][1] = 5;
	a[1][2] = 6;
	a[2][0] = 7;
	a[2][1] = 8;
	a[2][2] = 9;

	b[0][0] = 1;
	b[0][1] = 2;
	b[0][2] = 3;
	b[1][0] = 4;
	b[1][1] = 5;
	b[1][2] = 6;
	b[2][0] = 7;
	b[2][1] = 8;
	b[2][2] = 9;

	sf::Packet s_a;
	sf::Packet s_b;
	s_a << Uint8(RequestId::Set) << sf::Uint64(0) << a;
	s_b << Uint8(RequestId::Set) << sf::Uint64(1) << b;

	sluster_mutex.lock();
	for (size_t i = 0; i < cluster.size(); ++i)
	{
		cluster[i]->send(s_a);
		cluster[i]->send(s_b);

		Packet m;
		this_thread::sleep_for(1s);
		m << sf::Uint8(RequestId::Mul) << Uint64(0);
		cluster[i]->send(m);
		m.clear();
		this_thread::sleep_for(2s);
		m << sf::Uint8(RequestId::Mul) << Uint64(1);
		cluster[i]->send(m);
		m.clear();
		this_thread::sleep_for(2s);
		m << Uint8(RequestId::Mul) << Uint64(2);
		cluster[i]->send(m);
		this_thread::sleep_for(2s);

		Packet get;
		get << Uint8(RequestId::WaitAllGet);
		cluster[i]->send(get);

		Packet answer;
		PD::ClusterMatrix r;
		if (cluster[i]->receive(answer))
			continue;
		answer >> r;
		cout << "Answer:" << endl;
		cout << r << endl;
	}
	sluster_mutex.unlock();
}
