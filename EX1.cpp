#include <iostream>
using namespace std;

int gcdExtended(int a, int b, int &x, int &y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    int x1, y1;
    int gcd = gcdExtended(b, a % b, x1, y1);
    x = y1;
    y = x1 - (a / b) * y1;
    return gcd;
}

int modularInverse(int a, int m) {
    int x, y;
    int g = gcdExtended(a, m, x, y);
    if (g != 1) {
        cout << "Modular inverse doesn't exist for " << a << " mod " << m << endl;
        return -1;
    }
    return (x % m + m) % m;
}

int main() {
    int size;
    cout << "Enter the size of the arrays: ";
    cin >> size;

    int a[size], n[size], m[size], mi[size];
    int M = 1, Y = 0;

    cout << "Enter the values of a: ";
    for (int i = 0; i < size; i++) {
        cin >> a[i];
    }

    cout << "Enter the values of n: ";
    for (int i = 0; i < size; i++) {
        cin >> n[i];
        M *= n[i];
    }

    for (int i = 0; i < size; i++) {
        m[i] = M / n[i];
        mi[i] = modularInverse(m[i], n[i]);
        if (mi[i] == -1) {
            cout << "Error in calculating modular inverse for index " << i << endl;
            return -1;
        }
        Y += a[i] * m[i] * mi[i];
    }

    Y %= M;

    cout << "The result (Y mod M) is: " << Y << endl;
    return 0;
}
