module CppDSL

%access public export
%default total

{-
  A (very small) demonstration that we can model C++ and then emit it once it 
  has, for example, been modified.

  Think HaRe in terms of implementation.

  This is designed to show that we can, in principle, use Idris to define and 
  refactor C++ (or a small part of it). This could be the beginning of a 
  verified refactoring tool (in Idris), or the Idris refactorings could be used 
  as just the definitions for the refactorings that are then implemented in 
  e.g. Clang.
-}

data CTy : Type where
  CBool : CTy
  CInt  : CTy
  CVoid : CTy

||| ((line start, column start), (line end, column end))
Loc : Type
Loc = ((Nat, Nat), (Nat, Nat))

data CExp  : (fname : String) -> (loc : Loc) -> Type where
  Num  : (n : Int) -> (l : Loc) -> (k : CExp fn lK) -> CExp fn lK
  Str  : (s : String) -> (l : Loc) -> (k : CExp fn lK) -> CExp fn lK
  Var  : (v : String) -> (l : Loc) -> (k : CExp fn lK) -> CExp fn lK
  App  : (f : String) -> (xs : CExp fn lXS) -> (l : Loc) -> (k : CExp fn lK) -> CExp fn lK
  Ret  : (e : CExp fn lE) -> (l : Loc) -> (k : CExp fn lK) -> CExp fn lK
  Stop : (l : Loc) -> CExp fn l

data CDecl : (fname : String) -> (loc : Loc) -> Type where
  GFun : (ty : CTy) -> (f : String) -> (e : CExp fn lE) -> (l : Loc) -> (k : CDecl fn lK) -> CDecl fn l
  Durdurmak : (l : Loc) -> CDecl fn l

data CInclude : (fname : String) -> (loc : Loc) -> Type where
  Incl : (h : String) -> (l : Loc) -> (k : CInclude fn lK) -> CInclude fn l
  Inhibeo : (loc : Loc) -> CInclude fn loc

data CFile : (fname : String) -> Type where
  CFn : (incls : CInclude fn lI) -> (decls: CDecl fn lD) -> CFile fn

implementation Show CTy where
  show CBool = "bool"
  show CInt  = "int"
  show CVoid = "void"

printCExp : (CExp fn l) -> String
printCExp (Num n l k) = (show n) ++ (printCExp k)
printCExp (Str s l k) = s ++ (printCExp k)
printCExp (Var v l k) = v ++ (printCExp k)
printCExp (App f xs l k) =
  if f == "std::cout"
    then "std::cout << " ++ (printCExp xs) ++ ";\n"
    else f ++ "(" ++ (printCExp xs) ++ ");" 
printCExp (Ret e l k) = "return " ++ (printCExp e) ++ ";\n"
printCExp (Stop l) = ""

printCDecls : (CDecl fn l) -> String
printCDecls (GFun ty f e l k) =
  (show ty) ++ " " ++ f ++ "() {\n" ++ (printCExp e) ++ "\n}\n" ++ (printCDecls k)
printCDecls (Durdurmak l) = ""

printCIncludes : (CInclude fn l) -> String
printCIncludes (Incl h l k) = "#include <" ++ h ++ ">\n" ++ (printCIncludes k)
printCIncludes (Inhibeo l) = ""

printCFile : (CFile fn) -> String
printCFile (CFn incls decls) =
  (printCIncludes incls) ++ "\n" ++ (printCDecls decls)
