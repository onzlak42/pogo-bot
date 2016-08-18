#pragma once

#include <string>
#include <fstream>

#define CONFIG_MEM_BUILD(type, name, default_value)				\
public:																			\
type name() const {return m_##name;}				\
void name(type value) {m_##name = value;}		\
private:																		\
type m_##name = default_value								\




class config
{
public:
	config(const std::string &filename);
	~config();

	CONFIG_MEM_BUILD(double, latitude, 0);
	CONFIG_MEM_BUILD(double, longitude, 0);

	CONFIG_MEM_BUILD(std::string, login, "");
	CONFIG_MEM_BUILD(std::string, password, "");

	CONFIG_MEM_BUILD(double, target_lat, 0);
	CONFIG_MEM_BUILD(double, target_lon, 0);

public:
	void load();
	void save();

private:
	std::string m_filename;

};


#undef CONFIG_MEM_BUILD
