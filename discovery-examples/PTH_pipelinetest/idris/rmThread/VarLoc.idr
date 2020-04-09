module RemoveCreateJoin

import public CppDSL

%access public export
%default total

-- -----------------------------------------------------------------------------
-- 1. A Variable Declaration.
-- -----------------------------------------------------------------------------

||| Attempt at making MutLoc from RemoveMutex more general. Proof that a 
||| variable *v* with type *ty* is defined in a program *p*.
||| 
||| Assume that *v* is declared as a global variable. This is sane for our 
||| particular purposes (*pthread_t* and *pthread_mutex_t*) since this is most 
||| common (and simplest). This can either be extended to allow local variables 
||| and/or we can assume that any local variables have been made global.
|||
||| Assume that *CDecl* is defined s.t. it carries proofs that names are unique.
||| This **isn't** yet the case, but it can be defined as a relation/view on a 
||| *CDecl*.
|||
||| Since we are assuming that variable names are unique, we don't state the 
||| type of *v* here. This simplifies matters.
data VarLoc : (v : String) -> (p : CDecl) -> Type where
  HereVarDec  : VarLoc v (VarDec ty v e k)
  ThereVarDec : Not (v = w) -> VarLoc v k -> VarLoc v (VarDec ty w e k)
  ThereFunDec : VarLoc v k -> VarLoc v (FunDec ty f xs e k)

implementation Uninhabited (VarLoc v Quit) where
  uninhabited HereVarDec impossible
  uninhabited ThereVarDec impossible
  uninhabited ThereFunDec impossible

lemmaVarLocNeitherHereNorThere : (notHere : (v = w) -> Void) -> (notThere : VarLoc v k -> Void) -> VarLoc v (VarDec ty w e k) -> Void
lemmaVarLocNeitherHereNorThere notHere notThere HereVarDec = notHere Refl
lemmaVarLocNeitherHereNorThere notHere notThere (ThereVarDec contra prfK) =
  notThere prfK

||| Decision procedure for VarLoc.
varLoc : (v : String) -> (p : CDecl) -> Dec (VarLoc v p)
varLoc v (VarDec ty w e k) with (decEq v w)
  varLoc w (VarDec ty w e k) | (Yes Refl) = Yes HereVarDec
  varLoc v (VarDec ty w e k) | (No contra) with (varLoc v k)
    varLoc v (VarDec ty w e k) | (No contra) | (Yes inK) =
      Yes (ThereVarDec contra inK)
    varLoc v (VarDec ty w e k) | (No notHere) | (No notThere) =
      No (lemmaVarLocNeitherHereNorThere notHere notThere)
varLoc v (FunDec x f xs e k) with (varLoc v k)
  varLoc v (FunDec x f xs e k) | (Yes inK) = Yes (ThereFunDec inK)
  varLoc v (FunDec x f xs e k) | (No contra) =
    No (\(ThereFunDec k) => contra k)
varLoc v Quit = No absurd
