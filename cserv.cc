/*
 * Simple concurrent client which repeatedly receives lines from
 * clients and return them prefixed by the string "ACK: ".  Release
 * the connection with a client upon receipt of the string "quit"
 * (case sensitive) or upon a connection close by the client.
 *
 * Of course, the concurrent variant is almost identical to the purely
 * iterative one (i.e., the one that serves one client at a time),
 * except for the call to fork.  Do compare the complexity of this
 * code with the complexity of the code that simulates concurrency.
 *
 * By Stefan Bruda
 */


/*
* an extention to professor's implementation of concurrent server
* attempting to implement the CS 590 project
*
* By Shahriar Minaei-Jalil
* Student ID: 002 30 17 23
*/

#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include "tcp_utils.h"
#include <unistd.h> // getopt for command-line processing

#include <thread>
#include <pthread.h>
#include <mutex>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>

#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.


using namespace std;

class SynchronizedFile {
    public:
        SynchronizedFile (const string& path) : _path(path) {
            // Open file for writing...
            // stream to read and write from/into 'bbserv.txt' file
            
            strm_bbserv.open("bbserv.txt", std::ios_base::out | std::ios_base::app);
        }

        void write (const string& dataToWrite) {
            // Write to the file in a synchronized manner (described below)...

            std::lock_guard<std::mutex> lock(_writerMutex);
            strm_bbserv << dataToWrite << std::endl;

        }

    private:
        string _path;
        std::mutex _writerMutex;
        std::fstream strm_bbserv;
};

class Writer {
    public:
        Writer (std::shared_ptr<SynchronizedFile> sf) : _sf(sf) {}

        void someFunctionThatWritesToFile (string line="Some data to write...") {
            // Do some work...
            _sf->write(line);
            std::cout<<"write in some function that writes to file.\n";

        }
    private:
        std::shared_ptr<SynchronizedFile> _sf;
};

std::string generate_message_number(std::string counter){

    boost::uuids::random_generator generator;

    boost::uuids::uuid uuid1 = generator();
    // std::cout << uuid1 << std::endl;

    int previous = stoi(counter);

    // Function to find the last
    // character ch in str
    size_t found = to_string(uuid1).find_last_of("-");

    if (found != string::npos){
        return to_string(uuid1).substr(found+1) + to_string(previous+1);
    } else{
        return (std::string)"123456789000" + to_string(previous+1);
    }
    
}
/*
* returns a vector of user command splited by space
*/
std::vector<std::string> split(char str[], int n, char del){
    // declaring temp string to store the curr "word" upto del
    std::string temp = "";
    std::vector<std::string> words;

    for(int i=0 ; i<n ; i++){
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


/*
* process the client command
*/
void proccess_req(char req[]){
    std::string command = req;
    // printf("text in proccess_req is: %s.\n", command.c_str());

    std::vector<std::string> words;

    // words = split(req, ' ');

    for (std::vector<std::string>::const_iterator i = words.begin(); i != words.end(); i++){
        std::cout << *i << ' ';
        // printf("%s ", *i);
    }
}


/*
 * Repeatedly receives requests from the client and responds to them.
 * If the received request is an end of file or "quit", terminates the
 * connection.  Note that an empty request also terminates the
 * connection.  Same as for the purely iterative server.
 */
void do_client (int sd) {
    const int ALEN = 256;
    char req[ALEN];
    const char* ack = "ACK: ";
    int n;
    
    std::ofstream out_to_file;
    out_to_file.open("received_commands.txt", std::ios::app);

    // Create the synchronized file
    auto synchronizedFile = std::make_shared<SynchronizedFile>("bbserv.txt");

    // Create the writers using the same synchronized file
    Writer writer1(synchronizedFile);
    Writer writer2(synchronizedFile);

    // Loop while the client has something to say...
    while ((n = readline(sd,req,ALEN-1)) != recv_nodata) {
        if (strcmp(req,"quit") == 0) {
            printf("Received quit, sending EOF.\n");
            shutdown(sd,1);
            return;
        }

        //saves the received command to a local file
        out_to_file << req << std::endl;

        // proccess_req(req);
        std::vector<std::string> words;

        words = split(req, std::char_traits<char>::length(req), ' ');

        std::cout<<words[0]<<std::endl;

        if(words[0] == "WRITE"){
            std::cout<<"this is a write command.\n";
            auto uuid = generate_message_number(std::string("1000"));
            std::string temp = uuid;
            temp += "/";
            temp += "shahriar";
            temp += words[1];
            writer1.someFunctionThatWritesToFile(temp);
            

            send(sd,ack,strlen(ack),0);
            send(sd,temp.c_str(),temp.length(),0);
            send(sd,"\n",1,0);
        }
        

        // for (std::vector<std::string>::const_iterator i = words.begin(); i != words.end(); i++){
        //     std::cout << *i << ' ';
        //     // printf("%s ", *i);
        // }
        

        // send(sd,ack,strlen(ack),0);
        // send(sd,req,strlen(req),0);
        // send(sd,"\n",1,0);
    }
    // read 0 bytes = EOF:
    printf("Connection closed by client.\n");
    shutdown(sd,1);
}

/*
 * Cleans up "zombie children," i.e., processes that did not really
 * die.  Will run each time the parent receives a SIGCHLD signal.
 */
void cleanup_zombies (int sig) {
    int status;
    printf("Cleaning up... \n");
    while (waitpid(-1,&status,WNOHANG) >= 0)
        /* NOP */;
}

int main (int argc, char** argv) {
    const int port = 9001;   // port to listen to
    const int qlen = 32;     // incoming connections queue length

    int msock, ssock;               // master and slave socket
    struct sockaddr_in client_addr; // the address of the client...
    unsigned int client_addr_len = sizeof(client_addr); // ... and its length

    // out stream to write into 'bbserv.log' file
    std::fstream strm_log;
    strm_log.open("bbserv.log", std::ios_base::out | std::ios_base::app );

    // out stream to write into 'bbserv.pid' file
    std::fstream strm_pid;
    strm_pid.open("bbserv.pid", std::ios_base::out | std::ios_base::app );

    // out stream to write into 'bbserv.conf' file
    std::fstream strm_conf;
    strm_conf.open("bbserv.conf", std::ios_base::in );

    // // stream to read and write from/into 'bbserv.conf' file
    // std::fstream strm_bbserv;
    // strm_bbserv.open("bbserv.txt", std::ios_base::in | std::ios_base::out );

    std::vector<std::thread> threads;
    char greeting[] = "Greetings...\n";

    msock = passivesocket(port,qlen);
    if (msock < 0) {
        perror("passivesocket");
        return 1;
    }
    printf("Server up and listening on port %d.\n", port);
    
    signal(SIGCHLD,cleanup_zombies);  // clean up errant children...
    
    while (1) {
        // Accept connection:
        ssock = accept(msock, (struct sockaddr*)&client_addr, &client_addr_len);
        if (ssock < 0) {
            perror("accept");
            return 1;
        }
        
        int pid = fork();
        if (pid < 0)
            perror("fork");
        else if (pid == 0) {  // child, manages the client-server communication
            close(msock);     // child does not listen to msock...
            printf("Incoming client.\n");

            //sending a Greeting to newly connected client
            send(ssock,greeting,strlen(greeting),0);

            // do_client(ssock);  //calling the method to proccess the message
            threads.push_back(std::thread(do_client, ssock));
            threads.back().join();
            close(ssock);
            printf("Outgoing client removed.\n");
            return 0;
        }
        else // parent
            close(ssock);  // parent has no use for ssock...
    }
    return 0;   // will never reach this anyway...
}
