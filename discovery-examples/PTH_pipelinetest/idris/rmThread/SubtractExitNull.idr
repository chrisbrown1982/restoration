module SubtractExitNull

import public VarLoc

%access public export
%default total

||| Relation s.t. *e'* is *e* but with no *pthread_exit* statements.
data SECExp : (e : CExp) -> (e' : CExp) -> Type where
  SEDecl : SECExp k k' -> SECExp (Decl ty v k) (Decl ty v k')
  
  SEPThOp : Not (op = ExitNull) -> SECExp k k' -> SECExp (PThOp op k) (PThOp op k')

  SEExitNull : SECExp k k' -> SECExp (PThOp ExitNull k) k'

  SEIf : SECExp c c' -> SECExp tt tt' -> SECExp ff ff' -> SECExp k k' -> SECExp (If c tt ff k) (If c' tt' ff' k')
  
  SEWhile : SECExp c c' -> SECExp tt tt' -> SECExp k k' -> SECExp (While c b k) (While c' b' k')
  
  SEBlock : SECExp k k' -> SECExp (Block xs k) (Block xs k')
  
  SEStop : SECExp Stop Stop

||| Covering function for SECExp.
seCExp : (e : CExp) -> (e' ** SECExp e e')
seCExp (Decl ty v k) with (seCExp k)
  seCExp (Decl ty v k) | (k' ** seK) = (Decl ty v k' ** SEDecl seK)
seCExp (PThOp (CreateNull t f x) k) =
  let (k' ** seK) = seCExp k in (PThOp (CreateNull t f x) k' ** SEPThOp absurd seK)
seCExp (PThOp (MutexLock m) k) =
  let (k' ** seK) = seCExp k in (PThOp (MutexLock m) k' ** SEPThOp absurd seK)
seCExp (PThOp (MutexUnlock m) k) =
  let (k' ** seK) = seCExp k in (PThOp (MutexUnlock m) k' ** SEPThOp absurd seK)
seCExp (PThOp (JoinNull t) k) =
  let (k' ** seK) = seCExp k in (PThOp (JoinNull t) k' ** SEPThOp absurd seK)
seCExp (PThOp ExitNull k) =
  let (k' ** seK) = seCExp k in (k' ** SEExitNull seK)
seCExp (If c tt ff k) =
  let (c' ** seC) = seCExp c
      (tt' ** seTT) = seCExp tt
      (ff' ** seFF) = seCExp ff
      (k' ** seK) = seCExp k
  in (If c' tt' ff' k' ** SEIf seC seTT seFF seK)
seCExp (While c b k) =
  let (c' ** seC) = seCExp c
      (b' ** seB) = seCExp b
      (k' ** seK) = seCExp k
  in (While c' b' k' ** SEWhile seC seB seK)
seCExp (Block varsUsed k) with (seCExp k)
  seCExp (Block varsUsed k) | (k' ** seK) =
    (Block varsUsed k' ** SEBlock seK)
seCExp Stop = (Stop ** SEStop)

||| Relation s.t. *q* is *p* but with no *pthread_exit* statements.
|||
||| *p* and *q* are the same, save for function bodies.
data SECDecl : (p : CDecl) -> (q : CDecl) -> Type where
  SEVarDec : SECDecl k k' -> SECDecl (VarDec ty v e k) (VarDec ty v e k')
  SEFunDec : SECExp e e' -> SECDecl k k' -> SECDecl (FunDec ty f xs e k) (FunDec ty f xs e' k')
  SEQuit : SECDecl Quit Quit

||| Covering function for SECDecl.
seCDecl : (p : CDecl) -> (q ** SECDecl p q)
seCDecl (VarDec ty v e k) with (seCDecl k)
  seCDecl (VarDec ty v e k) | (k' ** seK) = (VarDec ty v e k' ** SEVarDec seK)
seCDecl (FunDec ty f xs e k) =
  let (e' ** seE) = seCExp e
      (k' ** seK) = seCDecl k
  in (FunDec ty f xs e' k' ** SEFunDec seE seK)
seCDecl Quit = (Quit ** SEQuit)
