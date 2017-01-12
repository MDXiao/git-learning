package jsy.student

import jsy.lab3.Lab3Like
import jsy.util.JsyApplication

import scala.text.DocCons

object Lab3 extends JsyApplication with Lab3Like {
  import jsy.lab3.ast._

  /*
   * CSCI 3155: Lab 3 
   * Callie Jones, Michael Xiao
   * 
   *
   * Collaborators: <Any Collaborators>
   */

  /*
   * Fill in the appropriate portions above by replacing things delimited
   * by '<'... '>'.
   * 
   * Replace the '???' expression with your code in each function.
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
   */

  /*
   * The implementations of these helper functions for conversions can come
   * Lab 2. The definitions for the new value type for Function are given.
   */

  def toNumber(v: Expr): Double = {
    require(isValue(v))
    (v: @unchecked) match {
      case N(n) => n
      case B(false) => 0
      case B(true) => 1
      case S(s) => try{s.toDouble}catch{case _: NumberFormatException => Double.NaN}
      case Function(_, _, _) => Double.NaN
      case Undefined => Double.NaN //All these cases are all straight from Lab 2
    }
  }

  def toBoolean(v: Expr): Boolean = {
    require(isValue(v))
    (v: @unchecked) match {
      case B(b) => b
      case N(n) => if(n == 0) false else if(n.isNaN) false else true
      case S(s) => if(s == "") false else true
      case Function(_, _, _) => true
      case Undefined => false
      //case _ => ??? // delete this line when done
    }
  }

  def toStr(v: Expr): String = {
    require(isValue(v))
    (v: @unchecked) match {
      case S(s) => s
      case N(n) => prettyNumber(n)
      case B(b) => if(b) "true" else "false"
      case Function(_, _, _) => "function"
      case Undefined => "undefined"
      // Here in toStr(Function(_, _, _)), we will deviate from Node.js that returns the concrete syntax
      // of the function (from the input program).
      //case _ => ??? // delete this line when done
    }
  }

  /*
   * Helper function that implements the semantics of inequality
   * operators Lt, Le, Gt, and Ge on values.
   *
   * We suggest a refactoring of code from Lab 2 to be able to
   * use this helper function in eval and step.
   */
  def inequalityVal(bop: Bop, v1: Expr, v2: Expr): Boolean = { //How do you use this help in eval?
    require(isValue(v1))
    require(isValue(v2))
    require(bop == Lt || bop == Le || bop == Gt || bop == Ge)
    (v1, v2) match {
      case (S(s1),S(s2)) => bop match{ //If they are both strings, just do what we did in lab 2
        case Lt => toBoolean(B(s1 < s2)) //Match based on their bop
        case Le => toBoolean(B(s1 <= s2))
        case Gt => toBoolean(B(s1 > s2))
        case Ge => toBoolean(B(s1 >= s2))
      }
      case(v1,v2) => bop match{ //If they are both wildcards, just convert them to number
        case Lt => toBoolean(B(toNumber(v1) < toNumber(v2)))
        case Le => toBoolean(B(toNumber(v1) <= toNumber(v2)))
        case Gt => toBoolean(B(toNumber(v1) > toNumber(v2)))
        case Ge => toBoolean(B(toNumber(v1) >= toNumber(v2)))
      }
      //case _ => ??? // delete this line when done
    }
  }


  /* Big-Step Interpreter with Dynamic Scoping */

