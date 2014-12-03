#pragma once

namespace lc {

/**
 *  @defgroup intrusive_ptr Intrusive smart pointer
 *  @brief    侵入型参照カウント方式スマートポインタ
 *  @ingroup  system
 *  @{
 */

/**
 *  @brief 侵入型参照カウント方式スマートポインタ
 *  @tparam T  スマートポインタ化する型。
 *             メンバ変数として intrusive_reference_count を持つもの。
 */
template <class T>
class IntrusivePtr {

private:
	T *m_pointer;

public:
	/**
	 *  @brief コンストラクタ（生ポインタからの初期化）
	 *  @param[in] p          スマートポインタ化するインスタンスを指すポインタ
	 */
	IntrusivePtr(T *p = nullptr)
		: m_pointer(p)
	{
		if(m_pointer){ m_pointer->intrusive_reference_count = 1; }
	}

	/**
	 *  @brief コピーコンストラクタ
	 *  @param[in] p  コピー元オブジェクト
	 */
	IntrusivePtr(const IntrusivePtr<T> &p)
		: m_pointer(p.m_pointer)
	{
		if(m_pointer){ ++m_pointer->intrusive_reference_count; }
	}

	/**
	 *  @brief デストラクタ
	 */
	~IntrusivePtr(){
		if(m_pointer && --m_pointer->intrusive_reference_count == 0){
			delete m_pointer;
		}
	}

	/**
	 *  @brief コピー代入演算子
	 *  @param[in] p  コピー元オブジェクト
	 *  @return    自身への参照
	 */
	IntrusivePtr<T> &operator=(const IntrusivePtr<T> &p){
		if(p.m_pointer){ ++p.m_pointer->intrusive_reference_count; }
		if(m_pointer && --m_pointer->intrusive_reference_count == 0){
			delete m_pointer;
		}
		m_pointer = p.m_pointer;
		return *this;
	}

	/// 間接メンバ参照
	const T *operator->() const { return m_pointer; }
	/// 間接メンバ参照
	T *operator->(){ return m_pointer; }
	/// デリファレンス
	const T &operator*() const { return *m_pointer; }
	/// デリファレンス
	T &operator*(){ return *m_pointer; }

	/// null/非null 判定
	operator bool() const { return m_pointer != nullptr; }
	/// null/非null 判定
	bool operator!() const { return m_pointer == nullptr; }

};

}

