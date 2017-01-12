import jsy.tester.JavascriptyTester
import org.scalatest._
import jsy.lab2.ast._

import jsy.student.Lab2
import Lab2._

/*
 * This file contains a number of *Spec classes that define a set of
 * tests.
 *
 * All of the tests are gathered together in Lab2Suite.
 */

class ToSpec extends FlatSpec{

  "toNumber" should "convert Strings into their correct format" in {
    val string = S("Test")
    val num = S("1.0")
    //assert(toNumber(string) == Double.NaN)
    assert(N(toNumber(num)) === N(1))
  }

  "toStr" should "convert values to their correct format" in {
    val num = N(11.0)
    assert(S(toStr(num)) === S("11"))
  }

}

class AndOrSpec extends FlatSpec {

  "And" should "return true only if both expressions are true" in {
    val t = B(true)
    val f = B(false)
    assert(eval(Binary(And,t,t)) === t)
    assert(eval(Binary(And,t,f)) === f)
    assert(eval(Binary(And,f,t)) === f)
    assert(eval(Binary(And,f,f)) === f)
  } 
 
  "And" should "return non-intuitive results from differing types" in {
    val e1 = N(0)
    val e2 = B(true)
    val e3 = eval(Binary(And, e1, e2))
    assert(e3 === N(0))
  }

  "And" should "should test functionality of AND" in {
    val e1 = N(0)
    val e2 = N(2)
    val e3 = N(Double.NaN)
    val e4 = eval(Binary(And, e1, e3))
    val e5 = eval(Binary(And,e2 , e3))
    assert(e4 === N(0))
    assert(e5 != N(Double.NaN))
  }

 
  "Or" should "return true if either or both expressions are true" in {
    val t = B(true)
    val f = B(false)
    assert(eval(Binary(Or,t,t)) === t)
    assert(eval(Binary(Or,f,t)) === t)
    assert(eval(Binary(Or,t,f)) === t)
    assert(eval(Binary(Or,f,f)) === f)
  }

  "Or" should "return non-intuitive results from differing types" in {
    val e1 = N(5)
    val e2 = B(false)
    val e3 = eval(Binary(Or, e1, e2))
    assert(e3 === N(5))
  }

  "Or" should "should test the String functionality of Or" in {
    val e1 = N(1)
    val e2 = S("Test")
    val e3 = N(0)
    val e4 = eval(Binary(Or, e1, e2))
    val e5 = eval(Binary(Or, e3, e2))
    assert(e4 === N(1.0))
    assert(e5 === S("Test"))
  }


}

class ArithmeticSpec extends FlatSpec {

  "Plus" should "add two number values and return a number" in {
    val e1 = N(1)
    val e2 = N(2)
    val e3 = eval(Binary(Plus, e1, e2))
    assert(e3 === N(3))
  }

  "Plus" should "append e2 to e1 if e1 is a string" in {
    val e1 = S("100")
    val e2 = N(2)
    val e3 = eval(Binary(Plus, e1, e2))
    val e4 = eval(Binary(Plus, e2, e1))
    assert(e3 === S("1002"))
    assert(e4 === S("2100"))
  }


  "Minus" should "subtract two number values and return a number" in {
    val e1 = N(3)
    val e2 = N(1)
    val e3 = eval(Binary(Minus, e1, e2))
    assert(e3 === N(2))
  }

  "Minus" should "subtract a number and a string correctly" in {
    val e1 = N(3)
    val e2 = S("1")
    val e4 = S("test")
    val e3 = eval(Binary(Minus, e1, e2))
    val e5 = eval(Binary(Minus, e1, e4))
    assert(e3 === N(2))
    assert(e5 === N(Double.NaN))
  }

  "Times" should "multiply two number values and return a number" in {
    val e1 = N(3)
    val e2 = N(2)
    val e3 = eval(Binary(Times, e1, e2))
    assert(e3 === N(6))
  }

  "Div" should "divide two number values and return a number" in {
    val e1 = N(8)
    val e2 = N(5)
    val e3 = eval(Binary(Div, e1, e2))
    assert(e3 === N(1.6))
  }

  "Div" should "pass these edge cases" in {
    val e1 = N(5)
    val e2 = N(-0)
    val e5 = N(0)
    val e3 = eval(Binary(Div, e1, e2))
    val e4 = eval(Binary(Div, e1, e5))
    val e7 = eval(Binary(Div, e2,e5))
    assert(e3 === N(Double.PositiveInfinity))
    assert(e4 == N(Double.PositiveInfinity))
    assert(e7 === N(Double.NaN))
  }

  "Arithmetic Operators" should "produce non-intuitive solutions given differing expression types" in {
    val e1 = B(true)
    val e2 = N(7)
    assert(eval(Binary(Plus,e1,e2)) == N(8))
  }

}

class ComparisonSpec extends FlatSpec {

