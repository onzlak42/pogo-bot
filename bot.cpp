#include "bot.h"
#include "config.h"
#include <chrono>
#include <thread>
#include <iostream>
#include "pokestop_strategy.h"
#include "pokemon_strategy.h"

// �������� ����:
// ���� ��� �����: �������� ���������, ������������ ����� � ����� ����.
// �� ����� �������� ��������� ��������� ��������� "���������" ������������� ������������ ������ �������� �
// ���������� ����������� �������� � ����������. (��������, �������� � �.�.)
// �� ����� ������������ ����� ��������� ��������� "�����" ��������������� � ������� ����� 
// � ������� �������. (����� ��������, ���� ���������, ������ ����)
// ����� ����� ���������� ����� ����� ���� �� ������ ��������� ��������� "������ �����".
// ������ ��������� �� ������ ������, ��������������� � ������� ��������� "���������" ���������
// ���������� ����� ����.

bot::bot()
{
	m_client = std::move(std::make_unique<decltype(m_client)::element_type>(coordinate{ config::pos.lat, config::pos.lon, 10.0 }));
	m_target_pos = m_client->get_position();
	m_distancer.speed = ((20.0 * 1000.0) / 60.0) / 60.0;

	m_pokestop_strategy = std::move(std::make_unique<pokestop_strategy>());
	m_pokemon_strategy = std::move(std::make_unique<pokemon_strategy>());

	m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
	m_client->authorize(config::user, config::pass);
	m_client->set_server();
	auto player = m_client->get_player();
	m_client->get_settings();

	std::cout << "username: " << player.player_data().username() << std::endl;

	// ������������ ��������� �����.
	m_timer_task.call([this]()
	{
		m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
		m_client->update_position();
		m_map_objects = m_client->get_map_objects();
		map_objects_updated();
	}, 10.0, 0);

	// ������������ ��������� ���������.
	m_timer_task.call([this]()
	{
		m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
		m_inventory = m_client->get_inventory();
		inventory_updated();
	}, 5.0, 0);
}


bot::~bot()
{
}

void bot::run()
{
	while (true)
	{
		m_timer_task.poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void bot::map_objects_updated()
{
	// ������ ���������.
	// ����� ���������.
	// ����������� ����.
	m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
	double dist_target = distance_earth(m_target_pos.latitude, m_target_pos.longitude,
		m_client->get_position().latitude, m_client->get_position().longitude);
	std::cout << "dist: " << dist_target << std::endl;
	m_pokestop_strategy->map_exec(*m_client, m_map_objects);
	m_pokemon_strategy->map_exec(*m_client, m_map_objects);

	// ���� ����� ����.
	m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
	auto target = m_pokestop_strategy->find_target(*m_client, m_map_objects);
	if (target)
	{
		m_target_pos = *target;
		return;
	}
	target = m_pokemon_strategy->find_target(*m_client, m_map_objects);
	if (target)
	{
		m_target_pos = *target;
		std::cout << "target: [" << m_target_pos.latitude << "][" << m_target_pos.longitude << "]" << std::endl;
		return;
	}
}

void bot::inventory_updated()
{
	// ������� �������� �����.
	// �������������� ���������.
	// ���������� �������� ���������.
	m_client->set_position(m_distancer(m_client->get_position(), m_target_pos));
	m_pokestop_strategy->inventory_exec(*m_client, m_inventory);
	m_pokemon_strategy->inventory_exec(*m_client, m_inventory);
}

