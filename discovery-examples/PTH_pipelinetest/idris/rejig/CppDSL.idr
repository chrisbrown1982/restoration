module CppDSL

import public Data.Vect

%default total
%access public export

||| Types
data CTy : Type where
  CBool : CTy
  CInt  : CTy
  CVoid : CTy
  CPtr  : (a : CTy) -> CTy

||| Expressions.
data CExp : Type where
  ||| Local variable declarations. Names are unique.
  Decl : (ty : CTy) -> (v : String) -> (k : CExp) -> CExp
  ||| Assignment statements.
  Assn : (v : String) -> (e : CExp) -> (k : CExp) -> CExp
  ||| Logical disjunction.
  |||
  ||| This has been added to enable expression of disjunctions of conditions in 
  ||| merging of iterations.
  |||
  ||| **FIXME**: This doesn't work since you can have iterations and 
  ||| assignments here. Simply set them to variables?
  Disj : (a : CExp) -> (b : CExp) -> (k : CExp) -> CExp
  ||| Function application. May be side effecting.
  ||| 
  ||| Arguments must be passed as arguments (to make totality simpler).
  ||| Model generation can simply declare and assign variables above the call.
  Appl : (f : String) -> (xs : List String) -> (k : CExp) -> CExp
  ||| If statements.
  If   : (c : CExp) -> (tt : CExp) -> (ff : CExp) -> (k : CExp) -> CExp
  ||| Iteration.
  |||
  ||| N.B. *Not* a general while/for loop.
  |||
  ||| Let us assume that we're iterating over a range of integers. Other things 
  ||| (should they be needed), e.g. vectors, can be reduced to this.
  |||
  ||| Assume that Iter initialises *i* to *z*. Corollary: *z* must be an Int.
  |||
  ||| Assume *c* is a boolean expression and does not contain any free 
  ||| variables, but may contain *i*.
  Iter : (is : Vect n String) -> (zs : Vect n Int) -> (c : CExp) -> (b : CExp) -> (k : CExp) -> CExp
  ||| Abstracted block.
  |||
  ||| Will ideally store what the block represents so that we can transform 
  ||| between models.
  Block : (k : CExp) -> CExp
  ||| End of continuation.
  Stop : CExp

ArgList : Type
ArgList = List (CTy, String)

||| Global declarations. (A program.)
data CDecl : Type where
  ||| Global variables. Names are unique.
  VarDec : (ty : CTy) -> (v : String) -> (e : CExp) -> (k : CDecl) -> CDecl
  ||| Functions. Names are unique. Assume not recursive.
  FunDec : (ty : CTy) -> (f : String) -> (xs : ArgList) -> (e : CExp) -> (k : CDecl) -> CDecl
  ||| End of continuation.
  Quit : CDecl

||| Returns the length of a continuation.
length : (e : CExp) -> Nat
length (Decl ty v k) = 1 + length k
length (Assn v e k) = 1 + length k
length (Disj a b k) = 1 + length k
length (Appl f xs k) = 1 + length k
length (If c tt ff k) = 1 + length k
length (Iter is zs c b k) = 1 + length k
length (Block k) = 1 + length k
length Stop = 0

||| Proof that *e* is not Stop.
data NonEmpty : (e : CExp) -> Type where
  IsNonEmptyDecl : NonEmpty (Decl ty v k)
  IsNonEmptyAssn : NonEmpty (Assn v e k)
  IsNonEmptyDisj : NonEmpty (Disj a b k)
  IsNonEmptyAppl : NonEmpty (Appl f xs k)
  IsNonEmptyIf : NonEmpty (If c tt ff k)
  IsNonEmptyIter : NonEmpty (Iter is zs c b k)
  IsNonEmptyBlock : NonEmpty (Block k)

||| length e = 1
data Singleton : (e : CExp) -> Type where
  IsSingletonDecl : Singleton (Decl ty v Stop)
  IsSingletonAssn : Singleton (Assn v e Stop)
  IsSingletonDisj : Singleton (Disj a b Stop)
  IsSingletonAppl : Singleton (Appl f xs Stop)
  IsSingletonIf : Singleton (If c tt ff Stop)
  IsSingletonIter : Singleton (Iter is zs c b Stop)
  IsSingletonBlock : Singleton (Block Stop)

||| Replaces the continuation of *e* with Stop, or returns Stop when *e = Stop*.
head : (e : CExp) -> CExp
head (Decl ty v k) = Decl ty v Stop
head (Assn v e k) = Assn v e Stop
head (Disj a b k) = Disj a b Stop
head (Appl f xs k) = Appl f xs Stop
head (If c tt ff k) = If c tt ff Stop
head (Iter is zs c b k) = Iter is zs c b Stop
head (Block k) = Block Stop
head Stop = Stop

{-
tail : (e : CExp) -> {auto ok : NonEmpty e} -> CExp
tail (Decl ty v k) {ok = IsNonEmptyDecl} = k
tail (Assn v x k) {ok = IsNonEmptyAssn} = k
tail (Disj a b k) {ok = IsNonEmptyDisj} = k
tail (Appl f xs k) {ok = IsNonEmptyAppl} = k
tail (If c tt ff k) {ok = IsNonEmptyIf} = k
tail (Iter is zs c b k) {ok = IsNonEmptyIter} = k
tail (Block k) {ok = IsNonEmptyBlock} = k
-}

||| Returns the continuation of *e*, or Stop when *e = Stop*.
|||
||| **FIXME**: Needs constraining?
tail : (e : CExp) -> CExp
tail (Decl ty v k) = k
tail (Assn v e k) = k
tail (Disj a b k) = k
tail (Appl f xs k) = k
tail (If c tt ff k) = k
tail (Iter is zs c b k) = k
tail (Block k) = k
tail Stop = Stop

infixr 7 ++
||| Appends two continuations.
|||
||| **NB Does not check for well-formedness of result**
|||
||| **FIXME**: Constrain this?
(++) : (e : CExp) -> (e' : CExp) -> CExp
(++) (Decl ty v k) rest = (Decl ty v (k ++ rest))
(++) (Assn v e k) rest = (Assn v e (k ++ rest))
(++) (Disj a b k) rest = (Disj a b (k ++ rest))
(++) (Appl f xs k) rest = (Appl f xs (k ++ rest))
(++) (If c tt ff k) rest = (If c tt ff (k ++ rest))
(++) (Iter is zs c b k) rest = (Iter is zs c b (k ++ rest))
(++) (Block k) rest = (Block (k ++ rest))
(++) Stop g = g
