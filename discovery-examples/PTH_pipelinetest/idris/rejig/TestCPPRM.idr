module TestCPPRM

import public MergeIter
import public UnfoldAbs
import public Reorder

%default total
%access public export

fnBufferIsEmptyBody : CExp
fnBufferIsEmptyBody =
  Decl CBool "state" $
  Assn "state" (Block Stop) $
  Block Stop

fnBufferIsFullBody : CExp
fnBufferIsFullBody =
  Decl CBool "state" $
  Assn "state" (Block Stop) $
  Block Stop

fnStageOneBody : CExp
fnStageOneBody =
  Decl CInt "id1" $
  Assn "id1" (Block Stop) $
  Block $
  Iter ["i1"] [0] (Block Stop) (Block Stop) $
  Stop

fnStageTwoBody : CExp
fnStageTwoBody =
  Decl CInt "id2" $
  Assn "id2" (Block Stop) $
  Block $
  Iter ["i2"] [0] (Block Stop) (Block Stop) $
  Stop

fnMainBody : CExp
fnMainBody =
    Decl CInt "rc" $
    Decl CInt "i0" $
    Iter ["i0"] [0] (Block Stop) stageOneBody $
    Iter ["i0"] [0] (Block Stop) stageTwoBody $
    Stop
  where
    stageOneBody : CExp
    stageOneBody =
      Decl (CPtr CInt) "arg1" $
      Assn "arg1" (Block Stop) $
      Appl "StageOne" ["arg1"] $
      Stop

    stageTwoBody : CExp
    stageTwoBody =
      Decl (CPtr CInt) "arg2" $
      Assn "arg2" (Block Stop) $
      Appl "StageTwo" ["arg2"] $
      Stop

prTest : CDecl
prTest =
  VarDec CInt "NUM_THREADS" (Block Stop) $
  VarDec CInt "BUFFER_SIZE" (Block Stop) $
  VarDec CInt "inBuffer" (Block Stop) $
  FunDec CBool "BufferIsEmpty" [] fnBufferIsEmptyBody $
  FunDec CBool "BufferIsFull" [] fnBufferIsFullBody $
  FunDec CBool "StageOne" [] fnStageOneBody $
  FunDec CBool "StageTwo" [] fnStageTwoBody $
  FunDec CBool "Main" [] fnMainBody $
  Quit
