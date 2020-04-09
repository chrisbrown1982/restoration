module RemoveMutex

import CppDSL
import TestCPP

%default total

||| View of the location of mutexes, declared as global variables.
|||
||| We assume for now that mutexes are *only* declared as global variables, but 
||| the principle can be extended to other locations (e.g. local declaration 
||| and passing to threads via structs).
data MutLoc : CDecl -> Type where
  HereGVar  : MutLoc (GVar (CPThTy MutexT) m e k)
  ThereGVar : MutLoc k -> MutLoc (GVar t v e k)
  ThereFun  : MutLoc k -> MutLoc (Fun t f xs e k)

implementation Uninhabited (MutLoc Quit) where
  uninhabited HereGVar impossible
  uninhabited (ThereGVar _) impossible
  uninhabited (ThereFun _) impossible

mutNeitherHereNorThere : (mutNotInK : MutLoc k -> Void) -> (tyNotMutexT : (ty = CPThTy MutexT) -> Void) -> MutLoc (GVar ty v e k) -> Void
mutNeitherHereNorThere mutNotInK tyNotMutexT HereGVar = tyNotMutexT Refl
mutNeitherHereNorThere mutNotInK tyNotMutexT (ThereGVar x) = mutNotInK x

mutNeitherHereNorThereFun : (mutNotInK : MutLoc k -> Void) -> MutLoc (Fun ty f xs e k) -> Void
mutNeitherHereNorThereFun mutNotInK (ThereFun x) = mutNotInK x

||| Finds the first mutex declaration (if it exists).
findMutex : (p : CDecl) -> Dec (MutLoc p)
findMutex (GVar ty v e k) =
  case decEq ty (CPThTy MutexT) of
    Yes Refl => Yes HereGVar
    No tyNotMutexT =>
      case findMutex k of
        Yes prfMutInK => Yes (ThereGVar prfMutInK)
        No mutNotInK => No (mutNeitherHereNorThere mutNotInK tyNotMutexT)
findMutex (Fun ty f xs e k) =
  case findMutex k of
    Yes prfMutInK => Yes (ThereFun prfMutInK)
    No mutNotInK => No (mutNeitherHereNorThereFun mutNotInK)
findMutex Quit = No absurd

findMutexName : MutLoc p -> String
findMutexName {p = (GVar (CPThTy MutexT) m e k)} HereGVar = m
findMutexName {p = (GVar t v e k)} (ThereGVar x) = findMutexName x
findMutexName {p = (Fun t f xs e k)} (ThereFun x) = findMutexName x

||| **FIXME**: should produce a Subexpression relation that demonstrates the output 
||| is a subexpression of p
findMutexDecContinuation : MutLoc p -> CDecl
findMutexDecContinuation {p = (GVar (CPThTy MutexT) m e k)} HereGVar = k
findMutexDecContinuation {p = (GVar t v e k)} (ThereGVar x) = findMutexDecContinuation x
findMutexDecContinuation {p = (Fun t f xs e k)} (ThereFun x) = findMutexDecContinuation x

||| Relation denoting that *v* appears as a subexpression in e. *v* may appear 
||| in literal variable, for-loops, function application, assignment, and 
||| pthread expressions. *v* will not appear in a *Decl* expression since *v* 
||| has already been declared and we assume unique names.
|||
||| As an existence proof, only the first instance is pointed at.
|||
||| This is effectively a subexpression relation, but specialised for variable 
||| names. (I suppose it would be okay to assume strings are subexpressions? 
||| For now, though, simple is best.)
|||
||| **FIXME**: variables should be changed to carry their type (or we should 
||| include some sort of context); this will aid well-formedness & simplify the 
||| *VarUsed* relation.
data VarUsed : (v : String) -> (e : CExp) -> Type where
  HereLit : VarUsed v (Var v k)
  HereFor : VarUsed v (For v c kt k)
  HereApp : VarUsed v (App v xs k)
  HereAss : VarUsed v (Assign v e k)
  HereAssPtr : VarUsed v (AssPtr v e k)
  HerePThCreateT : VarUsed v (PThOp (CreateNull v f x) k)
  HerePThCreateF : VarUsed v (PThOp (CreateNull t v x) k)
  HerePThCreateX : VarUsed v (PThOp (CreateNull t f v) k)
  HerePThMutexL  : VarUsed v (PThOp (MutexLock v) k)
  HerePThMutexU  : VarUsed v (PThOp (MutexUnlock v) k)
  HerePThJoin    : VarUsed v (PThOp (JoinNull v) k)

  ThereBool : VarUsed v k -> VarUsed v (Bool b k)
  ThereNum  : VarUsed v k -> VarUsed v (Num n k)
  ThereStr  : VarUsed v k -> VarUsed v (Str s k)
  ThereVar  : VarUsed v k -> VarUsed v (Var w k)
  ThereIfC  : VarUsed v c  -> VarUsed v (If c kt kf k)
  ThereIfKT : VarUsed v kt -> VarUsed v (If c kt kf k)
  ThereIfKF : VarUsed v kf -> VarUsed v (If c kt kf k)
  ThereIfK  : VarUsed v k  -> VarUsed v (If c kt kf k)
  ThereWhileC  : VarUsed v c  -> VarUsed v (While c kt k)
  ThereWhileKT : VarUsed v kt -> VarUsed v (While c kt k)
  ThereWhileK  : VarUsed v k  -> VarUsed v (While c kt k)
  ThereForC  : VarUsed v c  -> VarUsed v (For i c kt k)
  ThereForKT : VarUsed v kt -> VarUsed v (For i c kt k)
  ThereForK  : VarUsed v k  -> VarUsed v (For i c kt k)
  ThereUniOpE : VarUsed v e  -> VarUsed v (UniOp op e k)
  ThereUniOpK : VarUsed v k  -> VarUsed v (UniOp op e k)
  ThereBinOpL : VarUsed v l -> VarUsed v (BinOp op l r k)
  ThereBinOpR : VarUsed v r -> VarUsed v (BinOp op l r k)
  ThereBinOpK : VarUsed v k -> VarUsed v (BinOp op l r k)
  ThereCIOE : VarUsed v e -> VarUsed v (CIO op e k)
  ThereCIOK : VarUsed v k -> VarUsed v (CIO op e k)
  ThereAppXs : VarUsed v xs -> VarUsed v (App f xs k)
  ThereAppK  : VarUsed v k  -> VarUsed v (App f xs k)
  TherePThOp : VarUsed v k  -> VarUsed v (PThOp op k)
  ThereReturnE : VarUsed v e -> VarUsed v (Return e k)
  ThereReturnK : VarUsed v k -> VarUsed v (Return e k)
  ThereDecl : VarUsed v k -> VarUsed v (Decl ty w k)
  ThereAssignE : VarUsed v e -> VarUsed v (Assign w e k)
  ThereAssignK : VarUsed v k -> VarUsed v (Assign w e k)
  ThereAssPtrE : VarUsed v e -> VarUsed v (AssPtr w e k)
  ThereAssPtrK : VarUsed v k -> VarUsed v (AssPtr w e k)

implementation Uninhabited (VarUsed v Stop) where
  uninhabited HereLit impossible
  uninhabited HereFor impossible
  uninhabited HereApp impossible
  uninhabited HereAss impossible
  uninhabited HereAssPtr impossible
  uninhabited HerePThCreateT impossible
  uninhabited HerePThCreateF impossible
  uninhabited HerePThCreateX impossible
  uninhabited HerePThMutexL impossible
  uninhabited HerePThMutexU impossible
  uninhabited HerePThJoin impossible
  uninhabited ThereBool impossible
  uninhabited ThereNum impossible
  uninhabited ThereStr impossible
  uninhabited ThereVar impossible
  uninhabited ThereIfC impossible
  uninhabited ThereIfKT impossible
  uninhabited ThereIfKF impossible
  uninhabited ThereIfK impossible
  uninhabited ThereWhileC impossible
  uninhabited ThereWhileKT impossible
  uninhabited ThereWhileK impossible
  uninhabited ThereForC impossible
  uninhabited ThereForKT impossible
  uninhabited ThereForK impossible
  uninhabited ThereUniOpE impossible
  uninhabited ThereUniOpK impossible
  uninhabited ThereBinOpL impossible
  uninhabited ThereBinOpR impossible
  uninhabited ThereBinOpK impossible
  uninhabited ThereCIOE impossible
  uninhabited ThereCIOK impossible
  uninhabited ThereAppXs impossible
  uninhabited ThereAppK impossible
  uninhabited TherePThOp impossible
  uninhabited ThereReturnE impossible
  uninhabited ThereReturnK impossible
  uninhabited ThereDecl impossible
  uninhabited ThereAssignE impossible
  uninhabited ThereAssignK impossible
  uninhabited ThereAssPtrE impossible
  uninhabited ThereAssPtrK impossible

