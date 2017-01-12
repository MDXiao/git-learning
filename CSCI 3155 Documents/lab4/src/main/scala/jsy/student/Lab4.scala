package jsy.student

import jsy.lab4.Lab4Like

object Lab4 extends jsy.util.JsyApplication with Lab4Like {
  import jsy.lab4.ast._
  import jsy.lab4.Parser

  /*
   * CSCI 3155: Lab 4
   * Christine Samson, Michael Xiao
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
   * '???' as needed to get something that compiles without error. The '???'
   * is a Scala expression that throws the exception scala.NotImplementedError.
   */

  /* Collections and Higher-Order Functions */

  /* Lists */

  def compressRec[A](l: List[A]): List[A] = l match {
    case Nil | _ :: Nil => l
    case h1 :: (t1 @ (h2 :: _)) => if(h1 == h2) compressRec(t1) else h1 :: compressRec(t1) //Got from write-up
  }
  //Lists are ordered and no duplicates
  def compressFold[A](l: List[A]): List[A] = l.foldRight(Nil: List[A]){
    (h1, acc) => acc match {
      case Nil => h1 :: Nil
      case t1 @ (h2 :: _) => if(h1 == h2) acc else h1 :: acc
    } // h = 3, acc = list
  }

  //Understand this
  def mapFirst[A](l: List[A])(f: A => Option[A]): List[A] = l match {
    case Nil => l
    case h :: t => f(h) match {
      case Some(a) => a :: t
      case None => h :: mapFirst(t)(f)
    }
  }

  /* Trees */
  //Since we are folding to the left, we can just recursively call loop on the left side of the tree
  def foldLeft[A](t: Tree)(z: A)(f: (A, Int) => A): A = {
    def loop(acc: A, t: Tree): A = t match {
      case Empty => acc
      case Node(l, d, r) => loop(f(loop(acc, l), d), r) //Continously loop on the side of the tree
    }
    loop(z, t)
  }

  // An example use of foldLeft
  def sum(t: Tree): Int = foldLeft(t)(0){ (acc, d) => acc + d }

  // Create a tree from a list. An example use of the
  // List.foldLeft method.
  def treeFromList(l: List[Int]): Tree =
  l.foldLeft(Empty: Tree){ (acc, i) => acc insert i }

  def strictlyOrdered(t: Tree): Boolean = {
    val (b, _) = foldLeft(t)((true, None: Option[Int])){
      case ((b, None),i) => (b, Some(i))
      case ((b, Some(x)),i) => (b && (x < i), Some(i)) //i is the previous element
    }
    b
  }

  /* Type Inference */

  // While this helper function is completely given, this function is
  // worth studying to see how library methods are used.
  def hasFunctionTyp(t: Typ): Boolean = t match {
    case TFunction(_, _) => true
    case TObj(fields) if (fields exists { case (_, t) => hasFunctionTyp(t) }) => true
    case _ => false
  }

