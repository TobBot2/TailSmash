#include <thread>
#include <iostream>

#include "Server.h"

#include "Manager.h"
#include "Player.h"

Server::Server(Manager& manager) {
	socket.setBlocking(false);
	openToConn = false;
	id = 0;
	port = 0;
	this->onreceive = std::bind(&Manager::receivePacket, &manager, std::placeholders::_1);
	closeThread = false;
}
Server::~Server() {
	{ 
		Guard g(&closeThreadMut);
		closeThread = true;
	}
	listenThread.join();
}

void Server::open() {
	if (openToConn) return;

	port = sf::UdpSocket::AnyPort;
	socket.bind(port);
	listenThread = std::thread(&Server::update, this);
	//publicIp = sf::IpAddress::getPublicAddress(sf::seconds(10.f));
	publicIp = sf::IpAddress::getLocalAddress();
	port = socket.getLocalPort();
	//std::cout << "got ip: " << publicIp.toString() << std::endl;

	if (publicIp != sf::IpAddress::None) {
		openToConn = true;
	}

	srand(time(0));
	id = rand();
}

void Server::sendPlayer(Player* player) {
	sf::Packet packet;
	sf::Vector2f bodyPos = player->getShape().getPosition();

	packet << id << 0; // 0 is the flag for updating the player
	packet << bodyPos.x << bodyPos.y << player->getShape().getRotation();
	packet << player->getTailPos().x << player->getTailPos().y;

	for (int i = 0; i < peers.size(); i++) {
		socket.send(packet, peers[i].first, peers[i].second);
	}
}

bool Server::connect(sf::IpAddress peer, unsigned short peerPort) {
	sf::Clock timer;
	int attempts = 10;

	bool received = false;
	
	sf::Packet packet;
	packet << id << sf::Uint8(1); // 1 is the flag for connecting
	packet << publicIp.toInteger() << port;

	// wait x seconds to hear a response
	// udp hole punching??
	std::cout << "sending to " << peer.toString() << ":" << peerPort << std::endl;
	while (attempts > 0) {
		if (socket.send(packet, peer, peerPort) == sf::Socket::Done) {
			std::cout << "packet succesfully sent\n";
		}
		else {
			std::cout << "error sending packet\n";
		}
		if (!received) {
			if (socket.receive(packet, peer, peerPort) == sf::Socket::Done) {
				std::cout << "received\n";
				if (onreceive(packet)) {
					peers.push_back(std::make_pair(peer, peerPort));
					received = true;
				}
			}
		}

		while (timer.getElapsedTime().asSeconds() < 1.f) { /* wait */ }
		timer.restart();
		attempts--;
	}
	std::cout << "failed.\n";

	return false;
}

void Server::disconnect() {
	Guard g(&mut);
	for (int i = peers.size() - 1; i >= 0; i--) {
		sf::Packet packet;
		packet << id << 2; // 2 is the flag for disconnecting
		packet << publicIp.toInteger() << port;

		socket.send(packet, peers.back().first, peers.back().second);
		peers.pop_back();
	}
}

std::pair<sf::IpAddress, unsigned short> Server::getIp()
{
	if (openToConn) return std::pair<sf::IpAddress, unsigned short>(publicIp, port);
	else return std::pair<sf::IpAddress, unsigned short>();
}

void Server::update() {
	sf::Packet packet;
	while (true) {
		{
			Guard g(&mut);
			for (int i = 0; i < peers.size(); i++) {
				if (socket.receive(packet, peers[i].first, peers[i].second)) {
					onreceive(packet);
				}
			}
		}

		{
			Guard g(&closeThreadMut);
			if (closeThread) {
				return;
			}
		}
	}
}