lemmaNotThereBool : (contra : VarUsed v k -> Void) -> VarUsed v (Bool x k) -> Void
lemmaNotThereBool contra (ThereBool x) = contra x

lemmaNotThereNum : (contra : VarUsed v k -> Void) -> VarUsed v (Num n k) -> Void
lemmaNotThereNum contra (ThereNum x) = contra x

lemmaNotThereStr : Not (VarUsed v k) -> VarUsed v (Str s k) -> Void
lemmaNotThereStr contra (ThereStr k) = contra k

lemmaNotThereVar : Not (v=x) -> Not (VarUsed v k) -> VarUsed v (Var x k) -> Void
lemmaNotThereVar contraEq contraVU HereLit = contraEq Refl
lemmaNotThereVar contraEq contraVU (ThereVar x) = contraVU x

lemmaNotThereIf : Not (VarUsed v c) -> Not (VarUsed v kt) -> Not (VarUsed v kf) -> Not (VarUsed v k) -> VarUsed v (If c kt kf k) -> Void
lemmaNotThereIf cC cKT cKF cK (ThereIfC k) = cC k
lemmaNotThereIf cC cKT cKF cK (ThereIfKT k) = cKT k
lemmaNotThereIf cC cKT cKF cK (ThereIfKF k) = cKF k
lemmaNotThereIf cC cKT cKF cK (ThereIfK  k) = cK k

lemmaNotThereWhile : Not (VarUsed v c) -> Not (VarUsed v kt) -> Not (VarUsed v k) -> VarUsed v (While c kt k) -> Void
lemmaNotThereWhile cC cKT cK (ThereWhileC prf) = cC prf
lemmaNotThereWhile cC cKT cK (ThereWhileKT prf) = cKT prf
lemmaNotThereWhile cC cKT cK (ThereWhileK prf) = cK prf

lemmaNotThereFor : Not (v=i) -> Not (VarUsed v c) -> Not (VarUsed v kt) -> Not (VarUsed v k) -> VarUsed v (For i c kt k) -> Void
lemmaNotThereFor notHere cC cKT cK HereFor = notHere Refl
lemmaNotThereFor notHere cC cKT cK (ThereForC prf) = cC prf
lemmaNotThereFor notHere cC cKT cK (ThereForKT prf) = cKT prf
lemmaNotThereFor notHere cC cKT cK (ThereForK prf) = cK prf

lemmaNotThereUniOp : Not (VarUsed v e) -> Not (VarUsed v k) -> VarUsed v (UniOp op e k) -> Void
lemmaNotThereUniOp cE cK (ThereUniOpE prf) = cE prf
lemmaNotThereUniOp cE cK (ThereUniOpK prf) = cK prf

lemmaNotThereBinOp : Not (VarUsed v l) -> Not (VarUsed v r) -> Not (VarUsed v k) -> VarUsed v (BinOp op l r k) -> Void
lemmaNotThereBinOp cL cR cK (ThereBinOpL prf) = cL prf
lemmaNotThereBinOp cL cR cK (ThereBinOpR prf) = cR prf
lemmaNotThereBinOp cL cR cK (ThereBinOpK prf) = cK prf

lemmaNotThereCIO : Not (VarUsed v e) -> Not (VarUsed v k) -> VarUsed v (CIO op e k) -> Void
lemmaNotThereCIO cE cK (ThereCIOE prf) = cE prf
lemmaNotThereCIO cE cK (ThereCIOK prf) = cK prf

lemmaNotThereApp : Not (v=f) -> Not (VarUsed v xs) -> Not (VarUsed v k) -> VarUsed v (App f xs k) -> Void
lemmaNotThereApp notHere cXs cK HereApp = notHere Refl
lemmaNotThereApp notHere cXs cK (ThereAppXs prf) = cXs prf
lemmaNotThereApp notHere cXs cK (ThereAppK prf) = cK prf

lemmaNotThereCreateNull : Not (v=t) -> Not (v=f) -> Not (v=x) -> Not (VarUsed v k) -> VarUsed v (PThOp (CreateNull t f x) k) -> Void
lemmaNotThereCreateNull notT notF notX cK HerePThCreateT = notT Refl
lemmaNotThereCreateNull notT notF notX cK HerePThCreateF = notF Refl
lemmaNotThereCreateNull notT notF notX cK HerePThCreateX = notX Refl
lemmaNotThereCreateNull notT notF notX cK (TherePThOp prf) = cK prf

lemmaNotThereMutexLock : Not (v=m) -> Not (VarUsed v k) -> VarUsed v (PThOp (MutexLock m) k) -> Void
lemmaNotThereMutexLock notHere cK HerePThMutexL = notHere Refl
lemmaNotThereMutexLock notHere cK (TherePThOp prf) = cK prf

lemmaNotThereMutexUnlock : Not (v=m) -> Not (VarUsed v k) -> VarUsed v (PThOp (MutexUnlock m) k) -> Void
lemmaNotThereMutexUnlock notHere cK HerePThMutexU = notHere Refl
lemmaNotThereMutexUnlock notHere cK (TherePThOp prf) = cK prf

lemmaNotThereJoinNull : Not (v=t) -> Not (VarUsed v k) -> VarUsed v (PThOp (JoinNull t) k) -> Void
lemmaNotThereJoinNull notHere cK HerePThJoin = notHere Refl
lemmaNotThereJoinNull notHere cK (TherePThOp prf) = cK prf

lemmaNotThereExitNull : Not (VarUsed v k) -> VarUsed v (PThOp ExitNull k) -> Void
lemmaNotThereExitNull cK (TherePThOp prf) = cK prf

lemmaNotThereReturn : Not (VarUsed v e) -> Not (VarUsed v k) -> VarUsed v (Return e k) -> Void
lemmaNotThereReturn cE cK (ThereReturnE prf) = cE prf
lemmaNotThereReturn cE cK (ThereReturnK prf) = cK prf

lemmaNotThereDecl : Not (VarUsed v k) -> VarUsed v (Decl ty s k) -> Void
lemmaNotThereDecl cK (ThereDecl prf) = cK prf

lemmaNotThereAss : Not (v=w) -> Not (VarUsed v e) -> Not (VarUsed v k) -> VarUsed v (Assign w e k) -> Void
lemmaNotThereAss notHere cE cK HereAss = notHere Refl
lemmaNotThereAss notHere cE cK (ThereAssignE prf) = cE prf
lemmaNotThereAss notHere cE cK (ThereAssignK prf) = cK prf

lemmaNotThereAssPtr : Not (v=w) -> Not (VarUsed v e) -> Not (VarUsed v k) -> VarUsed v (AssPtr w e k) -> Void
lemmaNotThereAssPtr notHere cE cK HereAssPtr = notHere Refl
lemmaNotThereAssPtr notHere cE cK (ThereAssPtrE prf) = cE prf
lemmaNotThereAssPtr notHere cE cK (ThereAssPtrK prf) = cK prf

varUsed : (v : String) -> (e : CExp) -> Dec (VarUsed v e)
varUsed v (Bool x k) =
  case varUsed v k of
    Yes prf => Yes (ThereBool prf)
    No contra => No (lemmaNotThereBool contra)
varUsed v (Num n k) =
  case varUsed v k of
    Yes prf => Yes (ThereNum prf)
    No contra => No (lemmaNotThereNum contra)
