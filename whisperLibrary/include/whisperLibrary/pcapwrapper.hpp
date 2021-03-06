/*	<pcapwrapper.hpp>
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
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PCAPWRAPPER
#define PCAPWRAPPER

#include "common.hpp"
#include <vector>
#include <boost/circular_buffer.hpp>
#include <pcap.h>
#include <string>

#ifndef WIN32
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#define IE_TEMPLATE
#else
	/*	Set the following libs as dependency in the linker
		Ws2_32.lib", "wpcap.lib" */
	#include <winsock2.h>
	#include <ws2tcpip.h>

	#ifdef WHISPER_BUILD
		// dll export (Library)
		#define IE_TEMPLATE
	#else
		// dll import (Executable)
		#define IE_TEMPLATE extern
	#	endif
#endif

/* Define PCAP_NETMASK_UNKNOWN if not included on pcap.h */
#ifndef PCAP_NETMASK_UNKNOWN
	#define PCAP_NETMASK_UNKNOWN    0xffffffff
#endif

#ifdef DEBUG_LEVEL
/* define debug macro
the do {} while(0) ensures that the ; after DEBUG() is always correctly placed.
disabled compiler warning: C4127 (conditional expression is constant) */
#ifdef WIN32
#define DEBUG(MinLevel, message, ...) __pragma( warning( push ) ) \
	__pragma(warning(disable : 4127)) \
