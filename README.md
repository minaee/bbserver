# bbserver
CS 590 project


## how to run
 1. open terminal and run the following command:

    git clone https://github.com/minaee/bbserver.git

 2. change directory to bbserver folder

 3. clean the potential previous executable files and make again

    make clean
    make

 4. run the concurrent server:

    ./cserv

 5. open another terminal window and run the client to connect to `localhost@9001`

    ./triv_client localhost 9001

repeat step 5 for more clients.

 6. in the client's terminal you have these options:

   > `USER <the client's name>`
   >
   > `READ <message number>`
   >
   > `WRITE <message>`
   >
   > `REPLACE <message number>/<your new message to replcae old one>`
   >
   > `<quit> or <blank input>  // to disconnect this client from server`
   >

check bbserv.txt for received messages.
