#!/bin/bash

#test  some preset scenarios with known outcome
# start server just before running the test

#check server
is_running=1
pgrep sp_server || is_running=0

if [[ $is_running -eq 0 ]]; then
	echo "sp_server should be running"
	exit 0
fi

#set some vars

server=http://localhost:8888
MAX_TERMINALS=100
t_v="json={'CardType':'Visa','TransactionType':'Credit'}" 		#Visa:Credit
t_m="json={'CardType':'MasterCard','TransactionType':'Credit'}" #MAsterCard:Credit
t_s="json={'CardType':'EFTPOS','TransactionType':'Savings'}"	#Eftpos:Savings
t_c="json={'CardType':'EFTPOS','TransactionType':'Cheque'}"		#EFtpos:Cheque
post_data=""
q="-q"

curl_post() {

	data=$1
	url=$2
	
	curl -ss -X POST -d "$data" $server/$url
}
curl_get() {
	url=$1
	curl -ss -X GET $server/$url
}


#curl_post " " terminal
#curl_post "$t_v" "terminals/0"
#curl_get terminals
#curl_get "terminals/0"


# Scenario 0
# query /terminals - should get an error
# query /terminals/0 - should get an error

#begin
scenario=0 
t=0
echo "Scenario $scenario ====="
echo -n "   Test $t ... "
curl_get terminals | grep $q "error" && echo OK || echo FAIL
#curl -X GET -ss "$server/terminals" |grep $q "error" && echo OK || echo FAIL
sleep .5
t=$(($t + 1))
echo -n "   Test $t ... "
curl_get "terminals/0" | grep $q "error" && echo OK || echo FAIL
#curl -X GET -ss "$server/terminals/0" |grep $q "error" && echo OK || echo FAIL
sleep .5
#end

# Scenario 1
# test 0: create 1 terminal by POSTing to /terminal
# test 1: query /terminals - there should be one terminal with id 0
# test 2: query /terminals/0 - should be no error

#begin
scenario=$(($scenario + 1))
t=0
echo "Scenario $scenario ====="
echo -n "   Test $t ... "
curl_post "" "terminal" |grep $q "TerminalID" && echo OK || echo FAIL
sleep .1
t=$(($t + 1))
echo -n "   Test $t ... "
curl_get "terminals" |grep "TerminalID"|grep $q "0" && echo OK || echo FAIL
sleep .1
t=$(($t + 1))
echo -n "   Test $t ... "
curl_get "terminals/0" |grep $q "error" && echo FAIL || echo OK
sleep .1
# end

# Scenario 2
# create 10 more terminals by POSTing to /terminal 10 times
# test 0: query /terminals - there should be 11 terminals
# test 1: query /terminals/10 - should be no error
# test 2: query /terminals/11 - should be error

#begin
scenario=$(($scenario + 1))
t=0
echo "Scenario $scenario ====="
echo -n "   Test $t ... "
for i in $(seq 1 10); do
	curl_post ' ' "terminal"
	sleep .1
done
lines=$(curl_get terminals|grep "TerminalID"|wc -l)
if [[ $lines -eq 11 ]]; then
	echo OK
else
	echo FAIL
fi
t=$(($t + 1))
echo -n "   Test $t ... "
curl_get "terminals" |grep "TerminalID"|grep $q "10" && echo OK || echo FAIL
sleep .1
t=$(($t + 1))
echo -n "   Test $t ... "
curl_get "terminals" |grep "TerminalID"|grep $q "11" && echo FAIL || echo OK
sleep .1
#end

# Scenario 3
# attepmt to create MAX_TERMINALS more terminals
# test 0: make sure there are only MAX_TERMINALS
# test 1: query for terminal id MAX_TERMINALS - should FAIL
# test 2: query for terminal id (MAX_TERMINALS - 1) - should be OK

#begin
scenario=$(($scenario + 1))
t=0
echo "Scenario $scenario ====="
echo -n "   Test $t ... "
for i in $(seq 1 $MAX_TERMINALS); do
	curl_post ' ' terminal > /dev/null
	sleep .1
done
lines=$(curl_get terminals|grep "TerminalID"|wc -l)
if [[ $lines -eq $MAX_TERMINALS ]]; then
	echo OK
else
	echo FAIL
fi
t=$(($t + 1))
echo -n "   Test $t ... "
curl_get "terminals" |grep "TerminalID"|grep $q "$MAX_TERMINALS" && echo FAIL || echo OK
sleep .1
t=$(($t + 1))
echo -n "   Test $t ... "
curl_get "terminals" |grep "TerminalID"|grep $q "$(($MAX_TERMINALS - 1))" && echo OK || echo FAIL
sleep .1
#end

# Scenario 4
# create some transaction data and display result
# test 0: create 2 MasterCard/Credit transactions for term 0
# test 1: create 2 EFTOPS/savings transactions for term 0
# test 2: create 20 Visa transactions for all terms,
# 	take random terminal, there should be 20 Visa trasnactions, all type Credit


#begin
scenario=$(($scenario + 1))
t=0
echo "Scenario $scenario ====="
echo -n "   Test $t ... "
curl_post "$t_m" "terminals/0" > /dev/null
curl_post "$t_m" "terminals/0" > /dev/null
lines=$(curl_get terminals/0|grep "MasterCard"|grep "Credit"|wc -l)
if [[ $lines -eq 2 ]]; then
	echo OK
else
	echo FAIL
fi
t=$(($t + 1))
echo -n "   Test $t ... "
curl_post "$t_s" "terminals/0" > /dev/null
curl_post "$t_s" "terminals/0" > /dev/null
lines=$(curl_get terminals/0|grep "EFTPOS"|grep "Savings"|wc -l)
if [[ $lines -eq 2 ]]; then
	echo OK
else
	echo FAIL
fi

t=$(($t + 1))
max=$(expr $MAX_TERMINALS - 1)
for i in $(seq 0 $max); do
	for j in $(seq 1 20); do
	curl_post "$t_v" "terminals/$i" > /dev/null
	done
done

random=$(( ( RANDOM % $MAX_TERMINALS ) ))

count=$(curl_get "terminals/$random" | grep "Visa"|grep "Credit"|wc -l)

if [[ $count -eq 20 ]]; then
	echo OK
else
	echo FAIL
fi

#end
exit 0

