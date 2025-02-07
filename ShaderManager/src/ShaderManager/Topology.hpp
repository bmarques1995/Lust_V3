#pragma once

namespace Lust
{
	enum class Topology
	{
		LUST_TOPOLOGY_POINTLIST,
		LUST_TOPOLOGY_LINELIST,
		LUST_TOPOLOGY_LINELIST_ADJ,
		LUST_TOPOLOGY_LINESTRIP,
		LUST_TOPOLOGY_LINESTRIP_ADJ,
		LUST_TOPOLOGY_TRIANGLELIST,
		LUST_TOPOLOGY_TRIANGLELIST_ADJ,
		LUST_TOPOLOGY_TRIANGLESTRIP,
		LUST_TOPOLOGY_TRIANGLESTRIP_ADJ,
		LUST_TOPOLOGY_TRIANGLE_FAN,
	};
}