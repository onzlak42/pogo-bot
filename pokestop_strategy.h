#pragma once
#include "ibot_strategy.h"

class pokestop_strategy : public ibot_strategy
{
public:
	boost::optional<coordinate> find_target(client &m_client, const pogo::GetMapObjectsResponse &map) override;

	void inventory_exec(client &m_client, pogo::GetInventoryResponse &inventory) override;

	void map_exec(client &m_client, pogo::GetMapObjectsResponse &map) override;

private:
	bool m_strategy_exec = false;
};

