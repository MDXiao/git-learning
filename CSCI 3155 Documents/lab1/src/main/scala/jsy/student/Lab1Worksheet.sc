/*
 * CSCI 3155: Lab 1 Worksheet
 *
 * This worksheet demonstrates how you could experiment
 * interactively with your implementations in Lab1.scala.
 */

/*
 * Example: Test-driven development of plus
 */

/*
 * For writeup portion
 */
def g(x: Int):((Int,Int),Int)={
  val (a,b) = (1,(x,3))
  if(x == 0) (b,1) else(b, a+2)
}
//g is type ((Int, Int), Int)
g(0)

/*
 * For coding part
 */
def abs(n: Double): Double = if(n<0)n * -1 else(n) // Works!

  def xor(a: Boolean, b: Boolean): Boolean =
  if(a) if(b) false //If both are true, it's false
  else true //If a is true, and b is false, should return true
  else if(b) b //If a is false and b is true, return b
  else b //If a is false and b is false, return b, which is false

def repeat(s: String, n: Int): String = { require( n >= 0)
  if (n == 0)
    return ""
  else
    (for (i <- 1 to n) yield s).mkString
  }

  //Square Step taken directly from lab notes listed above question
def sqrtStep(c: Double, xn: Double): Double = xn - (((xn * xn) - c) / (2*xn))

def sqrtN(c: Double, x0: Double, n: Int): Double = {require(n >=0) // Taken from sqrt function in this lab
    if(n == 0)
        x0
    else
      sqrtN(c,sqrtStep(c,x0),n-1)
  }

  //Followed instructions literally, recurse the error checker upon condition is met
def sqrtErr(c: Double, x0: Double, epsilon: Double): Double = {require(epsilon > 0)
    if (abs((x0 * x0) - c) < epsilon) x0 else sqrtErr(c, sqrtStep(c, x0), epsilon) //Used sqrtStep since we need to recurse until Xn from X0
  }

sqrtN(4,1,2)

repeat("abc",3)

// Here we can write expressions to experiment with how we might implement
// something. The expression is evaluated interactively.
1 + 1
val n = 1 + 1
n + 3

// The worksheet is built with all of the project files, so we can call
// a function from your jsy.student.Lab1 object (in Lab1.scala).
jsy.student.Lab1.plus(3, 4)

// We can imports all of the functions from jsy.student.Lab1
import jsy.student.Lab1._
plus(3, 4)

// We can check the implementation here, though it better to write tests
// in Lab1Spec.scala.
assert(plus(1, 1) == 2)

// Braces {} can be used wherever parentheses () can be (but not the other
// way around). Braces {} introduce scope, while () do not.
assert {
  val two = 2
  plus(1, 1) == two
}
abs(-9)



repeat("abc", 3)
/* Exercises */

// Call jsy.student.Lab1.abs
//abs(-3) // Will fail until implemented in Lab1.scala.

// Call the JavaScripty parser (from the provided library) on a string
jsy.lab1.Parser.parse("-4")

// We can import the parse function from jsy.lab1.Parser to experiment
// with the provided parser.
import jsy.lab1.Parser.parse
val negFourAST = parse("-4")

// We also want to import the AST nodes for convenience.
import jsy.lab1.ast._
assert {
  negFourAST match {
    case Unary(_, _) => true
    case _ => false
  }
}

// Evaluate that JavaScripty expression.
//eval(negFourAST)

// For convenience, we also have an eval function that takes a string,
// which calls the parser and then delegates to your eval function.
//eval("1 + 1")
