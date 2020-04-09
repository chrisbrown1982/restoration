module RemoveMutex

import public CppDSL
import public Data.List

%access public export

%default total

-- ----------------------------------------------------------------------------
-- 1. A Mutex.
-- ----------------------------------------------------------------------------

||| Proof & location of mutex *m* in *p*.
|||
||| We assume for now that mutexes are *only* declared as global variables, but 
||| the principle can be extended to other locations (e.g. local declaration 
||| and passing to threads via structs).
|||
||| **FIXME**: constrain HereVarDec ty.
data MutLoc : (m : String) -> (p : CDecl) -> Type where
  HereVarDec  : MutLoc m (VarDec ty m e k)
  ThereVarDec : Not (m=v) -> MutLoc m k -> MutLoc m (VarDec ty v e k)
  ThereFunDec : MutLoc m k -> MutLoc m (FunDec ty f xs e k)

implementation Uninhabited (MutLoc m Quit) where
  uninhabited HereVarDec impossible
  uninhabited ThereVarDec impossible
  uninhabited ThereFunDec impossible

lemmaMutNeitherHereNorThereVarDec : Not (m = v) -> Not (MutLoc m k) -> MutLoc m (VarDec ty v e k) -> Void
lemmaMutNeitherHereNorThereVarDec notMIsV notThereK HereVarDec = notMIsV Refl
lemmaMutNeitherHereNorThereVarDec notMIsV notThereK (ThereVarDec p1 p2) = notThereK p2

lemmaNotThereFunDec : Not (MutLoc m k) -> MutLoc m (FunDec ty f xs e k) -> Void
lemmaNotThereFunDec notThereK (ThereFunDec prf) = notThereK prf

||| Decision procedure for MutLoc.
mutLoc : (m : String) -> (p : CDecl) -> Dec (MutLoc m p)
mutLoc m (VarDec ty v e k) =
  case decEq m v of
    Yes Refl => Yes HereVarDec
    No notMIsV =>
      case mutLoc m k of
        Yes thereK => Yes (ThereVarDec notMIsV thereK)
        No notThereK => No (lemmaMutNeitherHereNorThereVarDec notMIsV notThereK)
mutLoc m (FunDec ty f xs e k) =
  case mutLoc m k of
    Yes thereK => Yes (ThereFunDec thereK)
    No notThereK => No (lemmaNotThereFunDec notThereK)
mutLoc m Quit = No absurd

fetchK : MutLoc m p -> CDecl
fetchK {p = (VarDec ty m e k)} HereVarDec = k
fetchK {p = (VarDec ty v e k)} (ThereVarDec f x) = fetchK x
fetchK {p = (FunDec ty f xs e k)} (ThereFunDec x) = fetchK x

||| Relation that denotes a mutex; comprised of: its name, *m*; it's location 
||| *dec* in some program *p*; and the continuation *k* after its declaration.
data Mutex : (m : String) -> (dec : MutLoc m p) -> (k : CDecl) -> Type where
  ||| **FIXME**: this is only really a record at the moment; *k* could be 
  ||| anything, and needs to be properly related to *p*.
  Mut : (m : String) -> (dec : MutLoc m p) -> (k : CDecl) -> Mutex m dec k

lemmaMNotDeclared : (notInP : MutLoc m p -> Void) -> (dec : MutLoc m p ** k : CDecl ** Mutex m dec k) -> Void
lemmaMNotDeclared notInP (dec ** rest) = notInP dec

||| Decision procedure for Mutex.
mutex : (m : String) -> (p : CDecl)
     -> Dec (dec : MutLoc m p ** (k : CDecl ** Mutex m dec k))
mutex m p with (mutLoc m p)
  mutex m p | Yes dec =
    let k = fetchK dec
    in Yes (dec ** (k ** Mut m dec k))
  mutex m p | No notInP = No (lemmaMNotDeclared notInP)

-- ----------------------------------------------------------------------------
-- 2. Slicing a Continuation.
-- ----------------------------------------------------------------------------

