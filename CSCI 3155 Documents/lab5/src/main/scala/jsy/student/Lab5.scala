package jsy.student

import jsy.lab5.Lab5Like

object Lab5 extends jsy.util.JsyApplication with Lab5Like {
  import jsy.lab5.ast._
  import jsy.util.DoWith
  import jsy.util.DoWith._

  /*
   * CSCI 3155: Lab 5
   * Nhi Nguyen, Michael Xiao
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

  /*** Exercise with DoWith ***/

  //<M,e> => map {e ...} map gets the e

  def rename[W](env: Map[String,String], e: Expr)(fresh: String => DoWith[W,String]): DoWith[W,Expr] = {
    def ren(env: Map[String,String], e: Expr): DoWith[W,Expr] = e match {
      case N(_) | B(_) | Undefined | S(_) | Null | A(_) => doreturn(e)
      case Print(e1) => ren(env,e1) map { e1p => Print(e1p) }

      case Unary(uop, e1) => ren(env, e1) map { e1p => Unary(uop, e1p)}
      case Binary(bop, e1, e2) => ren(env,e1) flatMap {e1p => ren(env, e2) map {e2p => Binary(bop, e1p, e2p)}}
      case If(e1, e2, e3) => ren(env,e1) flatMap {e1p => ren(env, e2) flatMap {e2p => ren(env ,e3) map {e3p => If(e1p,e2p,e3p)}}}

      case Var(x) => doreturn(Var(env.getOrElse(x,x)))

      case Decl(m, x, e1, e2) => fresh(x) flatMap {
        /*
           val yrenamed = renameVar(y)
        Decl(mut, yrenamed, ren(e1), rename(env + (y -> yrenamed), e2))
         */

        xp => {
          ren(env, e1) flatMap {
            e1p => ren(env + (x -> xp), e2) map {
              e2p => Decl(m, xp, e1p, e2p)
            }
          }
        }

      }

      case Function(p, params, retty, e1) => {
        //R
        val w: DoWith[W,(Option[String], Map[String,String])] = p match {

          case None => doreturn((None, env))
          //xp renamed string
          case Some(x) => fresh(x) map {xp => (Some(xp), env + (x -> xp))}

        }
        //case match new name new env
        w flatMap { case (pp, envp) =>
          params.foldRight[DoWith[W,(List[(String,MTyp)],Map[String,String])]]( doreturn((Nil, envp)) ) {
            //name,mtyp, acc is (nil, envp)
            case ((x,mty), acc) => acc flatMap {
              case (paramsp, envpp) => fresh(x) map { xp => ((xp,mty) :: paramsp, envpp + (x -> xp))}
            }
          } flatMap {
            case (pars,newmap) => ren(newmap,e1) map{e1p => Function(pp,pars,retty,e1p)}
          }
        }
      }

      //case Call(e1, args) => Call(ren(e1), args map ren)
      case Call(e1, args) => {
        args.foldRight[DoWith[W,List[Expr]]](doreturn(List[Expr]())) {
          (h, acc) => ren(env, h) flatMap {e => acc map {f => e :: f}}
        } flatMap { args => ren(env,e1) map {e1p => Call(e1p,args)}}
      }

      case Obj(fields) => fields.foldRight[DoWith[W,Map[String,Expr]]](doreturn(Map[String, Expr]())) {
        case ((s,e), acc) => acc flatMap {newacc => ren(env, e) map {ep => newacc + (s -> ep) }}
      } map {
        ob => Obj(ob)
      }//According to Rename in AST for Obj

      case GetField(e1, f) => ren(env, e1) map {e1p => GetField(e1p, f)} //According to Rename in AST for GetField

      case Assign(e1, e2) => ren(env,e1) flatMap {e1p => ren(env, e2) map {e2p => Assign(e1p, e2p)}} //According to Rename in AST for Assign

      /* Should not match: should have been removed */
      case InterfaceDecl(_, _, _) => throw new IllegalArgumentException("Gremlins: Encountered unexpected expression %s.".format(e))
    }
    ren(env, e)
  }

