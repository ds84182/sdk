// Copyright (c) 2018, the Dart project authors.  Please see the AUTHORS file
// for details. All rights reserved. Use of this source code is governed by a
// BSD-style license that can be found in the LICENSE file.

/*strong.element: f:deps=[B],direct,explicit=[f.T],needsArgs,needsInst=[<B.S>]*/
/*omit.element: f:deps=[B]*/
int f<T>(T a) => null;

typedef int F<R>(R a);

/*strong.class: B:direct,explicit=[int Function(B.S)],needsArgs*/
class B<S> {
  F<S> c;

  method() {
    return () {
      c = f;
    };
  }
}

main() {
  new B().method();
}