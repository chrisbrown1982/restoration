module Slice

import public VarLoc
import public Data.List

%access public export
%default total

||| We define a slice of a pthread operation *op* w.r.t. the slicing criterion 
||| *v*, a variable, s.t. **FIXME**
data SlPThreadOp : (v : String) -> (op : PThreadOp) -> Type where
  SlCreateNullT : SlPThreadOp v (CreateNull v f x)
  SlCreateNullF : SlPThreadOp v (CreateNull t v x)
  SlCreateNullX : SlPThreadOp v (CreateNull t f v)
  SlCreateNull  : Not (v=t) -> Not (v=f) -> Not (v=x) -> SlPThreadOp v (CreateNull t f x)

  SlMutexLockM : SlPThreadOp v (MutexLock v)
  SlMutexLock  : Not (v=m) -> SlPThreadOp v (MutexLock m)

  SlMutexUnlockM : SlPThreadOp v (MutexUnlock v)
  SlMutexUnlock  : Not (v=m) -> SlPThreadOp v (MutexUnlock m)

  SlJoinNullT : SlPThreadOp v (JoinNull v)
  SlJoinNull  : Not (v=t) -> SlPThreadOp v (JoinNull t)

  SlExitNull : SlPThreadOp v ExitNull

||| Covering function for SlPThreadOp.
slPThreadOp : (v : String) -> (op : PThreadOp) -> SlPThreadOp v op
slPThreadOp v (CreateNull t f x) with (decEq v t)
  slPThreadOp t (CreateNull t f x) | (Yes Refl) = SlCreateNullT
  slPThreadOp v (CreateNull t f x) | (No notT) with (decEq v f)
    slPThreadOp f (CreateNull t f x) | (No notT) | (Yes Refl) = SlCreateNullF
    slPThreadOp v (CreateNull t f x) | (No notT) | (No notF) with (decEq v x)
      slPThreadOp x (CreateNull t f x) | (No notT) | (No notF) | (Yes Refl) = 
        SlCreateNullX
      slPThreadOp v (CreateNull t f x) | (No notT) | (No notF) | (No notX) = 
        SlCreateNull notT notF notX
slPThreadOp v (MutexLock m) with (decEq v m)
  slPThreadOp m (MutexLock m) | (Yes Refl) = SlMutexLockM
  slPThreadOp v (MutexLock m) | (No contra) = SlMutexLock contra
slPThreadOp v (MutexUnlock m) with (decEq v m)
  slPThreadOp m (MutexUnlock m) | (Yes Refl) = SlMutexUnlockM
  slPThreadOp v (MutexUnlock m) | (No contra) = SlMutexUnlock contra
slPThreadOp v (JoinNull t) with (decEq v t)
  slPThreadOp t (JoinNull t) | (Yes Refl) = SlJoinNullT
  slPThreadOp v (JoinNull t) | (No contra) = SlJoinNull contra
slPThreadOp v ExitNull = SlExitNull

||| We define a slice of an expression *e* w.r.t. the slicing criterion *v*, a 
||| variable, s.t. **FIXME**
data SlCExp : (v : String) -> (e : CExp) -> Type where
  ||| Names are unique.
  SlDeclHere : SlCExp v k -> SlCExp v (Decl ty v k)
  ||| Names are unique.
  SlDecl : Not (w = v) -> SlCExp v k -> SlCExp v (Decl ty w k)
  SlPThOp : SlPThreadOp v op -> SlCExp v k -> SlCExp v (PThOp op k)
  ||| Keep control-flow structure of interest.
  SlIf : SlCExp v c -> SlCExp v tt -> SlCExp v ff -> SlCExp v k -> SlCExp v (If c tt ff k)
  ||| Keep control-flow structure of interest.
  SlWhile : SlCExp v c -> SlCExp v b -> SlCExp v k -> SlCExp v (While c b k)
  ||| *v* does **not** appear as a subexpression within the block.
  ||| Recall that we do not allow *v* to be a subexpression of a block in the 
  ||| generation of a model.
  SlBlock : SlCExp m k -> SlCExp m (Block varsUsed k)
  ||| End of a continuation.
  SlStop : SlCExp m Stop

||| Covering function for SlCExp.
slCExp : (v : String) -> (e : CExp) -> SlCExp v e
slCExp v (Decl ty x k) with (decEq x v)
  slCExp v (Decl ty v k) | Yes Refl = SlDeclHere (slCExp v k)
  slCExp v (Decl ty x k) | No contra = SlDecl contra (slCExp v k)
slCExp v (PThOp op k) = SlPThOp (slPThreadOp v op) (slCExp v k)
slCExp v (If c tt ff k) =
  SlIf (slCExp v c) (slCExp v tt) (slCExp v ff) (slCExp v k)
slCExp v (While c b k) = SlWhile (slCExp v c) (slCExp v b) (slCExp v k)
slCExp v (Block varsUsed k) = SlBlock (slCExp v k)
slCExp v Stop = SlStop

||| We define a slice of a program *p* w.r.t. the slicing criterion *v*, a 
||| variable, s.t. **FIXME**
|||
||| Note that since we are primarily concerned with mutex and pthread variables 
||| here, we simplify the model and slice by assuming that we do not need to 
||| update the slicing criteria due to dependencies between variables
||| (e.g. x = y + 2). This is reasonable here, since it is unlikely 
||| that a 'well formed' C/C++ program will/can use a mutex or pthread variable 
||| in the RHS of an assignment statement.
|||
||| Proof that *v* is declared in *p* is not required here, since it may be the 
||| case that *v* does **not** appear as a subexpression in *p*. Here, the 
||| slice would be 'empty'.
data SlCDecl : (v : String) -> (p : CDecl) -> Type where
  ||| *v* is defined here.
  |||
  ||| Assume that all names are unique and that there is no aliasing.
  ||| Assume that in this model *v* is not allowed to appear in *e*; i.e. there 
  ||| is no explicit dependency between variables (e.g. x = y + 2).
  ||| Therefore, by definition, *v* cannot exist as a subexpression here.
  SlVarDecHere : SlCDecl v k -> SlCDecl v (VarDec ty v e k)
  ||| Assume that all names are unique and that there is no aliasing.
  ||| Assume that in this model *v* is not allowed to appear in *e*; i.e. there 
  ||| is no explicit dependency between variables (e.g. x = y + 2).
  ||| Therefore, by definition, *v* cannot exist as a subexpression here.
  SlVarDec : Not (w = v) -> SlCDecl v k -> SlCDecl v (VarDec ty w e k)
  ||| Alt. Defn.: prf that *e* does (not) contain a *v* as a subexpression.
  |||
  ||| Here, we do not distinguish between cases for simplicity. (I don't need 
  ||| to define what a subexpression means in Idris.)
  SlFunDec : SlCExp v e -> SlCDecl v k -> SlCDecl v (FunDec ty f xs e k)
  ||| End of a continuation.
  SlQuit : SlCDecl v Quit

||| Covering function for SlCDecl.
slCDecl : (v : String) -> (p : CDecl) -> SlCDecl v p
slCDecl v (VarDec ty x e k) with (decEq x v)
  slCDecl v (VarDec ty v e k) | Yes Refl = SlVarDecHere (slCDecl v k)
  slCDecl v (VarDec ty x e k) | No contra = SlVarDec contra (slCDecl v k)
slCDecl v (FunDec ty f xs e k) = SlFunDec (slCExp v e) (slCDecl v k)
slCDecl v Quit = SlQuit


