#include <iostream>
#include "Server.hpp"

using namespace std;

Server server;

int main()
{
	cout << "Hello I am MatricesClusterServerSFML!" << endl;
	
	server.work();

	return 0;
}