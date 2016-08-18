#include "pokestop_strategy.h"
#include <iostream>


boost::optional<coordinate> pokestop_strategy::find_target(client &m_client, const pogo::GetMapObjectsResponse &map)
{
	boost::optional<coordinate> target;
	if (!m_strategy_exec)
		return target;
	double min_dist = 0.0;

	for (const auto &c : map.map_cells())
	{
		for (const auto &f : c.forts())
		{
			if (f.type() == pogo::FortType::CHECKPOINT && f.cooldown_complete_timestamp_ms() < timestamp())
			{
				auto &client_pos = m_client.get_position();
				auto dist = distance_earth(f.latitude(), f.longitude(), client_pos.latitude, client_pos.longitude);
				if (!target)
				{
					min_dist = dist;
					target = { f.latitude() , f.longitude() , 0 };
				}
				else if (min_dist > dist)
				{
					min_dist = dist;
					target = { f.latitude() , f.longitude() , 0 };
				}
			}
		}
	}

// 	if (target)
// 	{
// 		std::cout << "next target: [" << target->latitude << "][" << target->longitude << "]"
// 			" dist: " << static_cast<uint64_t>(min_dist * 1000.0) << std::endl;
// 	}
	return target;
}

void pokestop_strategy::inventory_exec(client &m_client, pogo::GetInventoryResponse &inventory)
{
	int32_t pokeball_count = 0;
	for (const auto &i : inventory.inventory_delta().inventory_items())
	{
		if (i.has_inventory_item_data())
		{
			if (i.inventory_item_data().has_item())
			{
				const auto &item = i.inventory_item_data().item();
				if (item.item_id() == pogo::ITEM_POKE_BALL)
				{
					pokeball_count = item.count();
				}
			}
		}
	}

	if (pokeball_count < 30)
	{
		m_strategy_exec = true;
	}
	else if(pokeball_count > 60)
	{
		m_strategy_exec = false;
	}

	//m_strategy_exec = true;
}

void pokestop_strategy::map_exec(client &m_client, pogo::GetMapObjectsResponse &map)
{
	const uint64_t pokestop_cooldown = 1000 * 60 * 5;

	for (auto &c : *map.mutable_map_cells())
	{
		for (auto &f : *c.mutable_forts())
		{
			if (f.type() == pogo::FortType::CHECKPOINT && f.cooldown_complete_timestamp_ms() < timestamp())
			{
				auto &client_pos = m_client.get_position();
				auto dist = distance_earth(f.latitude(), f.longitude(), client_pos.latitude, client_pos.longitude);
				if (dist < 0.040)
				{
					auto fort = m_client.search_fort(f.id(), f.latitude(), f.longitude());
					if (fort.result() == pogo::FortSearchResponse_Result_SUCCESS)
					{
						f.set_cooldown_complete_timestamp_ms(timestamp() + pokestop_cooldown);
					}
					std::cout << "farm fort: [" << f.latitude() << "][" << f.longitude() << "] " << fort.result() << std::endl;
				}
			}
		}
	}
}
