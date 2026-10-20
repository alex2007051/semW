#include <cstddef>
#include <cstring>
#include <iostream>

constexpr std::size_t MAX_WORD_LENGTH = 12;
constexpr std::size_t BUFFER_CAPACITY = MAX_WORD_LENGTH + 1;
constexpr int INITIAL_ATTEMPTS = 3;
constexpr int INITIAL_HINTS = 2;

bool encryptWord(const char* original, char* encrypted, std::size_t capacity, int key) {
    std::size_t length = 0;
    for (const char* p = original; *p != '\0'; ++p) ++length;
    if (capacity < length + 1) {
        if (capacity > 0) encrypted[0] = '\0';
        return false;
    }
    const char* src = original;
    char* dst = encrypted;
    while (*src != '\0') {
        *dst = static_cast<char>('a' + (*src - 'a' + key) % 26);
        ++src;
        ++dst;
    }
    *dst = '\0';
    return true;
}

bool revealOneLetter(const char* original, char* revealed, std::size_t capacity) {
    std::size_t length = 0;
    for (const char* p = original; *p != '\0'; ++p) ++length;
    if (capacity < length + 1) return false;
    const char* origPtr = original;
    char* revPtr = revealed;
    while (*origPtr != '\0') {
        if (*revPtr == '_') {
            *revPtr = *origPtr;
            return true;
        }
        ++origPtr;
        ++revPtr;
    }
    return false;
}

bool checkAnswer(const char* userGuess, const char* original, int& attempts) {
    if (std::strcmp(userGuess, original) == 0) return true;
    --attempts;
    return false;
}

enum class ReadStatus { Ok, TooLong, End, Error };

ReadStatus readLine(char* buffer, std::size_t capacity) {
    std::size_t size = 0;
    bool tooLong = false;
    char ch = '\0';
    while (std::cin.get(ch)) {
        if (ch == '\n') {
            buffer[size] = '\0';
            return tooLong ? ReadStatus::TooLong : ReadStatus::Ok;
        }
        if (size < capacity - 1) {
            buffer[size++] = ch;
        } else {
            tooLong = true;
        }
    }
    buffer[size] = '\0';
    if (std::cin.bad()) return ReadStatus::Error;
    if (size == 0 && !tooLong) return ReadStatus::End;
    return tooLong ? ReadStatus::TooLong : ReadStatus::Ok;
}

bool isLowercaseWord(const char* text) {
    if (*text == '\0') return false;
    for (const char* p = text; *p != '\0'; ++p) {
        if (*p < 'a' || *p > 'z') return false;
    }
    return true;
}

int main() {
    const char secretWord[] = "cipher";
    const int key = 3;
    char encrypted[BUFFER_CAPACITY];
    char revealed[BUFFER_CAPACITY];
    char input[BUFFER_CAPACITY];
    if (!encryptWord(secretWord, encrypted, BUFFER_CAPACITY, key)) {
        std::cerr << "too small\n";
        return 1;
    }
    char* rp = revealed;
    for (const char* p = secretWord; *p != '\0'; ++p) {
        *rp++ = '_';
    }
    *rp = '\0';
    int attempts = INITIAL_ATTEMPTS;
    int hints = INITIAL_HINTS;
    bool won = false;
    while (attempts > 0) {
        ReadStatus status = readLine(input, BUFFER_CAPACITY);
        if (status == ReadStatus::End) {
            std::cout << "\nEnd\n";
            break;
        }
        if (status == ReadStatus::Error) {
            std::cerr << "Error\n";
            break;
        }
        if (status == ReadStatus::TooLong) {
            std::cout << "too long\n";
            continue;
        }
        if (input[0] == '\0') {
            continue;
        }
        if (std::strcmp(input, "hint") == 0) {
            if (hints <= 0) {
                std::cout << "Hints end.\n";
                continue;
            }
            if (revealOneLetter(secretWord, revealed, BUFFER_CAPACITY)) {
                --hints;
                std::cout << "Hint: " << revealed
                          << " (Tries: " << hints << ")\n";
            } else {
                std::cout << "All\n";
            }
            continue;
        }

        if (!isLowercaseWord(input)) {
            std::cout << "Only a..z.\n";
            continue;
        }
        if (checkAnswer(input, secretWord, attempts)) {
            std::cout << "Win word: " << secretWord << "\n";
            won = true;
            break;
        }
        std::cout << "No. Tries: " << attempts << "\n";
    }

    if (!won && attempts == 0) {
        std::cout << "Lose. Word: " << secretWord << "\n";
    }
    return 0;
}