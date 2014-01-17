//author: Jan Simon Bunten

#ifndef TCP_PACKET
#define TCP_PACKET

#include "common.hpp"
#include <vector>
#include <bitset>



using namespace std;
typedef unsigned int uint;
typedef unsigned long ulong;

namespace whisper_library {
	// This class represents a tcp packet. All fields are accessable.
    class /*WHISPERAPI*/ TcpPacket {
	public:
		// The empty constructor creates an empty tcp packet.
        TcpPacket();
        /* This constructor is used to fill in some of the important fields
         * beforehand.
         */
        TcpPacket(uint inSourcePort, 
					uint inDestPort, 
					ulong inSequenceNumber,
					ulong inAckNumber,
					bitset<4> inDataOffset,
					uint inWindowSize,
					vector<bool> inOptions);
		// start of the getters
        uint sourcePort();
        uint destPort();
        ulong sequenceNumber();
        ulong acknowlageNumber();
        bitset<4> dataOffset();
        bitset<3> reserved();
        bitset<9> flags();
        bool ns();
        bool cwr();
        bool ece();
        bool urg();
        bool ack();
        bool psh();
        bool rst();
        bool syn();
        bool fin();
        uint windowSize();
        uint checksum();
        uint urgentPointer();
        vector<bool> options();
        vector<bool> packet();
        vector<bool> data();
		// start of the setters
		void setSourcePort(uint val);
		void setDestPort(uint val);
		void setSequenceNumber(ulong val);
		void setAcknowlageNumber(ulong val);
		void setDataOffset(bitset<4> val);
		void setReserved(bitset<3> val);
        void setFlags(bitset<9> val);
		void setNs(bool val);
		void setCwr(bool val);
		void setEce(bool val);
		void setUrg(bool val);
		void setAck(bool val);
		void setPsh(bool val);
		void setRst(bool val);
		void setSyn(bool val);
		void setFin(bool val);
		void setWindowSize(uint val);
		void setChecksum(uint val);
        void setUrgentPointer(uint val);
        void setOptions(vector<bool> val);
        void setData(vector<bool> val);
        void setPacket(vector<bool> val);
        
        // other public functions
        
        /* This function is used to calculate the tcp checksum.
         * To do this it needs some additional information of the IP header as parameters.
         */
        void calculateChecksum(int sourceIp, int destIp, int reservedBits, int protocol);


	private:
        vector<bool> m_header;
        vector<bool> m_options;
        vector<bool> m_data;
        /* This function is used to convert a vector into an unsigned integer value.
         * The parameters are start and end of the number in the given vector
         * and the vector the number is stored in.
         */
        ulong vectorToULong(int start, int end, vector<bool> &vec);
        /* This function is used to insert an integer value into a given boolean vector.
         * The parameters needed are the start and end of where to put the integer
         * into the vector, the vector itself and the value to be inserted.
         */
        template <class T> void uIntToVector(int start, int end, vector<bool> &vec, T val);
        // This function is used to convert a given integer value into a boolean vector.
        template <class T> vector<bool> intToBoolVector(T val);
        // This function uses the one complement addition to add to boolean vectors of any size
        vector<bool> oneComplementAdd(vector<bool> vec1, vector<bool> vec2);
		// This function splits a 32 bit boolean vector into two 16 bit vectors
		vector<vector<bool> > split32BitVector(vector<bool> vec);
		// This function splits the complete packet into a vector of 16bit boolean vectors.
		vector<vector<bool> > splitHeaderTo16Bit();
	};
}
#endif