varUsed v (Str s k) =
  case varUsed v k of
    Yes prf => Yes (ThereStr prf)
    No contra => No (lemmaNotThereStr contra)
varUsed v (Var x k) =
  case decEq v x of
    Yes Refl => Yes HereLit
    No notHere =>
      case varUsed v k of
        Yes prf => Yes (ThereVar prf)
        No contra => No (lemmaNotThereVar notHere contra)
varUsed v (If c kt kf k) =
  case varUsed v c of
    Yes prfC => Yes (ThereIfC prfC)
    No contraC =>
      case varUsed v kt of
        Yes prfKT => Yes (ThereIfKT prfKT)
        No contraKT =>
          case varUsed v kf of
            Yes prfKF => Yes (ThereIfKF prfKF)
            No contraKF =>
              case varUsed v k of
                Yes prfK => Yes (ThereIfK prfK)
                No contraK =>
                  No (lemmaNotThereIf contraC contraKT contraKF contraK)
varUsed v (While c kt k) =
  case varUsed v c of
    Yes prfC => Yes (ThereWhileC prfC)
    No contraC =>
      case varUsed v kt of
        Yes prfKT => Yes (ThereWhileKT prfKT)
        No contraKT =>
          case varUsed v k of
            Yes prfK => Yes (ThereWhileK prfK)
            No contraK => No (lemmaNotThereWhile contraC contraKT contraK)
varUsed v (For i c kt k) =
  case decEq v i of
    Yes Refl => Yes HereFor
    No notHere =>
      case varUsed v c of
        Yes prfC => Yes (ThereForC prfC)
        No contraC =>
          case varUsed v kt of
            Yes prfKT => Yes (ThereForKT prfKT)
            No contraKT =>
              case varUsed v k of
                Yes prfK => Yes (ThereForK prfK)
                No contraK =>
                  No (lemmaNotThereFor notHere contraC contraKT contraK)
varUsed v (UniOp op e k) =
  case varUsed v e of
    Yes prfE => Yes (ThereUniOpE prfE)
    No contraE =>
      case varUsed v k of
        Yes prfK => Yes (ThereUniOpK prfK)
        No contraK => No (lemmaNotThereUniOp contraE contraK)
varUsed v (BinOp op l r k) =
  case varUsed v l of
    Yes prfL => Yes (ThereBinOpL prfL)
    No contraL =>
      case varUsed v r of
        Yes prfR => Yes (ThereBinOpR prfR)
        No contraR =>
          case varUsed v k of
            Yes prfK => Yes (ThereBinOpK prfK)
            No contraK => No (lemmaNotThereBinOp contraL contraR contraK)
varUsed v (CIO op e k) =
  case varUsed v e of
    Yes prfE => Yes (ThereCIOE prfE)
    No contraE =>
      case varUsed v k of
        Yes prfK => Yes (ThereCIOK prfK)
        No contraK => No (lemmaNotThereCIO contraE contraK)
varUsed v (App f xs k) =
  case decEq v f of
    Yes Refl => Yes HereApp
    No notHere =>
      case varUsed v xs of
        Yes prfXs => Yes (ThereAppXs prfXs)
        No contraXs =>
          case varUsed v k of
            Yes prfK => Yes (ThereAppK prfK)
            No contraK =>
              No (lemmaNotThereApp notHere contraXs contraK)
varUsed v (PThOp (CreateNull t f x) k) =
  case decEq v t of
    Yes Refl => Yes HerePThCreateT
    No notT =>
      case decEq v f of
        Yes Refl => Yes HerePThCreateF
        No notF =>
          case decEq v x of
            Yes Refl => Yes HerePThCreateX
            No notX =>
              case varUsed v k of
                Yes prfK => Yes (TherePThOp prfK)
                No contraK =>
                  No (lemmaNotThereCreateNull notT notF notX contraK)
varUsed v (PThOp (MutexLock m) k) =
  case decEq v m of
    Yes Refl => Yes HerePThMutexL
    No notHere =>
      case varUsed v k of
        Yes prfK => Yes (TherePThOp prfK)
        No contraK => No (lemmaNotThereMutexLock notHere contraK)
varUsed v (PThOp (MutexUnlock m) k) =
  case decEq v m of
    Yes Refl => Yes HerePThMutexU
    No notHere =>
      case varUsed v k of
        Yes prfK => Yes (TherePThOp prfK)
        No contraK => No (lemmaNotThereMutexUnlock notHere contraK)
varUsed v (PThOp (JoinNull t) k) =
  case decEq v t of
    Yes Refl => Yes HerePThJoin
    No notHere =>
      case varUsed v k of
        Yes prfK => Yes (TherePThOp prfK)
        No contraK => No (lemmaNotThereJoinNull notHere contraK)
varUsed v (PThOp ExitNull k) =
  case varUsed v k of
    Yes prfK => Yes (TherePThOp prfK)
    No contraK => No (lemmaNotThereExitNull contraK)
varUsed v (Return e k) =
  case varUsed v e of
    Yes prfE => Yes (ThereReturnE prfE)
    No contraE =>
      case varUsed v k of
        Yes prfK => Yes (ThereReturnK prfK)
        No contraK => No (lemmaNotThereReturn contraE contraK)
varUsed v (Decl ty s k) =
  case varUsed v k of
    Yes prfK => Yes (ThereDecl prfK)
    No contraK => No (lemmaNotThereDecl contraK)
varUsed v (Assign w e k) =
  case decEq v w of
    Yes Refl => Yes HereAss
    No notHere =>
      case varUsed v e of
        Yes prfE => Yes (ThereAssignE prfE)
        No contraE =>
          case varUsed v k of
            Yes prfK => Yes (ThereAssignK prfK)
            No contraK =>
              No (lemmaNotThereAss notHere contraE contraK)
varUsed v (AssPtr w e k) =
  case decEq v w of
    Yes Refl => Yes HereAssPtr
    No notHere =>
      case varUsed v e of
        Yes prfE => Yes (ThereAssPtrE prfE)
        No contraE =>
          case varUsed v k of
            Yes prfK => Yes (ThereAssPtrK prfK)
            No contraK =>
              No (lemmaNotThereAssPtr notHere contraE contraK)
varUsed v Stop = No absurd

||| Slice of Expressions.
|||
||| Slices are a view of expressions, declarations, &c.
||| Slicing criteria are limited to variables, where we assume that we are 
||| slicing the continutation of the expression/declaration where the variable 
||| is declared.
data SlCExp : (m : String) -> (e : CExp) -> Type where
  SlVar : SlCExp m k -> SlCExp m (Var m k)

  SlIf : SlCExp m c -> SlCExp m kt -> SlCExp m kf -> SlCExp m k -> SlCExp m (If c kt kf k)
  SlWhile : SlCExp m c -> SlCExp m kt -> SlCExp m k -> SlCExp m (While c kt k)
  SlFor : SlCExp m c -> SlCExp m kt -> SlCExp m k -> SlCExp m (For i c kt k)
  SlUniOp : SlCExp m e -> SlCExp m k -> SlCExp m (UniOp op e k)
  SlBinOp : SlCExp m l -> SlCExp m r -> SlCExp m k -> SlCExp m (BinOp op l r k)
  SlCIO : SlCExp m e -> SlCExp m k -> SlCExp m (CIO op e k)
  SlApp : SlCExp m xs -> SlCExp m k -> SlCExp m (App f xs k)
  SlPThOp : SlCExp m k -> SlCExp m (PThOp op k)
  SlReturn : SlCExp m e -> SlCExp m k -> SlCExp m (Return e k)
  SlAssign : SlCExp m e -> SlCExp m k -> SlCExp m (Assign v e k)
  SlAssPtr : SlCExp m e -> SlCExp m k -> SlCExp m (AssPtr v e k)
  SlStop : SlCExp m Stop

  SlSkipBool : SlCExp m k -> SlCExp m (Bool b k)
  SlSkipNum : SlCExp m k -> SlCExp m (Num n k)
  SlSkipStr : SlCExp m k -> SlCExp m (Str s k)
  SlSkipVar : SlCExp m k -> SlCExp m (Var v k)

  SlSkipIf : SlCExp m k -> SlCExp m (If c kt kf k)
  SlSkipWhile : SlCExp m k -> SlCExp m (While c kt k)
  SlSkipFor : SlCExp m k -> SlCExp m (For i c kt k)
  SlSkipUniOp : SlCExp m k -> SlCExp m (UniOp op e k)
  SlSkipBinOp : SlCExp m k -> SlCExp m (BinOp op l r k)
  SlSkipCIO : SlCExp m k -> SlCExp m (CIO op e k)
  SlSkipApp : SlCExp m k -> SlCExp m (App f xs k)
  SlSkipPThOp : SlCExp m k -> SlCExp m (PThOp op k)
  SlSkipReturn : SlCExp m k -> SlCExp m (Return e k)
  SlSkipDecl : SlCExp m k -> SlCExp m (Decl ty v k)
  SlSkipAssign : SlCExp m k -> SlCExp m (Assign v e k)
  SlSkipAssPtr : SlCExp m k -> SlCExp m (AssPtr v e k)