||| Slice of Expressions.
|||
||| This is more general than just for mutexes; will look for any variable *m* 
||| in *e* recursively.
|||
||| Note to self: don't just use 'm'.
|||
||| This is theoretically a forward (relational) slicing algorithm. No aliasing,
||| asignments, or locations (e.g. use in conditional expressions) have a 
||| bearing on the slice itself, and so no propagation of influence is 
||| considered here. As a result, it's a very customised slicing algorithm.
||| *This may need to be fixed for other rewrites.*
data SlCExp : (m : String) -> (e : CExp) -> Type where
  ||| Assuming names are unique so not shadowing here. If done properly, would 
  ||| have a 'Not (m=v)' argument.
  SlDecl : SlCExp m k -> SlCExp m (Decl ty v k)
  ||| *m* is the thread being created.
  SlPThCreate : SlCExp m k -> SlCExp m (PThOp (CreateNull m f x) k)
  ||| *m* is **not** the thread being created.
  SlSkipPThCreate : Not (m=t) -> SlCExp m k -> SlCExp m (PThOp (CreateNull t f x) k)
  ||| *m* is the mutex being locked.
  SlPThMutexL : SlCExp m k -> SlCExp m (PThOp (MutexLock m) k)
  ||| *m* is **not** the mutex being locked.
  SlSkipPThMutexL : Not (m=v) -> SlCExp m k -> SlCExp m (PThOp (MutexLock v) k)
  ||| *m* is the mutex being unlocked.
  SlPThMutexU : SlCExp m k -> SlCExp m (PThOp (MutexUnlock m) k)
  ||| *m* is **not** the mutex being unlocked.
  SlSkipPThMutexU : Not (m=v) -> SlCExp m k -> SlCExp m (PThOp (MutexUnlock v) k)
  ||| *m* is the thread being waited for.
  SlPThJoinNull : SlCExp m k -> SlCExp m (PThOp (JoinNull m) k)
  ||| *m* is **not** the thread being waited for.
  SlSkipPThJoinNull : Not (m=t) -> SlCExp m k -> SlCExp m (PThOp (JoinNull t) k)
  SlPThExitNull : SlCExp m k -> SlCExp m (PThOp ExitNull k)
  SlIf : SlCExp m c -> SlCExp m tt -> SlCExp m ff -> SlCExp m k -> SlCExp m (If c tt ff k)
  SlWhile : SlCExp m c -> SlCExp m b -> SlCExp m k -> SlCExp m (While c b k)
  ||| *m* is used within the block.
  SlBlock : Elem m varsUsed -> SlCExp m k -> SlCExp m (Block varsUsed k)
  ||| *m* is **not** used within the block.
  SlSkipBlock : Not (Elem m varsUsed) -> SlCExp m k -> SlCExp m (Block varsUsed k)
  ||| End of a continuation.
  SlStop : SlCExp m Stop

slCExp : (m : String) -> (e : CExp) -> SlCExp m e
slCExp m (Decl ty v k) = SlDecl (slCExp m k)
slCExp m (PThOp (CreateNull t f x) k) =
  case decEq m t of
    Yes Refl => SlPThCreate (slCExp m k)
    No contra => SlSkipPThCreate contra (slCExp m k)
slCExp m (PThOp (MutexLock x) k) =
  case decEq m x of
    Yes Refl => SlPThMutexL (slCExp m k)
    No contra => SlSkipPThMutexL contra (slCExp m k)
slCExp m (PThOp (MutexUnlock x) k) =
  case decEq m x of
    Yes Refl => SlPThMutexU (slCExp m k)
    No contra => SlSkipPThMutexU contra (slCExp m k)
slCExp m (PThOp (JoinNull t) k) =
  case decEq m t of
    Yes Refl => SlPThJoinNull (slCExp m k)
    No contra => SlSkipPThJoinNull contra (slCExp m k)
slCExp m (PThOp ExitNull k) = SlPThExitNull (slCExp m k)
slCExp m (If c tt ff k) =
  SlIf (slCExp m c) (slCExp m tt) (slCExp m ff) (slCExp m k)
slCExp m (While c b k) =
  SlWhile (slCExp m c) (slCExp m b) (slCExp m k)
slCExp m (Block varsUsed k) =
  case isElem m varsUsed of
    Yes inVarsUsed => SlBlock inVarsUsed (slCExp m k)
    No notInVarsUsed => SlSkipBlock notInVarsUsed (slCExp m k)
slCExp m Stop = SlStop

