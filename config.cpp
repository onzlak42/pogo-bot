#include "config.h"
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>


config::config(const std::string &filename)
{
	m_filename = filename;
	load();
}

config::~config()
{
	save();
}

void config::load()
{
	std::ifstream m_file(m_filename);
	if (m_file.is_open())
	{
		rapidjson::Document doc;
		m_file.seekg(0, std::ios::end);
		auto size = static_cast<size_t>(m_file.tellg());
		m_file.seekg(0, std::ios::beg);
		std::string data(size, 0);
		m_file.read(&data[0], size);

		doc.Parse(data.c_str());
		if (doc.IsObject())
		{
			if (doc.HasMember("latitude"))
			{
				rapidjson::Value &val = doc["latitude"];
				m_latitude = val.GetDouble();
			}
			if (doc.HasMember("longitude"))
			{
				rapidjson::Value &val = doc["longitude"];
				m_longitude = val.GetDouble();
			}
			if (doc.HasMember("login"))
			{
				rapidjson::Value &val = doc["login"];
				m_login = val.GetString();
			}
			if (doc.HasMember("password"))
			{
				rapidjson::Value &val = doc["password"];
				m_password = val.GetString();
			}

			if (doc.HasMember("target_lat"))
			{
				rapidjson::Value &val = doc["target_lat"];
				m_target_lat = val.GetDouble();
			}
			if (doc.HasMember("target_lon"))
			{
				rapidjson::Value &val = doc["target_lon"];
				m_target_lon = val.GetDouble();
			}
		}
		m_file.close();
	}
}

void config::save()
{
	std::ofstream m_file(m_filename);
	if (m_file.is_open())
	{
		rapidjson::Document doc;
		doc.SetObject();
		doc.AddMember("latitude", m_latitude, doc.GetAllocator());
		doc.AddMember("longitude", m_longitude, doc.GetAllocator());
		doc.AddMember("login", rapidjson::StringRef(m_login.c_str()), doc.GetAllocator());
		doc.AddMember("password", rapidjson::StringRef(m_password.c_str()), doc.GetAllocator());

		doc.AddMember("target_lat", m_target_lat, doc.GetAllocator());
		doc.AddMember("target_lon", m_target_lon, doc.GetAllocator());

		rapidjson::StringBuffer buffer;
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		doc.Accept(writer);

		m_file.write(buffer.GetString(), buffer.GetSize());
		m_file.close();
	}
}

