module MergeIter

import public CppDSL

%default total
%access public export

||| View/proof that *e* is two adjacent Iters.
data AdjacentIters : (e : CExp) -> Type where
  AdjIter : AdjacentIters (Iter i1 z1 c1 b1 (Iter i2 z2 c2 b2 k))

||| View/relation that makes a disjunction *c3* of two boolean expressions *c1*,
||| *c2*. This should probably be made a little more expressive. At the moment 
||| it doesn't guarantee that c3 is actually derived from *c1* and *c2*, nor 
||| does it guarantee that *c1* and *c2* are boolean expressions.
data Disjunction : (c1 : CExp) -> (c2 : CExp) -> (c3 : CExp) -> Type where
  Disj : (c1 : CExp) -> (c2 : CExp) -> Disjunction c1 c2 (Disj c1 c2 Stop)

||| Covering function of Disjunction.
disjunction : (c1 : CExp) -> (c2 : CExp) -> (c3 ** Disjunction c1 c2 c3)
disjunction c1 c2 = (Disj c1 c2 Stop ** Disj c1 c2) 

||| View/relation that *e'* is *e* but with two iterations merged.
data MergedIters : (e : CExp) -> (e' : CExp) -> Type where
  MgdIter : Disjunction c1 c2 c3 -> MergedIters (Iter i1 z1 c1 b1 (Iter i2 z2 c2 b2 k)) (Iter (i1 ++ i2) (z1 ++ z2) c3 (If c1 b1 Stop (If c2 b2 Stop Stop)) k)

||| Merges two iterations in the model.
|||
||| The two iterations must be adjacent. (Iterations may be *made* adjacent by 
||| using the reordering rewriting, assuming commutativity over composition of 
||| expressions.) Assume for simplicity that *e* is the first iteration, and 
||| its continuation immediately contains the second iteration. The merged 
||| condition will be the disjunction of both conditions, and merged body will 
||| contain the bodies of the originals, but wrapped in if-expressions with 
||| their original condition as the if-condition.
|||
||| This rewriting preserves functional correctness. **TODO**: Proof based on 
||| semantics.
|||
||| Could be generalised to *n* iterations.
mergeIter : (e : CExp) -> {prf : AdjacentIters e} -> (e' ** MergedIters e e')
mergeIter (Iter i1 z1 c1 b1 (Iter i2 z2 c2 b2 k)) {prf = AdjIter} =
  let (c3 ** prf) = disjunction c1 c2
  in (Iter (i1 ++ i2) (z1 ++ z2) c3 (If c1 b1 Stop (If c2 b2 Stop Stop)) k ** MgdIter prf)
