#include <stdlib.h>
#include "client.h"
#include <iostream>
#include <tuple>
#include <boost\timer.hpp>
#include "config.h"


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


// вероятно это не будет работать для больших расстояний.
coordinate new_pos(const coordinate &pos, const coordinate &target, double distance)
{
	// Находим расстояние до цели в км.
	// Строим вектор из нашей позиции к цели.
	// Изменяем длину вектора.
	// Перемещаем позицию на длину вектора.

	auto dist = distance_earth(pos.latitude, pos.longitude, target.latitude, target.longitude);
	coordinate dt = { target.latitude - pos.latitude, target.longitude - pos.longitude, 0 };
	auto k = distance / dist;
	dt.latitude *= k;
	dt.longitude *= k;

	return{ pos.latitude + dt.latitude, pos.longitude + dt.longitude, 0 };
}

struct distancer
{
	coordinate operator()(const coordinate &pos, const coordinate &target)
	{
		auto dist_target = distance_earth(pos.latitude, pos.longitude, target.latitude, target.longitude);
		coordinate delta_dist = { target.latitude - pos.latitude, target.longitude - pos.longitude, 0 };
		double time = (timestamp() - last_time) / 1000.0; // sec
		last_time = timestamp();
		double distance = (speed * time) / 1000.0;
		auto k = distance / dist_target;

		delta_dist.latitude *= k;
		delta_dist.longitude *= k;

		return{ pos.latitude + delta_dist.latitude, pos.longitude + delta_dist.longitude, 0 };
	}

	double speed = 5; // m/sec
	uint64_t last_time = timestamp();
};

std::tuple<bool, coordinate> find_pokestop(client &pgo_client)
{
	std::cout << std::endl;
	auto map = pgo_client.get_map_objects();

	coordinate min_coord{ 0, 0, 0 };
	double min_dist;
	bool finded = false;

	for (const auto &c : map.map_cells())
	{
		for (const auto &f : c.forts())
		{
			if (f.type() == pogo::FortType::CHECKPOINT && f.cooldown_complete_timestamp_ms() < timestamp())
			{
				auto dist = distance_earth(f.latitude(), f.longitude(), pgo_client.get_position().latitude, pgo_client.get_position().longitude);
				if (dist < 0.040)
				{
					std::cout << f.cooldown_complete_timestamp_ms() << std::endl;
					std::cout << timestamp() << std::endl;
					auto fort = pgo_client.search_fort(f.id(), f.latitude(), f.longitude());
					std::cout << "farm fort result: " << fort.result() << " (1 == true, other == error)" << std::endl;
					if (fort.result() != 2)
						continue;
				}

				if (!finded)
				{
					finded = true;
					min_dist = dist;
					min_coord.latitude = f.latitude();
					min_coord.longitude = f.longitude();
				}
				else if (min_dist > dist)
				{
					min_dist = dist;
					min_coord.latitude = f.latitude();
					min_coord.longitude = f.longitude();
				}
			}
		}
	}

	if (finded)
	{
		std::cout << "nearby fort: [" << min_coord.latitude << "][" << min_coord.longitude << "]" << std::endl;
		std::cout << "dist: " << min_dist << std::endl;
	}
	return{ finded, min_coord };
}

int main(void)
{
	client pgo_client({ config::pos.lat, config::pos.lon, 10 });

	pgo_client.authorize(config::user, config::pass);
	pgo_client.set_server();
	pgo_client.get_player();
	pgo_client.get_settings();

	distancer d;
	for (size_t i = 0; i < 100; ++i)
	{
		auto pokestop_pos = find_pokestop(pgo_client);
		if (std::get<0>(pokestop_pos))
		{
			pgo_client.set_position(d(pgo_client.get_position(), std::get<1>(pokestop_pos)));
			pgo_client.update_position();
		}
		std::this_thread::sleep_for(std::chrono::seconds(10));
	}

	update_inventory(pgo_client);
	update_map(pgo_client);
	update_inventory(pgo_client);

	system("pause");
	return 0;
}

