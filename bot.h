#pragma once

#include "client.h"
#include <memory>
#include "timer_task.h"
#include "ibot_strategy.h"

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

		if (k < 1.0)
		{
			delta_dist.latitude *= k;
			delta_dist.longitude *= k;
		}

		return{ pos.latitude + delta_dist.latitude, pos.longitude + delta_dist.longitude, 0 };
	}

	double speed = 5; // m/sec
	uint64_t last_time = timestamp();
};

class bot
{
public:
	bot();
	~bot();

	void run();

private:
	std::unique_ptr<client> m_client;
	timer_task m_timer_task;
	distancer m_distancer;
	coordinate m_target_pos;

	pogo::GetMapObjectsResponse m_map_objects;

	pogo::GetInventoryResponse m_inventory;

	std::unique_ptr<ibot_strategy> m_pokestop_strategy;
	std::unique_ptr<ibot_strategy> m_pokemon_strategy;

private:
	void map_objects_updated();
	void inventory_updated();
};