  def typeof(env: TEnv, e: Expr): Typ = {
    def err[T](tgot: Typ, e1: Expr): T = throw StaticTypeError(tgot, e1, e)

    e match {
      //Base Cases
      case Print(e1) => typeof(env, e1); TUndefined
      case N(_) => TNumber
      case B(_) => TBool
      case Undefined => TUndefined
      case S(_) => TString
      case Var(x) => lookup(env, x)

      //TypeConst
      case Decl(mode, x, e1, e2) => {
        typeof(extend(env,x,typeof(env, e1)), e2)
      }


      //TypeNeg and TypeNot
      case Unary(Neg, e1) => typeof(env, e1) match {
        case TNumber => TNumber
        case tgot => err(tgot, e1)
      }
      case Unary(Not, e1) => typeof(env, e1) match {
        case TBool => TBool
        case tgot => err(tgot, e1)
      }

      //TypeBinaries
      case Binary(Plus, e1, e2) => (typeof(env, e1), typeof(env, e2)) match {
        case (TNumber, TNumber) => TNumber
        case (TString, TString) => TString
        case (TNumber, tgot2) => err(tgot2, e2)
        case (TString, tgot2) => err(tgot2, e2)
        case (tgot1, _) => err(tgot1, e1)
      }
      case Binary(Minus|Times|Div, e1, e2) =>(typeof(env, e1), typeof(env, e2)) match {
        case (TNumber, TNumber) => TNumber
        case (TNumber, tgot2) => err(tgot2, e2)
        case (tgot1, _) => err(tgot1, e1)

      }

      //TypeEquality
      case Binary(Eq|Ne, e1, e2) => (typeof(env, e1), typeof(env, e2)) match {
        case (t1,t2) => if(t1 == t2 && !hasFunctionTyp(t1) && !hasFunctionTyp(t2)) TBool else err(t1, e1)
      }

      //TypeInequality
      case Binary(Lt|Le|Gt|Ge, e1, e2) => (typeof(env, e1), typeof(env, e2)) match {
        case (TNumber, TNumber) => TBool
        case (TString, TString) => TBool
        case (t1, tgot2) => err(tgot2, e2)
        case (tgot1, _) => err(tgot1, e1)
      }

      //TypeAndOr
      case Binary(And|Or, e1, e2) => (typeof(env, e1), typeof(env, e2)) match {
        case (TBool, TBool) => TBool
        case (t1, tgot2) => err(tgot2, e2)
        case (tgot1, _) => err(tgot1, e1)
      }

      //TypeSeq
      case Binary(Seq, e1, e2) => (typeof(env, e1), typeof(env,e2)) match {
        case (t1, t2) => t2
      }

      //Case If
      case If(e1, e2, e3) => (typeof(env, e1)) match {
        case TBool => {
          val tgot2 = typeof(env, e2)
          val tgot3 = typeof(env, e3)
          if(tgot2 == tgot3) tgot2 else err(tgot3, e3)
        }
        case tgot => err(tgot, e1)
      }

      case Function(p, params, tann, e1) => {
        // Bind to env1 an environment that extends env with an appropriate binding if
        // the function is potentially recursive.
        val env1 = (p, tann) match {
          /***** Add cases here *****/
          case (None, None) => env
          case (None, Some(t)) => env
          case (Some(p), Some(t)) => extend(env, p, TFunction(params,t))
          case _ => err(TUndefined, e1)
        }
        // Bind to env2 an environment that extends env1 with bindings for params.
        val env2 = params.foldLeft(env1) {
          //(acc, h) => extend(acc, h._1, h._2.t)
          //Allow function matching in the function parameters
          case (envacc, (xi, MTyp(_,ti))) => extend(envacc, xi, ti)
        }

        // Infer the type of the function body
        val te = typeof(env2, e1)
        // Check with the possibly annotated return type
        tann match {
          case None => te
          case Some(t) => if(t == te) te else err(te, e1) //What are the arguments to the StaticTypeError????
        }
        TFunction(params, te)
      }

      case Call(e1, args) => typeof(env, e1) match {
        case TFunction(params, tret) if (params.length == args.length) =>
          (params zip args).foreach {
            case (p,ei) => if(typeof(env, ei) == p._2.t) p._2.t else err(p._2.t, ei)
          };
          tret
        case tgot => err(tgot, e1)
      }

      case Obj(fields) => {
        val objField = fields map{
          case (f1,f2) => f1 -> typeof(env,f2)
        }
        TObj(objField)
      } //Returns Fields to types from fields to expressions

      case GetField(e1, f) => typeof(env,e1) match {
        case TObj(tF) => tF(f) //Look at lookup, tF is map and we can just lookup
        case tgot => err(tgot,e1)
      }
      //Returning a f:t given a field
    }
  }


  /* Small-Step Interpreter */

