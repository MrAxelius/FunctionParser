#include <FunctionParser/FunctionParser.h>
#include <chrono>
#include <cstdio>
#include <string_view>

namespace fp = FunctionParser;
using Reloj = std::chrono::steady_clock;

volatile double sink;

const char *casos[] = {
    "3*x*x*x - 2*x*x + x - 7",
    "sin(x) * cos(x) + tan(x / 2)",
    "((((x + 1) * (x - 1)) / (x + 2)) - 3)",
    "pow(sin(x), 2) + pow(cos(x), 2) - log(x * x + 1, e) / nrt(x * x + 4, 2)",
};
constexpr long long numCasos = sizeof(casos) / sizeof(casos[0]);

template <class F>
void medir(const char *nombre, long long ops, F &&f)
{
    auto t0 = Reloj::now();
    f();
    double s = std::chrono::duration<double>(Reloj::now() - t0).count();
    std::printf("%-8s %8.3f s %10.1f ns/op\n", nombre, s, s * 1e9 / static_cast<double>(ops));
}

void parse(int n)
{
    medir("parse", n * numCasos, [&] {
        for (int i = 0; i < n; ++i)
            for (auto c : casos)
                fp::Expression e(c);
    });
}

void eval(int n)
{
    medir("eval", n * numCasos, [&] {
        for (auto c : casos)
        {
            fp::Expression e(c);
            for (int i = 0; i < n; ++i)
                sink = e.eval(i * 1e-3).value_or(0.0);
        }
    });
}

void sample(int n)
{
    medir("sample", n * numCasos * 10001LL, [&] {
        for (auto c : casos)
        {
            fp::Expression e(c);
            for (int i = 0; i < n; ++i)
                sink = e.evaluateFunction({-10.0, 10.0, 10000}).back().y;
        }
    });
}

int main(int argc, char **argv)
{
    std::string_view fase = argc > 1 ? argv[1] : "all";
    if (fase == "parse" || fase == "all")
        parse(500000);
    if (fase == "eval" || fase == "all")
        eval(15000000);
    if (fase == "sample" || fase == "all")
        sample(1500);
}