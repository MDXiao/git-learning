/*
 * CSCI 3155: Lab 4 Worksheet
 *
 * This worksheet demonstrates how you could experiment
 * interactively with your implementations in Lab4.scala.
 */

// Imports the parse function from jsy.lab1.Parser
import jsy.lab4.Parser.parse

// Imports the ast nodes
import jsy.lab4.ast._

// Imports all of the functions form jsy.student.Lab2 (your implementations in Lab2.scala)
import jsy.student.Lab4._

// Try compressRec
//val cr1 = compressRec(List(1, 2, 2, 3, 3, 3))

// Parse functions with possibly multiple parameters and type annotations.
parse("function fst(x: number, y: number): number { return x }")
parse("function (x: number) { return x }")
parse("function (f: (y: number) => number, x: number) { return f(x) }")

// Parse objects
parse("{ f: 0, g: true }")
parse("x.f")

//Work
parse("const g = x; g+2;const h = function(x:string,y:string): string {return x+y}")

def duplicateRec[A](l:List[A]):List[A] = l match{
  case Nil => l
  case h :: t => h::h::duplicateRec(t)

}

def duplicateFold[A](l:List[A]):List[A] = l.foldRight(Nil:List[A]) { //acc is a empty list
  (h,acc) => h::h::acc
}

val g = List(1,2,3,4)

duplicateRec(g)
duplicateFold(g)

def strictlyOrdered(t: Tree): Boolean = {
  val (b, _) = foldLeft(t)((true, None: Option[Int])){
    (acc, t) => acc match {
      case (b, None) => (true, Some(t))
      case (b, Some(x)) => (x < t , Some(t))
    }
  }
  b
}

val testList = List(1,1,2)
val testList1 = List(1,2,3,4,5)
strictlyOrdered(treeFromList(testList))
strictlyOrdered(treeFromList(testList1))

parse("function(x: number) { return x } (4)")

val plus = "plus"
val x = "x"
val y = "y"
val problem = Decl(Const,plus,Function(None,List((x,MTyp(Const,TNumber)), (y,MTyp(Const,TNumber))),None,Binary(Plus,Var(x),Var(y))),Call(Var(plus),List(Binary(Plus,N(1.0),N(2.0)), N(3.0))))
val s0 = step(problem)

val hi = Call(Function(None,List((x,MTyp(Const,TNumber))),None,Var(x)),List(N(4.0)))
val s1 = step(hi)

val expr = parse("{x: 4, y: 34, str: 'string'}")
val test = typeof(Map(), expr)

