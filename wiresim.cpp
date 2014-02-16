
#include <cstring>

#include "wiresim.h"

WireSim::WireSim( int width, int height )
    : m_width( width )
    , m_height( height )
	, m_activeBuffer( 0 )
    , m_nextBuffer( 1 )
{
	for( int i = 0; i < 2; i++ )
	{
		m_nodeBuffer[ i ] = new WireNode[ width * height ];
		memset( (void*)m_nodeBuffer[ i ], 0, sizeof(WireNode) * width * height );
	}
}

WireSim::~WireSim()
{
	for( int i = 0; i < 2; i++ )
	{
		delete[] m_nodeBuffer[ i ];
	}
}

void WireSim::SetNode( int x, int y, WireNode node, int bufferIndex )
{
    m_nodeBuffer[ bufferIndex >= 0 ? bufferIndex : m_activeBuffer ][ y * m_width + x ] = node;
}

WireNode& WireSim::GetNode( int x, int y, int bufferIndex )
{
    return m_nodeBuffer[ bufferIndex >= 0 ? bufferIndex : m_activeBuffer ][ y * m_width + x ];
}

bool WireSim::InRange( int x, int y ) const
{
    return ( x >= 0 && y >= 0 && x < m_width && y < m_height );
}

void WireSim::Simulate()
{
	// Clear next buffer
	m_nextBuffer = ( m_activeBuffer + 1 ) % 2;
    memset( (void*)m_nodeBuffer[ m_nextBuffer ], 0, sizeof(WireNode) * m_width * m_height );

	WireNode cDefaultNode;

	// For each source element, simulate results from active to next buffer, then flip
	for( int y = 0; y < m_height; y++)
	{
		for( int x = 0; x < m_width; x++)
		{
			// Retrieve all adjacent cells, placing into list with our cell in the middle;
			// Any cells out of range will point to a default empty cell
            WireNode wireNodes[3][3];
			for( int dx = -1; dx <= 1; dx++ )
			{
				for( int dy = -1; dy <= 1; dy++ )
				{
					if( InRange( dx + 1, dy + 1 ) )
					{
						wireNodes[ dy + 1 ][ dx + 1 ] = GetNode( x + dx, y + dy );
					}
					else
					{
						wireNodes[ dy + 1 ][ dx + 1 ] = cDefaultNode;
					}
				}
			}

			// Simulate this specific node and the surrounding set
            WireNode resultNode = SimulateNode( x, y, wireNodes );
			WireNode& outputNode = GetNode( x, y, m_nextBuffer );

			// Copy type and mask (this never changes)
			outputNode.m_type = resultNode.m_type;
			outputNode.m_channels = resultNode.m_channels;

            // Only copy power change based on mask rules: only copy if they share a mask
            if( (outputNode.m_channels & resultNode.m_channels) != 0 )
            {
                // Always copy the biggest power value
                outputNode.m_power |= resultNode.m_power;
            }
        }
    }

    // Flip buffers
    m_activeBuffer = m_nextBuffer;
}

WireNode WireSim::SimulateNode( int x, int y, WireNode (&adjacentNodes)[3][3] )
{
	WireNode result = adjacentNodes[1][1];

    bool bottom, top;

	// 1. Test to produce any power
    switch( WireNodeType( result.m_type ) )
	{
    case cWireNodeType_None:
		result.m_power = 0;
		break;
		
    case cWireNodeType_Wire:
		// Spread to only directly-adjcent wires
		PowerAdjacentNodes( x, y, true );
		break;

    case cWireNodeType_Joint:
		PowerAdjacentNodes( x, y, false );
		break;
		
    case cWireNodeType_Switch: // Logically equivallent
    case cWireNodeType_And:
        // Signal is below, control is top
        TestInputNodes( x, y, top, bottom );
		if( top && bottom ) PowerAdjacentNodes( x, y, false );
		
    case cWireNodeType_Or:
        TestInputNodes( x, y, top, bottom );
		if( top || bottom ) PowerAdjacentNodes( x, y, false );
		break;

    case cWireNodeType_Not:
		if( GetNode( x, y ).m_power == 0 ) PowerAdjacentNodes( x, y, false );
		break;
		
    case cWireNodeType_Xor:
        TestInputNodes( x, y, top, bottom );
		if( top != bottom ) PowerAdjacentNodes( x, y, false );
		break;
	}

	// 2. Spread if power is high enough
    if( result.m_power == WireNode::cMaxPower )
	{
		PowerAdjacentNodes( x, y, false );
	}

	// 3. Power decays, but limited to 0
	result.m_power = result.m_power ? (result.m_power - 1) : 0;

	return result;
}

void WireSim::TestInputNodes( int x, int y, bool& topLeftPoweredOut, bool& bottomLeftPoweredOut )
{
	bottomLeftPoweredOut = topLeftPoweredOut = false;
    topLeftPoweredOut = InRange( x - 1, y - 1 ) && ( GetNode( x, y, m_nextBuffer ).m_power > 0 );
    bottomLeftPoweredOut = InRange( x - 1, y + 1 ) && ( GetNode( x, y, m_nextBuffer ).m_power > 0 );
}

void WireSim::PowerAdjacentNodes( int x, int y, bool respectChannels )
{
	WireNode activeNode = GetNode( x, y, m_activeBuffer );
	for( int dx = -1; dx <= 1; dx++ )
	{
		for( int dy = -1; dy <= 1; dy++ )
		{
			// Only directly-adjacent
			if( ( dx == 0 || dy == 0 ) && InRange( x + dx, y + dy ) )
			{
				// Affect anything except none-type and only no-powered nodes
				WireNode& nextNode = GetNode( x + dx, y + dy, m_nextBuffer );

                bool isNoneType = WireNodeType(nextNode.m_type) == cWireNodeType_None;
				bool sharesChannels = ( nextNode.m_channels & activeNode.m_channels ) != 0;

				if( ( !respectChannels || ( respectChannels && sharesChannels ) ) &&
					( !isNoneType && nextNode.m_power == 0 ) )
				{
					// Max out power
					nextNode.m_power |= WireNode::cMaxPower;
				}
			}
		}
	}
}
