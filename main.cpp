#include <stdlib.h>
#include "client.h"
#include <iostream>
#include <tuple>
#include <boost\timer.hpp>
#include "config.h"
#include "bot.h"
#include <thread>


int main(void)
{
	bot m_bot;
	std::thread thread([&m_bot]
	{
		m_bot.run();
	});
	
	int t;
	std::cin >> t;

	m_bot.stop();
	thread.join();

	return 0;
}

