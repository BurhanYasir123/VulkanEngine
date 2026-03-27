#pragma once

#define STR std::string()

namespace Log
{
	template<typename T>
	void print(T msg, const char* caller="APP")
	{
		std::cout << '[' << caller << "]: " << msg << '\n';
	}
}