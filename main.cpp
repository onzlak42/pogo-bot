#include <stdlib.h>
#include "client.h"
#include <iostream>
#include <tuple>
#include <boost\timer.hpp>
#include "config.h"
#include "bot.h"


void update_inventory(client &pgo_client)
{
	std::cout << std::endl;

	auto inventory = pgo_client.get_inventory();
	auto success = inventory.success();

	for (const auto &i : inventory.inventory_delta().inventory_items())
	{
		if (i.has_inventory_item_data())
		{
			if (i.inventory_item_data().has_player_stats())
			{
				const auto &player_stats = i.inventory_item_data().player_stats();
				std::cout << "level: " << player_stats.level() << std::endl;
				std::cout << "xp: " << player_stats.experience() << std::endl;
			}
		}
	}

	for (const auto &i : inventory.inventory_delta().inventory_items())
	{
		if (i.has_inventory_item_data())
		{
			if (i.inventory_item_data().has_pokemon_data())
			{
				std::cout << "pokemon id: " << i.inventory_item_data().pokemon_data().pokemon_id()
					<< " cp: " << i.inventory_item_data().pokemon_data().cp() << std::endl;
			}
		}
	}

	for (const auto &i : inventory.inventory_delta().inventory_items())
	{
		if (i.has_inventory_item_data())
		{
			if (i.inventory_item_data().has_item())
			{
				const auto &items = i.inventory_item_data().item();
				std::cout << "item id: " << items.item_id() << " count: " << items.count() << std::endl;
			}
		}
	}

	std::cout << "success: " << success << std::endl;
}

void update_map(client &pgo_client)
{
	std::cout << std::endl;

	auto map = pgo_client.get_map_objects();
	auto success = map.status();

	for (const auto &c : map.map_cells())
	{
		for (const auto &p : c.catchable_pokemons())
		{
			std::cout << "cpok: [" << p.latitude() << "][" << p.longitude() << "]" << std::endl;

			auto encounter = pgo_client.encounter_pokemon(p.encounter_id(), p.spawn_point_id());

			auto resp = pgo_client.catch_pokemon(p.encounter_id(), p.spawn_point_id(), pogo::ITEM_POKE_BALL);
		}

		for (const auto &f : c.forts())
		{
			if (f.type() == pogo::FortType::CHECKPOINT)
			{
				std::cout << "fort: [" << f.latitude() << "][" << f.longitude() << "]" << std::endl;

				auto dist = distance_earth(f.latitude(), f.longitude(), pgo_client.get_position().latitude, pgo_client.get_position().longitude);
				if (dist < 0.030)
				{
					auto fort = pgo_client.search_fort(f.id(), f.latitude(), f.longitude());
					std::cout << "farm fort result: " << fort.result() << " (1 == true, other == error)" << std::endl;
				}
			}
			if (f.type() == pogo::FortType::GYM)
			{
				std::cout << "gym:  [" << f.latitude() << "][" << f.longitude() << "]" << std::endl;
			}
		}

	}

	std::cout << "satus: " << success << std::endl;
}



int main(void)
{
	bot m_bot;
	m_bot.run();

	system("pause");
	return 0;
}