do { \
if (DEBUG_LEVEL >= MinLevel) { fprintf(stdout, message, ##__VA_ARGS__); } \
} while (0) __pragma(warning(pop))
#else
#define DEBUG(MinLevel, message, ...)  \
do { if (DEBUG_LEVEL >= MinLevel) { \
	fprintf(stdout, message, ##__VA_ARGS__); } } while (0)
#endif
#else
#define DEBUG(...)
#endif

#ifndef PCAP_NETMASK_UNKNOWN 
#define PCAP_NETMASK_UNKNOWN    0xffffffff 
#endif

IE_TEMPLATE template class WHISPERAPI std::vector<bool>;
IE_TEMPLATE template class WHISPERAPI std::vector<int>;
IE_TEMPLATE template class WHISPERAPI std::vector<std::string>;
IE_TEMPLATE template class WHISPERAPI std::vector< std::vector<std::string> >;
IE_TEMPLATE template class WHISPERAPI std::vector<pcap_t*>;

namespace whisper_library {
		
	class WHISPERAPI PcapWrapper {

		public:
			// Default Constructor
			PcapWrapper();

			// Destructor
			~PcapWrapper();

			typedef struct {
				struct pcap_pkthdr	header;
				const u_char*		payload;
			} PcapPacket;

			#define RETURN_VALUE(return_code, return_value) \
				m_last_return_codes.push_back(return_code); \
				return (return_value);
			#define RETURN_CODE(return_code) RETURN_VALUE(return_code, return_code)

			// Constants (for m_adapter_data access)
			// unsigned int instead of enum for type safety
			static const int					ADAPTER_NAME			= 0;
			static const int					ADAPTER_DESCRIPTION		= 1;
			static const int					ADAPTER_ADDRESS			= 2;
			static const int					DEFAULT_MAXPACKETSIZE	= 65535;
			static const int					RETURN_CODE_BUFFER_SIZE = 20;
			static const int					TIMEOUT_WAIT_FOREVER = 0;

			enum RC{
				NORMAL_EXECUTION			= 0,
				EMPTY_PACKET_DATA			= 1,
				CLOSE_ON_UNOPENED_HANDLE	= 2,
				OPEN_ON_OPENED_HANDLE		= 3,
				NO_ADAPTERS_FOUND			= 255,
				UNSPECIFIED_ERROR_OCCURED	= -1,
				ADAPTER_NOT_FOUND			= -2,
				ACCESS_ON_UNOPENED_HANDLE	= -3,
				ERROR_RETRIEVING_ADAPTERS	= -4,
				ERROR_OPENING_HANDLE		= -5,
				ERROR_COMPILING_FILTER		= -6,
				ERROR_APPLYING_FILTER		= -7,
				OUT_OF_MEMORY				= -254,
			};

			// Getter & Setter
			/**
				\fn int adapterCount()
				\brief retrieve the amount of available network adapters with a valid network address
			*/
			int									adapterCount		();
			/**
				\fn std::vector<std::string> adapterNames()
				\brief retrieve all adapter/device names (e.g. /dev/eth0) from network adapters with a valid network address
			*/
			std::vector<std::string>			adapterNames		(); 
			/**
				\fn std::string adapterName(int adapter_id)
				\brief Get the name from a specific network adapter
			*/
			std::string							adapterName			(int adapter_id);
			/**
			\fn std::vector<std::string> adapterAddresses(int adapter_id)
			\brief Get all network addresses from a specific network adapter
			*/
			std::vector<std::string>			adapterAddresses	(int adapter_id);

			/**
			\fn const std::string adapterDescription(int adapter_id)
			\brief Get the descriptional text from a specific network adapter (under windows e.g. the name of the hardware network device, under unix often <null>)
			*/
			std::string							adapterDescription	(int adapter_id);
			/**
			\fn int adapterId(std::string adapter_value, unsigned int value_type)
			\brief Get the id from an adapter with a specific value\n \
			Value Types:\n ADAPTER_NAME, ADAPTER_DESCRIPTION, ADAPTER_ADDRESS
			*/
			int									adapterId			(std::string adapter_value, int value_type);
			/**
			\fn boost::circular_buffer<int>* returnCodeBuffer()
			\brief Returns a pointer to the global return code buffer
			*/
			std::vector<int>					lastReturnCodes		();

			/**
				\fn void clearReturnCodes()
				\brief empties the PcapWrapper's global return code buffer
			*/
			void								clearReturnCodes	();
			
			/**
				\fn int retrieveAdapters()
				\brief Retrieves available network devices from the (local) machine
				\return
					0	- normal execution,
					-1	- Error occured,
					-2	- Out of memory
					255 - no adapters found
			*/
			int									retrieveAdapters	();
			/**
				\fn int openAdapter(std::string adapterName, int maxPacketSize, int promiscuous)
				\fn int openAdapter(int adapter_id, int max_packet_size, bool promiscuous_mode, int timeout)
				\brief Opens a live capture handle to the given device
				\param std::string	adapter_name		- Name of the adapter (pcap_if_t->name) to open
				\param int			max_packet_size		- Maximum number of bytes that should be captured from each packet. 
				65535 is enough for the whole packet in most networks.
				\param bool			promiscuous_mode 	- Open in promiscuous mode? false: No, true: Yes.
				\param int			timeout				- Specifies a timeout in ms. A value of 0 can cause the process to wait forever.
				promiscuous mode: capture all packets
				non-promiscuous:  capture only packets directed to the application
				\return 0 - normal execution,
				-1 - Error occured
			*/
			int									openAdapter		(std::string adapter_name, int max_packet_size, bool promiscuous_mode, int timeout);
			int									openAdapter		(int adapter_id, int max_packet_size, bool promiscuous_mode, int timeout);
			/**
				\fn int closeAdapter(std::string adapter_name)
				\fn int closeAdapter(int adapter_id)
				\brief Closes an openend handle on the adapter with the given name/id. 
			
			*/
			int									closeAdapter	(std::string adapter_name);
			int									closeAdapter	(int adapter_id);
			/**
				\fn int freeAdapters()
				\brief Frees unopened adapters and closes previously opened handles.
				\return 0 - normal execution,
				-1 Error occured
			*/
			int									freeAdapters	();
			/**
				\fn int applyFilter(int adapter_id, std::string filter)
				\fn int applyFilter(std::string adapter_name, std::string filter)
				\brief Applies a given filter with pcap syntax to the selected adapter
			*/
			int									applyFilter		(int adapter_id, std::string filter);
			int									applyFilter		(std::string adapter_name, std::string filter);
			/**
				\fn int removeFilter(int adapter_id)
				\fn int removeFilter(std::string adapter_name)
				\brief Removes any previously applied filter from the adapter handle
			*/
			int									removeFilter	(int adapter_id);
			int									removeFilter	(std::string adapter_name);

			/**
			\fn PcapPacket retrievePacket(int adapter_id)
			\fn PcapPacket retrievePacket(std::string adapter_name)
			\brief Retrieves the next packet from the capture device
			\return PcapPacket{NULL, NULL} if adapter was not found or if the specified adapter had no open handle \n
			or PcapPacket{pcap_pkthdr, NULL} if no packet passed through the configured filter\n
			or PcapPacket{pcap_pkthdr, NULL} if no packet arrived in a system dependent time window (timeout)\n
			or PcapPacket{pcap_pkthdr, const u_char*} where  const u_char* is the pointer to the packet data with the maximum size configured in openAdapter()
			*/
			PcapPacket							retrievePacket(int adapter_id);
			PcapPacket							retrievePacket(std::string adapter_name);

			/**
			\fn std::vector<bool> retrievePacketAsVector(int adapter_id)
			\fn std::vector<bool> retrievePacketAsVector(std::string adapter_name)
			\brief Calls retrievePacket(adapter_id) and converts the retrieved packet payload in a std::vector<bool>
			\return bitwise representation of the packet payload from retrievePacket() as a std::vector<bool> 
			*/
			std::vector<bool>					retrievePacketAsVector	(int adapter_id);
			std::vector<bool>					retrievePacketAsVector	(std::string adapter_name);

			/**
			\fn sendPacket(int adapter_id, unsigned char* packet_buffer, int buffer_size)
			\fn sendPacket(std::string adapter_name, unsigned char* packet_buffer, int buffer_size)
			\fn sendPacket(int adapter_id, std::vector<bool> packet_data)
			\fn sendPacket(std::string adapter_name, std::vector<bool> packet_data)
			\brief Sends a packet using a raw socket via the WinPcap driver (Note: libcap on linux currently doesn't allow sending packages)
			\return RC.NORMAL_EXECUTION - normal execution
					RC.ADAPTER_NOT_FOUND - adapter with given id not found
					RC.ACCESS_ON_UNOPENED_HANDLE - tried to access an unopened adapter (use openAdapter(...) beforehand)
					RC.UNSPECIFIED_ERROR_OCCURED - otherwise
			*/
			int									sendPacket(int adapter_id, unsigned char* packet_buffer, int buffer_size);
			int									sendPacket(std::string adapter_name, unsigned char* packet_buffer, int buffer_size);

			int									sendPacket(int adapter_id, std::vector<bool> packet_data);
			int									sendPacket(std::string adapter_name, std::vector<bool> packet_data);


	protected:
		/**
		\fn char* ipToString(sockaddr* sockaddr, char* buffer, size_t buffer_size)
		\brief Converts a given socket address to a human readable string
		*/
		char* ipToString(sockaddr* sockaddr, char* buffer, size_t buffer_size);

	private:
		/* adapters_data[x][y]
		x : adapter number
		[x][ADAPTER_NAME] : adapter/device name (e.g. /dev/eth0)
		[x][ADAPTER_DESCRIPTION] : adapter description
		[x][ADAPTER_ADDRESS+] : adapter addresses (NULL if not available)
		*/
		std::vector< std::vector<std::string> >	m_adapter_data;
		std::vector<pcap_t*>					m_adapter_handles;
		std::vector<bpf_u_int32>				m_adapter_netmasks;

		pcap_if_t*								m_adapter_raw_data;
		// Stores the last 20 method return codes
		boost::circular_buffer<int>				m_last_return_codes;

		// checkForAdapterId
		// Checks if a given adapter_id exists.
		bool									checkForAdapterId(int adapter_id);

		/* Private Getter & Setter */
		int										adapterId(std::string value, int key, bool increment_key);
	};
}
#endif // PCAPWRAPPER