  /*
      Fresh Function
      const a = (const a = 1;a) + (const a = 2;a)
      const x0 = (const x1 = 1; x1) + (const x2 = 2; x2)
   */
  def myuniquify(e: Expr): Expr = {
    val fresh: String => DoWith[Int,String] = { _ =>
      doget flatMap {
        w1 => doput(w1+1) map {_ => "x" + w1}
        //Do something like xp = "x" + w1 doput(w1 ???) ** ??? here should be + 1 I think **
      } //w1 is the state

    }
    val (_, r) = rename(empty, e)(fresh)(0) //This is the initial state for fresh, so start at 0
    r
  }

  /*** Helper: mapFirst to DoWith ***/

  // List map with an operator returning a DoWith
  def mapWith[W,A,B](l: List[A])(f: A => DoWith[W,B]): DoWith[W,List[B]] = {
    l.foldRight[DoWith[W,List[B]]](doreturn(List[B]())) {
      (h, acc) => acc flatMap {
        // f(h) => DoWith[W, B]
        i => f(h) map { h => h::i }
      }
    }
  }

  // Map map with an operator returning a DoWith
  def mapWith[W,A,B,C,D](m: Map[A,B])(f: ((A,B)) => DoWith[W,(C,D)]): DoWith[W,Map[C,D]] = {
    m.foldRight[DoWith[W,Map[C,D]]](doreturn(Map[C,D]())) {
      case ((k, v), acc) => acc flatMap {
        i => f((k,v)) map { v => i + v }
        //accp map of accumulator
        //case (h,acc) => {f(h) flatMap (hp=> acc map { accp => accp + hp})}
      }
    }
  }

  // Just like mapFirst from Lab 4 but uses a callback f that returns a DoWith in the Some case.
  def mapFirstWith[W,A](l: List[A])(f: A => Option[DoWith[W,A]]): DoWith[W,List[A]] = l match {
    case Nil => doreturn(l) //Or doreturn(Nil)
    case h :: t => f(h) match {
      case Some(a) => a map {hp => hp :: t}
      case None => mapFirstWith(t)(f) map {e => h::e}
    }
  }

  // There are better ways to deal with the combination of data structures like List, Map, and
  // DoWith, but we won't tackle that in this assignment.

  /*** Casting ***/

  def castOk(t1: Typ, t2: Typ): Boolean = (t1, t2) match {
    /***** Make sure to replace the case _ => ???. */
    //case _ => ???
    case (TNull,TObj(_)) => true
    case (_,_) if t1 == t2 => true
    case (TObj(f1), TObj(f2)) => if(f1.size <= f2.size) f1 forall { case (f, vi) => (vi == f2(f))} else f2 forall{ case (f,vi) => (vi == f1(f))}

    /***** Cases for the extra credit. Do not attempt until the rest of the assignment is complete. */
    case (TInterface(tvar, t1p), _) => ???
    case (_, TInterface(tvar, t2p)) => ???
    /***** Otherwise, false. */
    case _ => false
  }

  /*** Type Inference ***/

  // A helper function to check whether a jsy type has a function type in it.
  // While this is completely given, this function is worth studying to see
  // how library functions are used.
  def hasFunctionTyp(t: Typ): Boolean = t match {
    case TFunction(_, _) => true
    case TObj(fields) if (fields exists { case (_, t) => hasFunctionTyp(t) }) => true
    case _ => false
  }

  def isBindex(m: Mode, e: Expr): Boolean = m match {
    case MConst => true
    case MName => true
    case MVar => true
    case MRef => isLExpr(e)
  }

