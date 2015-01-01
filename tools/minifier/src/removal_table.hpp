#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class RemovalTable {

private:
	std::unordered_map<std::string, std::vector<bool>> m_table;

public:
	RemovalTable()
		: m_table()
	{ }

	void write(const std::string &filename, const std::vector<bool> &table){
		m_table[filename] = table;
	}
	std::vector<bool> operator[](const std::string &filename) const {
		const auto it = m_table.find(filename);
		if(it == m_table.end()){ return std::vector<bool>(); }
		return it->second;
	}

};

