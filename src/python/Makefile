.PHONY: swig python all clean

# Sick makefile for all your python cvxcanon needs

swig:
	swig -outcurrentdir -c++ -python -I../ ../CVXcanon.i
	g++ -std=c++11 -O3 -gdwarf-3 -fPIC -Wall -I../ `python-config --include --ldflags` -I/Users/JohnMiller/Documents/junior_year/quarter_3/ee364b/final_project/CVXcanon/.env/lib/python2.7/site-packages/numpy/core/include -shared ../LinOpOperations.cpp ../CVXcanon.cpp CVXcanon_wrap.cxx -o _CVXcanon.so
	cp CVXcanon.py _CVXcanon.so ../../tests/python

python:
	cd ../../include/cvxpy; sudo python setup.py install;

clean:
	$(RM) *.*o
	$(RM) *.pyc
	$(RM) CVXcanon.py
	$(RM) *_wrap.cxx
	$(RM) ../../tests/python/CVXcanon.py
	$(RM) ../../tests/python/_CVXcanon.so

all: 
	clean
	swig
	python