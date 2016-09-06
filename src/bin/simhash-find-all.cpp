#include <iostream>
#include <unordered_set>
#include <sstream>
#include <fstream>

#include <getopt.h>

#include "simhash.h"

void usage(int argc, char** argv)
{
    std::cout << "usage: " << argv[0]
              << " --blocks BLOCKS"
              << " --distance DISTANCE"
              << " --input INPUT"
              << " --output OUTPUT\n\n"
              << "Read simhashes from input, find all pairs within distance bits of \n"
              << "each other, writing them to output. The endianness of the output is \n"
              << "the same as that of the input.\n\n"
              << "  --blocks BLOCKS        Number of bit blocks to use\n"
              << "  --distance DISTANCE    Maximum bit distances of matches\n"
              << "  --input INPUT          Path to input ('-' for stdin)\n"
              << "  --output OUTPUT        Path to output ('-' for stdout)\n";
}

std::unordered_set<Simhash::hash_t> read_hashes(std::istream& stream)
{
    std::unordered_set<Simhash::hash_t> hashes;
    Simhash::hash_t hash(0);
    while (!stream.eof())
    {
        stream.read(reinterpret_cast<char*>(&hash), sizeof(hash));
        if (!stream.fail())
        {
            hashes.insert(hash);
        }
        else
        {
            break;
        }
    }
    return hashes;
}

void write_matches(std::ostream& stream, const Simhash::matches_t& matches)
{
    for (auto it = matches.begin(); it != matches.end() && !std::cout.fail(); ++it)
    {
        stream.write(
            reinterpret_cast<const char*>(&(it->first)), sizeof(Simhash::hash_t));
        stream.write(
            reinterpret_cast<const char*>(&(it->second)), sizeof(Simhash::hash_t));
    }
    stream.flush();
}

int main(int argc, char **argv) {

    std::string input, output;
    size_t blocks(0), distance(0);

    int getopt_return_value(0);
    while (getopt_return_value != -1)
    {
        int option_index = 0;
        static struct option long_options[] = {
            {"input",    required_argument, 0, 0 },
            {"output",   required_argument, 0, 0 },
            {"blocks",   required_argument, 0, 0 },
            {"distance", required_argument, 0, 0 },
            {"help",     no_argument,       0, 0 },
            {0,          0,                 0, 0 }
        };

        getopt_return_value = getopt_long(
            argc, argv, "i:o:b:d:h", long_options, &option_index);

        switch(getopt_return_value)
        {
            case 0:
                switch(option_index)
                {
                    case 0:
                        input = optarg;
                        break;
                    case 1:
                        output = optarg;
                        break;
                    case 2:
                        std::stringstream(std::string(optarg)) >> blocks;
                        break;
                    case 3:
                        std::stringstream(std::string(optarg)) >> distance;
                        break;
                    case 4:
                        usage(argc, argv);
                        return 0;
                }
                break;
            case 'i':
                input = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case 'b':
                std::stringstream(std::string(optarg)) >> blocks;
                break;
            case 'd':
                std::stringstream(std::string(optarg)) >> distance;
                break;
            case 'h':
                usage(argc, argv);
                return 0;
            case '?':
                return 1;
        }

    }

    if (blocks == 0)
    {
        std::cerr << "Blocks must be provided and > 0" << std::endl;
        return 2;
    }

    if (distance == 0)
    {
        std::cerr << "Distance must be provided and > 0" << std::endl;
        return 3;
    }

    if (input.empty())
    {
        std::cerr << "Input must be provided and non-empty." << std::endl;
        return 4;
    }

    if (output.empty())
    {
        std::cerr << "Output must be provided and non-empty." << std::endl;
        return 5;
    }

    if (blocks <= distance)
    {
        std::cerr << "Blocks (" << blocks << ") must be >= distance (" << distance << ")"
                  << std::endl;
        return 6;
    }

    // Read input
    std::unordered_set<Simhash::hash_t> hashes;
    if (input.compare("-") == 0)
    {
        std::cerr << "Reading hashes from stdin." << std::endl;
        hashes = read_hashes(std::cin);
    }
    else
    {
        std::cerr << "Reading hashes from " << input << std::endl;
        {
            std::ifstream fin(input, std::ifstream::in | std::ifstream::binary);
            if (!fin.good())
            {
                std::cerr << "Error reading " << input << std::endl;
                return 7;
            }
            hashes = read_hashes(fin);
        }
    }

    // Find matches
    std::cerr << "Computing matches..." << std::endl;
    Simhash::matches_t results = Simhash::find_all(hashes, blocks, distance);

    // Write output
    if (output.compare("-") == 0)
    {
        std::cerr << "Writing results to stdout." << std::endl;
        write_matches(std::cout, results);
    }
    else
    {
        std::cerr << "Writing matches to " << output << std::endl;
        {
            std::ofstream fout(output, std::ofstream::binary);
            if (!fout.good())
            {
                std::cerr << "Error writing " << output << std::endl;
                return 8;
            }
            write_matches(fout, results);
        }
    }

    return 0;
}
