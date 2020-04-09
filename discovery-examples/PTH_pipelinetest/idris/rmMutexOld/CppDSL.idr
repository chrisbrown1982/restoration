module CppDSL

%access public export
%default total

||| Types for PThread operations & variables.
data PThreadTy : Type where
  ThreadT : PThreadTy
  MutexT  : PThreadTy

||| Types
data CTy : Type where
  CBool : CTy
  CInt  : CTy
  CVoid : CTy
  ||| a*
  ||| Should perhaps assume this can only work on the above three?
  CPtr  : (a : CTy) -> CTy
  ||| PThread types.
  CPThTy : PThreadTy -> CTy

||| Unary operators
data CUOp : Type where
  ||| !e
  CNot : CUOp
  ||| -e
  Neg : CUOp
  ||| (ty)e
  Cast : CTy -> CUOp
  ||| *e
  Deref : CUOp

||| Binary operators
data CBOp : Type where
  LT : CBOp
  GT : CBOp
  EQ : CBOp
  LTE : CBOp
  GTE : CBOp
  Plus : CBOp
  Minus : CBOp
  Multiply : CBOp

||| PThread operations.
|||
||| Explicitly represented to simplify the model & semantics.
||| We additionally assume that the arguments to each of the operations are 
||| variables/pointers.
data PThreadOp : Type where
  ||| pthread_create(&t, NULL, f, x)
  ||| f & x must be pointers, assume t, f, & x variables for simplicity.
  CreateNull : (t : String) -> (f : String) -> (x : String) -> PThreadOp
  ||| pthread_mutex_lock(&m), where m is a variable.
  ||| Assume for now the `&` as part of the operation.
  MutexLock : (m : String) -> PThreadOp
  ||| pthread_mutex_unlock(&m), where m is a variable.
  ||| Assume for now the `&` as part of the operation.
  MutexUnlock : (m : String) -> PThreadOp
  ||| pthread_join(t, NULL);
  |||
  ||| N.B. Will need to worry about non-null return types later.
  JoinNull : (t : String) -> PThreadOp
  ||| pthread_exit(NULL);
  |||
  ||| N.B. Will need to worry about non-null return types later. 
  ExitNull : PThreadOp



||| IO Operators
data CIOOp : Type where
  COut : CIOOp

||| Expressions.
||| Possibly mixing statements and expressions here. Room for simplification?
||| 
||| Assumptions:
||| - Restricting numbers to Integers
||| - Integers represented as Nats since Int & Nat have same cardinality
||| - Names are unique
||| - Assume no aliasing.
|||
||| Notes:
||| - Modelling the PThread library as primitive operations makes this less 
|||   extensible, but serves as a better/simpler starting point.
data CExp : Type where
  ||| Literal booleans.
  Bool   : Bool -> CExp -> CExp
  ||| Literal integers, modelled as natural numbers (Odds: +, Evens: -).
  Num    : Nat -> CExp -> CExp
  ||| Literal strings.
  Str    : (s : String) -> (k : CExp) -> CExp
  ||| Variables, modeled as strings (maybe fix this later).
  Var    : String -> CExp -> CExp

  -- Looping & Branching

  ||| If statement; c is the condition, kt and kf are true and false branches.
  If     : (c : CExp) -> (kt : CExp) -> (kf : CExp) -> (k : CExp) -> CExp
  ||| While loop; c is the condition and kt is the body.
  While  : (c : CExp) -> (kt : CExp) -> (k : CExp) -> CExp
  ||| For loop; assume the form:
  |||   for (i = 0; c; i++) { kt }
  ||| where i is already declared.
  |||
  ||| For loops will be rewritten/interpreted as while loops. (Remove?)
  For    : (i : String) -> (c : CExp) -> (kt : CExp) -> (k : CExp) -> CExp

  -- Functions & operators

  ||| Unary operations.
  UniOp  : (op : CUOp) -> (e : CExp) -> (k : CExp) -> CExp
  ||| Binary operations.
  BinOp  : CBOp -> CExp -> CExp -> CExp -> CExp
  ||| Basic input/output operations.
  ||| Assume `<<` of COut cannot be chained.
  CIO    : (op : CIOOp) -> (e : CExp) -> (k : CExp) -> CExp
  ||| Function application. We assume that the function applied is a variable.
  App    : String -> CExp -> CExp -> CExp
  ||| PThread operation.
  PThOp  : PThreadOp -> CExp -> CExp
  ||| Return expression.
  Return : CExp -> CExp -> CExp

  -- Declarations & assignment

  ||| Variable declaration, no initialisation.
  Decl   : CTy -> String -> CExp -> CExp
  ||| Variable assignment.
  Assign : String -> CExp -> CExp -> CExp
  ||| *v = e
  AssPtr : (v : String) -> (e : CExp) -> (k : CExp) -> CExp

  -- Convenience.

  ||| End of a continuation.
  Stop : CExp

data CArg : Type where
  ||| An argument declaration to a function.
  ||| ty x, k
  Arg : (ty : CTy) -> (x : String) -> (k : CArg) -> CArg
  ||| End of a continuation.
  Arrete : CArg

