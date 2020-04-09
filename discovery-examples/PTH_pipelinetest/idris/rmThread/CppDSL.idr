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

implementation Uninhabited ((CreateNull t f x) = (MutexLock m)) where
  uninhabited Refl impossible
implementation Uninhabited ((CreateNull t f x) = (MutexUnlock m)) where
  uninhabited Refl impossible
implementation Uninhabited ((CreateNull t f x) = (JoinNull y)) where
  uninhabited Refl impossible
implementation Uninhabited ((CreateNull t f x) = ExitNull) where
  uninhabited Refl impossible
implementation Uninhabited ((MutexLock m) = (MutexUnlock n)) where
  uninhabited Refl impossible
implementation Uninhabited ((MutexLock m) = (JoinNull t)) where
  uninhabited Refl impossible
implementation Uninhabited ((MutexLock m) = ExitNull) where
  uninhabited Refl impossible
implementation Uninhabited ((MutexUnlock m) = (JoinNull t)) where
  uninhabited Refl impossible
implementation Uninhabited ((MutexUnlock m) = ExitNull) where
  uninhabited Refl impossible
implementation Uninhabited ((JoinNull t) = ExitNull) where
  uninhabited Refl impossible

lemmaCreateNullFNotEq : (contra : (f = z) -> Void) -> (CreateNull t f x = CreateNull t z w) -> Void
lemmaCreateNullFNotEq contra Refl = contra Refl
lemmaCreateNullTNotEq : (contra : (t = y) -> Void) -> (CreateNull t f x = CreateNull y z w) -> Void
lemmaCreateNullTNotEq contra Refl = contra Refl

implementation DecEq PThreadOp where
  decEq (CreateNull t f x) (CreateNull y z w) =
    case decEq t y of
      Yes Refl =>
        case decEq f z of
          Yes Refl =>
            case decEq x w of
              Yes Refl => Yes Refl
              No contra => No (\Refl => contra Refl)
          No contra => No (lemmaCreateNullFNotEq contra)
      No contra => No (lemmaCreateNullTNotEq contra)
  decEq (CreateNull t f x) (MutexLock m) = No absurd
  decEq (CreateNull t f x) (MutexUnlock m) = No absurd
  decEq (CreateNull t f x) (JoinNull y) = No absurd
  decEq (CreateNull t f x) ExitNull = No absurd
  decEq (MutexLock m) (CreateNull t f x) = No (\prf => absurd (sym prf))
  decEq (MutexLock m) (MutexLock x) with (decEq m x)
    decEq (MutexLock x) (MutexLock x) | Yes Refl = Yes Refl
    decEq (MutexLock m) (MutexLock x) | No contra = No (\Refl => contra Refl)
  decEq (MutexLock m) (MutexUnlock x) = No absurd
  decEq (MutexLock m) (JoinNull t) = No absurd
  decEq (MutexLock m) ExitNull = No absurd
  decEq (MutexUnlock m) (CreateNull t f x) = No (\prf => absurd (sym prf))
  decEq (MutexUnlock m) (MutexLock x) = No (\prf => absurd (sym prf))
  decEq (MutexUnlock m) (MutexUnlock x) with (decEq m x)
    decEq (MutexUnlock x) (MutexUnlock x) | Yes Refl = Yes Refl
    decEq (MutexUnlock m) (MutexUnlock x) | No contra =
      No (\Refl => contra Refl)
  decEq (MutexUnlock m) (JoinNull t) = No absurd
  decEq (MutexUnlock m) ExitNull = No absurd
  decEq (JoinNull t) (CreateNull x f y) = No (\prf => absurd (sym prf))
  decEq (JoinNull t) (MutexLock m) = No (\prf => absurd (sym prf))
  decEq (JoinNull t) (MutexUnlock m) = No (\prf => absurd (sym prf))
  decEq (JoinNull t) (JoinNull x) with (decEq t x)
    decEq (JoinNull x) (JoinNull x) | Yes Refl = Yes Refl
    decEq (JoinNull t) (JoinNull x) | No contra = No (\Refl => contra Refl)
  decEq (JoinNull t) ExitNull = No absurd
  decEq ExitNull (CreateNull t f x) = No (\prf => absurd (sym prf))
  decEq ExitNull (MutexLock m) = No (\prf => absurd (sym prf))
  decEq ExitNull (MutexUnlock m) = No (\prf => absurd (sym prf))
  decEq ExitNull (JoinNull t) = No (\prf => absurd (sym prf))
  decEq ExitNull ExitNull = Yes Refl

