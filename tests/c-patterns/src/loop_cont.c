int main() {
    volatile int a;
    volatile int b;
    int c = 0;

    for (int i = 0; i < a; i++) {
        if (i == b)
            continue;
        c++;
    }
    return c;
}