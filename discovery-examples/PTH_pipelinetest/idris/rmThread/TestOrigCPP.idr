module TestOrigCPP

import public SubtractSlice
import public SubtractExitNull

%default total
%access public export

fnBufferIsEmptyBody : CExp
fnBufferIsEmptyBody =
  PThOp (MutexLock "monk") $
  Decl CBool "state" $
  Block ["state", "inBuffer"] $
  PThOp (MutexUnlock "monk") $
  Block ["state"] $
  Stop

fnBufferIsFullBody : CExp
fnBufferIsFullBody =
  PThOp (MutexLock "monk") $
  Decl CBool "state" $
  Block ["state", "inBuffer"] $
  PThOp (MutexUnlock "monk") $
  Block ["state"] $
  Stop



fnStageOneBody : CExp
fnStageOneBody =
    Decl CInt "id" $
    Block ["threadarg","id"] $
    Decl CInt "i" $
    While (Block ["i"] Stop) fnWhileBody $
    PThOp ExitNull $
    Stop
  where
    fnIfTT : CExp
    fnIfTT =
      PThOp (MutexLock "monk") $
      Block ["inBuffer", "BUFFER_SIZE","id"] $
      PThOp (MutexUnlock "monk") $
      Block ["i"] $
      Stop
    
    fnWhileBody : CExp
    fnWhileBody =
      If (Block [] Stop) fnIfTT Stop Stop

fnStageTwoBody : CExp
fnStageTwoBody =
    Decl CInt "id" $
    Block ["threadarg","id"] $
    Decl CInt "i" $
    While (Block ["i"] Stop) fnWhileBody $
    PThOp ExitNull $
    Stop
  where
    fnIfTT : CExp
    fnIfTT =
      PThOp (MutexLock "monk") $
      Block ["inBuffer", "BUFFER_SIZE","id"] $
      PThOp (MutexUnlock "monk") $
      Block ["i"] $
      Stop
    
    fnWhileBody : CExp
    fnWhileBody =
      If (Block [] Stop) fnIfTT Stop Stop

fnMainBody : CExp
fnMainBody =
    Decl (CPThTy ThreadT) "t1" $
    Decl (CPThTy ThreadT) "t2" $
    Decl CInt "rc" $
    Decl CInt "i" $
    Block ["i"] $
    While (Block ["i","NUM_THREADS"] Stop) fnWhileOneBody $
    Block ["i"] $
    While (Block ["i","NUM_THREADS"] Stop) fnWhileTwoBody $
    PThOp (JoinNull "t1") $
    PThOp (JoinNull "t2") $
    Stop
  where
    fnWhileOneBody : CExp
    fnWhileOneBody =
      Decl (CPtr CInt) "arg" $
      Block ["arg","i"] $
      PThOp (CreateNull "t1" "StageOne" "arg") $
      Stop

    fnWhileTwoBody : CExp
    fnWhileTwoBody =
      Decl (CPtr CInt) "arg" $
      Block ["arg","i"] $
      PThOp (CreateNull "t2" "StageTwo" "arg") $
      Stop

prTestOrig : CDecl
prTestOrig =
  VarDec CInt "NUM_THREADS" (Block [] Stop) $
  VarDec CInt "BUFFER_SIZE" (Block [] Stop) $
  VarDec CInt "inBuffer" (Block [] Stop) $
  VarDec (CPThTy MutexT) "monk" Stop $
  FunDec CBool "BufferIsEmpty" [] fnBufferIsEmptyBody $
  FunDec CBool "BufferIsFull" [] fnBufferIsFullBody $
  FunDec (CPtr CVoid) "StageOne" [(CPtr CVoid, "threadarg")] fnStageOneBody $
  FunDec (CPtr CVoid) "StageTwo" [(CPtr CVoid, "threadarg")] fnStageTwoBody $
  FunDec CInt "main" [] fnMainBody $
  Quit

rmThreadsTestOrig : CDecl
rmThreadsTestOrig with (seCDecl prTestOrig)
  rmThreadsTestOrig | (p' ** secdecl) =
    let slP' = slCDecl "monk" p'
        (p'' ** ssdeclMonk) = ssDecl p' slP'
        slP'' = slCDecl "t1" p''
        (p''' ** ssdeclT1) = ssDecl p'' slP''
        slP''' = slCDecl "t2" p'''
        (q ** ssdeclT2) = ssDecl p''' slP'''
    in q


{-

  -- Remove Mutex

testFnBufferIsEmptyBody : Maybe CExp
testFnBufferIsEmptyBody =
  let e = fnBufferIsEmptyBody
      sl = slCExp "monk" e
  in case onlyMutexOpsExp sl of
    Yes omo => Just (subtractExp e sl omo)
    No contra => Nothing

testFnBufferIsFullBody : Maybe CExp
testFnBufferIsFullBody =
  let e = fnBufferIsFullBody
      sl = slCExp "monk" e
  in case onlyMutexOpsExp sl of
    Yes omo => Just (subtractExp e sl omo)
    No contra => Nothing

testFnStageOneBody : Maybe CExp
testFnStageOneBody =
  let e = fnStageOneBody
      sl = slCExp "monk" e
  in case onlyMutexOpsExp sl of
    Yes omo => Just (subtractExp e sl omo)
    No contra => Nothing

testFnStageTwoBody : Maybe CExp
testFnStageTwoBody =
  let e = fnStageTwoBody
      sl = slCExp "monk" e
  in case onlyMutexOpsExp sl of
    Yes omo => Just (subtractExp e sl omo)
    No contra => Nothing

testFnMainBody : Maybe CExp
testFnMainBody =
  let e = fnMainBody
      sl = slCExp "monk" e
  in case onlyMutexOpsExp sl of
    Yes omo => Just (subtractExp e sl omo)
    No contra => Nothing

||| **NB**: l should be related to p in that it is a substitution
switchK : (p : CDecl) -> (loc : MutLoc "monk" p) -> (l : CDecl) -> CDecl
switchK (VarDec ty "monk" e k) HereVarDec l = l
switchK (VarDec ty v e k) (ThereVarDec f x) l =
  VarDec ty v e (switchK k x l)
switchK (FunDec ty f xs e k) (ThereFunDec x) l =
  FunDec ty f xs e (switchK k x l)


testPrTestOrig : Maybe CDecl
testPrTestOrig with (mutex "monk" prTestOrig)
  testPrTestOrig | Yes (dec ** (k ** mut)) =
    let sl = slCDecl "monk" k
    in case onlyMutexOps sl of
          Yes omo => Just (switchK prTestOrig dec (subtract k sl omo))
          No contra => Nothing
  testPrTestOrig | No contra = Nothing

-}
