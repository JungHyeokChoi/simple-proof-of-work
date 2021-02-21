solo.out : block/block.o utils/utils.o sha256/sha256.o mining/mining.o solo.o
	gcc -o solo.out block/block.o utils/utils.o sha256/sha256.o mining/mining.o solo.o

miner_server.out : block/block.o utils/utils.o sha256/sha256.o mining/mining.o miner_server.o
	gcc -o miner_server.out block/block.o utils/utils.o sha256/sha256.o mining/mining.o miner_server.o -lpthread

miner_client.out : block/block.o utils/utils.o sha256/sha256.o mining/mining.o miner_client.o
	gcc -o miner_client.out block/block.o utils/utils.o sha256/sha256.o mining/mining.o miner_client.o -lpthread

mining_test.out : block/block.o utils/utils.o sha256/sha256.o mining/mining.o mining/mining_test.o
	gcc -o mining/mining_test.out block/block.o utils/utils.o sha256/sha256.o mining/mining.o mining/mining_test.o

sha256_test.out : sha256/sha256.o sha256/sha256_test.o
	gcc -o sha256/sha256_test.out sha256/sha256.o sha256/sha256_test.o

block_test.out : block/block.o utils/utils.o block/block_test.o
	gcc -o block/block_test.out utils/utils.o block/block.o block/block_test.o

utils.o : utils/utils.c
	gcc -c utils/utils.c

block.o : block/block.c
	gcc -c block/block.c

block_test.o : block/block_test.c
	gcc -c block/block_test.c

sha256.o : sha256/sha256.c
	gcc -c sha256/sha256.c

sha256_test.o : sha256/sha256_test.c
	gcc -c sha256/sha256_test.c

mining.o : mining/mining.c
	gcc -c mining/mining.c

mining_test.o : mining/mining_test.c
	gcc -c mining/mining_test.c

solo.o : solo.c
	gcc -c solo.c

miner_server.o : miner_server.c
	gcc -c miner_server.c

miner_client.o : miner_client.c
	gcc -c miner_client.c

all : solo.out miner_server.out miner_client.out mining_test.out sha256_test.out block_test.out

clean :
	rm block/*.o block/*.out mining/*.o mining/*.out sha256/*.o sha256/*.out utils/*.o *.o *.out