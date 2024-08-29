void func3() {
    return;
}

void func2() {
    func3();
    return;
}

void func4() {
    func3();
    return;
}

void func1() {
    func3();
    func4();
    return;
}

int main() {
    func1();
    func2();
    func4();
    return 0;
}