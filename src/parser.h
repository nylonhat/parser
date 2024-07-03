#ifndef PARSER_H
#define PARSER_H

#include <concepts>
#include <string>
#include <utility>
#include <vector>
#include <format>
#include <type_traits>


template<typename T>
using ParsePair = std::pair<T, std::string>;

template<typename T>
using ParseList = std::vector<ParsePair<T>>;

template<typename P>
using ParserType = std::invoke_result_t<P, std::string>::value_type::first_type;

template<typename P>
concept IsParser = requires(P p, std::string input){
	{p(input)} -> std::same_as<ParseList<typename std::invoke_result_t<P, std::string>::value_type::first_type>>;
};

template<typename ParserA, typename F>
concept Bindable = requires(F f){
	requires IsParser<std::invoke_result_t<F, ParserType<ParserA>>>;
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

template<typename P, typename F>
requires IsParser<P> && Bindable<P, F>
auto bind(P first_parser, F f){
	using A = ParserType<P>;
	using ParserB = std::invoke_result_t<F, A>;
	using B = ParserType<ParserB>;

	auto new_parser = [first_parser, f](std::string input){
		auto flat_list = ParseList<B>{};

		auto outer_list = first_parser(input);
		for(auto [value, unconsumed] : outer_list){
			auto second_parser = f(value);
			auto inner_list = second_parser(unconsumed);
			auto begin = inner_list.cbegin();
			auto end = inner_list.cend();
			flat_list.insert(flat_list.end(), begin, end);
			//flat_list.append_range(inner_list);
		}
		return flat_list;
	};

	return new_parser;
}



#endif
