#!/bin/bash
assert(){
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	cc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$input => $expected expected, but got $actual"
		exit 1
	fi
}

<<COMMENTOUT
assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 26 "2*3 + 4*5;"
assert 9 " 5/3 + 4/2*4;"
assert 4 "(3+5)/2;"
assert 10 "- 10 + 20;"
assert 5 " -+-5 ;"

assert 0 '0==1;'
assert 1 '42==42;'
assert 1 '0!=1;'
assert 0 '42!=42;'

assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

assert 2 '(1==1) + 1;'

assert 1 'hoge=1;'
assert 2 'a=1; b=2;'
assert 3 'a=1; b=2; a+b;'

COMMENTOUT

assert 3 'return 3;'
assert 3 'if (0) return 2 ; return 3;'
assert 5 'a=1; if (a==1) return 5; '
assert 3 'a=1; if(a==0) return 5; else return 3;'
assert 8 'i=0; while (i<8) i=i+2; return i;'
echo OK
