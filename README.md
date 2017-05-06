This is a Centralized Multi-User Concurrent Bank Account Manager.

Server:
1. The server receives queries from customers (ATM clients,Clients/Billing agencies performing online transactions) 
   for operations on accounts.
2. The server has the following functionalities:

          i) Is able to accept multiple concurrent customer requests(i.e. multi-threaded)
          ii) It provides locking/protection for access to an account records during shared access (i.e., a user might 
              be depositing money into his account and at the same time a online billing agent might be withdrawing 
              money from the same account).
          iii) Maintains correctness of records at each record, (i.e., allow withdrawal from an account only if it 
                 has sufficient funds etc)

 Example of input file (Records.txt) format read by the server : 

              101 Peter 16000
              102 John 1200
              103 Gambo 11000
  The format for each line being: account number, name, balance amount
  
Client:
1. A client issues requests to the server from a transaction based on account numbers.
2. A Client has the following functionalities:

         i) Issues withdrawal or deposit requests.
         ii) For ease of testing and to make experiments bigger, I have made our clients to issue requests at fixed 
             time intervals. 
         iii) A client can read an input file for transaction information and perform tasks 
              accordingly.

Example of the format I have taken as input file (Transaction.txt) for each client and used in testing.

         10 101 w 200
         25 101 d 300
         26 105 d 150