  def typeof(env: TEnv, e: Expr): Typ = {
    def err[T](tgot: Typ, e1: Expr): T = throw StaticTypeError(tgot, e1, e)
    e match {
      case Print(e1) => typeof(env, e1); TUndefined
      case N(_) => TNumber
      case B(_) => TBool
      case Undefined => TUndefined
      case S(_) => TString
      case Var(x) => env.get(x) match {
        case Some((MTyp(m, typ))) => typ
        case None => err(TUndefined, e)//Check how to error handle this
      }
      case Unary(Neg, e1) => typeof(env, e1) match {
        case TNumber => TNumber
        case tgot => err(tgot, e1)
      }
      /***** Cases directly from Lab 4. We will minimize the test of these cases in Lab 5. */
      //TypeNot
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
        //fixed unreachable code error
      case Binary(Lt|Le|Gt|Ge, e1, e2) => typeof(env, e1) match {
        case TNumber => typeof(env, e2) match{
          case TNumber => TBool
          case tgot => err(tgot,e2)
        }
        case TString => typeof(env, e2) match{
          case TString => TBool
          case tgot => err(tgot, e2)
        }
        case tgot => err(tgot,e1)
      }

      //TypeAndOr
      //fixed unreachable code error
      case Binary(And|Or, e1, e2) => typeof(env, e1) match {
        case TBool => typeof(env, e2) match {
          case TBool => TBool
          case tgot => err(tgot, e2)
        }
        case tgot => err(tgot,e1)
      }

      //TypeSeq
      case Binary(Seq, e1, e2) => (typeof(env, e1), typeof(env,e2)) match {
        case (t1, t2) => t2
      }

      //TypeIf
      case If(e1, e2, e3) => (typeof(env, e1)) match {
        case TBool => {
          val tgot2 = typeof(env, e2)
          val tgot3 = typeof(env, e3)
          if(tgot2 == tgot3) tgot2 else err(tgot3, e3)
        }
        case tgot => err(tgot, e1)
      }

      //TypeObj
      case Obj(fields) => {
        val objField = fields map{
          case (f1,f2) => f1 -> typeof(env,f2)
        }
        TObj(objField)
      }

      //TypeGetField
      case GetField(e1, f) => typeof(env,e1) match {
        case TObj(tF) => tF(f) //Look at lookup, tF is map and we can just lookup
        case tgot => err(tgot,e1)
      }

      /***** Cases from Lab 4 that need a small amount of adapting. */
      case Decl(m, x, e1, e2) if isBindex(m, e1) => {
        typeof(extend(env,x,MTyp(m,typeof(env, e1))), e2)
      }

      case Function(p, params, tann, e1) => {
        // Bind to env1 an environment that extends env with an appropriate binding if
        // the function is potentially recursive.
        val env1 = (p, tann) match {
          case (Some(f), Some(tret)) =>
            val tprime = TFunction(params, tret)
            extend(env,f,MTyp(MConst,tprime))
          case (None, _) => env
          case _ => err(TUndefined, e1)
        }
        // Bind to env2 an environment that extends env1 with bindings for params.
        val env2 = params.foldLeft(env1){
          case (envacc,(xi,MTyp(m,ti))) => extend(envacc,xi,MTyp(m,ti))
        }
        // Match on whether the return type is specified.
        tann match {
          case None => typeof(env2,e1)
          case Some(tret) => if(tret==typeof(env2,e1)) typeof(env2,e1) else err(typeof(env2,e1),e1)
        }
        TFunction(params, typeof(env2,e1))
      }

      case Call(e1, args) => typeof(env, e1) match {
        case TFunction(params, tret) if (params.length == args.length) =>
          (params, args).zipped.foreach { //Translated from inference rule, check if it's right
            case (p, ei) => if((typeof(env, ei) == p._2.t) && isBindex(p._2.m, ei)) p._2.t else err(p._2.t, ei)
          }
          tret
        case tgot => err(tgot, e1)
      }

      /***** New cases for Lab 5. ***/
      case Assign(Var(x), e1) => env.get(x) match {
        case Some(MTyp(m, typ)) => if(m == MRef || m == MVar) typ else err(TUndefined, e1)
        case None => err(TUndefined, e1)
      }

      case Assign(GetField(e1, f), e2) => typeof(env,e1) match{
        case TObj(fields) => fields.get(f) match{
          case Some(x) => if (x == typeof(env,e2)) x else err(typeof(env,e2),e2)
          case None => err(typeof(env,e2),e2)
        }
        case _ => err(typeof(env,e2),e2)
      }
      case Assign(_, _) => err(TUndefined, e)

      case Null => TNull

      case Unary(Cast(t), e1) => typeof(env, e1) match {
        case tgot if castOk(tgot, t) => t
        case tgot => err(tgot, e1)
      }

      /* Should not match: non-source expressions or should have been removed */
      case A(_) | Unary(Deref, _) | InterfaceDecl(_, _, _) => throw new IllegalArgumentException("Gremlins: Encountered unexpected expression %s.".format(e))
    }
  }