  /*
   * Start by copying your code from Lab 2 here.
   */
  def eval(env: Env, e: Expr): Expr = {
    e match {
      /* Base Cases */
      case N(_) | B(_) | S(_) | Undefined | Function(_, _, _) => e
      case Var(x) => lookup(env,x) //Added from lab 2

      /* Inductive Cases */
      case Print(e1) => println(pretty(eval(env, e1))); Undefined

      case Unary(Neg, e) =>  (eval(env,e)) match {case v1 => N(-toNumber(v1))}
      case Unary(Not, e) => (eval(env,e)) match {case v1 => B(!toBoolean(v1))}

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
        case (N(v1),N(v2)) =>{
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

      case Binary(And, e1, e2) => (eval(env,e1)) match{
        case v1 => if(toBoolean(v1) == true) eval(env,e2) else v1
      }
      case Binary(Or, e1, e2) => (eval(env,e1)) match{
        case v1 => if(toBoolean(v1) == false) eval(env,e2) else v1
      }

      case Binary(Eq, e1, e2) => (eval(env,e1), eval(env, e2)) match { //Fixed to account for Functions
        case (Function(_, _, _), v2) => throw new DynamicTypeError(e)
        case (v1, Function(_, _, _)) => throw new DynamicTypeError(e)
        case (S(s1), S(s2)) => B(s1 == s2)
        case (S(s1), v2) => B(false)
        case (v1, S(s2)) => B(false)
        case (v1, v2) => B(v1 == v2)
      }
      case Binary(Ne, e1, e2) => (eval(env,e1), eval(env, e2)) match { //Fixed to account for Functions
        case(Function(_,_,_),v2) => throw new DynamicTypeError(e)
        case(v1, Function(_,_,_)) => throw new DynamicTypeError(e)
        case(S(s1), S(s2)) => B(s1 != s2)
        case(S(s1), v2) => B(true)
        case(v1, S(s2)) => B(true)
        case(v1, v2) => B(v1 != v2)
      }

      case Binary(Lt, e1, e2) => B(inequalityVal(Lt,eval(env,e1),eval(env,e2)))
      case Binary(Le, e1, e2) => B(inequalityVal(Le,eval(env,e1),eval(env,e2)))
      case Binary(Gt, e1, e2) => B(inequalityVal(Gt,eval(env,e1),eval(env,e2)))
      case Binary(Ge, e1, e2) => B(inequalityVal(Ge,eval(env,e1),eval(env,e2)))

      case Binary(Seq,e1,e2) => (eval(env,e1), eval(env, e2)) match {
        case (v1, v2) => v2
      }

      case If(e1, e2, e3) => (eval(env,e1)) match{
        case v1 => if(toBoolean(v1) == true) eval(env,e2) else eval(env,e3)
      }

      case ConstDecl(x: String, e1: Expr, e2: Expr) => {
        eval(extend(env,x,eval(env,e1)),e2)
      }

      case Call(e1, e2) => eval(env, e1) match{
        case Function(None, x, ep) => {
          val Ev2 = extend(env, x, eval(env, e2))
          eval(Ev2, ep)
          //eval(extend(env,x,eval(env, e2)),ep)
        }
        case v1 @ Function(Some(x1), x2, ep) => {
          val Ev2 = extend(env, x2, eval(env, e2))
          val Ev1 = extend(Ev2, x1, v1)
          eval(Ev1, ep)
        }
        case _ => throw DynamicTypeError(e)

      }
    }
  }


  /* Small-Step Interpreter with Static Scoping */

  def iterate(e0: Expr)(next: (Expr, Int) => Option[Expr]): Expr = {
    def loop(e: Expr, n: Int): Expr = next(e, n) match {
      case None => e
      case Some(ep) => loop(ep, n+1)
    }
    loop(e0, 0)
  }

  /* Substitute's parameters
   * e = Is the expression we want to substitute in
   * v = expression we want to substitute
   * x = Is the string in e that we are looking for and we want replaced
   *
   * Functions => Name, Parameter passed into function, and the function's expression format
   */
  def substitute(e: Expr, v: Expr, x: String): Expr = { //Majority of substitute is taken from page 52 in the course notes
    require(isValue(v))
    e match {
      case N(_) | B(_) | Undefined | S(_) => e
      case Var(y) => if(y == x) v else Var(y) //y = x1, x = x, e prime = v
      case Print(e1) => Print(substitute(e1, v, x))
      case Unary(uop, e1) => Unary(uop,substitute(e1, v ,x))
      case Binary(bop, e1, e2) => Binary(bop, substitute(e1,v,x), substitute(e2,v,x))
      case If(e1, e2, e3) => If(substitute(e1,v,x), substitute(e2,v,x), substitute(e3,v,x))
      case Call(e1, e2) => Call(substitute(e1,v,x),substitute(e2,v,x))
      case Function(None, y, e1) => Function(None, y, if(y != x) substitute(e1, v, x) else e1) //Check if the parameter is not equal to x then do this
      case Function(Some(y1), y2, e1) => Function(Some(y1), y2, if(y1 != x && y2 != x) substitute(e1, v, x) else e1)
      //For ConstDecl => y = x1, x = x, e1 = e1, e2 = e2
      case ConstDecl(y, e1, e2) => if(y == x) ConstDecl(y, substitute(e1, v, x), e2) else ConstDecl(y, substitute(e1,v,x), substitute(e2,v ,x))
    }
  }

  def step(e: Expr): Expr = {
    e match {
      /* Base Cases: Do Rules */
      case Print(v1) if isValue(v1) => println(pretty(v1)); Undefined
      case Unary(Neg, v1) if isValue(v1) => N(-toNumber(v1))  //DoNeg
      case Unary(Not, v1) if isValue(v1) => B(!toBoolean(v1)) //DoNot

      //DoPlus
      case Binary(Plus, S(s1),v2) if isValue(v2) => S(s1 + toStr(v2))
      case Binary(Plus, v1,S(s2)) if isValue(v1)=> S(toStr(v1) + s2)
      case Binary(Plus, v1, v2) if isValue(v1) && isValue(v2) => N(toNumber (v1) + toNumber (v2) )

      //Equal and Not Equal
      case Binary(Eq, v1, v2) if isValue(v1) && isValue(v2) => (v1,v2) match {
        case (Function(_, _, _), v2) => throw new DynamicTypeError(e)
        case (v1, Function(_, _, _)) => throw new DynamicTypeError(e)
        case (S(s1), S(s2)) => B(s1 == s2)
        case (S(s1), v2) => B(false)
        case (v1, S(s2)) => B(false)
        case (v1, v2) => B(toNumber(v1) == toNumber(v2))
      }
      case Binary(Ne,v1, v2) if isValue(v1) && isValue(v2) => (v1,v2) match {
        case(Function(_,_,_),v2) => throw new DynamicTypeError(e)
        case(v1, Function(_,_,_)) => throw new DynamicTypeError(e)
        case(S(s1), S(s2)) => B(s1 != s2)
        case(S(s1), v2) => B(true)
        case(v1, S(s2)) => B(true)
        case(v1, v2) => B(toNumber(v1) != toNumber(v2))
      }

      //DoSeq
      case Binary(Seq, v1, e2) if isValue(v1) => e2

      //DoInequality
      case Binary(Lt,v1,v2) if isValue(v1) && isValue(v2) => B(inequalityVal(Lt, v1, v2))
      case Binary(Le,v1,v2) if isValue(v1) && isValue(v2) => B(inequalityVal(Le, v1, v2))
      case Binary(Gt,v1,v2) if isValue(v1) && isValue(v2) => B(inequalityVal(Gt, v1, v2))
      case Binary(Ge,v1,v2) if isValue(v1) && isValue(v2) => B(inequalityVal(Ge, v1, v2))

      //DoAnd and DoOr
      case Binary(And, v1, e2) if isValue(v1) => if(toBoolean(v1) == true) e2 else v1 //DoAnd
      case Binary(Or, v1, e2) if isValue(v1) => if(toBoolean(v1) == false) e2 else v1 //DoOr

      //DoArith
      case Binary(bop, v1, v2) if isValue(v1) && isValue(v2) => bop match {
        case Minus => N(toNumber(v1) - toNumber(v2))
        case Times => N(toNumber(v1) * toNumber(v2))
        case Div => N(toNumber(v1) / toNumber(v2))
      }

      //DoIf
      case If(v1,e2,e3) if isValue(v1) => if(toBoolean(v1) == true) e2 else e3 //DoIf

      //DoConst
      case ConstDecl(x, v1, e2) if isValue(v1) => substitute(e2, v1, x) //From handout, read from the judgment
      //substituting v1 with x

      /* DoCall
       * Functions Parameters: Function(p, x, e)
       * p = Function Name (Can be None or Some())
       * x = Parameter passed in
       * e = Expression of function (??)
       */
      case Call(v1, v2) if isValue(v1) && isValue(v2) => v1 match { //From Notes
        case Function(None, x, e1) => substitute(e1, v2, x)
        case Function(Some(x1), x2, e1) => substitute(substitute(e1, v1, x1), v2, x2) //DoCallRec
        //Understand DoCallRec
        case _ => throw new DynamicTypeError(e)
      }

      /* Inductive Cases: Search Rules */
      case Print(e1) => Print(step(e1))

      //SearchUnary
      case Unary(uop, e1) => {
        val e1p = step(e1)
        Unary(uop,e1p)
      }

      //SearchEquality2
      case Binary(Eq, v1, e2) if isValue(v1) => { //How do you do funtion checking?
        val e2p = step(e2)
        Binary(Eq, v1, e2p)
      }

      case Binary(Ne, v1, e2) if isValue(v1) => {
        val e2p = step(e2)
        Binary(Ne, v1, e2p)
      }

      //SearchBinaryArith2
      case Binary(bop, v1, e2) if isValue(v1) => {
        val e2p = step(e2)
        Binary(bop,v1,e2p)
      }

      //SearchBinary1
      case Binary(bop, e1, e2) => {
        val e1p = step(e1)
        Binary(bop,e1p,e2)
      }


      //SearchIf
      case If(e1,e2,e3) => {
        val e1p = step(e1)
        If(e1p,e2,e3)
      }

      //SearchConst
      case ConstDecl(x, e1, e2) => {
        val e1p = step(e1)
        ConstDecl(x, e1p, e2)
      }

      case Call(e1,e2) if isValue(e1) => {
        val e2p = step(e2)
        Call(e1, e2p)

      }

      //SearchCall1
      case Call(e1,e2) => {
        val e1p = step(e1)
        Call(e1p,e2)
      }

      /* Cases that should never match. Your cases above should ensure this. */
      case Var(_) => throw new AssertionError("Gremlins: internal error, not closed expression.")
      case N(_) | B(_) | Undefined | S(_) | Function(_, _, _) => throw new AssertionError("Gremlins: internal error, step should not be called on values.");
    }
  }


  /* External Interfaces */

  //this.debug = true // uncomment this if you want to print debugging information
  this.keepGoing = true // comment this out if you want to stop at first exception when processing a file

}
