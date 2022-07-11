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
* an extention to original implementation of concurrent server
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
#include <regex>

std::mutex m;   //  to gaurd the counter
int counter = 1000; // counter to follow the message-number sequence
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



/*
* generate a unique message number that follows 
* the previous received message number
*/
std::string generate_message_number(int counter){

    boost::uuids::random_generator generator;

    boost::uuids::uuid uuid1 = generator();
    // std::cout << uuid1 << std::endl;

    // int previous = stoi(counter);

    // Function to find the last
    // character ch in str
    size_t found = to_string(uuid1).find_last_of("-");
    
    
    if (found != string::npos){
        return to_string(uuid1).substr(found+1) + to_string(counter);
    } else{
        return (std::string)"123456789ABC" + to_string(counter);
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
    
    std::string poster = "nobody";  // name of the current user
    
    // stroing received commands
    std::ofstream out_to_file;
    out_to_file.open("received_commands.txt", std::ios::app);

    // Create the synchronized file
    auto synchronizedFile = std::make_shared<SynchronizedFile>("bbserv.txt");

    // Create the writers using the same synchronized file
    Writer writer1(synchronizedFile);
    // Writer writer2(synchronizedFile);

    // Loop while the client has something to say...
    while ((n = readline(sd,req,ALEN-1)) != recv_nodata) {
        if ((strcmp(req,"quit") == 0 ) | (strcmp(req, "") == 0)) {
            printf("Received quit, sending EOF.\n");
            shutdown(sd,1);
            return;
        }

        //saves the received command to a local file
        out_to_file << req << std::endl;

        // proccess_req(req);
        std::vector<std::string> words;

        words = split(req, std::char_traits<char>::length(req), ' ');
        // std::cout<<words[0]<<std::endl;

        if(words[0] == "USER"){
            std::cout<<"received a USER command.\n";

            //regex to find letters and numbers and space
            if( !std::regex_match(words[1], std::regex("^[A-Za-z0-9 _]*[A-Za-z0-9][A-Za-z0-9 _]*$"))){ 
                std::string respond = "ERROR USER bad user name. it can only contains alphabets and numbers.\n";
                std::cout<<respond;
                send(sd,respond.c_str(),respond.length(),0);
                send(sd,"\n",1,0);
            } else {
                poster = words[1];
                std::string respond = "Hello ";
                respond += poster;
                send(sd,respond.c_str(),respond.length(),0);
                send(sd,"\n",1,0);
            }
            
        }
        else if(words[0] == "READ"){
            std::string message_number = words[1] ;

            std::fstream strm;
            strm.open("bbserv.txt", std::ios_base::in );
            std::string usr, msg, respond;
            if(strm.is_open()){
                // std::cout<<"bbserv file opened!\n";

                std::vector<std::string> temp;
                char line[250];
                bool found = 0;

                while ( strm.getline(line, 250, '\n') ) {
                        // strm.getline(line, 250, '\n');
                        // std::cout << line << std::endl;
                        temp = split(line, std::char_traits<char>::length(line), '/');

                        // for (std::vector<std::string>::const_iterator i = temp.begin(); i != temp.end(); i++){
                        //     std::cout << *i << ' ';
                        // }
                        // std::cout<<std::endl<<temp.size()<<std::endl;
                        if(temp[0] == message_number){
                            usr = temp[1];
                            msg = temp[2];
                            // MESSAGE message-number poster/message
                            respond = "MESSAGE" + ' ' + message_number + ' ' + usr + '/' + msg;
                            send(sd,respond.c_str(),respond.length(),0);
                            send(sd,"\n",1,0);
                            found = true;
                        }
                }       
                if(found == false){
                    // UNKNOWN message-number text
                    respond = "UNKNOWN" + ' ' + message_number + ' ' + "message not found!\n";
                    send(sd,respond.c_str(),respond.length(),0);
                    send(sd,"\n",1,0);
                }

            } else {
                // std::cout<<"Error, bbserv file Not opened!\n";
                // ERROR READ text
                respond = "ERROR READ the bulletin board file not found!\n";
                send(sd,respond.c_str(),respond.length(),0);
                send(sd,"\n",1,0);
            }
        }
        else if(words[0] == "WRITE"){
            std::cout<<"received a WRITE command.\n";
            
            auto uuid = generate_message_number(counter); //generate uuid and assign counter to the end of it
            
            std::lock_guard<std::mutex> lock(m);
            counter += 1;    // increase counter for the next message
            
            uuid += "/";
            uuid += poster;
            uuid += "/";
            for( int i=1 ; i<(int)words.size() ; i++){
                uuid += words[i];
                uuid += " ";
            }
            
            writer1.someFunctionThatWritesToFile(uuid);
            

            send(sd,ack,strlen(ack),0);
            send(sd,uuid.c_str(),uuid.length(),0);
            send(sd,"\n",1,0);
        }
        else if(words[0] == "REPLACE"){
            std::cout<<"received a REPLACE command.\n";

            std::string command_body;
            for(int i=1 ; i<(int)words.size() ; i++)
                command_body += words[i] + " ";
            std::vector<std::string> temp = split((char*)command_body.c_str(), command_body.length(), '/');

            std::cout<<temp[0]<< "\t" << temp[1]<<std::endl;
            std::string msg_number = temp[0];
            std::string new_msg = temp[1];

            std::string respond;

            std::fstream strm("bbserv.txt",
                            std::ios_base::in | std::ios_base::out | std::ios_base::binary);

            if(strm.is_open()){
                std::cout<<"the bbserv file is open."<<std::endl;

                std::vector<std::string> temp_line;
                char line[250];
                bool found = 0;
                // int size_old_msg ;

                while ( strm.getline(line, 250, '\n') ) {
                        // strm.getline(line, 250, '\n');
                        // std::cout << line << std::endl;
                        temp_line = split(line, std::char_traits<char>::length(line), '/');

                        // for (std::vector<std::string>::const_iterator i = temp_line.begin(); i != temp_line.end(); i++){
                        //     std::cout << *i << ' ';
                        // }
                        std::cout<<temp_line[0]<<"\t"<<temp_line[1]<<"\t"<<msg_number<<std::endl;
                        if(temp_line[0] == msg_number){

                            found = true;                            
                            
                            long long int pos = strm.tellp(); 
                            std::string null_msg;
                            for(int i=0 ; i<(int)strlen(line) ; i++)
                                null_msg[i] = ' ';
                            strm.seekp(pos - strlen(line) - 1);
                            strm << null_msg;
                            respond = msg_number + "/" + poster + "/" + new_msg;
                            strm << respond;
                        } 
                }

                if(found == false){
                    std::cout<<"message number not found!\n";
                }
                
                

            } else {
                // std::cout<<"Error, bbserv file Not opened!\n";
                // ERROR READ text
                respond = "ERROR REPLACE the bulletin board file not found!\n";
                send(sd,respond.c_str(),respond.length(),0);
                
            }
        }
        

        
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
    // for(int i=0 ; i<5 ; i++){
    //     threads.push_back(std::thread(do_client, ssock));
    // }


    std::string greeting = "Greetings...\n";
    greeting += "You can enter these commands:\n";
    greeting += "\t- USER <your name>\n";
    greeting += "\t- READ <message number>\n";
    greeting += "\t- WRITE <your message>\n";
    greeting += "\t- REPLACE <message number>/<your new message to replcae old one>\n";
    greeting += "\t- <quit> or null input to end the connection with server.\n";

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
            send(ssock,greeting.c_str(),greeting.length(),0);

            do_client(ssock);  //calling the method to proccess the message
            // threads.push_back(std::thread(do_client, ssock));
            // threads.back().join();

            close(ssock);
            printf("Outgoing client removed.\n");
            return 0;
        }
        else // parent
            close(ssock);  // parent has no use for ssock...
    }
    return 0;   // will never reach this anyway...
}
