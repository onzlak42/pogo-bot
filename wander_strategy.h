#pragma once
#include "ibot_strategy.h"

class wander_strategy : public ibot_strategy
{
public:

	// Если у нас есть точка в радиусе и мы не дошли до нее, идем туда.
	// Иначе формируем новую точку в радиусе.
	boost::optional<coordinate> find_target(client &m_client, const pogo::GetMapObjectsResponse &map) override;

	void inventory_exec(client &m_client, pogo::GetInventoryResponse &inventory) override;

	// Просто ловим покемонов в зоне поражения.
	void map_exec(client &m_client, pogo::GetMapObjectsResponse &map) override;

private:
	const uint32_t radius = 400; // in meters
	boost::optional<coordinate> m_target;
};