  "Eq" should "return true if two numerical values are the same" in {
    val e1 = N(5)
    val e2 = N(5)
    val e3 = eval(Binary(Eq, e1, e2))
    assert(e3 === B(true))
  } 
  
  "Eq" should "return false if two numerical values are not the same" in {
    val e1 = N(5)
    val e2 = N(7)
    val e3 = eval(Binary(Eq, e1, e2))
    assert(e3 === B(false))
  }

  "Eq" should "return true if two strings are the same and false otherwise" in {
    val e1 = S("Test")
    val e2 = S("NotTest")
    val e3 = S("Test")
    val e4 = eval(Binary(Eq, e1, e2))
    val e5 = eval(Binary(Eq, e1, e3))
    assert(e4 === B(false))
    assert(e5 === B(true))
  }

  "Eq" should "return false if two values are not the same type and true otherwise" in {
    val e1 = N(1)
    val e2 = S("1")
    val e3 = B(true)
    val e4 = eval(Binary(Eq, e1, e2))
    val e5 = eval(Binary(Eq, e1, e3))
    assert(e4 === B(false))
    assert(e5 === B(false))
  }

  "Ne" should "return true if two numerical values are different" in {
    val e1 = N(5)
    val e2 = N(7)
    val e3 = eval(Binary(Ne, e1, e2))
    assert(e3 === B(true))
  } 
  
  "Ne" should "return false if two numerical values are the same" in {
    val e1 = N(5)
    val e2 = N(5)
    val e3 = eval(Binary(Ne, e1, e2))
    assert(e3 === B(false))
  }

  "Ne" should "return false if two strings are the same and true otherwise" in {
    val e1 = S("Test")
    val e2 = S("NotTest")
    val e3 = S("Test")
    val e4 = eval(Binary(Ne, e1, e2))
    val e5 = eval(Binary(Ne, e1, e3))
    assert(e4 === B(true))
    assert(e5 === B(false))
  }

  "Lt" should "return true if the first expression is less than the second" in {
    val e1 = N(5)
    val e2 = N(7)
    val e3 = eval(Binary(Lt, e1, e2))
    assert(e3 === B(true))
  } 
  
  "Lt" should "return false if the first expression is not strictly less than the second" in {
    val e1 = N(7)
    val e2 = N(5)
    val e3 = eval(Binary(Lt, e1, e2))
    assert(e3 === B(false))
  } 
  
  "Lt" should "return false if two number values are the same" in {
    val e1 = N(5)
    val e2 = N(5)
    val e3 = eval(Binary(Lt, e1, e2))
    assert(e3 === B(false))
  }

  "Lt" should "return false if e1 is longer than e2 and true otherwise" in { //Weird test Only if the two strings are initially similar than differ it's true
    val e1 = S("Test")
    val e2 = S("Tests")
    val e3 = S("Test")
    val e4 = eval(Binary(Lt, e1, e2))
    val e5 = eval(Binary(Lt, e1, e3))
    assert(e4 === B(true))
    assert(e5 === B(false))
  }


  "Lt" should "return false if a number is compared with a string" in { //Weird test Only if the two strings are initially similar than differ it's true
    val e1 = N(11)
    val e2 = S("2")
    val e4 = eval(Binary(Lt, e1, e2))
    assert(e4 === B(false))
  }

  "Lt" should "checks if the two string numbers relationship is true" in { //Weird test Only if the two strings are initially similar than differ it's true
    val e1 = S("30")
    val e2 = S("4")
    val e4 = eval(Binary(Lt, e1, e2))
    assert(e4 === B(true))
  }

  "Le" should "return true if the first expression is less than the second" in {
    val e1 = N(5)
    val e2 = N(7)
    val e3 = eval(Binary(Le, e1, e2))
    assert(e3 === B(true))
  } 
  
  "Le" should "return false if the first expression is greater than the second" in {
    val e1 = N(7)
    val e2 = N(5)
    val e3 = eval(Binary(Le, e1, e2))
    assert(e3 === B(false))
  } 
  
  "Le" should "return true if two number values are the same" in {
    val e1 = N(5)
    val e2 = N(5)
    val e3 = eval(Binary(Le, e1, e2))
    assert(e3 === B(true))
  }

  "Le" should "return false if the strings are not equal and true otherwise" in {
    val e1 = S("Test")
    val e2 = S("NotTest")
    val e3 = S("Test")
    val e4 = eval(Binary(Le, e1, e2))
    val e5 = eval(Binary(Le, e1, e3))
    assert(e4 === B(false))
    assert(e5 === B(true))
  }

  "Gt" should "return true if the first expression is greater than the second" in {
    val e1 = N(8)
    val e2 = N(7)
    val e3 = eval(Binary(Gt, e1, e2))
    assert(e3 === B(true))
  } 
  