  /*** Small-Step Interpreter ***/

  /*
   * Helper function that implements the semantics of inequality
   * operators Lt, Le, Gt, and Ge on values.
   *
   * We suggest a refactoring of code from Lab 2 to be able to
   * use this helper function in eval and step.
   *
   * This should the same code as from Lab 3 and Lab 4.
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

  /* Capture-avoiding substitution in e replacing variables x with esub. */
  def substitute(e: Expr, esub: Expr, x: String): Expr = {
    def subst(e: Expr): Expr = e match {
      case N(_) | B(_) | Undefined | S(_) | Null | A(_) => e
      case Print(e1) => Print(subst(e1))
      /***** Cases from Lab 3 */
      case Unary(uop, e1) => Unary(uop, subst(e1))
      case Binary(bop, e1, e2) => Binary(bop, subst(e1), subst(e2))
      case If(e1, e2, e3) => If(subst(e1), subst(e2), subst(e3))
      case Var(y) => if(y == x) esub else Var(y)
      /***** Cases need a small adaption from Lab 3 */
      case Decl(mut, y, e1, e2) => Decl(mut, y, subst(e1), if (x == y) e2 else subst(e2))
      /***** Cases needing adapting from Lab 4 */
        //copied over most of this from lab 4, not sure if correct
      case Function(p, paramse, retty, e1) => {
      if (paramse.foldLeft(false)((bool, param) => (param._1 == x) || bool) ||  Some(x) == p)
        e
      else
      Function(p, paramse, retty, subst(e1))
      }
      /***** Cases directly from Lab 4 */
      case Call(e1, args) => Call(subst(e1), args map subst)
      case Obj(fields) => Obj(fields.mapValues(subst))
      case GetField(e1, f) => GetField(subst(e1),f)
      /***** New case for Lab 5 */
      case Assign(e1, e2) => Assign(subst(e1),subst(e2))//check on this

      /* Should not match: should have been removed */
      case InterfaceDecl(_, _, _) => throw new IllegalArgumentException("Gremlins: Encountered unexpected expression %s.".format(e))
    }

    def myrename(e: Expr): Expr = {
      val fvs = freeVars(esub)
      def fresh(x: String): String = if (fvs contains x) fresh(x + "$") else x
      rename[Unit](e)(Nil){ x => doreturn(fresh(x)) }
    }

    subst(myrename(e))
  }

  /* Check whether or not an expression is reduced enough to be applied given a mode. */
  def isRedex(mode: Mode, e: Expr): Boolean = mode match {
    case MConst => !isValue(e)
    case MName => false
    case MVar => !isValue(e)
    case MRef => !isLValue(e)
  }

  def getBinding(mode: Mode, e: Expr): DoWith[Mem,Expr] = {
    require(!isRedex(mode,e), s"expression ${e} must not reducible under mode ${mode}")
    mode match {
      case MConst => doreturn(e)
      case MName => doreturn(e)
      case MVar => memalloc(e) map { a => Unary(Deref, a)}
      case MRef => doreturn(e)
    }
  }

