/**
 *  @file libcomp/string/name_table.hpp
 */
#pragma once
#include <string>
#include <vector>
#include <unordered_map>

namespace lc {

/**
 *  @defgroup name_table Name table
 *  @brief    名前変換テーブル
 *  @ingroup  string
 *  @{
 */

/**
 *  @brief 名前変換テーブル
 *
 *  文字列と整数の間で変換を行うテーブル。
 *  operator[] で必要に応じて新たなインデックスが発行される。
 */
class NameTable {

private:
	std::unordered_map<std::string, int> m_table;
	std::vector<std::string> m_inv_table;

public:
	/**
	 *  @brief デフォルトコンストラクタ
	 */
	NameTable(){ }

	/**
	 *  @brief  名前の存在チェック
	 *  @return テーブル中に名前sが存在すればtrue、そうでなければfalse。
	 */
	bool has_name(const std::string &s){
		return m_table.find(s) != m_table.end();
	}

	/**
	 *  @brief 名前の追加
	 *  @param[in] s  名前
	 *  @return    追加された名前に対応するインデックス
	 */
	int add_name(const std::string &s){
		return (*this)[s];
	}

	/**
	 *  @brief 名前からインデックスへの変換
	 *  @param[in] s  名前
	 *  @return    名前に対応するインデックス
	 */
	int operator[](const std::string &s){
		const auto it = m_table.find(s);
		if(it != m_table.end()){ return it->second; }
		const int t = m_inv_table.size();
		m_table.insert(std::make_pair(s, t));
		m_inv_table.push_back(s);
		return t;
	}

	/**
	 *  @brief インデックスから名前への逆変換
	 *  @param[in] i  インデックス
	 *  @return    インデックスに対応する名前
	 */
	const std::string &operator[](size_t i) const {
		return m_inv_table[i];
	}

	/**
	 *  @brief  変換テーブルのエントリ数取得
	 *  @return 変換テーブルに記録されている名前の数
	 */
	size_t size() const { return m_inv_table.size(); }

};

/**
 *  @}
 */

}

