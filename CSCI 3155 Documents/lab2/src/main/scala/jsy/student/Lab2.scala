package jsy.student

import jsy.lab2.Lab2Like

object Lab2 extends jsy.util.JsyApplication with Lab2Like {
  import jsy.lab2.Parser
  import jsy.lab2.ast._

  /*
   * CSCI 3155: Lab 2
   * Michael Xiao
   * 
   * Partner: David Kleckner
   * Collaborators: <Any Collaborators>
   */

  /*
   * Fill in the appropriate portions above by replacing things delimited
   * by '<'... '>'.
   * 
   * Replace the '???' expression with  your code in each function.
   * 
   * Do not make other modifications to this template, such as
   * - adding "extends App" or "extends Application" to your Lab object,
   * - adding a "main" method, and
   * - leaving any failing asserts.
   * 
   * Your lab will not be graded if it does not compile.
   * 
   * This template compiles without error. Before you submit comment out any
   * code that does not compile or causes a failing assert. Simply put in a
   * '???' as needed to get something  that compiles without error. The '???'
   * is a Scala expression that throws the exception scala.NotImplementedError.
   *
   */

  /* We represent a variable environment as a map from a string of the
   * variable name to the value to which it is bound.
   * 
   * You may use the following provided helper functions to manipulate
   * environments, which are just thin wrappers around the Map type
   * in the Scala standard library.  You can use the Scala standard
   * library directly, but these are the only interfaces that you
   * need.
   */


  /* Some useful Scala methods for working with Scala values include:
   * - Double.NaN
   * - s.toDouble (for s: String)
   * - n.isNaN (for n: Double)
   * - n.isWhole (for n: Double)
   * - s (for n: Double)
   * - s format n (for s: String [a format string like for printf], n: Double)
   *
   * You can catch an exception in Scala using:
   * try ... catch { case ... => ... }
   */

  def toNumber(v: Expr): Double = {
    require(isValue(v))
    (v: @unchecked) match {
      case N(n) => n //Just return the number n if it's a num
      case S(s) => try{s.toDouble}catch{case _: NumberFormatException => Double.NaN}//From below functions toStr converts the string to a double
      case B(b) => if (b) 1 else 0 //Case of boolean, if b is true, return 1 else return 0
      case Undefined => Double.NaN//Throw an exception if the user passes in something that doesn't fit any criteria
      case _: Throwable => Double.NaN//Throw an exception if the user passes in something that doesn't fit any criteria
      //Just state that the case is not a number to clear up the eval on src/test/resources
    }

  }


  def toBoolean(v: Expr): Boolean = {
    require(isValue(v))
    (v: @unchecked) match {
      case B(b) => b
      case N(n) => if(n == 0) false else if(n.isNaN) false else true //Need to Implement (Every number except 0 is true)
      case S(s) => if(s == "") false else true //Need to Implement (Every string except "" is true)
      case Undefined => false
      case _ => false
    }
  }

  def toStr(v: Expr): String = {
    require(isValue(v))
    (v: @unchecked) match {
      case S(s) => s
      case N(n) => prettyNumber(n) //Need to Implement converts the number to a string
      case B(b) => if(b) "true" else "false"//converts boolean to the string version of the true/false
      case Undefined => "undefined"
      case _ => "undefined"
    }
  }

