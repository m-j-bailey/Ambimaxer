#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <boost/filesystem.hpp>


namespace little_endian_io {
    
    template <typename Word>
    std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word )) {
    for (; size; --size, value >>= 8)
        outs.put( static_cast <char> (value & 0xFF));
    return outs;
  }
}

using namespace little_endian_io;

bool isBigEndian() {                                       
    
    int a = 1;
    return !((char*)&a)[0];                                 // Return the opposite of the first binary number of the memory address stored in 'a'.
}

int convertToInt(char* buffer, int len) {
    
    int a = 0;
    
    if(!isBigEndian())
        for (int i = 0; i < len; i++)
            ((char*)&a)[i] = buffer[i];
    else
        for (int i = 0; i < len; i++)
            ((char*)&a)[3-i] = buffer[i];
    
    return a;
}

char* readAndWrite(std::ofstream &out, std::string ifn, int &track) {
    
    std::ifstream in(ifn, std::ios::binary); // Open current input file
    unsigned int input;
    char buffer[4];
    
    in.clear();
    in.seekg(0, std::ios::beg);
    
    // Check that the file is a .wav file //
    in.read(buffer, 4);                                     // Should be "RIFF"
    if(strncmp(buffer, "RIFF", 4)!=0) {                     // Check it's a RIFF file
        std::cout << "Track " << track << " is not a valid WAVE file." <<  std::endl;
        return NULL;
    } else {
        std::cout << "This is a valid file." << std::endl;
    }
    
    std::cout << "Writing track " << track << std::endl;
    
    /// CORE WAVE FILE ///
    
    int chan, samplerate, bps, size;
    
    if (track < 1) {
        out.write(buffer, 4);                               // Write "RIFF"
        in.read(buffer, 4);                                 // File size
        out.write(buffer, 4);                               // Write file size
        in.read(buffer, 4);                                 // "WAVE"
        out.write(buffer, 4);                               // Write "WAVE"
        in.read(buffer, 4);                                 // "fmt "
        out.write(buffer, 4);                               // Write "fmt "
        in.read(buffer, 4);                                 // 16 bytes for PCM
        out.write(buffer, 4);                               // Write bit depth
        in.read(buffer, 2);                                 // Type of format, should be 1 for PCM
        out.write(buffer, 2);                               // Write the type of format
        in.read(buffer, 2);                                 // Number of channels
        chan = convertToInt(buffer, 2);
        write_word(out, chan, 2);                           // Write the number of channels
        in.read(buffer, 4);                                 // Sample rate
        samplerate = convertToInt(buffer, 4);
        write_word(out, samplerate, 4);                     // Write the sample rate
        in.read(buffer, 4);                                 // Byte rate = (sample rate * bitspersample * channels)/8
        out.write(buffer, 4);                               // Write the byte rate
        in.read(buffer, 2);                                 // Block Align
        out.write(buffer, 2);                               // Write the block align
        in.read(buffer, 2);                                 // Bits Per Sample
        bps = convertToInt(buffer, 2); 
        write_word(out, bps, 2);                            // Write the bits per sample
        in.read(buffer, 4);                                 // Subchunk2ID - "data"
        out.write(buffer, 4);                               // Write "data"
        in.read(buffer, 4);
        size = convertToInt(buffer, 4);                     // Size of the data
        write_word(out, size, 4);                           // Write the size of data
    } else {
        in.seekg(40);                                       // Set read point to pass the skipped section
    }
    
    write_word(out, track, 1);                              // Track number
    
    std::cout << "Enter X position for track " << track << ": " << std::endl;
    std::cin >> input;
    while(input < 0 || input > 255) {
        std::cout << "Input out of range! Enter a valid input for X in " << track << ":" << std::endl;
        std::cin >> input;
    }
    write_word(out, input, 1);                              // X position
    
    std::cout << "Enter Y position for track " << track << ": " << std::endl;
    std::cin >> input;
    while(input < 0 || input > 255) {
        std::cout << "Input out of range! Enter a valid input for Y in " << track << ":" << std::endl;
        std::cin >> input;
    }
    write_word(out, input, 1);                              // Y position
    
    std::cout << "Enter Z position for track " << track << ": " << std::endl;
    std::cin >> input;
    while(input < 0 || input > 255) {
        std::cout << "Input out of range! Enter a valid input for Z in " << track << ":" << std::endl;
        std::cin >> input;
    }
    write_word(out, input, 1);                              // Z position
    
    // The audio data of .WAV file
    char* data = new char[size];                            // Declare binary data variable
    in.read(data, size);                                    // Load audio data
    out.write(data, size);                                  // Write the data
    
    /// /CORE WAVE FILE ///
    
    in.close();
    
    return data;                                            // Return this audio data
}

int countFiles(const char* dir) {
    
    int i = 0;
    boost::filesystem::path p (dir);
    boost::filesystem::directory_iterator end_itr;
    
    // Cycle through the directory
    for(boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr, ++i) {
        // If it's not a directory, list it.
        if(boost::filesystem::is_regular_file(itr->path())) {
            // Assign current file name to current_file and print it out to console.
            std::string current_file = itr->path().string();
            std::cout << "Found " << current_file << std::endl;
        }
    }
    return i;
}

void writeAllFiles(std::ofstream &out, const char* dir) {
    
    int i = 0;
    boost::filesystem::path p (dir);
    boost::filesystem::directory_iterator end_itr;
    
    // Cycle through the directory
    for(boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr, ++i) {
        // If it's not a directory, list it.
        if(boost::filesystem::is_regular_file(itr->path())) {
            // Assign current file name to current_file and print it out to console.
            std::string current_file = itr->path().string();
            std::cout << current_file << std::endl;
            
            readAndWrite(out, current_file, i);
        }
    }
    return;
}

int main() {
    
    int foundFiles = 0;
    char dir[50];
    std::ofstream out;
    out.open("output.ambx", std::ios::binary);
    out.write("AMBI", 4);                                   // Write the "AMBI" file header

    std::cout << "Enter the directory containing valid .WAV files:" << std::endl;
    std::cin.getline(dir, 50);
    
    foundFiles = countFiles(dir);
    std::cout << "Found " << foundFiles << " files." << std::endl;
    write_word(out, foundFiles, 4);                         // Write number of tracks field
    writeAllFiles(out, dir);
    out.close();
    
    std::cout << "Found " << foundFiles << " files." << std::endl;

    return 0;
}