||| A view on the slice for expressions, *sl*. This should neatly divide the 
||| output of a slice in to two cases: where an expression is kept, and where 
||| an expression is removed from the slice.
|||
||| **FIXME**: work out how to constrain *SlKeep* and *SlSkip* to their 
||| respective sets of constructors. A type-function over *sl* perhaps?
data SlCExpKind : (sl : SlCExp m e) -> Type where
  SlKeep : {sl : SlCExp m e} -> SlCExpKind sl
  SlSkip : {sl : SlCExp m e} -> SlCExpKind sl

mutual
  slCExpKind : (m : String) -> (e : CExp) -> (thisSl : SlCExp m e ** SlCExpKind {sl=thisSl})
  slCExpKind m e with (slCExp m e)
    slCExpKind m (Var m k) | (SlVar slK) =
      let slvar = (SlVar slK) in (slvar ** SlKeep)
    slCExpKind m (If c kt kf k) | (SlIf slC slKT slKF slK) =
      let slret = (SlIf slC slKT slKF slK)
      in (slret ** SlKeep)
    slCExpKind m (While c kt k) | (SlWhile slC slKT slK) =
      let slret = (SlWhile slC slKT slK)
      in (slret ** SlKeep)
    slCExpKind m (For i c kt k) | (SlFor slC slKT slK) =
      let slret = (SlFor slC slKT slK)
      in (slret ** SlKeep)
    slCExpKind m (UniOp op e k) | (SlUniOp slE slK) =
      let slret = (SlUniOp slE slK)
      in (slret ** SlKeep)
    slCExpKind m (BinOp op l r k) | (SlBinOp slL slR slK) =
      let slret = (SlBinOp slL slR slK)
      in (slret ** SlKeep)
    slCExpKind m (CIO op e k) | (SlCIO slE slK) =
      let slret = (SlCIO slE slK)
      in (slret ** SlKeep)
    slCExpKind m (App s e k) | (SlApp slE slK) =
      let slret = (SlApp slE slK)
      in (slret ** SlKeep)
    slCExpKind m (PThOp op k) | (SlPThOp slK) =
      let slret = (SlPThOp slK)
      in (slret ** SlKeep)
    slCExpKind m (Return e k) | (SlReturn slE slK) =
      let slret = (SlReturn slE slK)
      in (slret ** SlKeep)
    slCExpKind m (Assign v e k) | (SlAssign slE slK) =
      let slret = (SlAssign slE slK)
      in (slret ** SlKeep)
    slCExpKind m (AssPtr v e k) | (SlAssPtr slE slK) =
      let slret = (SlAssPtr slE slK)
      in (slret ** SlKeep)
    slCExpKind m Stop | SlStop =
      let slret = SlStop
      in (slret ** SlKeep)
    slCExpKind m (Bool b k) | (SlSkipBool slK) =
      let slret = (SlSkipBool slK)
      in (slret ** SlSkip)
    slCExpKind m (Num n k) | (SlSkipNum slK) =
      let slret = (SlSkipNum slK)
      in (slret ** SlSkip)
    slCExpKind m (Str s k) | (SlSkipStr slK) =
      let slret = (SlSkipStr slK)
      in (slret ** SlSkip)
    slCExpKind m (Var v k) | (SlSkipVar slK) =
      let slret = (SlSkipVar slK)
      in (slret ** SlSkip)
    slCExpKind m (If c kt kf k) | (SlSkipIf slK) =
      let slret = (SlSkipIf slK)
      in (slret ** SlSkip)
    slCExpKind m (While c kt k) | (SlSkipWhile slK) =
      let slret = (SlSkipWhile slK)
      in (slret ** SlSkip)
    slCExpKind m (For i c kt k) | (SlSkipFor slK) =
      let slret = (SlSkipFor slK)
      in (slret ** SlSkip)
    slCExpKind m (UniOp op e k) | (SlSkipUniOp slK) =
      let slret = (SlSkipUniOp slK)
      in (slret ** SlSkip)
    slCExpKind m (BinOp op l r k) | (SlSkipBinOp slK) =
      let slret = (SlSkipBinOp slK)
      in (slret ** SlSkip)
    slCExpKind m (CIO op e k) | (SlSkipCIO slK) =
      let slret = (SlSkipCIO slK)
      in (slret ** SlSkip)
    slCExpKind m (App f xs k) | (SlSkipApp slK) =
      let slret = (SlSkipApp slK)
      in (slret ** SlSkip)
    slCExpKind m (PThOp op k) | (SlSkipPThOp slK) =
      let slret = (SlSkipPThOp slK)
      in (slret ** SlSkip)
    slCExpKind m (Return e k) | (SlSkipReturn slK) =
      let slret = (SlSkipReturn slK)
      in (slret ** SlSkip)
    slCExpKind m (Decl ty v k) | (SlSkipDecl slK) =
      let slret = (SlSkipDecl slK)
      in (slret ** SlSkip)
    slCExpKind m (Assign v e k) | (SlSkipAssign slK) =
      let slret = (SlSkipAssign slK)
      in (slret ** SlSkip)
    slCExpKind m (AssPtr v e k) | (SlSkipAssPtr slK) =
      let slret = (SlSkipAssPtr slK)
      in (slret ** SlSkip)


  ||| **FIXME**: missing Assign case where v=m.
  ||| **FIXME**: what about the case when f=m in App?
  slCExp : (m : String) -> (e : CExp) -> SlCExp m e
  slCExp m (Bool b k) = SlSkipBool (slCExp m k)
  slCExp m (Num n k) = SlSkipNum (slCExp m k)
  slCExp m (Str s k) = SlSkipStr (slCExp m k)
  slCExp m (Var v k) =
    case decEq m v of
      Yes Refl => SlVar (slCExp m k)
      No contra => SlSkipVar (slCExp m k)
  slCExp m (If c kt kf k) =
    case slCExpKind m c of
      (slC ** SlKeep) => SlIf slC (slCExp m kt) (slCExp m kf) (slCExp m k)
      (slC ** SlSkip) =>
        case slCExpKind m kt of
          (slKT ** SlKeep) => SlIf slC slKT (slCExp m kf) (slCExp m k)
          (slKT ** SlSkip) =>
            case slCExpKind m kf of
              (slKF ** SlKeep) => SlIf slC slKT slKF (slCExp m k)
              (slKF ** SlSkip) =>
                case slCExpKind m k of
                  (slK ** SlKeep) => SlIf slC slKT slKF slK
                  (slK ** SlSkip) => SlSkipIf slK
  slCExp m (While c kt k) =
    case slCExpKind m c of
      (slC ** SlKeep) => SlWhile slC (slCExp m kt) (slCExp m k)
      (slC ** SlSkip) =>
        case slCExpKind m kt of
          (slKT ** SlKeep) => SlWhile slC slKT (slCExp m k)
          (slKT ** SlSkip) =>
            case slCExpKind m k of
              (slK ** SlKeep) => SlWhile slC slKT slK
              (slK ** SlSkip) => SlSkipWhile slK
  slCExp m (For i c kt k) =
    case slCExpKind m c of
      (slC ** SlKeep) => SlFor slC (slCExp m kt) (slCExp m k)
      (slC ** SlSkip) =>
        case slCExpKind m kt of
          (slKT ** SlKeep) => SlFor slC slKT (slCExp m k)
          (slKT ** SlSkip) =>
            case slCExpKind m k of
              (slK ** SlKeep) => SlFor slC slKT slK
              (slK ** SlSkip) => SlSkipFor slK
  slCExp m (UniOp op e k) =
    case slCExpKind m e of
      (slE ** SlKeep) => SlUniOp slE (slCExp m k)
      (slE ** SlSkip) =>
        case slCExpKind m k of
          (slK ** SlKeep) => SlUniOp slE slK
          (slK ** SlSkip) => SlSkipUniOp slK
  slCExp m (BinOp op l r  k) =
    case slCExpKind m l of
      (slL ** SlKeep) => SlBinOp slL (slCExp m r) (slCExp m k)
      (slL ** SlSkip) =>
        case slCExpKind m r of
          (slR ** SlKeep) => SlBinOp slL slR (slCExp m k)
          (slR ** SlSkip) =>
            case slCExpKind m k of
              (slK ** SlKeep) => SlBinOp slL slR slK
              (slK ** SlSkip) => SlSkipBinOp slK
  slCExp m (CIO op e k) =
    case slCExpKind m e of
      (slE ** SlKeep) => SlCIO slE (slCExp m k)
      (slE ** SlSkip) =>
        case slCExpKind m k of
          (slK ** SlKeep) => SlCIO slE slK
          (slK ** SlSkip) => SlSkipCIO slK
  slCExp m (App f xs k) =
    case slCExpKind m xs of
      (slXS ** SlKeep) => SlApp slXS (slCExp m k)
      (slXS ** SlSkip) =>
        case slCExpKind m k of
          (slK ** SlKeep) => SlApp slXS slK
          (slK ** SlSkip) => SlSkipApp slK
  slCExp m (PThOp op k) =
    case slCExpKind m k of
      (slK ** SlKeep) => SlPThOp slK
      (slK ** SlSkip) => SlSkipPThOp slK
  slCExp m (PThOp (CreateNull t f x) k) =
    case slCExpKind m k of
      (slK ** SlKeep) => SlPThOp slK
      (slK ** SlSkip) => SlSkipPThOp slK
  slCExp m (PThOp (MutexLock v) k) =
    case decEq m v of
      Yes Refl => SlPThOp (slCExp m k)
      No contra =>
        case slCExpKind m k of
          (slK ** SlKeep) => SlPThOp slK
          (slK ** SlSkip) => SlSkipPThOp slK
  slCExp m (PThOp (MutexUnlock v) k) =
    case decEq m v of
      Yes Refl => SlPThOp (slCExp m k)
      No contra =>
        case slCExpKind m k of
          (slK ** SlKeep) => SlPThOp slK
          (slK ** SlSkip) => SlSkipPThOp slK
  slCExp m (PThOp (JoinNull t) k) =
    case slCExpKind m k of
      (slK ** SlKeep) => SlPThOp slK
      (slK ** SlSkip) => SlSkipPThOp slK
  slCExp m (PThOp ExitNull k) =
    case slCExpKind m k of
      (slK ** SlKeep) => SlPThOp slK
      (slK ** SlSkip) => SlSkipPThOp slK
  slCExp m (Return e k) =
    case slCExpKind m e of
      (slE ** SlKeep) => SlReturn slE (slCExp m k)
      (slE ** SlSkip) =>
        case slCExpKind m k of
          (slK ** SlKeep) => SlReturn slE slK
          (slK ** SlSkip) => SlSkipReturn slK
  slCExp m (Decl ty v k) = SlSkipDecl (slCExp m k)
  slCExp m (Assign v e k) =
    case slCExpKind m e of
      (slE ** SlKeep) => SlAssign slE (slCExp m k)
      (slE ** SlSkip) =>
        case slCExpKind m k of
          (slK ** SlKeep) => SlAssign slE slK
          (slK ** SlSkip) => SlSkipAssign slK
  slCExp m (AssPtr v e k) =
    case slCExpKind m e of
      (slE ** SlKeep) => SlAssPtr slE (slCExp m k)
      (slE ** SlSkip) =>
        case slCExpKind m k of
          (slK ** SlKeep) => SlAssPtr slE slK
          (slK ** SlSkip) => SlSkipAssPtr slK
  slCExp m Stop = SlStop
  -- slCExp m e = ?slCExpRest

