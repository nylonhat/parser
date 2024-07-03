#include <concepts>
#include <print>
#include <string>
#include <utility>
#include <vector>
#include <format>
#include <ranges>
#include <functional>
#include <type_traits>


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

template<typename T>
using ParsePair = std::pair<T, std::string>;

template<typename T>
using ParseList = std::vector<ParsePair<T>>;

template<typename P>
using ParserType = std::invoke_result_t<P, std::string>::value_type::first_type;

template<typename P>
concept Parser = requires(P p, std::string input){
	{p(input)} -> std::same_as<ParseList<typename std::invoke_result_t<P, std::string>::value_type::first_type>>;
};


template<typename T>
auto result(T value){
	auto parser = [value](std::string input){
		auto pair = ParsePair<T>(value, input);
		return ParseList<T>{pair};
	};

	return parser;
}

auto zero = [](){
	auto parser = [](std::string input){
		return ParseList<int>{};
	};

	return parser;
};

ParseList<char> item_p(std::string input){
	if(input.empty()){
		return ParseList<char>{};
	}
	char front = input[0];
	auto unparsed = input.substr(1, input.size());
	auto pair = std::pair(front, unparsed);
	return ParseList<char>{pair};	
}

auto item(){
	auto parser = [](std::string input){
		if(input.empty()){
			return ParseList<char>{};
		}
		char front = input[0];
		auto unparsed = input.substr(1, input.size());
		auto pair = std::pair(front, unparsed);
		return ParseList<char>{pair};	
	};

	return parser;

}

template<Parser P, typename F>
auto bind(P p1, F f){
	using A = ParserType<P>;
	using ParserB = std::invoke_result_t<F, A>;
	using B = ParserType<ParserB>;

	auto parser = [p1, f](std::string input1){
		auto out_list = ParseList<B>{};

		auto list = p1(input1);
		for(auto [value, unconsumed] : list){
			auto p2 = f(value);
			auto list2 = p2(unconsumed);
			out_list.insert(out_list.end(), list2.cbegin(), list2.cend());
		}
		return out_list;
	};

	return parser;
}


int main(){
	//auto r42 = result(42);
	//auto list = r42("Hello World");
	
	auto item_parser = item();
	std::println("{}", item_parser("Hello world!"));

	auto ir_p = bind(item_p, result<char>);
	std::println("{}", ir_p("ksdjflksdfj"));
	return 0;
}
