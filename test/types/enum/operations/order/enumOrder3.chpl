enum color { red=42, green=22, blue=32};
config const negOne = -1;
proc test(c: color) {
  const order = chpl__enumToOrder(c);
  writeln(c, " = ", order, " = ", chpl__orderToEnum(order, color));
}

proc test(param c: color, param val: int) {
  if (chpl__enumToOrder(c) != val) then
    compilerError("Shouldn't get here");
  writeln("Param test passed");
  test(c);
}

test(color.red, 0);
test(color.green, 1);
test(color.blue, 2);

// try an illegal case:
writeln(chpl__orderToEnum(negOne, color));
