module SubtractSlice

import public Slice

%access public export
%default total

||| Relation s.t. *e'* is *e* with the slice *sl* subtracted.
data SSCExp : (e : CExp) -> (sl : SlCExp v e) -> (e' : CExp) -> Type where
  ||| Don't keep.
  SSDeclHere : SSCExp k slK k' -> SSCExp (Decl ty x k) (SlDeclHere slK) k'

  ||| Keep.
  SSDecl  : SSCExp k slK k' -> SSCExp (Decl ty x k) (SlDecl prf slK) (Decl ty x k')

  ||| Remove -- *v* is *t*.
  SSCreateNullT : SSCExp k slK k' -> SSCExp (PThOp (CreateNull v f x) k) (SlPThOp SlCreateNullT slK) k'

  ||| Remove -- *v* is *f*.
  SSCreateNullF : SSCExp k slK k' -> SSCExp (PThOp (CreateNull t v x) k) (SlPThOp SlCreateNullF slK) k'

  ||| Remove -- *v* is *x*.
  SSCreateNullX : SSCExp k slK k' -> SSCExp (PThOp (CreateNull t f v) k) (SlPThOp SlCreateNullX slK) k'

  ||| Keep.
  SSCreateNull : SSCExp k slK k' -> SSCExp (PThOp (CreateNull t f x) k) (SlPThOp (SlCreateNull notT notF notX) slK) (PThOp (CreateNull t f x) k')

  ||| Remove -- *v* is *m*.
  SSMutexLockM : SSCExp k slK k' -> SSCExp (PThOp (MutexLock v) k) (SlPThOp SlMutexLockM slK) k'
  
  ||| Keep.
  SSMutexLock : SSCExp k slK k' -> SSCExp (PThOp (MutexLock m) k) (SlPThOp (SlMutexLock notM) slK) (PThOp (MutexLock m) k')

  ||| Remove -- *v* is *m*.
  SSMutexUnlockM : SSCExp k slK k' -> SSCExp (PThOp (MutexUnlock v) k) (SlPThOp SlMutexUnlockM slK) k'
  
  ||| Keep.
  SSMutexUnlock : SSCExp k slK k' -> SSCExp (PThOp (MutexUnlock m) k) (SlPThOp (SlMutexUnlock notM) slK) (PThOp (MutexUnlock m) k')

  ||| Remove -- *v* is *t*.
  SSJoinNullT : SSCExp k slK k' -> SSCExp (PThOp (JoinNull v) k) (SlPThOp SlJoinNullT slK) k'
  
  ||| Keep.
  SSJoinNull : SSCExp k slK k' -> SSCExp (PThOp (JoinNull t) k) (SlPThOp (SlJoinNull notT) slK) (PThOp (JoinNull t) k')

  ||| Keep.
  SSExitNull : SSCExp k slK k' -> SSCExp (PThOp ExitNull k) (SlPThOp SlExitNull slK) (PThOp ExitNull k')

  ||| Keep.
  SSIf : SSCExp c slC c' -> SSCExp tt slT tt' -> SSCExp ff slFF ff' -> SSCExp k slK k' -> SSCExp (If c tt ff k) (SlIf slC slTT slFF slK) (If c' tt' ff' k')

  ||| Keep.
  SSWhile : SSCExp c slC c' -> SSCExp b slT b' -> SSCExp k slK k' -> SSCExp (While c b k) (SlWhile slC slTT slK) (While c' b' k')

  ||| Keep.
  SSBlock : SSCExp k slK k' -> SSCExp (Block xs k) (SlBlock slK) (Block xs k')

  ||| Keep.
  SSStop  : SSCExp Stop SlStop Stop

