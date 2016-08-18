#pragma once

#include "client.h"
#include <memory>
#include "timer_task.h"
#include "ibot_strategy.h"
#include <atomic>
#include "config.h"

struct distancer
{
	coordinate operator()(const coordinate &pos, const coordinate &target)
	{
		auto dist_target = distance_earth(pos.latitude, pos.longitude, target.latitude, target.longitude);
		coordinate delta_dist = { target.latitude - pos.latitude, target.longitude - pos.longitude, 0 };
		double time = (timestamp() - last_time) / 1000.0; // sec
		last_time = timestamp();

		double rspeed = static_cast<double>(rand() % 101) / 10.0; //[0 .. 100]
		double s = (speed + rspeed) * (1000.0 / (60.0 * 60.0));
		double distance = (s * time) / 1000.0;
		auto k = dist_target == 0.0 ? 0.0 : distance / dist_target;

		k += static_cast<double>(rand() % 101 - 50) / 100000.0; // [-50 .. 50]
		if (k < 1.0)
		{
			delta_dist.latitude *= k;
			delta_dist.longitude *= k;
		}

		return{ pos.latitude + delta_dist.latitude, pos.longitude + delta_dist.longitude, 0 };
	}

	double speed = 15.0; // km/h  
	uint64_t last_time = timestamp();
};

class bot
{
public:
	bot();
	~bot();

	void run();

	void stop();

	const coordinate &pos() const;

private:
	std::unique_ptr<client> m_client;
	timer_task m_timer_task;
	distancer m_distancer;
	coordinate m_target_pos;

	pogo::GetMapObjectsResponse m_map_objects;

	pogo::GetInventoryResponse m_inventory;

	std::vector<std::unique_ptr<ibot_strategy>> m_strategy;

	std::atomic<bool> m_closed = false;

	config m_config;

private:
	void map_objects_updated();
	void inventory_updated();

};

