#pragma once
#include <SFML/Network.hpp>
#include <functional>
#include <mutex>

class Player;
class Manager;
class Server
{
public:
	struct Guard {
		std::mutex* mut;
		Guard(std::mutex* mut) {
			this->mut = mut;
			this->mut->lock();
		}
		~Guard() {
			mut->unlock();
		}
	};

	Server(Manager&);
	~Server();

	void open();
	void sendPlayer(Player* player);
	bool connect(sf::IpAddress peer, unsigned short port);
	void disconnect();

	std::pair<sf::IpAddress, unsigned short> getIp();

	void update();
	std::mutex* getMutex() { return &mut; }
private:
	sf::Uint32 id;
	bool openToConn;
	sf::IpAddress publicIp;
	unsigned short port;

	std::mutex closeThreadMut;
	bool closeThread;
	std::thread listenThread;

	std::function<bool(sf::Packet)> onreceive;
	sf::UdpSocket socket;
	std::mutex mut;

	std::vector<std::pair<sf::IpAddress, unsigned short>> peers;
	std::vector<std::pair<sf::IpAddress, unsigned short>> peersBuffer;
};