||| Covering function for SSCExp.
ssExp : (e : CExp) -> (sl : SlCExp v e) -> (e' ** SSCExp e sl e')
ssExp (Decl ty w k) (SlDeclHere slK) with (ssExp k slK)
  ssExp (Decl ty w k) (SlDeclHere slK) | (k' ** ssK) =
    (k' ** SSDeclHere ssK)
ssExp (Decl ty w k) (SlDecl prf slK) with (ssExp k slK)
  ssExp (Decl ty w k) (SlDecl prf slK) | (k' ** ssK) =
    (Decl ty w k' ** SSDecl ssK)

ssExp (PThOp (CreateNull v f x) k) (SlPThOp SlCreateNullT slK) with (ssExp k slK)
  ssExp (PThOp (CreateNull v f x) k) (SlPThOp SlCreateNullT slK) | (k' ** ssK) =
    (k' ** SSCreateNullT ssK)
ssExp (PThOp (CreateNull t v x) k) (SlPThOp SlCreateNullF slK) with (ssExp k slK)
  ssExp (PThOp (CreateNull t v x) k) (SlPThOp SlCreateNullF slK) | (k' ** ssK) =
    (k' ** SSCreateNullF ssK)
ssExp (PThOp (CreateNull t f v) k) (SlPThOp SlCreateNullX slK) with (ssExp k slK)
  ssExp (PThOp (CreateNull t f v) k) (SlPThOp SlCreateNullX slK) | (k' ** ssK) =
    (k' ** SSCreateNullX ssK)
ssExp (PThOp (CreateNull t f x) k) (SlPThOp (SlCreateNull notT notF notX) slK) with (ssExp k slK)
  ssExp (PThOp (CreateNull t f x) k) (SlPThOp (SlCreateNull notT notF notX) slK) | (k' ** ssK) =
    (PThOp (CreateNull t f x) k' ** SSCreateNull ssK)

ssExp (PThOp (MutexLock v) k) (SlPThOp SlMutexLockM slK) with (ssExp k slK)
  ssExp (PThOp (MutexLock v) k) (SlPThOp SlMutexLockM slK) | (k' ** ssK) =
    (k' ** SSMutexLockM ssK)
ssExp (PThOp (MutexLock m) k) (SlPThOp (SlMutexLock notM) slK) with (ssExp k slK)
  ssExp (PThOp (MutexLock m) k) (SlPThOp (SlMutexLock notM) slK) | (k' ** ssK) =
    (PThOp (MutexLock m) k' ** SSMutexLock ssK)

ssExp (PThOp (MutexUnlock v) k) (SlPThOp SlMutexUnlockM slK) with (ssExp k slK)
  ssExp (PThOp (MutexUnlock v) k) (SlPThOp SlMutexUnlockM slK) | (k' ** ssK) =
    (k' ** SSMutexUnlockM ssK)
ssExp (PThOp (MutexUnlock m) k) (SlPThOp (SlMutexUnlock notM) slK) with (ssExp k slK)
  ssExp (PThOp (MutexUnlock m) k) (SlPThOp (SlMutexUnlock notM) slK) | (k' ** ssK) =
    (PThOp (MutexUnlock m) k' ** SSMutexUnlock ssK)

ssExp (PThOp (JoinNull v) k) (SlPThOp SlJoinNullT slK) with (ssExp k slK)
  ssExp (PThOp (JoinNull v) k) (SlPThOp SlJoinNullT slK) | (k' ** ssK) =
    (k' ** SSJoinNullT ssK)
ssExp (PThOp (JoinNull t) k) (SlPThOp (SlJoinNull notT) slK) with (ssExp k slK)
  ssExp (PThOp (JoinNull t) k) (SlPThOp (SlJoinNull notT) slK) | (k' ** ssK) =
    (PThOp (JoinNull t) k' ** SSJoinNull ssK)

ssExp (PThOp ExitNull k) (SlPThOp SlExitNull slK) with (ssExp k slK)
  ssExp (PThOp ExitNull k) (SlPThOp SlExitNull slK) | (k' ** ssK) =
    (PThOp ExitNull k' ** SSExitNull ssK)

ssExp (If c tt ff k) (SlIf slC slTT slFF slK) =
  let (c' ** ssC) = ssExp c slC
      (tt' ** ssTT) = ssExp tt slTT
      (ff' ** ssFF) = ssExp ff slFF
      (k' ** ssK) = ssExp k slK
  in (If c' tt' ff' k' ** SSIf ssC ssTT ssFF ssK)
ssExp (While c b k) (SlWhile slC slB slK) =
  let (c' ** ssC) = ssExp c slC
      (b' ** ssB) = ssExp b slB
      (k' ** ssK) = ssExp k slK
  in (While c' b' k' ** SSWhile ssC ssB ssK)
ssExp (Block varsUsed k) (SlBlock slK) with (ssExp k slK)
  ssExp (Block varsUsed k) (SlBlock slK) | (k' ** ssK) =
    (Block varsUsed k' ** SSBlock ssK)
ssExp Stop SlStop = (Stop ** SSStop)

||| Relation s.t. *q* is *p* with the slice *sl* of *p* subtracted.
data SSCDecl : (p : CDecl) -> (sl : SlCDecl v p) -> (q : CDecl) -> Type where
  SSVarDecHere : SSCDecl k slK k' -> SSCDecl (VarDec ty w e k) (SlVarDecHere slK) k'
  SSVarDec : SSCDecl k slK k' -> SSCDecl (VarDec ty w e k) (SlVarDec prf slK) (VarDec ty w e k')
  ||| The subtracted *e* is the main reason for p and q being different.
  SSFunDec : SSCExp e slE e' -> SSCDecl k slK k' -> SSCDecl (FunDec ty f xs e k) (SlFunDec slE slK) (FunDec ty f xs e' k')
  ||| End of a continuation.
  SSQuit : SSCDecl Quit (SlQuit) Quit

||| Covering function for SSDecl.
ssDecl : (p : CDecl) -> (sl : SlCDecl v p) -> (q ** SSCDecl p sl q)
ssDecl (VarDec ty w e k) (SlVarDecHere slK) with (ssDecl k slK)
  ssDecl (VarDec ty w e k) (SlVarDecHere slK) | (k' ** ssK) =
    (k' ** SSVarDecHere ssK)
ssDecl (VarDec ty w e k) (SlVarDec prf slK) with (ssDecl k slK)
  ssDecl (VarDec ty w e k) (SlVarDec prf slK) | (k' ** ssK) =
    ((VarDec ty w e k') ** SSVarDec ssK)
ssDecl (FunDec ty f xs e k) (SlFunDec slE slK) with (ssExp e slE)
  ssDecl (FunDec ty f xs e k) (SlFunDec slE slK) | (e' ** ssE) with (ssDecl k slK)
    ssDecl (FunDec ty f xs e k) (SlFunDec slE slK) | (e' ** ssE) | (k' ** ssK) =
      ((FunDec ty f xs e' k') ** SSFunDec ssE ssK)
ssDecl Quit SlQuit = (Quit ** SSQuit)
