#include <iostream>
#include <fstream>
#include <string.h>
#include <boost/filesystem.hpp>

bool isBigEndian()
{
    int a = 1;
    return !((char*)&a)[0];                                 // Return the opposite of the first binary number of the memory address stored in 'a'.
}

int convertToInt(char* buffer, int len)
{
    int a = 0;
    
    if(!isBigEndian())
        for (int i = 0; i < len; i++)
            ((char*)&a)[i] = buffer[i];
    else
        for (int i = 0; i < len; i++)
            ((char*)&a)[3-i] = buffer[i];
    
    return a;
}

char* readAndWrite(std::ofstream &out, std::string ifn, int track)
{
    int chan = 0;
    int samplerate = 0;
    int bps = 0;
    int size = 0;
    
    char buffer[4];
    std::ifstream in(ifn, std::ios::binary); // Open current input file
    //std::ofstream out;
    //out.open("output.wav", std::ios::binary);
    
    std::cout << "writing track " << track << std::endl;
    
    //out.write(track, 4); // Write the tracks field with "tracks" integer
    
    
    /// CORE WAVE FILE ///
    in.read(buffer, 4);                                     // Should be "RIFF"
    
    if(strncmp(buffer, "RIFF", 4)!=0)                       // Check it's a RIFF file
    {
        std::cout << "This is not a valid WAVE file." <<  std::endl;
        return NULL;
    }
    
    out.write(buffer, 4); // Write "RIFF"
    in.read(buffer, 4);                                     // File size
    out.write(buffer, 4); // Write file size
    in.read(buffer, 4);                                     // "WAVE"
    out.write(buffer, 4); // Write "WAVE"
    in.read(buffer, 4);                                     // "fmt "
    out.write(buffer, 4); // Write "fmt "
    in.read(buffer, 4);                                     // 16 bytes for PCM
    out.write(buffer, 4); // Write bit depth
    in.read(buffer, 2);                                     // Type of format, should be 1 for PCM
    out.write(buffer, 2); // Write the type of format
    in.read(buffer, 2); chan = convertToInt(buffer, 2);     // Number of channels
    out.write(buffer, 2); // Write the number of channels
    in.read(buffer, 4); samplerate = convertToInt(buffer, 4); // Sample rate
    out.write(buffer, 4); // Write the sample rate
    in.read(buffer, 4);                                     // Byte rate = (sample rate * bitspersample * channels)/8
    out.write(buffer, 4); // Write the byte rate
    in.read(buffer, 2);                                     // Block Align
    out.write(buffer, 2); // Write the block align
    in.read(buffer, 2); bps = convertToInt(buffer, 2);      // Bits Per Sample
    out.write(buffer, 2); // Write the bits per sample
    in.read(buffer, 4);                                     // Subchunk2ID - "data"
    out.write(buffer, 4); // Write "data"
    in.read(buffer, 4); size = convertToInt(buffer, 4);     // Size of the data
    out.write(buffer, 4); // Write the size of data
    
    // The audio data of .WAV file
    char* data = new char[size]; // Declare binary data variable
    in.read(data, size); // Load audio data
    out.write(data, size); // Write the data
    /// CORE WAVE FILE ///
    
    return data; // Return this audio data
}

int findFiles(std::ofstream &out, const char* dir)
{
    int i = 0;
    boost::filesystem::path p (dir);
    boost::filesystem::directory_iterator end_itr;
    
    // Cycle through the directory
    for(boost::filesystem::directory_iterator itr(p); itr != end_itr; ++itr, ++i)
    {
        // If it's not a directory, list it.
        if(boost::filesystem::is_regular_file(itr->path()))
        {
            // Assign current file name to current_file and print it out to console.
            std::string current_file = itr->path().string();
            std::cout << current_file << std::endl;
            
            readAndWrite(out, current_file, i);
        }
    }
    return i;
}

int main() 
{
    std::ofstream out;
    out.open("output.wav", std::ios::binary);
    
    out.write("AMBI", 4); // Write "AMBI" // Write the "AMBI" file header
    
    int foundFiles = 0;
    char dir[50];
    std::ifstream file1;
    
    std::cout << "Enter the directory containing valid .WAV files:" << std::endl;
    std::cin.getline(dir, 50);
    
    foundFiles = findFiles(out, dir);
    std::cout << "Found " << foundFiles << " files." << std::endl;

    return 0;
}
