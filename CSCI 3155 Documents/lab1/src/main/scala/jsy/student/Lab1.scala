package jsy.student

import jsy.lab1._

object Lab1 extends jsy.util.JsyApplication with jsy.lab1.Lab1Like {
  import jsy.lab1.Parser
  import jsy.lab1.ast._

  /*
   * CSCI 3155: Lab 1
   * Michael Xiao
   *
   * Partner: Monte Anderson
   * Collaborators: Christine Samson, Callie Jones
   */

  /*
   * Fill in the appropriate portions above by replacing things delimited
   * by '<'... '>'.
   *
   * Replace the '???' expression with your code in each function. The
   * '???' expression is a Scala expression that throws a NotImplementedError
   * exception.
   *
   * Do not make other modifications to this template, such as
   * - adding "extends App" or "extends Application" to your Lab object,
   * - adding a "main" method, and
   * - leaving any failing asserts.
   *
   * Your lab will not be graded if it does not compile.
   *
   * This template compiles without error. Before you submit comment out any
   * code that does not compile or causes a failing assert.  Simply put in a
   * '???' as needed to get something that compiles without error.
   */

  /*
   * Example: Test-driven development of plus
   *
   * A convenient, quick-and-dirty way to experiment, especially with small code
   * fragments, is to use the interactive Scala interpreter. The simplest way
   * to use the interactive Scala interpreter in IntelliJ is through a worksheet,
   * such as Lab1Worksheet.sc. A Scala Worksheet (e.g., Lab1Worksheet.sc) is code
   * evaluated in the context of the project with results for each expression
   * shown inline.
   *
   * Step 0: Sketch an implementation in Lab1.scala using ??? for unimmplemented things.
   * Step 1: Do some experimentation in Lab1Worksheet.sc.
   * Step 2: Write a test in Lab1Spec.scala, which should initially fail because of the ???.
   * Step 3: Fill in the ??? to finish the implementation to make your test pass.
   */

  //def plus(x: Int, y: Int): Int = ???

  def plus(x: Int, y: Int): Int = x + y

  /* Exercises */

  //Straight forward one line
  def abs(n: Double): Double = if(n<0)n * -1 else(n)

  def xor(a: Boolean, b: Boolean): Boolean =
    if(a) if(b) false //If both are true, it's false
    else true //If a is true, and b is false, should return true
    else if(b) b //If a is false and b is true, return b
    else b //If a is false and b is false, return b, which is false

  def repeat(s: String, n: Int): String = { require( n >= 0) //To make sure we can't repeat the string negative number of times
    if (n == 0)
      return "" //Test case: if the time to repeat is 0, just return an empty string
    else
      (for (i <- 1 to n) yield s).mkString // Make a new string every time and append it to the end n times
  }

  //Square Step taken directly from lab notes listed above question
  def sqrtStep(c: Double, xn: Double): Double = xn - (((xn * xn) - c) / (2*xn))

  def sqrtN(c: Double, x0: Double, n: Int): Double = {require(n >=0) // Taken from sqrt function in this lab
    if(n == 0) // Test Case that if n is zero just return x0
      x0
    else //Else just recursively call the function stepping further through the sqrt process
      sqrtN(c,sqrtStep(c,x0),n-1) //Decrement n every time so we can get to the base case listed above
  }

  //Followed instructions literally, recurse the error checker upon condition is met
  def sqrtErr(c: Double, x0: Double, epsilon: Double): Double = {require(epsilon > 0) //Found this error out from running the tests and the notes
    if (abs((x0 * x0) - c) < epsilon) x0 else sqrtErr(c, sqrtStep(c, x0), epsilon) //Used sqrtStep since we need to recurse until Xn from X0
  }


  def sqrt(c: Double): Double = {
    require(c >= 0)
    if (c == 0) 0 else sqrtErr(c, 1.0, 0.0001)
  }

  /* Search Tree */

  // Defined in Lab1Like.scala:
  //
  // sealed abstract class SearchTree
  // case object Empty extends SearchTree
  // case class Node(l: SearchTree, d: Int, r: SearchTree) extends SearchTree


