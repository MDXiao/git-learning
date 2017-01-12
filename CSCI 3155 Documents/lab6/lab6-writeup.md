#Lab 6 Writeup

### Problem 1

i) 

```
re ::= union
union ::= intersect | union '|' intersect
concat ::= not | concat not
not ::= ~not | star
star ::= star* | star? | star+ | atom
atom ::= . | ! | # | c | '(' re ')'
```

ii)

An example of a condition where it would go into an infinite loop would be the scala code that was provided in class for the following grammar:

Grammar:
```
re ::= union
union ::= not | union '|' not
not ::= ~not | c
```

Scala Code:
```
1  def union (next) = intersect(next) match {
2    case Success(...) => ...
3    case _ => union(next) match {...}
4  }
```

This code would go into an infinite recursion because the call to union on line 3 is the same as our initial call, which the next is not decremented and we infinite loop on the union.

iii)
```
re ::= union
union ::= intersect {'|' intersect}
intersect ::= concat {'&' concat}
concat ::= not {not}
not ::= {'~'} star
star ::= atom { '*' | '?' | '+'}
atom ::= '.' | '!' | '#' | c | '(' re ')'
```

iv)
```
re ::= union
union ::= intersect unions
unions ::= ε | '|' intersect unions
intersect ::= concat intersects
intersects ::= ε | '&' concat intersects
concat ::= not concats
concats ::= ε | not concats
not ::= nots star
nots ::= ε | nots '~'
star ::= atom stars
stars ::= ε | '+' stars | '*' stars | '?' stars
atom ::= '.' | '!' | '#' | c | '(' re ')'
```

### Problem 2

ci) 
TypeTest
```
   Γ ⊢ e1: RegExpr  Γ ⊢ e2: String
----------------------------------------
        Γ ⊢ e1.test(e2): Bool 

```

DoTest
```
        b = test(/^re$/, s)
----------------------------------------
    <M, /^re$/.test(s)> -> <M, b>

```


SearchTest1
```
        <M, e1> -> <Mp, e1p>
----------------------------------------
<M, e1.test(e2)> -> <Mp, e1p.test(e2)>

```

SearchTest2
```
         <M, e2> -> <Mp, e2p>
----------------------------------------
<M, /^re$/.test(e2)> -> <Mp, /^re$/.test(e2p)>

```