||| Slice of Programs.
|||
||| Slices are a view of expressions, declarations, &c.
||| Slicing criteria are limited to variables, where we assume that we are 
||| slicing the continutation of the expression/declaration where the variable 
||| is declared.
data SlCDecl : (m : String) -> (p : CDecl) -> Type where
  ||| *m* is used in *e*. *m* still cannot be prop. equal to *f* or any 
  ||| argument in *xs* since we assume uniqueness of names.
  SlFun : VarUsed m e -> SlCExp m e -> SlCDecl m k -> SlCDecl m (Fun ty f xs e k)
  ||| *m* is neither used in *e* nor declared (impossible due to unique names) 
  ||| in a global variable declaration.
  |||
  ||| Since we assume for now that there's no renaming or aliasing of
  ||| variables, one global variable will not be used in the definition of 
  ||| another. We can assume this (for now) since we're only interested (for 
  ||| now) in mutexes.
  |||
  ||| **N.B.** Have removed 'Not (VarUsed m e) ->' from type to discharge proof 
  ||| obligation due to assumption.
  SlSkipGVar : SlCDecl m k -> SlCDecl m (GVar ty v e k)
  ||| *m* is neither used in *e*, nor *f* or declared in *xs* (impossible due 
  ||| to unique names)
  SlSkipFun : Not (VarUsed m e) -> SlCDecl m k -> SlCDecl m (Fun ty f xs e k)
  ||| End of a continuation.
  SlQuit : SlCDecl m Quit

||| Covering/slicing function for SlCDecl.
|||
||| Slices are a view of expressions, declarations, &c.
slCDecl : (m : String) -> (p : CDecl) -> SlCDecl m p
slCDecl m (GVar ty v e k) = SlSkipGVar (slCDecl m k)
  -- case varUsed m e of
  --   Yes mUsedInE => ?holeMUsedInEOhNoFIXME
  --   No mNotUsedInE => SlSkipVar mNotUsedInE (slCDecl m k)
slCDecl m (Fun ty f xs e k) =
  case varUsed m e of
    Yes mUsedInEFun => SlFun mUsedInEFun (slCExp m e) (slCDecl m k)
    No mNotUsedInEFun => SlSkipFun mNotUsedInEFun (slCDecl m k)
slCDecl m Quit = SlQuit

-- ----------------------------------------------------------------------------
-- Inspecting a slice
-- ----------------------------------------------------------------------------