||| Slice of Programs.
data SlCDecl : (m : String) -> (p : CDecl) -> Type where
  ||| We assume that all names are unique, that variables are declared only 
  ||| once, and that there is no aliasing (simplified), so we say that it 
  ||| cannot be here by definition. This should probably be fixed...
  |||
  ||| **NB** technically, I should have a 'Not (VarUsed m e) ->' here, but 
  ||| left to assumptions for the time being.
  SlVarDec : SlCDecl m k -> SlCDecl m (VarDec ty v e k)
  ||| Not bothering to split Fun & SkipFun this time to make it easier; they 
  ||| effectively do the same thing anyway. Saves me implementing VarUsed as 
  ||| well.
  SlFunDec : SlCExp m e -> SlCDecl m k -> SlCDecl m (FunDec ty f xs e k)
  ||| End of a continuation.
  SlQuit : SlCDecl m Quit

slCDecl : (m : String) -> (p : CDecl) -> SlCDecl m p
slCDecl m (VarDec ty v e k) = SlVarDec (slCDecl m k)
slCDecl m (FunDec ty f xs e k) = SlFunDec (slCExp m e) (slCDecl m k)
slCDecl m Quit = SlQuit

-- ----------------------------------------------------------------------------
-- 3. Inspecting the Slice.
-- ----------------------------------------------------------------------------

||| We only want slices for our mutex variable that contain mutex operations. 
||| (Of course, this may already be the case depending on our definition of 
||| well-formedness, but let's worry about that later.)
|||
||| Implemented as a View on a slice s.t. we can only construct elements of the 
||| view when the slice is constructed using a subset of constructors. This is 
||| trivial for declarations but not for expressions.
data OnlyMutexOpsExp : SlCExp m e -> Type where
  OMODecl : OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlDecl k)
  OMOPThCreate : OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlSkipPThCreate pf k)
  OMOPThMutexL : OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlPThMutexL k)
  OMOSkipPThMutexL : OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlSkipPThMutexL pf k)
  OMOPThMutexU : OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlPThMutexU k)
  OMOSkipPThMutexU : OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlSkipPThMutexU pf k)
  OMOPThJoinNull : OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlSkipPThJoinNull pf k)
  OMOPThExitNull : OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlPThExitNull k)
  OMOIf : OnlyMutexOpsExp c -> OnlyMutexOpsExp tt -> OnlyMutexOpsExp ff -> OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlIf c tt ff k)
  OMOWhile : OnlyMutexOpsExp c -> OnlyMutexOpsExp b -> OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlWhile c b k)
  OMOBlock : OnlyMutexOpsExp k -> OnlyMutexOpsExp (SlSkipBlock pf k)
  OMOStop : OnlyMutexOpsExp SlStop

implementation Uninhabited (OnlyMutexOpsExp (SlPThCreate j)) where
  uninhabited (OMODecl _) impossible
  uninhabited (OMOPThCreate _) impossible
  uninhabited (OMOPThMutexL _) impossible
  uninhabited (OMOSkipPThMutexL _) impossible
  uninhabited (OMOPThMutexU _) impossible
  uninhabited (OMOSkipPThMutexU _) impossible
  uninhabited (OMOPThJoinNull _) impossible
  uninhabited (OMOPThExitNull _) impossible
  uninhabited (OMOIf _ _ _ _) impossible
  uninhabited (OMOWhile _ _ _) impossible
  uninhabited (OMOBlock _) impossible
  uninhabited OMOStop impossible

implementation Uninhabited (OnlyMutexOpsExp (SlPThJoinNull x)) where
  uninhabited (OMODecl _) impossible
  uninhabited (OMOPThCreate _) impossible
  uninhabited (OMOPThMutexL _) impossible
  uninhabited (OMOSkipPThMutexL _) impossible
  uninhabited (OMOPThMutexU _) impossible
  uninhabited (OMOSkipPThMutexU _) impossible
  uninhabited (OMOPThJoinNull _) impossible
  uninhabited (OMOPThExitNull _) impossible
  uninhabited (OMOIf _ _ _ _) impossible
  uninhabited (OMOWhile _ _ _) impossible
  uninhabited (OMOBlock _) impossible
  uninhabited OMOStop impossible

