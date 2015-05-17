rm *.*o
rm *.pyc
rm CVXcanonPy.py
rm *_wrap.cxx

swig -c++ -python CVXcanonPy.i
# g++-4.9 -std=c++11 -Wall `python-config --include` -L/Users/jack/anaconda/lib -shared CVXcanon.cpp CVXcanonPy_wrap.cxx LinOp.cpp -o _CVXcanonPy.so
# g++-4.9 -std=c++11  -I/Users/jack/anaconda/
# g++-4.9 -std=c++11 -I/Users/jack/anaconda/include/python2.7 -L/Users/jack/anaconda/lib/python2.7 -o _CVXcanonPy.so -shared -fPIC CVXcanon.cpp LinOp.cpp  CVXcanonPy_wrap.cxx 
 g++-4.9 -std=c++11 -g -gdwarf-3 -fPIC -Wall `python-config --include --ldflags` -shared LinOpOperations.cpp CVXcanon.cpp CVXcanonPy_wrap.cxx  -o _CVXcanonPy.so
