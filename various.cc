#include <iostream>
// #include <thread>
#include <vector>
#include <string>
#include <fstream>
#include <stdio.h>
// #include <boost/uuid/uuid.hpp>            // uuid class
// #include <boost/uuid/uuid_generators.hpp> // generators
// #include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

using namespace std;



// auto generate_uuid(){
//     boost::uuids::random_generator generator;

//     boost::uuids::uuid uuid1 = generator();
//     std::cout << uuid1 << std::endl;

//     // boost::uuids::uuid uuid2 = generator();
//     // std::cout << uuid2 << std::endl;

//     return uuid1;
// }


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


void change_line(){
    
    
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


    std::vector<std::string> words;
    std::string text = "User shahriar mj";
    
    std::fstream strm;

    strm.open("configuration.txt", std::ios_base::in | std::ios_base::out | std::ios_base::app );

    if(strm.is_open()){
        std::cout<<"configuration file opened!\n";
        char line[100];
        while (strm >> line) {
                strm.getline(line, 100, '\n');
                std::cout << line << std::endl;
                words = split(line, std::char_traits<char>::length(line), ' ');

                    for (std::vector<std::string>::const_iterator i = words.begin(); i != words.end(); i++){
                        std::cout << *i << '\n';
                    }
                    std::cout<<std::endl<<words[1]<<std::endl;
        }       

    } else {
        std::cout<<"Error, configuration file Not opened!\n";
    }
    
    
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

    

    



    return 0;
    
}