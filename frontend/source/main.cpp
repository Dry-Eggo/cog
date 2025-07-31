#include <format>
#include <cstdio>

int main() {
        printf("%s", std::format("Hello World\n").c_str());
}