  "Gt" should "return false if the first expression is not strictly greater than the second" in {
    val e1 = N(4)
    val e2 = N(5)
    val e3 = eval(Binary(Gt, e1, e2))
    assert(e3 === B(false))
  } 
  
  "Gt" should "return false if two number values are the same" in {
    val e1 = N(5)
    val e2 = N(5)
    val e3 = eval(Binary(Gt, e1, e2))
    assert(e3 === B(false))
  }

  "Gt" should "return false if e1 is not longer than e2 initially" in {
    val e1 = S("Test")
    val e2 = S("Testss")
    val e3 = S("Test")
    val e7 = S("aaaaaaa")
    val e4 = eval(Binary(Gt, e1, e2))
    val e5 = eval(Binary(Gt, e1, e3))
    val e6 = eval(Binary(Gt, e1, e7))
    assert(e4 === B(false))
    assert(e5 === B(false))
    assert(e6 === B(false))
  }

  "Gt" should "return false if either e1 and e2 are undefined" in {
    val e1 = Undefined
    val e2 = N(30.0)
    val e3 = eval(Binary(Gt,e2,e1))
    assert(e3 === B(false))
  }

  "Gt" should "return true if either e1 and e2 are undefined" in {
    val e1 = S("30")
    val e2 = N(4)
    val e3 = eval(Binary(Gt,e1,e2))
    assert(e3 === B(true))
  }


  "Ge" should "return true if the first expression is greater than the second" in {
    val e1 = N(8)
    val e2 = N(7)
    val e3 = eval(Binary(Ge, e1, e2))
    assert(e3 === B(true))
  } 
  
  "Ge" should "return false if the first expression is less than the second" in {
    val e1 = N(4)
    val e2 = N(5)
    val e3 = eval(Binary(Ge, e1, e2))
    assert(e3 === B(false))
  } 
  
  "Ge" should "return true if two number values are the same" in {
    val e1 = N(5)
    val e2 = N(5)
    val e3 = eval(Binary(Ge, e1, e2))
    assert(e3 === B(true))
  }

  "Comparisons" should "produce non-intuitive results given the expressions given" in {
    val e1 = N(5)
    val e2 = Undefined
    assert(eval(Binary(Eq,e1,e2)) === B(false))
  } 
}

class ConstSpec extends FlatSpec {

  "ConstDecl" should "extend the environment with the first expression results bound to the identifier, and then eval the second expression" in {
    val e1 = N(3)
    val e2 = Binary(Plus, Var("x"), N(1))
    val e3 = eval(ConstDecl("x", e1, e2)) 
    assert(e3 === N(4))
  } 
  
}

class IfSpec extends FlatSpec {

  "If" should "eval the first expression if the conditional is true" in {
    val e1 = Binary(Plus, N(3), N(2))
    val e2 = Binary(Plus, N(1), N(1))
    val e3 = eval(If(B(true), e1, e2)) 
    assert(e3 === N(5))
  } 
  
  "If" should "eval the second expression if the conditional is false" in {
    val e1 = Binary(Plus, N(3), N(2))
    val e2 = Binary(Plus, N(1), N(1))
    val e3 = eval(If(B(false), e1, e2)) 
    assert(e3 === N(2))
  }

  "If" should "eval the second expression if the conditional is undefined" in {
    val e1 = Binary(Plus, N(3), N(2))
    val e2 = Binary(Plus, N(1), N(1))
    val e3 = eval(If(Undefined, e1, e2))
    assert(e3 === N(2))
  }

}

class SeqSpec extends FlatSpec {

  "Seq" should "execute the first expression, followed by the second, and should eval to the second expression" in {
    val e1 = Binary(Plus, N(3), N(2))
    val e2 = Binary(Plus, N(1), N(1))
    val e3 = eval(Binary(Seq, e1, e2)) 
    assert(e3 === N(2))
  } 
  
}

class UnarySpec extends FlatSpec {

  "Neg" should "return the negation of a number value" in {
    val e1 = N(5)
    val e2 = eval(Unary(Neg, e1))
    assert(e2 === N(-5))
  } 
  
  "Not" should "return the compliment of a boolean value" in {
    val e1 = B(true)
    val e2 = B(false)
    val e3 = eval(Unary(Not, e1))
    val e4 = eval(Unary(Not, e2))
    assert(e3 === B(false))
    assert(e4 === B(true))
  }
}

// The next bit of code runs a test for each .jsy file in src/test/resources/lab2.
// The test expects a corresponding .ans file with the expected result.
class Lab2JsyTests extends JavascriptyTester(None, "lab2", Lab2)

class Lab2Suite extends Suites(
  new ToSpec,
  new AndOrSpec,
  new ArithmeticSpec,
  new ComparisonSpec,
  new ConstSpec,
  new IfSpec,
  new SeqSpec,
  new UnarySpec,
  new Lab2JsyTests
)