||| Proof of usage of the mutex *m* anywhere in the slice of expression *e*.
data NonMutexUsageExp : SlCExp m e -> Type where
  MutexUsedHereVar : NonMutexUsageExp (SlVar k)
  MutexUsedThereVar : NonMutexUsageExp k -> NonMutexUsageExp (SlVar k)
  MutexUsedThereIfC : NonMutexUsageExp c -> NonMutexUsageExp (SlIf c kt kf k)
  MutexUsedThereIfKT : NonMutexUsageExp kt -> NonMutexUsageExp (SlIf c kt kf k)
  MutexUsedThereIfKF : NonMutexUsageExp kf ->  NonMutexUsageExp (SlIf c kt kf k)
  MutexUsedThereIfK : NonMutexUsageExp k ->  NonMutexUsageExp (SlIf c kt kf k)
  MutexUsedThereWhileC : NonMutexUsageExp c -> NonMutexUsageExp (SlWhile c kt k)
  MutexUsedThereWhileKT : NonMutexUsageExp kt -> NonMutexUsageExp (SlWhile c kt k)
  MutexUsedThereWhileK : NonMutexUsageExp k ->  NonMutexUsageExp (SlWhile c kt k)
  MutexUsedThereForC : NonMutexUsageExp c -> NonMutexUsageExp (SlFor c kt k)
  MutexUsedThereForKT : NonMutexUsageExp kt -> NonMutexUsageExp (SlFor c kt k)
  MutexUsedThereForK : NonMutexUsageExp k ->  NonMutexUsageExp (SlFor c kt k)
  MutexUsedThereUniOpE : NonMutexUsageExp e -> NonMutexUsageExp (SlUniOp e k)
  MutexUsedThereUniOpK : NonMutexUsageExp k ->  NonMutexUsageExp (SlUniOp e k)
  MutexUsedThereBinOpL : NonMutexUsageExp l -> NonMutexUsageExp (SlBinOp l r k)
  MutexUsedThereBinOpR : NonMutexUsageExp r -> NonMutexUsageExp (SlBinOp l r k)
  MutexUsedThereBinOpK : NonMutexUsageExp k -> NonMutexUsageExp (SlBinOp l r k)
  MutexUsedThereCIOE : NonMutexUsageExp e -> NonMutexUsageExp (SlCIO e k)
  MutexUsedThereCIOK : NonMutexUsageExp k ->  NonMutexUsageExp (SlCIO e k)
  MutexUsedThereAppXS : NonMutexUsageExp xs -> NonMutexUsageExp (SlApp xs k)
  MutexUsedThereAppK : NonMutexUsageExp k ->  NonMutexUsageExp (SlApp xs k)
  MutexUsedTherePThOp : NonMutexUsageExp k ->  NonMutexUsageExp (SlPThOp k)
  MutexUsedThereReturnE : NonMutexUsageExp e -> NonMutexUsageExp (SlReturn e k)
  MutexUsedThereReturnK : NonMutexUsageExp k ->  NonMutexUsageExp (SlReturn e k)
  MutexUsedThereAssignE : NonMutexUsageExp e -> NonMutexUsageExp (SlAssign e k)
  MutexUsedThereAssignK : NonMutexUsageExp k ->  NonMutexUsageExp (SlAssign e k)
  MutexUsedThereAssPtrE : NonMutexUsageExp e -> NonMutexUsageExp (SlAssPtr e k)
  MutexUsedThereAssPtrK : NonMutexUsageExp k ->  NonMutexUsageExp (SlAssPtr e k)
  MutexUsedThereSkipBool : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipBool k)
  MutexUsedThereSkipNum : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipNum k)
  MutexUsedThereSkipStr : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipStr k)
  MutexUsedThereSkipVar : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipVar k)
  MutexUsedThereSkipIf : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipIf k)
  MutexUsedThereSkipWhile : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipWhile k)
  MutexUsedThereSkipFor : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipFor k)
  MutexUsedThereSkipUniOp : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipUniOp k)
  MutexUsedThereSkipBinOp : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipBinOp k)
  MutexUsedThereSkipCIO : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipCIO k)
  MutexUsedThereSkipApp : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipApp k)
  MutexUsedThereSkipPThOp : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipPThOp k)
  MutexUsedThereSkipReturn : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipReturn k)
  MutexUsedThereSkipDecl : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipDecl k)
  MutexUsedThereSkipAssign : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipAssign k)
  MutexUsedThereSkipAssPtr : NonMutexUsageExp k -> NonMutexUsageExp (SlSkipAssPtr k)

implementation Uninhabited (NonMutexUsageExp SlStop) where
  uninhabited MutexUsedHereVar impossible
  uninhabited MutexUsedThereVar impossible
  uninhabited MutexUsedThereIfC impossible
  uninhabited MutexUsedThereIfKT impossible
  uninhabited MutexUsedThereIfKF impossible
  uninhabited MutexUsedThereIfK impossible
  uninhabited MutexUsedThereWhileC impossible
  uninhabited MutexUsedThereWhileKT impossible
  uninhabited MutexUsedThereWhileK impossible
  uninhabited MutexUsedThereForC impossible
  uninhabited MutexUsedThereForKT impossible
  uninhabited MutexUsedThereForK impossible
  uninhabited MutexUsedThereUniOpE impossible
  uninhabited MutexUsedThereUniOpK impossible
  uninhabited MutexUsedThereBinOpL impossible
  uninhabited MutexUsedThereBinOpR impossible
  uninhabited MutexUsedThereBinOpK impossible
  uninhabited MutexUsedThereCIOE impossible
  uninhabited MutexUsedThereCIOK impossible
  uninhabited MutexUsedThereAppXS impossible
  uninhabited MutexUsedThereAppK impossible
  uninhabited MutexUsedTherePThOp impossible
  uninhabited MutexUsedThereReturnE impossible
  uninhabited MutexUsedThereReturnK impossible
  uninhabited MutexUsedThereAssignE impossible
  uninhabited MutexUsedThereAssignK impossible
  uninhabited MutexUsedThereAssPtrE impossible
  uninhabited MutexUsedThereAssPtrK impossible
  uninhabited MutexUsedThereSkipBool impossible
  uninhabited MutexUsedThereSkipNum impossible
  uninhabited MutexUsedThereSkipStr impossible
  uninhabited MutexUsedThereSkipVar impossible
  uninhabited MutexUsedThereSkipIf impossible
  uninhabited MutexUsedThereSkipWhile impossible
  uninhabited MutexUsedThereSkipFor impossible
  uninhabited MutexUsedThereSkipUniOp impossible
  uninhabited MutexUsedThereSkipBinOp impossible
  uninhabited MutexUsedThereSkipCIO impossible
  uninhabited MutexUsedThereSkipApp impossible
  uninhabited MutexUsedThereSkipPThOp impossible
  uninhabited MutexUsedThereSkipReturn impossible
  uninhabited MutexUsedThereSkipDecl impossible
  uninhabited MutexUsedThereSkipAssign impossible
  uninhabited MutexUsedThereSkipAssPtr impossible

lemmaNonMutexUsageExpIf : Not (NonMutexUsageExp c) -> Not (NonMutexUsageExp kt) -> Not (NonMutexUsageExp kf) -> Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlIf c kt kf k) -> Void
lemmaNonMutexUsageExpIf notC notKT notKF notK (MutexUsedThereIfC prf) = notC prf
lemmaNonMutexUsageExpIf notC notKT notKF notK (MutexUsedThereIfKT prf) = notKT prf
lemmaNonMutexUsageExpIf notC notKT notKF notK (MutexUsedThereIfKF prf) = notKF prf
lemmaNonMutexUsageExpIf notC notKT notKF notK (MutexUsedThereIfK prf) = notK prf

lemmaNonMutexUsageExpWhile : Not (NonMutexUsageExp c) -> Not (NonMutexUsageExp kt) -> Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlWhile c kt k) -> Void
lemmaNonMutexUsageExpWhile notC notKT notK (MutexUsedThereWhileC prf) = notC prf
lemmaNonMutexUsageExpWhile notC notKT notK (MutexUsedThereWhileKT prf) = notKT prf
lemmaNonMutexUsageExpWhile notC notKT notK (MutexUsedThereWhileK prf) = notK prf

lemmaNonMutexUsageExpFor : Not (NonMutexUsageExp c) -> Not (NonMutexUsageExp kt) -> Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlFor c kt k) -> Void
lemmaNonMutexUsageExpFor notC notKT notK (MutexUsedThereForC prf) = notC prf
lemmaNonMutexUsageExpFor notC notKT notK (MutexUsedThereForKT prf) = notKT prf
lemmaNonMutexUsageExpFor notC notKT notK (MutexUsedThereForK prf) = notK prf

lemmaNonMutexUsageExpUniOp : Not (NonMutexUsageExp e) -> Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlUniOp e k) -> Void
lemmaNonMutexUsageExpUniOp notE notK (MutexUsedThereUniOpE prf) = notE prf
lemmaNonMutexUsageExpUniOp notE notK (MutexUsedThereUniOpK prf) = notK prf

