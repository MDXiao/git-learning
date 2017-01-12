## Lab Questions:

###Question 2:
(a) Consider the following Scala code.
```scala
1  val pi = 3.14
2  def circumference(r: Double): Double = {
3    val pi = 3.14159
4    2.0 * pi * r
5  }
6  def area(r: Double): Double = 
7    pi * r * r
```
Answer:

The use of pi at line 4 is bounded to the val pi = 3.14159 on line 3, because that pi is the closest in the scope to the output.
Also, since the val is created within the circumference function, the functions and code on outside won't be able to access that
specific val pi within the circumference function. The use of pi at line 7 is bounded to the val pi = 3.14 on line 1, because that is the closest pi in the scope of the function, as it can't access the pi on line 3.

(b) Consider the following Scala code.
```scala
1  val x = 3
2  def f(x: Int): Int =
3    x match {
4    case 0 => 0
5    case x => {
6        val y = x + 1
7        ({
8            val x = y + 1
9            y
10        } * f(x - 1))
11     }
12   }
13 val y = x + f(x)
```
Answer:

The use of x at line 3 is bounded to the paramter x: Int on line 2, as to do pattern match we will need the input from the user
to follow through with the code, and the x is the scope. The use of x at line 6 is bound to the x: Int on line 2 as well, as it depends on x match which depends on the parameter on line 2. Similar to the two x's above, the use of x at line 10 is bound to the parameter x: Int on line 2. These three variables are all bounded by the x parameter on line 2.
The x on line 13 is actualy bounded by the val x = 3 on line 1, since it does not depend on the x: Int on line 2, and the val x = 3 on line 1 is the closest in it's scope.

###Question 3:
Is the body of g well-typed?
```scala
1 def g(x: Int) = {
2    val (a, b) = (1, (x, 3))
3    if (x == 0) (b, 1) else (b, a + 2)
4 }
```
Answer:

I would say that the return type of the function g is Int or an integer, or specifically, an integer tuple of format (x, y). First, the types for the vals a and b: a is an Int and b is a tuple Int, due to the val (a, b) statement on line 2. We see that a = 1 and b = (x, 3), where x is also an Int due to the parameters on line 1 (x: Int). 

Body Expression (e : τ):

1. val (a, b) = (1, (x,3)) (Line 2) : (Int, (Int, Int)) with descriptions below, since both vals are declared here. 
  * a: Int -> Because in the delcaration of val (a,b), we see that a equals an integer, which is the integer 1.
  * b: (Int, Int) -> In the delcartion of val(a,b), we see that b is equal to a int tuple, which is (x,3). Explanation of tuple values below.
  * x: Int -> Declarated int he parameters in the function above (x: Int).
2. if (x == 0) (b,1) else (b, a + 2)
  * if (x == 0) : Boolean -> if statements should return true if x == 0 and false otherwise.
  * (b,1): ((Int, Int), Int) -> since b is declare above to be a 2 Int tuple, and 1 is an Int, we get ((Int, Int), Int).
  * (b, a + 2): ((Int, Int), Int) -> since b is the declared 2 Int tuple on line 2, and a is also an Int just added by 2, we get ((Int, Int), Int)
  * x: Int -> Declarated int he parameters in the function above (x: Int).
  * 3: Int -> An integer within that tuple set.

In the two return values in the if and else statements on line 3, we already know that a, b, and x are all Int values. So the return value are Int tuples, since (b,1) and (b , a + 2) are both Intger tuples. The function would be def g(x: Int): ((Int,Int), Int).





