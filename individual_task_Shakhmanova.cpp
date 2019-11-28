#include <iostream>
#include <chrono>
#include <locale.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer.h>
#include <cilk/reducer_opadd.h>

const double a = -1;
const double b = 1;

double f(double x)
{
	return 8 / (2 + 2 * x * x);
}

// формула трапеций
double trapez(int n)
{
	double S, h;
	S = 0;
	h = (b - a) / n;
	for (int i = 0; i < n; i++)
		S += (f(a + i * h) + f(a + (i + 1) * h));
	return 0.5 * S * h;
}

// параллельна€ реализаци€
double parallel_trapez(int n)
{
	double h = (b - a) / n;
	cilk::reducer_opadd<double> S(0.0);
	cilk_for(int i = 0; i < n; i++)
		S += f(a + i * h) + f(a + (i + 1) * h);
	return S->get_value()*h * 0.5;
}

int main()
{
	int N = 10000000;
	int point = 100000000;
	double I;

	std::chrono::high_resolution_clock::time_point t0, t3;
	std::chrono::duration<double> t1, t2;
	
	t0 = std::chrono::high_resolution_clock::now();
	I = trapez(N);
	t1 = std::chrono::high_resolution_clock::now() - t0;
	std::cout << "Integral = " << I << std::endl;
	std::cout << "Serial time = " << t1.count() << "\n" << std::endl;

	__cilkrts_set_param("nworkers", "4");
	t3 = std::chrono::high_resolution_clock::now();
	I = parallel_trapez(N);
	t2 = std::chrono::high_resolution_clock::now() - t3;
	std::cout << "Parallel integral = " << I << std::endl;
	std::cout << "Parallel time = " << t2.count() << "\n" << std::endl;

	std::cout << "Acceleration = " << t1.count() / t2.count() << "\n" << std::endl;
	

	std::cout << "Serial integral:" << std::endl;
	for (int i = 100; i < point; i *= 10)
	{
		t0 = std::chrono::high_resolution_clock::now();
		I = trapez(i);
		t1 = std::chrono::high_resolution_clock::now() - t0;
		std::cout << "Step = " << i << " Time = " << t1.count() << std::endl;
	}
	std::cout << std::endl;
	
	std::cout << "Parallel integral: " << std::endl;
	for (int i = 100; i < point; i *= 10)
	{
		t3 = std::chrono::high_resolution_clock::now();
		I = parallel_trapez(i);
		t2 = std::chrono::high_resolution_clock::now() - t3;
		std::cout << "Step = " << i << " Time = " << t2.count() << std::endl;
	}
	
	return 0;
}