#ifndef CHANNEL_MANAGER
#define CHANNEL_MANAGER

#include "common.hpp"
#include <vector>
#include "tcppacket.hpp"

namespace whisper_library {

class CovertChannel;
class SocketConnector;

/*
	A ChannelManager is the connecting part for the covert channel and the framework. You can add and remove
	covert channels you want to use. You pass the data you want to send to this class.

	It is connected to the network via a SocketConnector.
*/
class WHISPERAPI ChannelManager {

public:
	// constructor
	ChannelManager();
	// adds a channel to the available channels
	void addChannel(CovertChannel* channel);
	void selectChannel(int index);
	//removes a channel from the available channels
	void removeChannel(CovertChannel* channel);
	void outputMessage(std::string message);
	TcpPacket getTcpPacket();
	void packetReceived(TcpPacket packet);
	SocketConnector* sender();
	
private:
	// hold all available channels
	std::vector<CovertChannel*> m_channels;
	CovertChannel* m_current_channel;
	// connects the ChannelManager to the network
	SocketConnector* m_socket;
};
}
#endif // CHANNEL_MANAGER