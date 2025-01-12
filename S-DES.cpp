#include <iostream>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

int binaryToDecimal(string binary) {
    int decimal = 0;
    int length = binary.length();
    for (int i = 0; i < length; i++) {
        if (binary[length - i - 1] == '1') {
            decimal += pow(2, i);
        }
    }
    return decimal;
}

string DecimalToBinary(int num) {
    string str;
    while (num) {
        if (num & 1)
            str += '1';
        else
            str += '0';
        num >>= 1; // Right Shift by 1  
    }
    return str;
}

string reverse(string str) {
    string out = "";
    for (int i = str.size() - 1; i >= 0; i--)
        out += str[i];

    return out;
}

string keyGen(string key, int* ip) {
    string key1 = "";
    for (int i = 0; i < 8; i++) {
        key1 += key[ip[i] - 1]; // Adjusting for 1-based indexing
    }
    return key1;
}

string key2Gen(string key, int* ip) {
    string l = key.substr(0, 4);
    string r = key.substr(4, 8);

    int l1 = binaryToDecimal(l);
    int r1 = binaryToDecimal(r);
    l1 = (l1 << 2);
    r1 = (r1 << 2);
    string cls = reverse(DecimalToBinary(l1)) + reverse(DecimalToBinary(r1));
    string key2 = keyGen(cls, ip);

    return key2;
}

string binXOR(string p1, string p2) {
    string result = "";
    for (int i = 0; i < p1.size(); i++) {
        if (p1[i] == p2[i]) {
            result += "0";
        } else {
            result += "1";
        }
    }
    return result;
}

string encrypt(string pt, string key, int* ip, int* ip1, int* ep, int s0[4][4], int s1[4][4], int* p) {
    string key1 = keyGen(key, ip);
    string ptTrans = keyGen(pt, ip1);

    string l = ptTrans.substr(0, 4);
    string r = ptTrans.substr(4, 8);
    string epTrans = keyGen(r, ep);

    string xo = binXOR(epTrans, key1);
    string l1 = xo.substr(0, 4);
    string r1 = xo.substr(4, 8);

    string s0RowBinary = string(1, l1[0]) + string(1, l1[3]);
    int s0Row = binaryToDecimal(s0RowBinary);
    int s0Col = binaryToDecimal(l1.substr(1, 2));

    string s1RowBinary = string(1, r1[0]) + string(1, r1[3]);
    int s1Row = binaryToDecimal(s1RowBinary);
    int s1Col = binaryToDecimal(r1.substr(1, 2));

    int s00 = s0[s0Row][s0Col];
    int s01 = s1[s1Row][s1Col];

    string s = reverse(DecimalToBinary(s00)) + DecimalToBinary(s01);
    string pTrans = keyGen(s, p);

    string t = binXOR(l, pTrans);
    string ct = r + t;

    return ct;
}

int main() {
    int ip[] = {6, 3, 7, 4, 8, 5, 10, 9};
    int ip1[] = {2, 6, 3, 1, 4, 8, 5, 7};
    int ep[] = {4, 1, 2, 3, 2, 3, 4, 1};
    int s0[4][4] = {{1, 0, 3, 2}, {3, 2, 1, 0}, {0, 2, 1, 3}, {3, 1, 3, 2}};
    int s1[4][4] = {{0, 1, 2, 3}, {2, 0, 1, 3}, {3, 0, 1, 0}, {2, 1, 0, 3}};
    int p[] = {2, 4, 3, 1};

    string pt = "10010111";
    string key = "0000111000";

    string k1 = keyGen(key, ip);
    string k2 = key2Gen(key, ip);
    string ct = encrypt(pt, key, ip, ip1, ep, s0, s1, p);

    cout << "Ciphertext: " << ct << endl;
    return 0;
}
