#include <iostream>
#include <array>


int main() {
    // Open the file for writing in binary mode
    FILE* file = fopen("newFile.test", "wb");
    std::array<char, 5> buffer;
    char l = 65;  // ASCII value of 'A'

    for (int i = 0; i < 10; i++) {
        fwrite(&l, sizeof(l), 1, file); // Write one character at a time
        l++;
    }

    // Close the file after writing
    fclose(file);

    // Reopen the file for reading in binary mode
    file = fopen("newFile.test", "rb");
    
    // Read 5 characters into the buffer
    fread(buffer.data(), sizeof(char), 5, file);

    std::cout << "read: ";
    for (char num : buffer) {
        std::cout << num << " ";  // Print the ASCII values
    }
    std::cout << "\n";

    // Close the file after reading
    fclose(file);

    return 0;
}