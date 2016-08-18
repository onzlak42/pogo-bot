#include "target_strategy.h"




target_strategy::target_strategy(const coordinate &coord)
{
	m_target = coord;
}

boost::optional<coordinate> target_strategy::find_target(client &m_client, const pogo::GetMapObjectsResponse &map)
{
	if (m_target)
	{
		const auto &my_pos = m_client.get_position();
		double dist_target = distance_earth(m_target->latitude, m_target->longitude, my_pos.latitude, my_pos.longitude);
		if (dist_target < 0.005)
		{
			m_target.reset();
		}
	}

	return m_target;
}

void target_strategy::inventory_exec(client &m_client, pogo::GetInventoryResponse &inventory)
{

}

void target_strategy::map_exec(client &m_client, pogo::GetMapObjectsResponse &map)
{

}
