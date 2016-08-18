#include "bot.h"
#include "config.h"
#include <chrono>
#include <thread>
#include <iostream>
#include "pokestop_strategy.h"
#include "pokemon_strategy.h"
#include <fstream>
#include "wander_strategy.h"
#include "target_strategy.h"

// Основная идея:
// Есть три этапа: изучение инвентаря, сканирование карты и выбор цели.
// Во время изучения инвентаря различные стратегии "инвентаря" устанавливают определенным флагам значения и
// производят необходимые действия с инвентарем. (трансфер, эволюция и т.д.)
// Во время сканирования карты различные стратегии "карты" взаимодействуют с внешним миром 
// в текущей позиции. (ловля покемона, фарм покестопа, захват гима)
// После этого происходит поиск новой цели на основе различных стратегий "поиска целей".
// Данные стратегии на основе флагов, сгенерированных с помощью стратегий "инвентаря" формируют
// координаты новой цели.

bot::bot()
	: m_config("config.ini")
{
	m_client = std::move(std::make_unique<decltype(m_client)::element_type>(coordinate{ m_config.latitude(), m_config.longitude(), 10.0 }));
	m_target_pos = m_client->get_position();
	m_distancer.speed = ((20.0 * 1000.0) / 60.0) / 60.0;

	m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
	m_client->authorize(m_config.login(), m_config.password());
	m_client->set_server();
	auto player = m_client->get_player();
	auto settings = m_client->get_settings();

	auto inventory = m_client->get_inventory();
	for (const auto &i : inventory.inventory_delta().inventory_items())
	{
		if (i.has_inventory_item_data())
		{
			if (i.inventory_item_data().has_player_stats())
			{
				const auto &stats = i.inventory_item_data().player_stats();
				std::cout << "level: " << stats.level() << " xp: " << stats.experience() << std::endl;
			}
		}
	}
	
	std::cout << "username: " << player.player_data().username() << std::endl;


	m_strategy.push_back(std::move(std::make_unique<target_strategy>(coordinate{ m_config.target_lat(), m_config.target_lon() , 0})));
	m_strategy.push_back(std::move(std::make_unique<pokestop_strategy>()));
	m_strategy.push_back(std::move(std::make_unique<pokemon_strategy>()));
	m_strategy.push_back(std::move(std::make_unique<wander_strategy>()));

	// Периодически обновляем карту.
	m_timer_task.call([this]
	{
		m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
		m_client->update_position();
		m_map_objects = m_client->get_map_objects();
		map_objects_updated();
	}, 10.0, 0);

	// Периодически обновляем инвентарь.
	m_timer_task.call([this]
	{
		m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
		m_inventory = m_client->get_inventory();
		inventory_updated();
	}, 5.0, 0);

	m_timer_task.call([this]
	{
		m_config.save();
	}, 10.0, 0);
}


bot::~bot()
{
}

void bot::run()
{
	while (!m_closed)
	{
		m_timer_task.poll();
		const auto &pos = m_client->get_position();
		m_config.latitude(pos.latitude);
		m_config.longitude(pos.longitude);
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void bot::stop()
{
	m_closed = true;
}

const coordinate & bot::pos() const
{
	return m_client->get_position();
}

void bot::map_objects_updated()
{
	// Крутим покестопы.
	// Ловим покемонов.
	// Захватываем гимы.
	m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));

	double dist_target = distance_earth(m_target_pos.latitude, m_target_pos.longitude,
		m_client->get_position().latitude, m_client->get_position().longitude);
	std::cout << "dist: " << dist_target << std::endl;

	for (auto &i : m_strategy)
	{
		i->map_exec(*m_client, m_map_objects);
	}

	// Ищем новую цель.
	m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
	for (auto &i : m_strategy)
	{
		if (auto target = i->find_target(*m_client, m_map_objects))
		{
			m_target_pos = *target;
			std::cout << "target: [" << m_target_pos.latitude << "][" << m_target_pos.longitude << "]" << std::endl;
			break;
		}
	}
}

void bot::inventory_updated()
{
	// Удаляем ненужные итемы.
	// Эволюционируем покемонов.
	// Трансферим ненужных покемонов.
	m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
	for (auto &i : m_strategy)
	{
		i->inventory_exec(*m_client, m_inventory);
	}
}



