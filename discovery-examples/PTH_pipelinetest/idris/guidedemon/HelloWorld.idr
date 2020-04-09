module HelloWorld

import public CppDSL

%default total

-- Can be generated using, e.g., clang.

hWIncludes : CInclude "helloworld.cpp" ((1,1),(1,20))
hWIncludes = Incl "iostream" ((1,1),(1,20)) $ Inhibeo ((1,0),(1,20))

mainBody : CExp "helloworld.cpp" ((4,5), (5,14))
mainBody =
  App "std::cout" (Str "Hello, World!" ((4,18), (4,33)) (Stop ((4,18), (4,33)))) ((4,5),(4,34)) $
  Ret (Num 0 ((5,12),(5,13)) (Stop ((5,12),(5,13)))) ((5,5),(5,14)) $
  Stop ((4,5), (5,14))

hWDeclarations : CDecl "helloworld.cpp" ((3,1),(6,2))
hWDeclarations =
  GFun CInt "main" mainBody ((3,1),(6,2)) $ Durdurmak ((3,1),(6,2))

helloWorld : CFile "helloworld.cpp"
helloWorld = CFn hWIncludes hWDeclarations