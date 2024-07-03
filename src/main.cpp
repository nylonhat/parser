#include <print>

#include "parser.h"
#include "range_formatter.h"


int main(){
	//auto r42 = result(42);
	//auto list = r42("Hello World");
	
	auto item_parser = item();
	std::println("{}", item_parser("Hello world!"));

	auto two_item = bind(item(), [](char c){return item();});
	std::println("{}", two_item("ksdjflksdfj"));
	return 0;
}
