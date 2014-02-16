
#ifndef __WIRESIM_H__
#define __WIRESIM_H__

//#pragma once

#include <stdint.h>

// All node types; requires three bits to represent
enum WireNodeType
{
    cWireNodeType_None = 0, // Acts as insulator material
    cWireNodeType_Wire,     // Acts to carry current
    cWireNodeType_Joint,    // Connects adjacent wires
    cWireNodeType_Switch,   // Transistor switch. receives control from top, and flows on left-to-right
    cWireNodeType_And,      // Bitwise and, receives two wires in quadrants II and III, outputs one wire middle-right
    cWireNodeType_Or,       // Bitwise or, receives two wires "", outputs one wire ""
    cWireNodeType_Not,      // Bitwise not, receives one wire "", outputs one wire ""
    cWireNodeType_Xor,      // Bitwise xor, receives two wires "", outputs one wire ""
};

// A node data structure: the entire grid is made
// of nodes; "empty" nodes are nodes with type "None"
// Current total of bits: 11 bits
struct WireNode
{
    // Power takes two bits, as there are 0-3 (inclusive) levels of power
    // Power only moves to adjacent wire tiles / joints, with anew value of 4, while all other power loses a level of power
    // This creates the "move ahead" logic of power
    // Power only transmites of the current node is on the highest power
    enum { cMaxPower = 3 };
	uint8_t m_power : 2;

    // The type requires three bits (see enum above)
    enum { cTypeCount = 8 };
    uint8_t m_type : 3;

    // Bit-mask of channels; if more than one channel is active on the same node, the power is transmitted!
    uint8_t m_channels : 4;

    // Short-hand helpers
    enum ChannelMask
    {
        cChannelsMaskNone = 0x00,
        cChannelsMask1 = 0x01,
        cChannelsMask2 = 0x02,
        cChannelsMask3 = 0x04,
        cChannelsMask4 = 0x08,
    };

    // Default constructor
    WireNode()
        : m_power( 0 )
        , m_type( 0 )
        , m_channels( 0 )
    { }

    // Constructor with node data
    WireNode( WireNodeType nodeType, uint8_t powerLevel, uint8_t channelsMask )
        : m_power( powerLevel )
        , m_type( (uint8_t)nodeType )
        , m_channels( channelsMask )
    { }
};

class WireSim
{
public:

    WireSim( int width, int height );
    ~WireSim();

    void SetNode( int x, int y, WireNode node, int bufferIndex = -1 );
    WireNode& GetNode( int x, int y, int bufferIndex = -1 );

    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }

    bool InRange( int x, int y ) const;

    void Simulate();

protected:

	WireNode SimulateNode( int x, int y, WireNode (&adjacentNodes)[3][3] );

	// Test the two-point input corners (top-left and bottom-left)
    void TestInputNodes( int x, int y, bool& topLeftPoweredOut, bool& bottomLeftPoweredOut );

	// Spread power to directly adjacent nodes at the target
	void PowerAdjacentNodes( int x, int y, bool respectChannels );

private:

    // Two 2D buffers, indexed by convention [buffer-index][y * m_width + x]
    WireNode* m_nodeBuffer[2];
    int m_width, m_height;
	int m_activeBuffer, m_nextBuffer;

};

#endif