implementation Show CTy where
  show CBool = "bool"
  show CInt = "int"
  show CVoid = "void"
  show (CPtr t) = "*" ++ show t
  show (CPThTy ThreadT) = "pthread_t"
  show (CPThTy MutexT) = "pthread_mutex_t"

-- ----------------------------------------------------------------------------
-- Pretty Printing
-- ----------------------------------------------------------------------------

interp : Nat -> String
interp Z = ""
interp (S k) = " " ++ interp k

mutual
  pprintE : Nat -> CExp -> String
  pprintE ind (Decl ty v k) = interp ind ++ show ty ++ " " ++ v ++ ";\n" ++ pprintE ind k
  pprintE ind (PThOp (CreateNull t f x) k) =
    interp ind ++ "pthread_create(" ++ t ++ ", NULL, " ++ f ++ ", " ++ x ++ ");\n" ++ pprintE ind k
  pprintE ind (PThOp (MutexLock m) k) =
    interp ind ++ "pthread_mutex_lock(" ++ m ++ ");\n" ++ pprintE ind k
  pprintE ind (PThOp (MutexUnlock m) k) =
    interp ind ++ "pthread_mutex_unlock(" ++ m ++ ");\n" ++ pprintE ind k
  pprintE ind (PThOp (JoinNull t) k) =
    interp ind ++ "pthread_join(" ++ t ++ ", NULL);\n" ++ pprintE ind k
  pprintE ind (PThOp ExitNull k) =
    interp ind ++ "pthread_exit(NULL);\n" ++ pprintE ind k
  pprintE ind (If c tt ff k) =
    interp ind ++ "if (" ++ pprintEInline c ++ ") {\n" ++
      pprintE (ind + 2) tt ++ "\n" ++ interp ind ++ "} {" ++
      pprintE (ind + 2) ff ++ "\n" ++ interp ind ++ "}\n" ++ pprintE ind k
  pprintE ind (While c b k) =
    interp ind ++ "while (" ++ pprintEInline c ++ ") {\n" ++
      pprintE (ind + 2) b ++ "\n" ++ interp ind ++ "}\n" ++ pprintE ind k
  pprintE ind (Block varsUsed k) = interp ind ++ "...\n" ++ pprintE ind k
  pprintE ind Stop = ""

  pprintEInline : CExp -> String
  pprintEInline e =
    let ls = lines (pprintE 0 e)
    in foldr (++) " " ls

pprintD : CDecl -> String
pprintD (VarDec ty v Stop k) = (show ty) ++ " " ++ v ++ ";\n" ++ pprintD k
pprintD (VarDec ty v e k) =
  (show ty) ++ " " ++ v ++ " = " ++ pprintEInline e ++ ";\n" ++ pprintD k
pprintD (FunDec ty f xs e k) =
    let args = foldr fix "" xs
    in (show ty) ++ " " ++ f ++ "(" ++ args ++ ") {\n" ++ pprintE 2 e ++ "};\n" ++ pprintD k
  where
    fix : (CTy, String) -> String -> String
    fix (ty,n) acc = show ty ++ " " ++ n ++ ", " ++ acc
pprintD Quit = "\n"

pprint : CDecl -> IO ()
pprint p = do
  putStrLn (pprintD p)