lemmaNonMutexUsageExpBinOp : Not (NonMutexUsageExp l) -> Not (NonMutexUsageExp r) -> Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlBinOp l r k) -> Void
lemmaNonMutexUsageExpBinOp notL notR notK (MutexUsedThereBinOpL prf) = notL prf
lemmaNonMutexUsageExpBinOp notL notR notK (MutexUsedThereBinOpR prf) = notR prf
lemmaNonMutexUsageExpBinOp notL notR notK (MutexUsedThereBinOpK prf) = notK prf

lemmaNonMutexUsageExpCIO : Not (NonMutexUsageExp e) -> Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlCIO e k) -> Void
lemmaNonMutexUsageExpCIO notE notK (MutexUsedThereCIOE prf) = notE prf
lemmaNonMutexUsageExpCIO notE notK (MutexUsedThereCIOK prf) = notK prf

lemmaNonMutexUsageExpApp : Not (NonMutexUsageExp e) -> Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlApp e k) -> Void
lemmaNonMutexUsageExpApp notE notK (MutexUsedThereAppXS prf) = notE prf
lemmaNonMutexUsageExpApp notE notK (MutexUsedThereAppK prf) = notK prf

lemmaNonMutexUsageExpPThOp : Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlPThOp k) -> Void
lemmaNonMutexUsageExpPThOp notK (MutexUsedTherePThOp prf) = notK prf

lemmaNonMutexUsageExpReturn : Not (NonMutexUsageExp e) -> Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlReturn e k) -> Void
lemmaNonMutexUsageExpReturn notE notK (MutexUsedThereReturnE prf) = notE prf
lemmaNonMutexUsageExpReturn notE notK (MutexUsedThereReturnK prf) = notK prf

lemmaNonMutexUsageExpAssign : Not (NonMutexUsageExp e) -> Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlAssign e k) -> Void
lemmaNonMutexUsageExpAssign notE notK (MutexUsedThereAssignE prf) = notE prf
lemmaNonMutexUsageExpAssign notE notK (MutexUsedThereAssignK prf) = notK prf

lemmaNonMutexUsageExpAssPtr : Not (NonMutexUsageExp e) -> Not (NonMutexUsageExp k) -> NonMutexUsageExp (SlAssPtr e k) -> Void
lemmaNonMutexUsageExpAssPtr notE notK (MutexUsedThereAssPtrE prf) = notE prf
lemmaNonMutexUsageExpAssPtr notE notK (MutexUsedThereAssPtrK prf) = notK prf


nonMutexUsageExp : (sl : SlCExp m e) -> Dec (NonMutexUsageExp sl)
nonMutexUsageExp (SlVar k) = Yes MutexUsedHereVar
nonMutexUsageExp (SlIf c kt kf k) =
  case nonMutexUsageExp c of
    Yes prfC => Yes (MutexUsedThereIfC prfC)
    No notC =>
      case nonMutexUsageExp kt of
        Yes prfKT => Yes (MutexUsedThereIfKT prfKT)
        No notKT =>
          case nonMutexUsageExp kf of
            Yes prfKF => Yes (MutexUsedThereIfKF prfKF)
            No notKF =>
              case nonMutexUsageExp k of
                  Yes prfK => Yes (MutexUsedThereIfK prfK)
                  No notK => No (lemmaNonMutexUsageExpIf notC notKT notKF notK)
nonMutexUsageExp (SlWhile c kt k) =
  case nonMutexUsageExp c of
    Yes prfC => Yes (MutexUsedThereWhileC prfC)
    No notC =>
      case nonMutexUsageExp kt of
        Yes prfKT => Yes (MutexUsedThereWhileKT prfKT)
        No notKT =>
          case nonMutexUsageExp k of
              Yes prfK => Yes (MutexUsedThereWhileK prfK)
              No notK => No (lemmaNonMutexUsageExpWhile notC notKT notK)
nonMutexUsageExp (SlFor c kt k) =
  case nonMutexUsageExp c of
    Yes prfC => Yes (MutexUsedThereForC prfC)
    No notC =>
      case nonMutexUsageExp kt of
        Yes prfKT => Yes (MutexUsedThereForKT prfKT)
        No notKT =>
          case nonMutexUsageExp k of
              Yes prfK => Yes (MutexUsedThereForK prfK)
              No notK => No (lemmaNonMutexUsageExpFor notC notKT notK)
nonMutexUsageExp (SlUniOp e k) =
  case nonMutexUsageExp e of
    Yes prfE => Yes (MutexUsedThereUniOpE prfE)
    No notE =>
      case nonMutexUsageExp k of
          Yes prfK => Yes (MutexUsedThereUniOpK prfK)
          No notK => No (lemmaNonMutexUsageExpUniOp notE notK)
nonMutexUsageExp (SlBinOp l r k) =
  case nonMutexUsageExp l of
    Yes prfL => Yes (MutexUsedThereBinOpL prfL)
    No notL =>
      case nonMutexUsageExp r of
        Yes prfR => Yes (MutexUsedThereBinOpR prfR)
        No notR =>
          case nonMutexUsageExp k of
              Yes prfK => Yes (MutexUsedThereBinOpK prfK)
              No notK => No (lemmaNonMutexUsageExpBinOp notL notR notK)
nonMutexUsageExp (SlCIO e k) =
  case nonMutexUsageExp e of
    Yes prfE => Yes (MutexUsedThereCIOE prfE)
    No notE =>
      case nonMutexUsageExp k of
          Yes prfK => Yes (MutexUsedThereCIOK prfK)
          No notK => No (lemmaNonMutexUsageExpCIO notE notK)
nonMutexUsageExp (SlApp xs k) =
  case nonMutexUsageExp xs of
    Yes prfXs => Yes (MutexUsedThereAppXS prfXs)
    No notXs =>
      case nonMutexUsageExp k of
          Yes prfK => Yes (MutexUsedThereAppK prfK)
          No notK => No (lemmaNonMutexUsageExpApp notXs notK)
nonMutexUsageExp (SlPThOp k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedTherePThOp prfK)
    No notK => No (lemmaNonMutexUsageExpPThOp notK)
nonMutexUsageExp (SlReturn e k) =
  case nonMutexUsageExp e of
    Yes prfE => Yes (MutexUsedThereReturnE prfE)
    No notE =>
      case nonMutexUsageExp k of
          Yes prfK => Yes (MutexUsedThereReturnK prfK)
          No notK => No (lemmaNonMutexUsageExpReturn notE notK)
nonMutexUsageExp (SlAssign e k) =
  case nonMutexUsageExp e of
    Yes prfE => Yes (MutexUsedThereAssignE prfE)
    No notE =>
      case nonMutexUsageExp k of
          Yes prfK => Yes (MutexUsedThereAssignK prfK)
          No notK => No (lemmaNonMutexUsageExpAssign notE notK)
nonMutexUsageExp (SlAssPtr e k) =
  case nonMutexUsageExp e of
    Yes prfE => Yes (MutexUsedThereAssPtrE prfE)
    No notE =>
      case nonMutexUsageExp k of
          Yes prfK => Yes (MutexUsedThereAssPtrK prfK)
          No notK => No (lemmaNonMutexUsageExpAssPtr notE notK)
nonMutexUsageExp SlStop = No absurd
nonMutexUsageExp (SlSkipBool k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipBool prfK)
    No notK => No (\(MutexUsedThereSkipBool prf) => notK prf)
nonMutexUsageExp (SlSkipNum k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipNum prfK)
    No notK => No (\(MutexUsedThereSkipNum prf) => notK prf)
nonMutexUsageExp (SlSkipStr k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipStr prfK)
    No notK => No (\(MutexUsedThereSkipStr prf) => notK prf)
nonMutexUsageExp (SlSkipVar k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipVar prfK)
    No notK => No (\(MutexUsedThereSkipVar prf) => notK prf)
nonMutexUsageExp (SlSkipIf k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipIf prfK)
    No notK => No (\(MutexUsedThereSkipIf prf) => notK prf)
nonMutexUsageExp (SlSkipWhile k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipWhile prfK)
    No notK => No (\(MutexUsedThereSkipWhile prf) => notK prf)
nonMutexUsageExp (SlSkipFor k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipFor prfK)
    No notK => No (\(MutexUsedThereSkipFor prf) => notK prf)