data CDecl : Type where
  ||| Global variable declaration.
  ||| ty v = e; k
  GVar : (ty : CTy) -> (v : String) -> (e : CExp) -> (k : CDecl) -> CDecl
  ||| Function declaration.
  ||| ty f(xs) { e }; k
  Fun : (ty : CTy) -> (f : String) -> (xs : CArg) -> (e : CExp) -> (k : CDecl) -> CDecl
  ||| End of a continuation.
  Quit : CDecl

-- ----------------------------------------------------------------------------
-- Implementations
-- ----------------------------------------------------------------------------

implementation Uninhabited (CBool = CInt) where
  uninhabited Refl impossible
implementation Uninhabited (CBool = CVoid) where
  uninhabited Refl impossible
implementation Uninhabited (CBool = (CPtr t2)) where
  uninhabited Refl impossible
implementation Uninhabited (CBool = (CPThTy ThreadT)) where
  uninhabited Refl impossible
implementation Uninhabited (CBool = (CPThTy MutexT)) where
  uninhabited Refl impossible
implementation Uninhabited (CInt = CVoid) where
  uninhabited Refl impossible
implementation Uninhabited (CInt = (CPtr t2)) where
  uninhabited Refl impossible
implementation Uninhabited (CInt = (CPThTy ThreadT)) where
  uninhabited Refl impossible
implementation Uninhabited (CInt = (CPThTy MutexT)) where
  uninhabited Refl impossible
implementation Uninhabited (CVoid = (CPtr t2)) where
  uninhabited Refl impossible
implementation Uninhabited (CVoid = (CPThTy ThreadT)) where
  uninhabited Refl impossible
implementation Uninhabited (CVoid = (CPThTy MutexT)) where
  uninhabited Refl impossible
implementation Uninhabited ((CPtr t1) = (CPThTy ThreadT)) where
  uninhabited Refl impossible
implementation Uninhabited ((CPtr t1) = (CPThTy MutexT)) where
  uninhabited Refl impossible
implementation Uninhabited ((CPThTy ThreadT) = (CPThTy MutexT)) where
  uninhabited Refl impossible

lemmaCPtrT1T2NotSame : (contra : (t1 = t2) -> Void) -> (CPtr t1 = CPtr t2) -> Void
lemmaCPtrT1T2NotSame contra Refl = contra Refl

implementation DecEq CTy where
  decEq CBool CBool = Yes Refl
  decEq CBool CInt = No absurd
  decEq CBool CVoid = No absurd
  decEq CBool (CPtr t2) = No absurd
  decEq CBool (CPThTy ThreadT) = No absurd
  decEq CBool (CPThTy MutexT) = No absurd
  decEq CInt CBool = No (\prf => absurd (sym prf))
  decEq CInt CInt = Yes Refl
  decEq CInt CVoid = No absurd
  decEq CInt (CPtr t2) = No absurd
  decEq CInt (CPThTy ThreadT) = No absurd
  decEq CInt (CPThTy MutexT) = No absurd
  decEq CVoid CBool = No (\prf => absurd (sym prf))
  decEq CVoid CInt = No (\prf => absurd (sym prf))
  decEq CVoid CVoid = Yes Refl
  decEq CVoid (CPtr t2) = No absurd
  decEq CVoid (CPThTy ThreadT) = No absurd
  decEq CVoid (CPThTy MutexT) = No absurd
  decEq (CPtr t1) CBool = No (\prf => absurd (sym prf))
  decEq (CPtr t1) CInt = No (\prf => absurd (sym prf))
  decEq (CPtr t1) CVoid = No (\prf => absurd (sym prf))
  decEq (CPtr t1) (CPtr t2) =
    case decEq t1 t2 of
      Yes Refl => Yes Refl
      No contra => No (lemmaCPtrT1T2NotSame contra)
  decEq (CPtr t1) (CPThTy ThreadT) = No absurd
  decEq (CPtr t1) (CPThTy MutexT) = No absurd
  decEq (CPThTy ThreadT) CBool = No (\prf => absurd (sym prf))
  decEq (CPThTy ThreadT) CInt = No (\prf => absurd (sym prf))
  decEq (CPThTy ThreadT) CVoid = No (\prf => absurd (sym prf))
  decEq (CPThTy ThreadT) (CPtr t2) = No (\prf => absurd (sym prf))
  decEq (CPThTy ThreadT) (CPThTy ThreadT) = Yes Refl
  decEq (CPThTy ThreadT) (CPThTy MutexT) = No absurd
  decEq (CPThTy MutexT) CBool = No (\prf => absurd (sym prf))
  decEq (CPThTy MutexT) CInt = No (\prf => absurd (sym prf))
  decEq (CPThTy MutexT) CVoid = No (\prf => absurd (sym prf))
  decEq (CPThTy MutexT) (CPtr t2) = No (\prf => absurd (sym prf))
  decEq (CPThTy MutexT) (CPThTy ThreadT) = No (\prf => absurd (sym prf))
  decEq (CPThTy MutexT) (CPThTy MutexT) = Yes Refl
  