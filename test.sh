#run-uuos -m pytest -s programs/uuos/extlib/uuosio/tests/test_micropython.py
#run-uuos -m pytest -s --count 30000 -x programs/uuos/extlib/uuosio/tests/test_micropython.py

cp -r programs/uuos/extlib/uuosio/* /Users/newworld/opt/anaconda3/lib/python3.7/site-packages/uuosio
run-uuos -m pytest -x -s $1 $2 $3 $4