  //http://stackoverflow.com/questions/4575406/check-if-a-tree-is-a-binary-search-tree
  //Referenced the stackoverflow post, and translated code from Java to Scala
  def repOk(t: SearchTree): Boolean = {
    def check(t: SearchTree, min: Int, max: Int): Boolean = t match {
      case Empty => true
      case Node(l, d, r) => {
        if(Node(l,d,r) == null)
          true
        if((min <= d) && (max > d) && check(l,min,d) && check(r,d,max)) //Recursively cycle through the tree in a traversal to check nodes
          true //Tree traversal, returns tree if all conditions are met
        else
          false //If the condition is not met, the tree is not valid
      }
    }
    check(t, Int.MinValue, Int.MaxValue)
  }

  def insert(t: SearchTree, n: Int): SearchTree = t match{
    case Empty => Node(Empty,n,Empty) //To resolve the one failed test case that if a tree is completely empty
    case Node(l,d,r) => { //If the case is just a regular node with a left and right child
      if(n >= d) //Insert exactly like how you would in C++ if the n val is greater, move to the right
        Node(l,d,insert(r,n)) //Recursively move to the right and insert the node if end is reached
      else //If the value to insert is greater than the current node's value, do this
        Node(insert(l,n),d,r) //Else move to the left
    }
  }



  def deleteMin(t: SearchTree): (SearchTree, Int) = {
    require(t != Empty)
    (t: @unchecked) match {
      case Node(Empty, d, r) => (r, d) //Once we reach that leftmost node, we just return it's right child and it's data
      case Node(l, d, r) =>
        val (l1, m) = deleteMin(l) //Recursively traverses the nodes till we get to the left most node
        (Node(l1,d,r), m) //Since we need to return a (SearchTree, Int) value,
    }
  }

  def magicFunc(t: SearchTree): Double = t match {
    case Empty => 0
    case Node(l,d,r) => (if (d %2 != 0) d else 0) + magicFunc(l) + magicFunc(r)
  }


  //Really Similar to insert above
  def delete(t: SearchTree, n: Int): SearchTree = t match {
    case Empty => Empty //If the tree is empty, just return Empty
    case Node(Empty, d, Empty) => if(n == d) Empty else Node(Empty,d,Empty)// If the node has no children, just delete it if the val is found, else just cycle through the tree
    case Node(l,d,r) =>{ //Only one test case is needed, just if a node is found
      if (n == d){ //Once the node is found, delete the min using the helper
      val (r1, m) = deleteMin(r) //
        Node(l, m, r1)}
      else if (n > d) //Recursively call the delete on the right subtree if the value is greater than d
        Node(l,d,delete(r,n))
      else //Recursively call the delete function on the left subtree if the value is lesser
        Node(delete(l,n),d,r)
    }
  }

  /* JavaScripty */

//Fairly straightfoward evaluator
  def eval(e: Expr): Double = e match {
    case N( n ) => n //Exactly like in the lab 1 handout where N( n ) n
    case Unary(Neg, e1) => if(eval(e1) >= 0) -eval(e1) else eval(e1) //Discussed in class the uop in this case is the negative
    //These 4 binary operators are all similar, we evaluate the expressions and apply the correct Binary Operator
    case Binary(Plus, e1, e2) => eval(e1) + eval(e2)
    case Binary(Minus, e1, e2) => eval(e1) - eval(e2)
    case Binary(Times, e1, e2) => eval(e1) * eval(e2)
    case Binary(Div, e1, e2) => eval(e1) / eval(e2)
  }

  // Interface to run your interpreter from a string.  This is convenient
  // for unit testing.
  def eval(s: String): Double = eval(Parser.parse(s))



  /* Interface to run your interpreter from the command-line.  You can ignore the code below. */

  def processFile(file: java.io.File) {
    if (debug) { println("Parsing ...") }

    val expr = Parser.parseFile(file)

    if (debug) {
      println("\nExpression AST:\n  " + expr)
      println("------------------------------------------------------------")
    }

    if (debug) { println("Evaluating ...") }

    val v = eval(expr)

    println(prettyNumber(v))
  }

}