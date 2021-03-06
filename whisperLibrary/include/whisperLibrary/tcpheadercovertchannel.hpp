/*	<tcpheadercovertchannel.hpp>
	Copyright(C) 2013,2014  Jan Simon Bunten
							Simon Kadel
							Martin Sven Oehler
							Arne Sven Stühlmeyer

	This File is part of the WhisperLibrary

	WhisperLibrary is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	WhisperLibrary is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef TCP_HEADER_COVERT_CHANNEL
#define TCP_HEADER_COVERT_CHANNEL

#include "common.hpp"
#include "../../src/tcppacket.hpp"
#include "genericpacket.hpp"
#include <string>
#include <vector>
#include <bitset>
#include "bitSetCoder.hpp"
#include "covertchannel.hpp"
#include <functional>

using namespace std;

namespace whisper_library {

	/*
		Covert Channel, that uses 3 bits of the TCP Header, that are not used, to hide a message.
		To use this class, call 'sendMessage' with your message as a string. Received packets are used to call 'receiveMessage'.
		The decoded message is given to the callback function 'm_output'.
	*/
	class TcpHeaderCovertChannel : public CovertChannel {
	public:
		/*
			Constructor
			output is a function pointer that is called, when a complete message arrived. Its parameter is this message.
			send is a function pointer that is called to send a TcpPacket via the socket.
			getPacket is a function pointer, that has to return a valid TcpPacket, that is used to insert the data.
		*/
		TcpHeaderCovertChannel(function<void(string)> output, function<void(TcpPacket)> send, function<TcpPacket(void)> getPacket)
			: CovertChannel(),
				m_remaining_packets(0), 
				m_output(output), 
				m_send(send),
				m_getPacket(getPacket) {};

		//	SendMessage sends a message of type string through the Tcp Header Covert Channel.
		void sendMessage(string message);

		/*
			This function is called, when a new packet arrives at the socket. 
			It collects them and returns the message via the callback function "m_output".
		*/
		void receiveMessage(GenericPacket& packet);

		// No available arguments - empty function
		void setArguments(string arguments) {};

		// Returns a string with the name of the covert channel "TCP Header Covert Channel"
		string name() const;

		// Returns a string with basic information about the tcp header covert channel
		string info() const;

		// Returns the protocol used by this covert channel (tcp)
		string protocol() const;

		// Returns the used port (8080)
		short port() const;

	private:
		//	encodeMessageWithLength splits the message into parts of 3 bits and adds length blocks inbetween.
		vector<bitset<3> > encodeMessageWithLength(string message);

		//	modifyTcpPacket sets the reserved bits of packet to the value of data.
		void modifyTcpPacket(TcpPacket& packet, bitset<3> data);

		// extractData extracts the data we hid in the packet.
		bitset<3> extractData(TcpPacket& packet);

		// This vector stores the bit blocks we received in the current communication
		vector<bitset<3> > m_data_blocks;

		/* 
			Stores the number of packets, we expect to receive in the current communication. 
			0 means, all packets were received and the channel is ready to receive a new length packet.
		*/
		int m_remaining_packets;

		// The encoder/decoder we use, to split messages into bit blocks
		BitSetCoder<3> m_coder;

		// callback function pointer that is used to return received messages as a string
		function<void(string)> m_output;

		// function pointer that is used to send Tcp Packets via the socket
		function<void(TcpPacket)> m_send;

		// function pointer that is used to retrieve valid tcp packets, that are used to hide the data
		function<TcpPacket(void)> m_getPacket;
	};
}
#endif // TCP_HEADER_COVERT_CHANNEL