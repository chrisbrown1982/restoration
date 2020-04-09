module Reorder

import public CppDSL

%default total
%access public export

{-
||| Relation s.t. *e1;e2* and *e2;e1* are functionally equivalent.
data Commutative : (e1 : CExp) -> (e2 : CExp) -> Type where

||| Relation s.t. *e'' = e2;e1;k*, where *e1*, *e2*, and *k* originally form 
||| the continuation *e1;e2;k*, and *e1* and *e2* are commutative.
data Reordered : (e1 : CExp) -> (e2 : CExp) -> (k : CExp) -> (e : CExp) -> Type where
  IsReordered : (length e1 = 2) -> (length e2 = 2) -> Commutative e1 e2 -> Reordered e1 e2 k (e1 ++ e2 ++ k)

||| Swaps the order of the first two continuations.
|||
||| *e* must be a continuation of at least length 3. The first two 
||| continuations must be commutative; i.e. e_1;e_2 must be functionally 
||| equivalent to *e_2;e_1* (using standard statement composition here for 
||| illustrative purposes).
reorder : (e1 : CExp) -> (e2 : CExp) -> (k : CExp) -> {prfLen1 : (length e1 = 2)} -> {prfLen2 : (length e1 = 2)} -> {prfComm : Commutative e1 e2} -> (e ** Reordered e1 e2 k e)
-}

-- make a split relation on an original e s.t.:
-- reorder : (e : CExp) -> {split : SplitE e1 e2 k} -> {prfComm : Commutative e1 e2} -> (e' ** Reordered e1 e2 k e')
-- propagate this idea to Reordered?

||| *v* does not occur in *e* where *e* is a singleton expression.
data NotUsed : (v : String) -> (e : CExp) -> Type where

||| Relation s.t. *e1;e2* and *e2;e1* are functionally equivalent.
||| Assume *e1* and *e2* both have length 1 (for simplicity, can extend later).
data Commutative : (e1 : CExp) -> (e2 : CExp) -> Type where
  Decl : {ok : Singleton e2} -> NotUsed v e2 -> Commutative (Decl ty v Stop) e2
  Assn : {ok : Singleton e2} -> NotUsed v e2 -> Commutative (Assn v e Stop) e2
  Disj : {ok : Singleton e2} -> NotAssnd a 

||| Relation s.t. *e1* is the first continuation of *e*, *e2* is the second 
||| continuation of *e*, and *k* is the third continuation of *e*. *e* must be 
||| at least length 2. *e1* and *e2* are both of length 1.
data SplitCExp : (e : CExp) -> (e1 : CExp) -> (e2 : CExp) -> (k : CExp) -> Type where
  IsSplitCExp : {ok : GTE (length e) 2} -> SplitCExp e (head e) (head (head e)) (tail (tail e))

||| Relation s.t. the first two continuations of *e* are swapped in *e'*.
data Reordered : (e : CExp) -> (e' : CExp) -> Type where
  IsReordered : SplitCExp e e1 e2 k -> Commutative e1 e2 -> Reordered e (e2 ++ e1 ++ k)

||| Swaps the order of the first two continuations in *e*, producing *e'*.
|||
||| *e* must be a continuation of at least length 2, of which the first two 
||| continuations must be commutative (i.e. *e1;e2* must be functionally 
||| equivalent to *e2;e1*).
reorder : (e : CExp) -> {split : SplitCExp e e1 e2 k} -> {prf : Commutative e1 e2} -> (e' ** Reordered e e')