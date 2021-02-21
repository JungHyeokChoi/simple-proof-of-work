# Simple-proof-of-work

 #### - This is simple proof-of-work(pow) example <br />

## 1. environment

 - Ubuntu 18.04.3 LTS

 - gcc (Ubuntu 7.5.0-3ubuntu1~18.04) 7.5.0

 - C

   

## 2. Data structure

  -	LinkedList
  -	file : ./block/block.c



## 3. Hash

 - SHA256
 - file : ./sha256/sha256.c
 - Author : Brad Conte (brad AT bradconte.com)



## 4. Mining

 - Proof-of work
 - file : ./mining/mining.c



## 5. Get Start

#### a. Makefile

```
make all
```



#### b.  solo.out

```
./solo.out
```



#### c. Competitive mining (Imperfect)

**Terminal Server (TCP)**

```
./miner_server 8080
```

**Terminal Client (TCP)**

```
./miner_client 127.0.0.1 8080
```



#### d. clean

```
make clean
```

