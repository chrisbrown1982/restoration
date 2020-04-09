module CppDSL

%default total
%access public export

||| Types for PThread operations & variables.
data PThreadTy : Type where
  ThreadT : PThreadTy
  MutexT  : PThreadTy

||| Types
data CTy : Type where
  CBool : CTy
  CInt  : CTy
  CVoid : CTy
  CPtr  : (a : CTy) -> CTy
  ||| PThread types.
  CPThTy : PThreadTy -> CTy

||| PThread operations.
|||
||| Limited to this set for simplicity.
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

||| Expressions.
data CExp : Type where
  ||| Local variable declarations.
  Decl  : (ty : CTy) -> (v : String) -> (k : CExp) -> CExp
  ||| PThread operations.
  PThOp : (op : PThreadOp) -> (k : CExp) -> CExp
  ||| Branching.
  If    : (c : CExp) -> (tt : CExp) -> (ff : CExp) -> (k : CExp) -> CExp
  ||| Looping.
  While : (c : CExp) -> (b : CExp) -> (k : CExp) -> CExp
  ||| Block -- abstraction over multiple expressions/statements.
  |||
  ||| **NB** The real thing will need enough information to get back to a more 
  ||| detailed model.
  Block : (varsUsed : List String) -> (k : CExp) -> CExp
  ||| End of continuation.
  Stop  : CExp

||| Global declarations.
data CDecl : Type where
  ||| Global variable declaration.
  VarDec : (ty : CTy) -> (v : String) -> (e : CExp) -> (k : CDecl) -> CDecl
  ||| Function declaration.
  FunDec : (ty : CTy) -> (f : String) -> (xs : List (CTy,String)) -> (e : CExp) -> (k : CDecl) -> CDecl
  ||| End of continuation.
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
      No contra => No (\Refl => contra Refl)
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