implementation Uninhabited (OnlyMutexOpsExp (SlBlock x y)) where
  uninhabited (OMODecl _) impossible
  uninhabited (OMOPThCreate _) impossible
  uninhabited (OMOPThMutexL _) impossible
  uninhabited (OMOSkipPThMutexL _) impossible
  uninhabited (OMOPThMutexU _) impossible
  uninhabited (OMOSkipPThMutexU _) impossible
  uninhabited (OMOPThJoinNull _) impossible
  uninhabited (OMOPThExitNull _) impossible
  uninhabited (OMOIf _ _ _ _) impossible
  uninhabited (OMOWhile _ _ _) impossible
  uninhabited (OMOBlock _) impossible
  uninhabited OMOStop impossible

onlyMutexOpsExp : (sl : SlCExp m e) -> Dec (OnlyMutexOpsExp sl)
onlyMutexOpsExp (SlDecl k) =
  case onlyMutexOpsExp k of
    Yes omoK => Yes (OMODecl omoK)
    No contra => No (\(OMODecl k) => contra k)
onlyMutexOpsExp (SlPThCreate k) = No absurd
onlyMutexOpsExp (SlSkipPThCreate pf k) =
  case onlyMutexOpsExp k of
    Yes omoK => Yes (OMOPThCreate omoK)
    No contra => No (\(OMOPThCreate k) => contra k)
onlyMutexOpsExp (SlPThMutexL k) = 
  case onlyMutexOpsExp k of
    Yes omoK => Yes (OMOPThMutexL omoK)
    No contra => No (\(OMOPThMutexL k) => contra k)
onlyMutexOpsExp (SlSkipPThMutexL pf k) =
  case onlyMutexOpsExp k of
    Yes omoK => Yes (OMOSkipPThMutexL omoK)
    No contra => No (\(OMOSkipPThMutexL k) => contra k)
onlyMutexOpsExp (SlPThMutexU k) =
  case onlyMutexOpsExp k of
    Yes omoK => Yes (OMOPThMutexU omoK)
    No contra => No (\(OMOPThMutexU k) => contra k)
onlyMutexOpsExp (SlSkipPThMutexU pf k) =
  case onlyMutexOpsExp k of
    Yes omoK => Yes (OMOSkipPThMutexU omoK)
    No contra => No (\(OMOSkipPThMutexU k) => contra k)
onlyMutexOpsExp (SlPThJoinNull k) = No absurd
onlyMutexOpsExp (SlSkipPThJoinNull pf k) =
  case onlyMutexOpsExp k of
    Yes omoK => Yes (OMOPThJoinNull omoK)
    No contra => No (\(OMOPThJoinNull k) => contra k)
onlyMutexOpsExp (SlPThExitNull k) =
  case onlyMutexOpsExp k of
    Yes omoK => Yes (OMOPThExitNull omoK)
    No contra => No (\(OMOPThExitNull k) => contra k)
onlyMutexOpsExp (SlIf c tt ff k) =
  case onlyMutexOpsExp c of
    Yes omoC =>
      case onlyMutexOpsExp tt of
        Yes omoTT =>
          case onlyMutexOpsExp ff of
            Yes omoFF =>
              case onlyMutexOpsExp k of
                Yes omoK => Yes (OMOIf omoC omoTT omoFF omoK)
                No contraK => No (\(OMOIf c tt ff k) => contraK k)
            No contraFF => No (\(OMOIf c tt ff k) => contraFF ff)
        No contraTT => No (\(OMOIf c tt ff k) => contraTT tt)
    No contraC => No (\(OMOIf c tt ff k) => contraC c)
onlyMutexOpsExp (SlWhile c b k) =
  case onlyMutexOpsExp c of
    Yes omoC =>
      case onlyMutexOpsExp b of
        Yes omoB =>
          case onlyMutexOpsExp k of
            Yes omoK => Yes (OMOWhile omoC omoB omoK)
            No contraK => No (\(OMOWhile c b k) => contraK k)
        No contraTT => No (\(OMOWhile c b k) => contraTT b)
    No contraC => No (\(OMOWhile c b k) => contraC c)
onlyMutexOpsExp (SlBlock pf k) = No absurd
onlyMutexOpsExp (SlSkipBlock pf k) =
  case onlyMutexOpsExp k of
    Yes omoK => Yes (OMOBlock omoK)
    No contra => No (\(OMOBlock k) => contra k)
onlyMutexOpsExp SlStop = Yes OMOStop

