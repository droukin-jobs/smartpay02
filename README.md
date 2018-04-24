## Dmitri Roukin for Smartpay 20-04-2018

[Download:]`git clone https://github.com/droukin-jobs/smartpay02.git`

## *This README document is based on old version (smartpay01), there maybe some obsolete infomration*

TOC

### 1 Intro
### 2 Compile
### 3 Inastall
### 4 Usage
### 5 Testing

---

## 1 INTRO

This project was designed as a main requirement for Smartpay job test (started a 16.00 18-04-2018 NZT, will be handed in by 16.00 20-04-2018). 

At the moment the following features are complete:
 - Endpoint to create new terminals
 - Endpoint to list terminals
 - Endpoint to view terminal details
 - Each connection runs in it's own thread
 - Tests for helper functions
 - Documentation (you are reading it)
 - Terminals structure is able to accept transactions
 - JSON POST input to update transactions via `"json={...}"`

Partially complete features:
 - Terminal creation is done only by empty POST request to `/terminal` endpoint

Not implemented features:
 - JSON POST input to create terminals
 - Comprehensive main program testing
 - Server config options (not required for the project but would be nice to have)
 - Needs JSON syntax checker

This version is based on a different approach to POST processing. The bsae is taken from `post_example.c` at the docs directory of libmicrohttpd source.


### Assumptions and notes

The project is supposed to compile and run on a `POSIX` compatible system, with `GNU make` and `gcc`. 

Successfuly tested on `Slackware Linux 3.9.5 #2 SMP x86_64 i5-3470 CPU @ 3.20GHz` and `Darwin 14.3.0 PDT 2015 x86_64`

---
## 2 COMPILE

There are two libraries used in this project:
 
 a. libmicrohttpd 
  - https://www.gnu.org/software/libmicrohttpd/
  - provides web server API
 
 b. CUnit
  - http://cunit.sourceforge.net/
  - provides framework for testing

These files required for compilation:

 - `main.c` 			= the server
 - `json.c json.h` 		= json utility functions
 - `url.c url.h` 		= url processing functions
 - `terminal.c terminal.h` = terminal operations 
 - `makefile` 			= can be used for compiling and some runtime testing

To compile main project use the makefile and GNU make in the projects root directory

To compile tests use the makefile in the `tests/` directory

---
## 3 INSTALL

After successful compilation the  binary `sp_server` will be available in the project root directory. Copy it wherever seems suitable.

---
## 4 USAGE

After launching the server will be available on port 8888. For GET requests you can us a web browser:
 ```
 localhost:8888
     /terminals 			list available terminals
     /terminals/{id} 		display particular terminal info
 ```
For POST requests use curl or similar programm:
```
 curl -X POST -d "json={}" localhost:8888/terminal 	create new terminal
```
```
 curl -X POST -d "json={'CardType':'Visa','TransactionType':'Credit'}" localhost:8888/terminals{id} 	create new terminal
```

To stop the server please press a key in the server's terminal. 

#Important: use `application/x-www-form-urlencoded` for Content Type


---
## 5 TESTING

Runtime tests are in `tests/test_scenarios.sh`
 - The scenarios test server operation and simulate user queries (acceptance testing)
 - Make sure `sp_server` is just started
 - Run `make test`

All function tests are in file `tests/test.c`
- At the moment there are only helper function tests checking for validity of input and some boundary conditions.
- Use `tests/makefile` to compile the tests
- Use `tests/test binary` to run tests

Note: `json_float_test` fails due to floating point format not being carefully specified, but this function is not used in the project and there fore will not be fixed in the nearest future.

---
### PS. Thank you

I am thankful to Smartpay for allowing me to take this test. This was very challenging and intense experience. I plan on resolving the issues even after the test's due date - at least I will have something interesting to work on.

Kind regards,

Dmitri