  /*
   * Helper function that implements the semantics of inequality
   * operators Lt, Le, Gt, and Ge on values.
   *
   * We suggest a refactoring of code from Lab 2 to be able to
   * use this helper function in eval and step.
   *
   * This should the same code as from Lab 3.
   */
  def inequalityVal(bop: Bop, v1: Expr, v2: Expr): Boolean = {
    require(isValue(v1), s"inequalityVal: v1 ${v1} is not a value")
    require(isValue(v2), s"inequalityVal: v2 ${v2} is not a value")
    require(bop == Lt || bop == Le || bop == Gt || bop == Ge)
    (v1, v2) match {
      case (S(s1), S(s2)) => bop match { //For Strings
        case Lt => s1 < s2
        case Le => s1 <= s2
        case Gt => s1 > s2
        case Ge => s1 >= s2
      }
      case (N(n1), N(n2)) => bop match { //For Numbers
        case Lt => n1 < n2
        case Le => n1 <= n2
        case Gt => n1 > n2
        case Ge => n1 >= n2
      }
    }
  }

  /* This should be the same code as from Lab 3 */
  def iterate(e0: Expr)(next: (Expr, Int) => Option[Expr]): Expr = {
    def loop(e: Expr, n: Int): Expr = next(e, n) match {
      case None => e
      case Some(ep) => loop(ep, n+1)
    }
    loop(e0, 0)
  }

  /* Capture-avoiding substitution in e replacing variables x with esub. */
  def substitute(e: Expr, esub: Expr, x: String): Expr = {
    def subst(e: Expr): Expr = e match {
      case N(_) | B(_) | Undefined | S(_) => e
      case Print(e1) => Print(substitute(e1, esub, x))
      /***** Cases from Lab 3 */
      case Unary(uop, e1) => Unary(uop, substitute(e1, esub, x))
      case Binary(bop, e1, e2) => Binary(bop, substitute(e1, esub, x), substitute(e2, esub, x))
      case If(e1, e2, e3) => If(substitute(e1,esub,x), substitute(e2, esub, x), substitute(e3,esub,x))
      case Var(y) => if(y == x) esub else Var(y)
      case Decl(mode, y, e1, e2) => if(y == x) Decl(mode, y, substitute(e1, esub, x) , e2) else Decl(mode, y, substitute(e1, esub, x), substitute(e2, esub, x))
      /***** Cases needing adapting from Lab 3 */

      case Function(p, params, tann, e1) => {

        if (params.foldLeft(false)((bool, param) => (param._1 == x) || bool) ||  Some(x) == p)
          e
        else
          Function(p, params, tann, subst(e1))
      }
      /*
      case Function (p, params, tann, e1) => {
        //case None => if (params.exists(tup => tup._1 == x)) Function(p, params, tann, e1) else Function(p, params, tann, substitute(e1, esub, x))
        //case Some (n) => if (params.exists(tup => (tup._1 == x) && (tup._1 == n))) Function(p, params, tann, e1) else Function(p, params, tann, substitute(e1, esub, x))

        if (p == Some(x) || (params exists {case (y, _) => x == y})) e else Function(p, params, tann, substitute(e1, esub, x))

      }
*/
      case Call(e1, args) => {
       Call(substitute(e1,esub,x), args map subst)
      }
      /***** New cases for Lab 4 */

      //case Obj(fields) => Obj(fields.map(field => (field._1, subst(field._2))))
      case Obj(fields) => Obj(fields.mapValues(subst))
      //case GetField(e1, f) => if (f != x) GetField(subst(e1), f) else e
      case GetField(e1, f) => GetField(substitute(e1, esub, x), f)
    }

    val fvs = freeVars(esub)
    def fresh(x: String): String = if (fvs(x)) fresh(x + "$") else x
    subst(e)
  }

