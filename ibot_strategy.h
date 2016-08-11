#pragma once

#include "client.h"
#include "pogo_proto.h"
#include <boost/optional.hpp>
#include "util.h"

class ibot_strategy
{
public:
	virtual ~ibot_strategy() {};

	virtual boost::optional<coordinate> find_target(client &m_client, const pogo::GetMapObjectsResponse &map) = 0;

	virtual void inventory_exec(client &m_client, pogo::GetInventoryResponse &inventory) = 0;

	virtual void map_exec(client &m_client, pogo::GetMapObjectsResponse &map) = 0;
};

