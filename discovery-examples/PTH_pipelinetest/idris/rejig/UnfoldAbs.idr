module UnfoldAbs

import public CppDSL

%default total
%access public export

||| View/proof that *e* is a function application expression that calls the 
||| function declaration, *f*.
data FunDecAppl : (f : CDecl) -> (e : CExp) -> Type where
  IsFunDecAppl : FunDecAppl (FunDec ty f xs e k) (Appl f ys k')

||| Relation s.t. *e'* is *e* where all *xs* have been substituted for *ys*.
data SubstVars : (e : CExp) -> (xs : ArgList) -> (ys : List String) -> (e' : CExp) -> Type where

||| Relation s.t. *e'* is *e* where the final 'Stop' expression has been 
||| replaced with *k*.
data SubstStop : (e : CExp) -> (k : CExp) -> (e' : CExp) -> Type where

||| Relation stating that *e'* is the result of the unfolded function call *e*.
data UnfoldedAbs : (f : CDecl) -> (e : CExp) -> (e' : CExp) -> Type where
  IsUnfoldedAbs : SubstVars b xs ys b' -> SubstStop b' k' b'' -> UnfoldedAbs (FunDec ty f xs b k) (Appl f ys k') b''

||| Unfolds a function application expression.
|||
||| *f* must be a non-recursive function definition, *e* must be a function 
||| application expression, and *f* must be the function being applied in *e*.
||| The result is the body of *f*, such that the arguments of *f* are 
||| substituted for the variables passed to *f* in *e*, and the continuation of 
||| *e* is substituted for the end of the continuation in *f*.
unfoldAbs : (f : CDecl) -> (e : CExp) -> {prf : FunDecAppl f e} -> CExp
unfoldAbs (FunDec ty f xs e k) (Appl f ys k') {prf = IsFunDecAppl} =
  ?holeHere