  /* Rename bound variables in e */
  def rename(e: Expr)(renameVar: String => String): Expr = {
    def ren(env: Map[String,String], e: Expr): Expr = {
      e match {
        case N(_) | B(_) | Undefined | S(_) => e
        case Print(e1) => Print(ren(env, e1))

        case Unary(uop, e1) => Unary(uop, ren(env, e1))
        case Binary(bop, e1, e2) => Binary(bop, ren(env,e1), ren(env, e2))
        case If(e1, e2, e3) => If(ren(env, e1), ren(env, e2), ren(env, e3))

        case Var(y) => Var(env(y))
        case Decl(mode, y, e1, e2) => {
          val yp = renameVar(y)
          val env2 = env + (y -> yp)
          Decl(mode, yp, ren(env2, e1), ren(env2, e2))
        }

        case Function(p, params, retty, e1) => {
          val (pp, envp): (Option[String], Map[String,String]) = p match {
            case None => ???
            case Some(x) => ???
          }
          val (paramsp, envpp) = params.foldRight( (Nil: List[(String,MTyp)], envp) ) {
            ??? //
          }
          Function(pp, paramsp, retty, ren(envpp, e1))
        }

        case Call(e1, args) => Call(ren(env, e1) , args)
        case Obj(fields) => Obj(fields.mapValues(ei => ren(env, ei)))
        case GetField(e1, f) => GetField(ren(env, e1), f)
      }
    }
    ren(Map.empty, e)
  }

  /* Check whether or not an expression is reduced enough to be applied given a mode. */
  def isRedex(mode: Mode, e: Expr): Boolean = mode match {
    case Const => !isValue(e) // Is this correct???
    case Name => false //Think of name x = console.log("Hi"), 6; x+ x ===> console.log("hi"),6 + console.log("hi"),6
  }

