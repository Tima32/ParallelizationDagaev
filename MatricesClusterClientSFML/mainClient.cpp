#include <iostream>
#include <thread>
#include <SFML/Network.hpp>
#include "Client.hpp"

using namespace std;

Client client;

int main()
{
	cout << "Hello I am MatricesClusterClientSFML!" << endl;

	client.work();
	
	return 0;
}