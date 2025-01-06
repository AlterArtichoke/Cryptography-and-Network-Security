#include <iostream>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

string PlayfairText(string text) {
    string result = "";
    for (char c : text) {
        if (isalpha(c)) {
            result += toupper(c);
        }
    }
    for (size_t i = 0; i < result.length(); i += 2) {
        if (i + 1 < result.length() && result[i] == result[i + 1]) {
            result.insert(i + 1, "X");
        }
    }
    if (result.length() % 2 != 0) {
        result += "X";
    }
    return result;
}

string caesarCipher(string text, int shift, bool encrypt) {
    string result = "";
    for (char c : text) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            int offset = (encrypt ? shift : -shift) % 26;
            result += (c - base + offset + 26) % 26 + base;
        } else {
            result += c;
        }
    }
    return result;
}

string vigenereCipher(string text, string key, bool encrypt) {
    string result = "";
    int keyIndex = 0;
    for (char c : text) {
        if (isalpha(c)) {
            char base = isupper(c) ? 'A' : 'a';
            char keyChar = toupper(key[keyIndex % key.length()]) - 'A';
            int offset = encrypt ? keyChar : -keyChar;
            result += (c - base + offset + 26) % 26 + base;
            keyIndex++;
        } else {
            result += c;
        }
    }
    return result;
}

string playfairCipher(string text, string key, bool encrypt) {
    vector<vector<char>> table(5, vector<char>(5));
    vector<bool> seen(26, false);
    key += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    key.erase(remove(key.begin(), key.end(), 'J'), key.end()); // Remove 'J'
    string filteredKey = "";

    for (char c : key) {
        c = toupper(c);
        if (isalpha(c) && !seen[c - 'A']) {
            seen[c - 'A'] = true;
            filteredKey += c;
        }
    }

    for (size_t i = 0; i < filteredKey.size(); ++i) {
        table[i / 5][i % 5] = filteredKey[i];
    }

    auto findPosition = [&](char c) {
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < 5; ++j) {
                if (table[i][j] == c) return make_pair(i, j);
            }
        }
        return make_pair(-1, -1);
    };

    text = PlayfairText(text);
    string result = "";

    for (size_t i = 0; i < text.length(); i += 2) {
        char a = text[i], b = text[i + 1];
        auto [row1, col1] = findPosition(a);
        auto [row2, col2] = findPosition(b);

        if (row1 == row2) {
            result += table[row1][(col1 + (encrypt ? 1 : 4)) % 5];
            result += table[row2][(col2 + (encrypt ? 1 : 4)) % 5];
        } else if (col1 == col2) {
            result += table[(row1 + (encrypt ? 1 : 4)) % 5][col1];
            result += table[(row2 + (encrypt ? 1 : 4)) % 5][col2];
        } else {
            result += table[row1][col2];
            result += table[row2][col1];
        }
    }

    return result;
}

int main() {
    int choice;
    cout << "Choose an encryption method:\n";
    cout << "1. Caesar Cipher\n2. Vigenere Cipher\n3. Playfair Cipher\n";
    cin >> choice;

    cin.ignore();
    string text, key;
    int shift;

    switch (choice) {
        case 1:
            cout << "Enter text: ";
            getline(cin, text);
            cout << "Enter shift value: ";
            cin >> shift;
            cout << "Encrypted text: " << caesarCipher(text, shift, true) << endl;
            cout << "Decrypted text: " << caesarCipher(caesarCipher(text, shift, true), shift, false) << endl;
            break;

        case 2:
            cout << "Enter text: ";
            getline(cin, text);
            cout << "Enter keyword: ";
            cin >> key;
            cout << "Encrypted text: " << vigenereCipher(text, key, true) << endl;
            cout << "Decrypted text: " << vigenereCipher(vigenereCipher(text, key, true), key, false) << endl;
            break;

        case 3:
            cout << "Enter text: ";
            getline(cin, text);
            cout << "Enter keyword: ";
            cin >> key;
            cout << "Encrypted text: " << playfairCipher(text, key, true) << endl;
            cout << "Decrypted text: " << playfairCipher(playfairCipher(text, key, true), key, false) << endl;
            break;

        default:
            cout << "Invalid choice.\n";
    }

    return 0;
}