nonMutexUsageExp (SlSkipUniOp k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipUniOp prfK)
    No notK => No (\(MutexUsedThereSkipUniOp prf) => notK prf)
nonMutexUsageExp (SlSkipBinOp k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipBinOp prfK)
    No notK => No (\(MutexUsedThereSkipBinOp prf) => notK prf)
nonMutexUsageExp (SlSkipCIO k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipCIO prfK)
    No notK => No (\(MutexUsedThereSkipCIO prf) => notK prf)
nonMutexUsageExp (SlSkipApp k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipApp prfK)
    No notK => No (\(MutexUsedThereSkipApp prf) => notK prf)
nonMutexUsageExp (SlSkipPThOp k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipPThOp prfK)
    No notK => No (\(MutexUsedThereSkipPThOp prf) => notK prf)
nonMutexUsageExp (SlSkipReturn k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipReturn prfK)
    No notK => No (\(MutexUsedThereSkipReturn prf) => notK prf)
nonMutexUsageExp (SlSkipDecl k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipDecl prfK)
    No notK => No (\(MutexUsedThereSkipDecl prf) => notK prf)
nonMutexUsageExp (SlSkipAssign k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipAssign prfK)
    No notK => No (\(MutexUsedThereSkipAssign prf) => notK prf)
nonMutexUsageExp (SlSkipAssPtr k) =
  case nonMutexUsageExp k of
    Yes prfK => Yes (MutexUsedThereSkipAssPtr prfK)
    No notK => No (\(MutexUsedThereSkipAssPtr prf) => notK prf)

||| Proof of usage of the mutex *m* anywhere in the slice of program *p*.
data NonMutexUsage : SlCDecl m p -> Type where
  MutexUsedHere : NonMutexUsageExp slE -> NonMutexUsage (SlFun usedE slE slK)
  MutexUsedThereFun : NonMutexUsage slK -> NonMutexUsage (SlFun usedE slE slK)
  MutexUsedThereGVar : NonMutexUsage k -> NonMutexUsage (SlSkipGVar k)
  MutexUsedThereSkipFun : NonMutexUsage k -> NonMutexUsage (SlSkipFun notUsedE k)

implementation Uninhabited (NonMutexUsage SlQuit) where
  uninhabited MutexUsedHere impossible
  uninhabited MutexUsedThereFun impossible
  uninhabited MutexUsedThereGVar impossible
  uninhabited MutexUsedThereSkipFun impossible

lemmaNonMutexUsageNotHereOrThere : Not (NonMutexUsageExp slE) -> Not (NonMutexUsage slK) -> NonMutexUsage (SlFun usedE slE slK) -> Void
lemmaNonMutexUsageNotHereOrThere notUsedInE contra (MutexUsedHere slE) = notUsedInE slE
lemmaNonMutexUsageNotHereOrThere notUsedInE contra (MutexUsedThereFun slK) = contra slK

lemmaNonMutexUsageNotThereGVar : Not (NonMutexUsage slK) -> NonMutexUsage (SlSkipGVar slK) -> Void
lemmaNonMutexUsageNotThereGVar contra (MutexUsedThereGVar slK) = contra slK

lemmaNonMutexUsageNotThereSkipFun : Not (NonMutexUsage slK) -> NonMutexUsage (SlSkipFun notUsedE slK) -> Void
lemmaNonMutexUsageNotThereSkipFun contra (MutexUsedThereSkipFun usedK) = contra usedK

nonMutexUsage : (sl : SlCDecl m p) -> Dec (NonMutexUsage sl)
nonMutexUsage (SlFun usedE slE slK) =
  case nonMutexUsageExp slE of
    Yes prfUsedInE => Yes (MutexUsedHere prfUsedInE)
    No notUsedInE =>
      case nonMutexUsage slK of
        Yes prfK => Yes (MutexUsedThereFun prfK)
        No contra => No (lemmaNonMutexUsageNotHereOrThere notUsedInE contra)
nonMutexUsage (SlSkipGVar slK) =
  case nonMutexUsage slK of
    Yes prfK => Yes (MutexUsedThereGVar prfK)
    No contra => No (lemmaNonMutexUsageNotThereGVar contra)
nonMutexUsage (SlSkipFun notUsedE slK) =
  case nonMutexUsage slK of
    Yes prfK => Yes (MutexUsedThereSkipFun prfK)
    No contra => No (lemmaNonMutexUsageNotThereSkipFun contra)
nonMutexUsage SlQuit = No absurd

-- ----------------------------------------------------------------------------
-- Subtracting a Slice from a Continuation
-- ----------------------------------------------------------------------------

data SlSubExp : (e : CExp) -> (sl : SlCExp m e) -> (d : CExp) -> Type where

slSubExp : (e : CExp) -> (sl : SlCExp m e) -> (d ** SlSubExp e sl d)

||| Relation on programs *p*, slices *sl* of *p* w.r.t *m*, and the subtraction 
||| of *sl* from *p*, denoted *q*.
|||
||| Implemented as a view on *p*, *sl*, and *q*.
data SlSub : (p : CDecl) -> (sl : SlCDecl m p) -> (q : CDecl) -> Type where
  SsFun : SlSubExp e slE d
       -> SlSub k slK kq
       -> SlSub (Fun ty f xs e k) (SlFun usedE slE slK) (Fun ty f xs d kq)
  SsSkipGVar : SlSub k slK kq
            -> SlSub (GVar ty v e k) (SlSkipGVar slK) (GVar ty v e kq)
  SsSkipFun  : SlSub k slK kq
            -> SlSub (Fun ty f xs e k) (SlSkipFun nUsedE slK) (Fun ty f xs e kq)
  SsQuit : SlSub Quit SlQuit Quit

slSub : (p : CDecl) -> (sl : SlCDecl m p) -> (q ** SlSub p sl q)
slSub (Fun ty f xs e k) (SlFun usedE slE slK) =
  let (d ** ssE)  = slSubExp e slE
      (kq ** ssK) = slSub k slK
  in ((Fun ty f xs d kq) ** SsFun ssE ssK)
slSub (GVar ty v e k) (SlSkipGVar slK) =
  let (kq ** ssK) = slSub k slK
  in ((GVar ty v e kq) ** SsSkipGVar ssK)
slSub (Fun ty f xs e k) (SlSkipFun notUsedE slK) =
  let (kq ** ssK) = slSub k slK
  in ((Fun ty f xs e kq) ** SsSkipFun ssK)
slSub Quit SlQuit = (Quit ** SsQuit)

-- subtractMutexSlice : (p : CDecl) -> (sl : SlCDecl m p) -> {prfOnlyMuts : Not (NonMutexUsage sl)} -> CDecl
-- subtractMutexSlice (Fun ty f xs e k) (SlFun usedE slE slK) = ?holeSubFun
-- subtractMutexSlice (GVar ty v e k) (SlSkipGVar slK) = ?holeSubSkipGVar
-- subtractMutexSlice (Fun ty f xs e k) (SlSkipFun notUsedE slK) {prfOnlyMuts} =
--   Fun ty f xs e (?holeHere) -- prf here needs to be a view on the void prf.
-- subtractMutexSlice Quit SlQuit = Quit

-- ||| Relation stating that *q* is the subtraction of *sl* from *p*.
-- data SliceSubtraction : (p : CDecl) -> (sl : SlCDecl m p) -> (q : CDecl) -> Type where
--   SlSubtaction : (p : CDecl) -> (sl : SlCDecl m p) -> (prf: Not (NonMutexUsage sl)) -> (q : (subtractMutexSlice sl prf)) -> SliceSubtraction p sl q

-- ----------------------------------------------------------------------------
-- Application to TestCPP
-- ----------------------------------------------------------------------------

applyingToTestCPP : Maybe Bool
applyingToTestCPP =
  case findMutex prOrig of
    Yes mut =>
      let
        mutName = findMutexName mut
        mutCont = findMutexDecContinuation mut
        slCont  = slCDecl mutName mutCont
      in
        case nonMutexUsage slCont of
          Yes prfUsed => Nothing
          No prfOnlyMutexs => ?holeApplyTestCPP
    No contra => Nothing