  /* A small-step transition. */
  def step(e: Expr): DoWith[Mem, Expr] = {
    require(!isValue(e), "stepping on a value: %s".format(e))
    e match {
      /* Base Cases: Do Rules */
      case Print(v1) if isValue(v1) => doget map { m => println(pretty(m, v1)); Undefined }

      /** *** Cases needing adapting from Lab 3. */

      //DoNeg -> Is this right?
      case Unary(Neg, N(n)) => doget map { m => N(-n) }

      //DoNot
      case Unary(Not, B(b)) => doget map { m => B(!b) }

      //DoSeq
      case Binary(Seq, v1, e2) if isValue(v1) => doget map { m => e2 }

      //DoPlus
      case Binary(Plus, N(v1), N(v2)) => doget map { m => N(v1 + v2) }

      //DoPlusString
      case Binary(Plus, S(s1), S(s2)) => doget map { m => S(s1 + s2) }

      //DoInequalityString
      case Binary(bop, S(s1), S(s2)) if bop == Lt || bop == Le || bop == Gt || bop == Ge => doget map { m => B(inequalityVal(bop, S(s1), S(s2))) }

      //DoInequalityNumber
      case Binary(bop, N(n1), N(n2)) if bop == Lt || bop == Le || bop == Gt || bop == Ge => doget map { m => B(inequalityVal(bop, N(n1), N(n2))) }

      //DoEquality
      case Binary(Eq, v1, v2) if isValue(v1) && isValue(v2) => (v1, v2) match {
        case (N(n1), N(n2)) => doget map { m => B(n1 == n2) }
        case (S(s1), S(s2)) => doget map { m => B(s1 == s2) }
        case (v1, v2) => doget map { m => B(v1 == v2) }
      }
      case Binary(Ne, v1, v2) if isValue(v1) && isValue(v2) => (v1, v2) match {
        case (N(n1), N(n2)) => doget map { m => B(n1 != n2) }
        case (S(s1), S(s2)) => doget map { m => B(s1 != s2) }
        case (v1, v2) => doget map { m => B(v1 != v2) }
      }

      //DoArith
      case Binary(bop, N(n1), N(n2)) if bop == Minus || bop == Times || bop == Div =>
        (bop: @unchecked) match {
          case Minus => doget map { m => N(n1 - n2) }
          case Times => doget map { m => N(n1 * n2) }
          case Div => doget map { m => N(n1 / n2) }
        }
      //Not sure if it is better to change v1 to B(true) or B(false)

      // DoAndTrue
      case Binary(And, v1, e2) if v1 == B(true) => doget map { m => e2 }
      //DoAndFalse
      case Binary(And, v1, e2) if v1 == B(false) => doget map { m => B(false) }
      //DoOrTrue
      case Binary(Or, v1, e2) if v1 == B(true) => doget map { m => B(true) }
      //DoOrFalse
      case Binary(Or, v1, e2) if v1 == B(false) => doget map { m => e2 }
      //DoIfTrue
      case If(v1, e2, e3) if v1 == B(true) => doget map { m => e2 }
      //DoIfFalse
      case If(v1, e2, e3) if v1 == B(false) => doget map { m => e3 }

      /** *** More cases here */
      /** *** Cases needing adapting from Lab 4. */
      case Obj(fields) if (fields forall { case (_, vi) => isValue(vi) }) => memalloc(e)

      case GetField(a@A(_), f) => doget map { m => m(a) match {
        case Obj(fields) => fields(f)
        case _ => throw NullDereferenceError(e)
      }
      }

      case Decl(MConst|MName, x, v1, e2) if isValue(v1) => getBinding(MConst, v1) map { ebind => substitute(e2, ebind, x) }
      case Decl(MRef, x, v1, e2) if isLValue(v1) => getBinding(MRef, v1) map {ebind => substitute(e2, ebind,x)}
      case Decl(MVar, x, v1, e2) if isValue(v1) => getBinding(MVar, v1) map { ebind => substitute(e2, ebind, x) }



      /** *** New cases for Lab 5. */
      //DoDeref
      case Unary(Deref, a@A(_)) => doget map {
        m => m(a)
      }

      //DoAssignVar
      case Assign(Unary(Deref, a@A(_)), v) if isValue(v) =>
        domodify[Mem] { m => m + (a -> v) } map { _ => v }

      case Assign(GetField(a@A(_), f), v) if isValue(v) => //CHeck if f exists
        domodify[Mem] { m => m(a) match {
          case Obj(fields) => m + (a -> Obj(fields + (f -> v)))
        }
        } map { _ => v }

      // doreturn(fields) map { fields => fields + (fi, step(vi)) }


      case Call(v@Function(p, params, _, e), args) => {
        val pazip = params zip args
        if (pazip.forall { case ((_, MTyp(mi, _)), ei) => !isRedex(mi, ei) }) {
          val dwep = pazip.foldRight(doreturn(e): DoWith[Mem, Expr]) {
            case (((xi, MTyp(mi, _)), ei), dwacc) => getBinding(mi, ei) flatMap { eip => dwacc map { expr => substitute(expr, eip, xi) } }

          }
          p match {
            case None => dwep
            case Some(x) => dwep map { expr => substitute(expr, v, x) }
          }
        }
        else {
          val dwpazipp = mapFirstWith(pazip) {
            case (pt@(xi, MTyp(m, t)), ai) => if (isRedex(m, ai)) Some(step(ai) map { aip => (pt, aip) }) else None
          }
          dwpazipp map { pazip1 => Call(v, pazip1.unzip._2 ) }
        }
      }

      //DoCast
      case Unary(Cast(t), v) if(isValue(v)) => doreturn(v)

      //NullErrorGetField -is this right???
      case GetField(Null, _) => throw NullDereferenceError(e)

       //NullErrorAssignField
      case Assign(GetField(Null,_), _) => throw NullDereferenceError(e)

      /* Base Cases: Error Rules */
      /** *** Replace the following case with a case to throw NullDeferenceError.  */
      //case _ => throw NullDeferenceError(e)

      case Unary(Cast(t), v) => v match {
        case Null => doreturn(Null)
        case Obj(fields) => {
          val dwfield = fields.find{case (f, e) =>  ???  }

          dwfield match {
            case Some((f, ep)) => ???
            case None => throw ???
          }
        }
      }
      /* Inductive Cases: Search Rules */
      /** *** Cases needing adapting from Lab 3. Make sure to replace the case _ => ???. */
      case Print(e1) => step(e1) map { e1p => Print(e1p) }

      //SearchUnary -> Followed SearchPrint above
      case Unary(uop, e1) => step(e1) map { e1p => Unary(uop, e1p) }

      //SearchBinary2
      case Binary(bop, v1, e2) if isValue(v1) => step(e2) map { e2p => Binary(bop, v1, e2p)}

        //SearchBinary1
      case Binary(bop, e1, e2) => step(e1) map { e1p => Binary(bop, e1p, e2) }


      //SearchIf
      case If(e1, e2, e3) => step(e1) map { e1p => If(e1p, e2, e3) }


      /** *** Cases needing adapting from Lab 4 */

      //SearchGetField
      case GetField(e1, f) => step(e1) map { e1p => GetField(e1p, f) }

      //SearchObj
      case Obj(fields) =>{
        val dwfields = fields.find { case (_, ex) => !isValue(ex) }

        dwfields match {
        //case (fi, ei) => fi -> step(ei)
        case Some((f, exp)) => step(exp) map { ep => Obj(fields + (f -> ep))}
        case None => doreturn(Obj(fields))
      }
     }
      //SearchDecl
      case Decl(mode, x, e1, e2) if isRedex(mode,e1) => step(e1) map {e1p => Decl(mode, x, e1p, e2)}

      //SearchCall1
      case Call(e1, args) => step(e1) map {e1p => Call(e1p, args)}

      /***** New cases for Lab 5.  */
      //SearchAssign1
      case Assign(e1, e2) if !isLValue(e1) => step(e1) map {e1p => Assign(e1p, e2)}

      //SearchAssign2
      case Assign(lv1, e2) => step(e2) map {e2p => Assign(lv1, e2p)}

      /* Everything else is a stuck error. */
      case _ => throw StuckError(e)
    }
  }

  /*** Extra Credit: Lowering: Remove Interface Declarations ***/

  def lower(e: Expr): Expr =
  /* Do nothing by default. Change to attempt extra credit. */
  e

  /*** External Interfaces ***/

  //this.debug = true // comment this out or set to false if you don't want print debugging information
  this.maxSteps = Some(1000) // comment this out or set to None to not bound the number of steps.
  this.keepGoing = true // comment this out if you want to stop at first exception when processing a file
}