  def step(e: Expr): Expr = {
    require(!isValue(e), s"step: e ${e} to step is a value")
    e match {
      /* Base Cases: Do Rules */
      case Print(v1) if isValue(v1) => println(pretty(v1)); Undefined

      /** *** Cases needing adapting from Lab 3. */

      //DoUnaries
      case Unary(Neg, N(n1)) => N(-n1)
      case Unary(Not, B(b1)) => B(!b1)

      //DoSeq
      case Binary(Seq, v1, e2) if isValue(v1) => e2

      //DoPlus
      case Binary(Plus, v1, v2) if isValue(v1) && isValue(v2) => (v1, v2) match {
        case (N(n1), N(n2)) => N(n1 + n2)
        case (S(s1), S(s2)) => S(s1 + s2)
      }

      //DoArith
      case Binary(Minus, N(n1), N(n2)) => N(n1 - n2)
      case Binary(Times, N(n1), N(n2)) => N(n1 * n2)
      case Binary(Div, N(n1), N(n2)) => N(n1 / n2)

      //DoEquality --- IS THIS CORRECT??
      case Binary(Eq, v1, v2) if isValue(v1) && isValue(v2) => (v1, v2) match {
        case (N(n1), N(n2)) => B(n1 == n2)
        case (B(b1), B(b2)) => B(b1 == b2)
        case (S(s1), S(s2)) => B(s1 == s2)
        case (v1, v2) => B(v1 == v2)
      }
      case Binary(Ne, v1, v2) if isValue(v1) && isValue(v2) => (v1, v2) match {
        case (N(n1), N(n2)) => B(n1 != n2)
        case (B(b1), B(b2)) => B(b1 != b2)
        case (S(s1), S(s2)) => B(s1 != s2)
        case (v1, v2) => B(v1 != v2)
      }
      //DoInequality
      case Binary(Lt, N(v1), N(v2)) => B(inequalityVal(Lt, N(v1), N(v2)))
      case Binary(Lt, S(v1), S(v2)) => B(inequalityVal(Lt, S(v1), S(v2)))
      case Binary(Le, N(v1), N(v2)) => B(inequalityVal(Le, N(v1), N(v2)))
      case Binary(Le, S(v1), S(v2)) => B(inequalityVal(Le, S(v1), S(v2)))
      case Binary(Gt, N(v1), N(v2)) => B(inequalityVal(Gt, N(v1), N(v2)))
      case Binary(Gt, S(v1), S(v2)) => B(inequalityVal(Gt, S(v1), S(v2)))
      case Binary(Ge, N(v1), N(v2)) => B(inequalityVal(Ge, N(v1), N(v2)))
      case Binary(Ge, S(v1), S(v2)) => B(inequalityVal(Ge, S(v1), S(v2)))


      //DoAnd and DoOr
      case Binary(And, B(b1), e2) => b1 match {
        case true => e2
        case false => B(false)
      }
      case Binary(Or, B(b1), e2) => b1 match {
        case false => e2
        case true => B(true)
      }

      //DoIf
      case If(B(b1), e2, e3) => b1 match {
        case true => e2
        case false => e3
      }

      //DoDecl -> What is a Redex??
      case Decl(mode: Mode, x: String, e1: Expr, e2: Expr) if !isRedex(mode, e1) => substitute(e2, e1, x)


      //DoGetField - The Inference Rule says "given a field, return the value of that specific field"
      case GetField(Obj(f), v) => f(v)

      /** *** More cases here */
      case Call(v1, args) if isValue(v1) => //How do you implement Call??
        v1 match {
          case Function(p, params, _, e1) => {
            val pazip = params zip args
            // DoCall and DoCallRec
            if (pazip.forall{ case((_,MTyp(mi,_)),ei) => !isRedex(mi,ei)} ) { //How do you access the Modes in Params??
              val e1p = pazip.foldRight(e1) {
               case (pa_i @(pi@(xi,mti),ei), acc) => substitute(acc, ei, xi)
              }
              p match {
                case None => e1p
                case Some(x1) => substitute(e1p, v1, x1)
              }
            }
            //SearchCall2
            else {
              val pazipp = mapFirst(pazip) {
                case (p @(si,MTyp(mi,ti)),ai) => if(isRedex(mi, ai)) Some(p, step(ai)) else None
              }

              val new_args = pazipp.map(x => x._2)
              Call(v1, new_args)
            }
          }
          case _ => throw StuckError(e)
        }

      /** *** New cases for Lab 4. */

      /* Inductive Cases: Search Rules */
      case Print(e1) => Print(step(e1))

      /** *** Cases from Lab 3. */

      //SearchUnary
      case Unary(uop, e1) => {
        val e1p = step(e1)
        Unary(uop, e1p)
      }

      //SearchEquality2
      case Binary(Eq, v1, e2) if isValue(v1) => {
        //How do you do function checking?
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
        Binary(bop, v1, e2p)
      }

      //SearchBinary1
      case Binary(bop, e1, e2) => {
        val e1p = step(e1)
        Binary(bop, e1p, e2)
      }


      //SearchIf
      case If(e1, e2, e3) => {
        val e1p = step(e1)
        If(e1p, e2, e3)
      }

      //SearchDecl
      case Decl(mode: Mode, x: String, e1: Expr, e2: Expr) => {
        val e1p = step(e1)
        Decl(mode, x, e1p, e2)
      }

      /** *** More cases here */
      /** *** Cases needing adapting from Lab 3 */


      //SearchCall1.1
      case Call(e1, args) => Call(step(e1), args)

      /** *** New cases for Lab 4. */

      //SearchObj - Not sure if this is done correctly
      case Obj(fields) => {
        val objField = fields.find{case (_, ex) => !isValue(ex)} match {
          //case (fi, ei) => fi -> step(ei)
          case Some((f, exp)) => fields + (f -> step(exp))
          case None => fields
        }
        Obj(objField)
      }

      //SearchGetField
      case GetField(e1, f) => GetField(step(e1), f)

      /* Everything else is a stuck error. Should not happen if e is well-typed.
       *
       * Tip: you might want to first develop by comment out the following line to see which
       * cases you have missing. You then uncomment this line when you are sure all the cases
       * that you have left the ones that should be stuck.
       */
      case _ => throw StuckError(e)
    }
  }


  /* External Interfaces */

  //this.debug = true // uncomment this if you want to print debugging information
  this.keepGoing = true // comment this out if you want to stop at first exception when processing a file
}
