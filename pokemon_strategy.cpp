#include "pokemon_strategy.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>

boost::optional<coordinate> pokemon_strategy::find_target(client &m_client, const pogo::GetMapObjectsResponse &map)
{
	auto target = find_wild(m_client, map);
// 	if (!target)
// 	{
// 		target = find_nearby(m_client, map);
// 	}

	return target;
}

void pokemon_strategy::inventory_exec(client &m_client, pogo::GetInventoryResponse &inventory)
{

}

void pokemon_strategy::map_exec(client &m_client, pogo::GetMapObjectsResponse &map)
{
	for (const auto &c : map.map_cells())
	{
		for (const auto &p : c.catchable_pokemons())
		{
			auto encount_result = m_client.encounter_pokemon(p.encounter_id(), p.spawn_point_id());
			auto encount_status = encount_result.status();

			auto catch_result = m_client.catch_pokemon(p.encounter_id(), p.spawn_point_id(), pogo::ITEM_POKE_BALL);
			auto catch_status = catch_result.status();

			std::cout << "pokemon: [" << p.latitude() << "][" << p.longitude() << "]" << std::endl;
			std::cout << "encount_status: " << encount_status << " catch_status " << catch_status << std::endl;
		}
	}
}

boost::optional<coordinate> pokemon_strategy::find_wild(client &m_client, const pogo::GetMapObjectsResponse &map)
{
	boost::optional<coordinate> target;
	double min_dist = 0.0;
	const auto &client_pos = m_client.get_position();

	for (const auto &c : map.map_cells())
	{
		for (const auto &p : c.wild_pokemons())
		{
			auto dist = distance_earth(p.latitude(), p.longitude(), client_pos.latitude, client_pos.longitude);
			if (!target)
			{
				min_dist = dist;
				target = { p.latitude() , p.longitude() , 0 };
			}
			else if (min_dist > dist)
			{
				min_dist = dist;
				target = { p.latitude() , p.longitude() , 0 };
			}
		}
	}

	if (target) std::cout << "wild: " << min_dist << std::endl;

	return target;
}

boost::optional<coordinate> pokemon_strategy::find_nearby(client &m_client, const pogo::GetMapObjectsResponse &map)
{
	boost::optional<coordinate> target;
	double min_dist = 0.0;
	bool finded = false;

	for (const auto &c : map.map_cells())
	{
		for (const auto &p : c.nearby_pokemons())
		{
			auto dist = p.distance_in_meters();
			if (!finded)
			{
				min_dist = dist;
				finded = true;
			}
			else if (min_dist > dist)
			{
				min_dist = dist;
			}
			std::cout << "nearby: " << dist << std::endl;
		}
	}

	if (finded)
	{
		//std::cout << "nearby: " << min_dist << std::endl;
	}

	return target;
}