||| We only want slices for our mutex variable that contain mutex operations. 
||| (Of course, this may already be the case depending on our definition of 
||| well-formedness, but let's worry about that later.)
|||
||| Implemented as a View on a slice s.t. we can only construct elements of the 
||| view when the slice is constructed using a subset of constructors. This is 
||| trivial for declarations but not for expressions.
data OnlyMutexOps : SlCDecl m p -> Type where
  OMOVarDec : OnlyMutexOps k -> OnlyMutexOps (SlVarDec k)
  OMOFunDec : OnlyMutexOpsExp e -> OnlyMutexOps k -> OnlyMutexOps (SlFunDec e k)
  OMOQuit : OnlyMutexOps SlQuit

onlyMutexOps : (sl : SlCDecl m p) -> Dec (OnlyMutexOps sl)
onlyMutexOps (SlVarDec k) =
  case onlyMutexOps k of
    Yes omoK => Yes (OMOVarDec omoK)
    No contra => No (\(OMOVarDec k) => contra k)
onlyMutexOps (SlFunDec e k) with (onlyMutexOps k)
  onlyMutexOps (SlFunDec e k) | Yes omoK with (onlyMutexOpsExp e)
    onlyMutexOps (SlFunDec e k) | Yes omoK | Yes omoE =
      Yes (OMOFunDec omoE omoK)
    onlyMutexOps (SlFunDec e k) | Yes omoK | No contra =
      No (\(OMOFunDec e k) => contra e)
  onlyMutexOps (SlFunDec e k) | No contra = No (\(OMOFunDec e k) => contra k)
onlyMutexOps SlQuit = Yes OMOQuit

-- ----------------------------------------------------------------------------
-- 4. Subtracting the Slice.
-- ----------------------------------------------------------------------------

subtractExp : (e : CExp) -> (sl : SlCExp m e) -> (omo : OnlyMutexOpsExp sl) -> CExp
subtractExp (Decl ty v x) (SlDecl k) (OMODecl y) = 
  Decl ty v (subtractExp x k y)
subtractExp (PThOp (CreateNull t f x) y) (SlSkipPThCreate pf k) (OMOPThCreate z) =
  PThOp (CreateNull t f x) (subtractExp y k z)
subtractExp (PThOp (MutexLock m) x) (SlPThMutexL k) (OMOPThMutexL y) =
  subtractExp x k y
subtractExp (PThOp (MutexLock v) x) (SlSkipPThMutexL pf k) (OMOSkipPThMutexL y) =
  PThOp (MutexLock v) (subtractExp x k y)
subtractExp (PThOp (MutexUnlock m) x) (SlPThMutexU k) (OMOPThMutexU y) =
  subtractExp x k y
subtractExp (PThOp (MutexUnlock v) x) (SlSkipPThMutexU pf k) (OMOSkipPThMutexU y) =
  PThOp (MutexUnlock v) (subtractExp x k y)
subtractExp (PThOp (JoinNull t) x) (SlSkipPThJoinNull pf k) (OMOPThJoinNull y) = 
  PThOp (JoinNull t) (subtractExp x k y)
subtractExp (PThOp ExitNull x) (SlPThExitNull k) (OMOPThExitNull y) =
  PThOp ExitNull (subtractExp x k y)
subtractExp (If x y z w) (SlIf c tt ff k) (OMOIf s t u v) =
  If (subtractExp x c s) (subtractExp y tt t) (subtractExp z ff u) (subtractExp w k v)
subtractExp (While x y z) (SlWhile c b k) (OMOWhile w s t) =
  While (subtractExp x c w) (subtractExp y b s) (subtractExp z k t)
subtractExp (Block varsUsed x) (SlSkipBlock pf k) (OMOBlock y) =
  Block varsUsed (subtractExp x k y)
subtractExp Stop SlStop OMOStop = Stop

subtract : (p : CDecl) -> (sl : SlCDecl m p) -> (omo : OnlyMutexOps sl) -> CDecl
subtract (VarDec ty v e x) (SlVarDec k) (OMOVarDec y) = VarDec ty v e (subtract x k y)
subtract (FunDec ty f xs e x) (SlFunDec y k) (OMOFunDec z w) =
  FunDec ty f xs (subtractExp e y z) (subtract x k w)
subtract Quit SlQuit OMOQuit = Quit

-- ----------------------------------------------------------------------------
-- A1. Equivalence of *p* and *p'*.
-- ----------------------------------------------------------------------------
