/*
 * CSCI 3155: Lab 2 Worksheet
 *
 * This worksheet demonstrates how you could experiment
 * interactively with your implementations in Lab2.scala.
 */

// Imports the parse function from jsy.lab1.Parser
import jsy.lab2.Parser.parse
import jsy.student.Lab2._

// Imports the ast nodes
import jsy.lab2.ast._

// Imports all of the functions form jsy.student.Lab2 (your implementations in Lab2.scala)
import jsy.student.Lab2._

// Call the JavaScripty parser (from the provided library) on a string
val negFourAST = parse("-4")

// Evaluate that JavaScripty expression.
//eval(negFourAST)

// For convenience, we also have an eval function that takes a string,
// which calls the parser and then delegates to your eval function.
//eval("undefined + 1")


type Env = Map[String, Expr]
val empty: Env = Map()
def lookup(env: Env, x: String): Expr = env(x)
def extend(env: Env, x: String, v: Expr): Env = {
  require(isValue(v))
  env + (x -> v)
}
//Scratch Work Below


def toBoolean(v: Expr): Boolean = {
  require(isValue(v))
  try {
    (v: @unchecked) match {
      case B(b) => b
      case S(s) => ???
    }
  }
  catch {
    case _: Throwable => throw new UnsupportedOperationException
  }
}

def toNumber(v: Expr): Double = {
  require(isValue(v))
  try {
    (v: @unchecked) match {
      case N(n) => n //Just return the number n if it's a num
      case S(s) => s.toDouble//From below functions toStr converts the string to a double
      case B(b) => if (b) 1 else 0 //Case of boolean, if b is true, return 1 else return 0
    }
  }
  catch{
    case _ => Double.NaN//Throw an exception if the user passes in something that doesn't fit any criteria
    //Just state that the case is not a number to clear up the eval on src/test/resources
  }
}
toBoolean(parse("true"))


val y = 1

val r = "HELLO"
//val q = r.toDouble
