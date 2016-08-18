#include "wander_strategy.h"



boost::optional<coordinate> wander_strategy::find_target(client &m_client, const pogo::GetMapObjectsResponse &map)
{
	srand(time(nullptr));

	const auto &my_pos = m_client.get_position();

	// Если цель очень далеко иди слишком близко, удаляем ее.
	if (m_target)
	{
		double dist_target = distance_earth(m_target->latitude, m_target->longitude, my_pos.latitude, my_pos.longitude);
		if (dist_target > static_cast<double>(radius) / 1000.0 || dist_target < 0.005)
		{
			m_target.reset();
		}
	}

	// Если нет цели, нужно сформировать ее.
	if (!m_target)
	{
		// Нормализованный вектор.
		double vec_x = rand() % (100 * 2) - 100;
		double vec_y = rand() % (100 * 2) - 100;
		double len = 1.0 / std::sqrt(vec_x * vec_x + vec_y * vec_y);
		vec_x *= len;
		vec_y *= len;

		// Рандомное расстояние в метрах.
		double dist = static_cast<double>(rand() % radius);
		// В градусах.
		double degree = dist * (1.0 / (40075000.0 / 360.0));
		// Смещение в градусах.
		vec_x *= degree;
		vec_y *= degree;

		m_target = { my_pos.latitude + vec_x, my_pos.longitude + vec_y, my_pos.altitude };

		auto d = distance_earth(m_target->latitude, m_target->longitude, my_pos.latitude, my_pos.longitude);

		int k = 0;
	}

	return m_target;
}

void wander_strategy::inventory_exec(client &m_client, pogo::GetInventoryResponse &inventory)
{

}

void wander_strategy::map_exec(client &m_client, pogo::GetMapObjectsResponse &map)
{

}