  def eval(env: Env, e: Expr): Expr = {
    //println(e)

    e match {
      /* Base Cases */
      case N(n) => e
      case B(b) => e
      case S(s) => e
      case Undefined => Undefined

      /* Inductive Cases */

      //UnarySpec
      case Unary(Neg, e) =>  (eval(env,e)) match {case v1 => N(-toNumber(v1))} //Just take the negative of a number which is why we are using N and toNumber
      case Unary(Not, e) => (eval(env,e)) match {case v1 => B(!toBoolean(v1))}  //Just take the not of a bool, which is why we are using B and toBoolean

      //For ArithmeticSpec, to finish these cases, similar to eval in lab1 no corner cases testing yet
      case Binary(Plus, e1, e2) => (eval(env,e1), eval(env, e2)) match {
        case (S(s1), v2) => S(s1 + toStr(v2))
        case (v1, S(s2)) => S(toStr(v1) + s2)
        case (v1,v2) => N(toNumber(v1) + toNumber(v2))
      }
      case Binary(Minus, e1, e2) => (eval(env,e1), eval(env, e2)) match {
        case (v1,v2) => N(toNumber(v1) - toNumber(v2))
      }
      case Binary(Times, e1, e2) => (eval(env,e1), eval(env, e2)) match {
        case (v1,v2) => N(toNumber(v1) * toNumber(v2))
      }
      case Binary(Div, e1, e2) => (eval(env,e1), eval(env, e2)) match {
        case(N(v1), N(v2)) => {
          if((toNumber(N(v2)) == 0) &&(toNumber(N(v1))) > 0)
            {
              N(Double.PositiveInfinity)
            }
          else if((toNumber(N(v2)) == -0) && (toNumber(N(v1))) > 0)
            {
              N(Double.NegativeInfinity)
            }
          else if((toNumber(N(v1)) < 0) && (toNumber(N(v2)) == -0)){
              N(Double.PositiveInfinity)
          }
          else if((toNumber(N(v1)) < 0) && (toNumber(N(v2)) == 0)){
            N(Double.NegativeInfinity)
          }
          else if(((toNumber(N(v1)) == 0) || (toNumber(N(v1)) == -0)) && ((toNumber(N(v2)) == -0) || (toNumber(N(v2)) == 0)))
          {
              N(Double.NaN)
          }
          else
          {
             N(toNumber(N(v1)) / toNumber(N(v2)))
          }
        }
        case (v1,v2) => N(toNumber(v1) / toNumber(v2))
      }


      //For AndOrSpec
      // (Super easy way): B(toBoolean(e1) && (toBoolean(e2))) Are we allowed to use these operators??
      case Binary(And, e1, e2) =>  (eval(env,e1)) match{//Works for base case if both e1 and e2 are booleans, no intuitive handling yet...
        case v1 => {
          if(toBoolean(v1) == true) //undefined == false
          {
            eval(env,e2) //(true - 1) && (undefined/0)
          }
          else //console.log also evaluates to false
          {
            v1
          }
        }
      }
      case Binary(Or, e1, e2) => (eval(env,e1)) match{ //Works for base case if both e1 and e2 are booleans, no intuitive handling yet...
        case v1 => {
          if(toBoolean(v1) == false)
          {
            eval(env,e2)
          }
          else
          {
            v1
          }
        }
       }

      //For Comparison Spec
      //To finish the last case, just test for corner cases and intuition
      //All cases here are really simple just return the bool of the expressions below
      case Binary(Eq, e1, e2) => (eval(env,e1), eval(env, e2)) match {
        case(S(s1), S(s2)) => B(s1 == s2)
        case(S(s1), v2) => B(false) //Added for String comparisons
        case(v1, S(s2)) => B(false)
        case(v1, v2) => B(v1 == v2)
      }
      case Binary(Ne, e1, e2) => (eval(env,e1), eval(env, e2)) match {
        case(S(s1), S(s2)) => B(s1 != s2)
        case(S(s1), v2) =>B(true) //Added for String comparisons
        case(v1, S(s2)) => B(true)
        case(v1, v2) => B(v1 != v2)
      }
      case Binary(Lt, e1, e2) => (eval(env,e1), eval(env, e2)) match {
        case (S(v1),S(v2)) => B(v1 < v2)
        case(v1,v2) => B(toNumber(v1) < toNumber(v2))
      }
      case Binary(Le, e1, e2) => (eval(env,e1), eval(env, e2)) match {
        case (S(v1),S(v2)) => B(v1 <= v2)
        case(v1,v2) => B(toNumber(v1) <= toNumber(v2))
      }
      case Binary(Gt, e1, e2) => (eval(env,e1), eval(env, e2)) match {
        case (S(v1),S(v2)) => B(v1 > v2)
        case(v1,v2) => B(toNumber(v1) > toNumber(v2))
      }
      case Binary(Ge, e1, e2) => (eval(env,e1), eval(env, e2)) match {
        case (S(v1),S(v2)) => B(v1 >= v2)
        case(v1,v2) => B(toNumber(v1) >= toNumber(v2))
      }
      case If(e1, e2, e3) => (eval(env,e1)) match{ //Updated If to recursively call eval
        //if(toBoolean(v1)) eval(env,v2) else eval(env,v3)
        case v1 => if(toBoolean(v1) == true) eval(env,e2) else eval(env,e3)
      }
      //SeqSpec ->format according to SeqSpec are Binary(seq,e1,e2)

      case Binary(Seq,e1,e2) => (eval(env,e1), eval(env, e2)) match {
        case (v1, v2) => v1;v2 //Didn't know this but for continuous execution, you can just use a semicolon!
      }

      case ConstDecl(x: String, e1: Expr, e2: Expr) => {
        eval(extend(env,x,eval(env,e1)),e2)
        /*
         val newEnv = extend(env,x,eval(env,e1))
         eval(newEnv,e2)
         */
      }

      case Print(e1) => println(pretty(eval(env, e1))); Undefined

      case Var(x) => lookup(env, x)
      case _ => e //Placeholder so some tests can pass
    }
  }



  /* Interface to run your interpreter from the command-line.  You can ignore what's below. */
  def processFile(file: java.io.File) {
    if (debug) { println("Parsing ...") }

    val expr = Parser.parseFile(file)

    if (debug) {
      println("\nExpression AST:\n  " + expr)
      println("------------------------------------------------------------")
    }

    if (debug) { println("Evaluating ...") }

    val v = eval(expr)

    println(pretty(v))
  }

}