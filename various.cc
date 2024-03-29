#include <iostream>
// #include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.
#include <regex>
#include "tcp_utils.h"


using namespace std;




std::string generate_uuid(){

    std::ifstream infile;     
    infile.open("bbserv.txt", std::ios_base::in);
    string lastLine;  
    if(infile.is_open())
    {
        // getline(infile,lastLine);                      // Read the current line
        // cout << "getline: " << lastLine << '\n';     // Display it

        int pos = infile.tellg();
        std::cout<< "before seekg: " << pos<<std::endl;

        infile.seekg(-1, ios_base::end);  // go to one spot before the EOF

        pos = infile.tellg();
        std::cout<< "after seekg: " << pos<<std::endl;

        bool keepLooping = true;
        while(keepLooping) {
            char ch;
            infile.get(ch);                            // Get current byte's data

            int pos = infile.tellg();
            std::cout<< "pos first: " << pos<<std::endl;

            if((int)infile.tellg() <= 1) {             // If the data was at or before the 0th byte
                infile.seekg(0);                       // The first line is the last line
                keepLooping = false;                // So stop there
            }
            else if(ch == '\n') {                   // If the data was a newline
                keepLooping = false;                // Stop at the current position.
            }
            else {                                  // If the data was neither a newline nor at the 0 byte
                infile.seekg(-2,ios_base::cur);        // Move to the front of that data, then to the front of the data before it
            }
        }

                  
        getline(infile,lastLine);                      // Read the current line
        cout << "Result: " << lastLine << '\n';     // Display it

        infile.close();
    }

    return lastLine;
    
}


void func(int a){
    std::cout<<"this is the "<<a<<" thread! haha "<<std::endl;
}

std::vector<std::string> split(std::string str, char del){
    // declaring temp string to store the curr "word" upto del
    std::string temp = "";
    std::vector<std::string> words;

    
    for(int i=0; i<(int)str.size(); i++){
        // If cur char is not del, then append it to the cur "word", otherwise
        // you have completed the word, print it, and start a new word.
        if(str[i] != del){
            temp += str[i];
        }
        else{
            // std::cout <<"in split: "<< temp << " ";
            words.push_back(temp);
            
            temp = "";
        }
    }
    // std::cout <<"in split: "<< temp <<std::endl;
    words.push_back(temp);
    
    return words;
}

std::vector<std::string> split(char str[], int n, char del){
    std::string temp = "";
    std::vector<std::string> words;

    for(int i=0 ; i<n ; i++){
        if(str[i] != del){
            temp += str[i];
        }
        else{
            // std::cout <<"in split: "<< temp << " ";
            words.push_back(temp);
            
            temp = "";
        }
    }
    // std::cout <<"in split: "<< temp <<std::endl;
    words.push_back(temp);

    
    
    return words;
}


int main (){
    
    // std::vector<std::thread> threads;
    // threads.push_back(std::thread(func, 1));
    // threads.back().join();

    // boost::uuids::random_generator generator;
    // boost::uuids::uuid uuid1 = generator();
    // std::cout << uuid1 << std::endl;
    // boost::uuids::uuid uuid2 = generator();
    // std::cout << uuid2 << std::endl;

    auto uuid = generate_uuid();
    std::cout<<"last line: "<<uuid<<std::endl;
    // std::cout<<uuid<<std::endl;
    // std::cout<<uuid.size()<<std::endl;

    // bool contains_non_alpha = std::regex_match(uuid, std::regex("^[A-Za-z0-9 _]*[A-Za-z0-9][A-Za-z0-9 _]*$"));


    // // std::cout<<contains_non_alpha<<std::endl;
    // std::vector<std::string> words;
    // // std::string text = "User shahriar mj";
    
    // std::fstream strm;

    // strm.open("bbserv.txt", std::ios_base::in   );

    // if(strm.is_open()){
    //     std::cout<<"bbserv file opened!\n";

    //     // std::string line;
    //     char line[250];

    //     while ( strm.getline(line, 250, '\n') ) {
    //             // strm.getline(line, 250, '\n');
    //             std::cout << line << std::endl;
    //             words = split(line, std::char_traits<char>::length(line), '/');

    //             // for (std::vector<std::string>::const_iterator i = words.begin(); i != words.end(); i++){
    //             //     std::cout << *i << ' ';
    //             // }
    //             std::cout<<std::endl<<words.size()<<std::endl;
    //     }       

    // } else {
    //     std::cout<<"Error, bbserv file Not opened!\n";
    // }
    
    
    // strm << text << std::endl;

    // std::string line = "";
    // strm>>line;
    // std::cout<<line<<std::endl;

    /*  out stream to file */
    // std::ofstream out;
    // // std::ios::app is the open mode "append" meaning
    // // new data will be written to the end of the file.
    // out.open("file.txt", std::ios::app);
    // std::string str = "I am here.";
    // out << text << std::endl;

    /*  iterating through vector  */
    // words = split(text, ' ');
    // for (std::vector<std::string>::const_iterator i = words.begin(); i != words.end(); i++){
    //     std::cout << *i << ' ';
    // }
    // std::cout<<std::endl<<words[1]<<std::endl;
    // for (auto i: words){
    //     std::cout << i << std::endl;
    // }

    // std::fstream strm{"bbserv.txt",
    //                         std::ios_base::in | std::ios_base::out | std::ios_base::binary};
    // if (!strm.is_open())
    // {
    //     std::cerr << "Failed to open file" << std::endl;
    //     return -1;
    // }

    // std::string movieName{};
    // std::getline(std::cin, movieName);

    // std::string line{};
    // line.reserve(256);

    // long long int pos = strm.tellp();
    // for (line; std::getline(strm, line);)
    // {
    //     if (line.find(movieName) != std::string::npos){
    //         std::cout<<line<< " " << line.find(movieName)<<std::endl;
    //         break;
    //     }
    //     line.clear();
    //     pos = strm.tellp();
    //     std::cout<<"pos: "<<pos<<std::endl;

    // }

    // if (strm.eof())
    // {
    //     std::cerr << "Failed to find the movie by name" << std::endl;
    //     return -1;
    // }

    // long long int curPos = strm.tellp();

    // // TODO: check format
    // long long int commaPos = line.find('/');
    // std::cout<<"slash position: " << commaPos << " pos + slash: " << pos+commaPos << std::endl;
    // strm.seekp( pos + commaPos + 1);

    // std::string liked = " inputs ";
    // strm << liked;
    // // strm.seekp(pos + commaPos);
    // // strm << ++liked;


    return 0;
    
}