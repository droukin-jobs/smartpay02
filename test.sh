echo "Dummy request"
curl -ss -X POST -d " " http://localhost:8888

echo "create terminal 0"
curl -ss -X POST -d " " http://localhost:8888/terminal

echo "create terminal 1"
curl -ss -X POST -d " " http://localhost:8888/terminal

echo "add transaction Visa:Credit to term 0"
curl -ss -X POST -d "json={'Visa','Credit'}" http://localhost:8888/terminals/0

echo "add transaction EFTPOS:Cheque to term 0"
curl -ss -X POST -d "json={'CardType':'EFTPOS','TransactionType':'Cheque'}" http://localhost:8888/terminals/0

echo "add transaction Visa:Credit to term 1"
curl -ss -X POST -d "json={'CardType':'Visa','TransactionType':'Credit'}" http://localhost:8888/terminals/1

echo "show terminal info 0"
curl -ss -X GET http://localhost:8888/terminals/0

echo "show terminal info 1"
curl -ss -X GET http://localhost:8888/terminals/1
