#ifndef RANGE_FORMATTER_H
#define RANGE_FORMATTER_H

#include <utility>
#include <format>

template <std::ranges::input_range R>
struct std::formatter<R>{
	using T = remove_cvref_t<ranges::range_reference_t<R>>;
	std::formatter<T> underlying;

	constexpr auto parse(auto& context) {
		return underlying.parse(context);
	}

    auto format(R const& r, auto& context) const {
        auto out = context.out();
		const char* delimiter  = "";
		out = std::format_to(out, "[");

		for(auto&& element : r){
			out = std::format_to(out, "{}", std::exchange(delimiter, ", " ));
			context.advance_to(out);
			out = underlying.format(element, context);
		}
		return std::format_to(out, "]");
    }
};

template<class A, class B>
struct std::formatter<std::pair<A, B>>{
	
	constexpr auto parse(auto& context) {
		return context.begin();
	}

    auto format(std::pair<A,B> const& pair, auto& context) const {
        auto out = context.out();
		return std::format_to(out, "({}, {})", pair.first, pair.second);
    }


};

#endif
