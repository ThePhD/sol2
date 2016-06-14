#pragma once

#include "traits.hpp"

namespace sol {

	template <typename T>
	struct bases_for {
		typedef types<> type;
	};

	template <typename T>
	struct has_bases : std::integral_constant<bool, ( bases_for<T>::type::size() > 0 )> {};
}
