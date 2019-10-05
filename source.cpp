#include <stdio.h>
#include <iostream>

#include "delegate.hpp"

class my_class_t
{
public:
	int f(const int & n) { std::cout << "f " << n << std::endl;  return 2 * n; }
	int f(const int & n) const { std::cout << "f Const " << n << std::endl;  return 2 * n; }

	int p(int m, int && n, const int & o) { std::cout << "p " << m << " " << n << " " << o << std::endl;  return 2 * n; }
	int p(int m, int && n, const int & o) const { std::cout << "p Const " << m << " " << n << " " << o << std::endl;  return 2 * n; }

	int g(int & n) { return n = (n * 17) % 13; }
};

int f(const int & n) { std::cout << "f Static " << n << std::endl;  return 2 * n; }

int p(int m, int && n, const int & o) { std::cout << "p Static " << m << " " << n << " " << o << std::endl;  return 2 * n; }

#include <chrono>
#include <functional>

int main()
{
	int n = 12;
	const int m = 15;
	my_class_t me;
	const my_class_t me_const;

	{
		delegate_t<int(my_class_t::*)(const int&), &my_class_t::f> method(&me);
		method(n);

		delegate_t<int(my_class_t::*)(const int&) const, &my_class_t::f> const_method(&me_const);
		const_method(n);

		delegate_t<int(*)(const int&), &f> static_method;
		static_method(n);

		auto method_any = make_delegate_any<int(my_class_t::*)(const int&), &my_class_t::f>(&me);
		method_any(n);

		auto const_method_any = make_delegate_any<int(my_class_t::*)(const int&) const, &my_class_t::f>(&me_const);
		const_method_any(n);

		auto static_method_any = make_delegate_any<int(*)(const int&), &f>();
		static_method_any(n);
	}

	std::cout << std::endl;

	{
		delegate_t<int(my_class_t::*)(int, int&&, const int&), &my_class_t::p> method(&me);
		method(1, n * m, m);

		delegate_t<int(my_class_t::*)(int, int&&, const int&) const, &my_class_t::p> const_method(&me_const);
		const_method(2, n * m, m);

		delegate_t<int(*)(int, int&&, const int&), &p> static_method;
		static_method(2, n * m, m);

		auto method_any = make_delegate_any<int(my_class_t::*)(int, int&&, const int&), &my_class_t::p>(&me);
		method_any(2, n * m, m);

		auto method_const_any = make_delegate_any<int(my_class_t::*)(int, int&&, const int&) const, &my_class_t::p>(&me_const);
		method_const_any(2, n * m, m);

		auto static_method_any = make_delegate_any<int(*)(int, int&&, const int&), &p>();
		static_method_any(2, n * m, m);
	}

	for (int cnt = 0; cnt < 3; ++cnt)
	{
		std::cout << "\n#########################" << std::endl;
		std::cout << std::endl;

		{
			using namespace std::chrono;

			delegate_t<int(my_class_t::*)(int &), &my_class_t::g> custom_bind(&me);
			auto t = high_resolution_clock::now();
			for (int i = 0; i < 10'000'000; ++i)
				custom_bind(n);
			auto _diff = duration<double, std::milli>(high_resolution_clock::now() - t).count();
			std::cout << "delegate_t: \t\t" << _diff << " ms" << std::endl;
		}

		std::cout << std::endl;

		{
			using namespace std::chrono;

			auto custom_bind = make_delegate_any<int(my_class_t::*)(int &), &my_class_t::g>(&me);
			auto t = high_resolution_clock::now();
			for (int i = 0; i < 10'000'000; ++i)
				custom_bind(n);
			auto _diff = duration<double, std::milli>(high_resolution_clock::now() - t).count();
			std::cout << "delegate_any_t: \t" << _diff << " ms" << std::endl;
		}

		std::cout << std::endl;

		{
			using namespace std::chrono;

			auto std_bind = std::bind(&my_class_t::g, &me, std::placeholders::_1);
			auto t = high_resolution_clock::now();
			for (int i = 0; i < 10'000'000; ++i)
				std_bind(n);
			auto _diff = duration<double, std::milli>(high_resolution_clock::now() - t).count();
			std::cout << "std::bind: \t\t" << _diff << " ms" << std::endl;
		}
	}

	std::cout << "\npress enter to exit... ";
	getchar();
	return 0;
}
