use CGMakeA;

enum classVals {S, W, A, B, C, D, O};


config const probClass = classVals.S;

const Class = {classVals.S..classVals.O};
const probSizes:   [Class] int = [ 1400, 7000, 14000, 75000, 150000, 150000, 1400 ],
      nonZeroes:   [Class] int = [ 7, 8, 11, 13, 15, 21, 7 ],
      shifts:      [Class] int = [ 10, 12, 20, 60, 110, 500, 10 ],
      numIters:    [Class] int = [ 15, 15, 15, 75, 75, 100, 15 ],
      verifyZetas: [Class] real = [ 8.5971775078648, 10.362595087124,
                                    17.130235054029, 22.712745482631,
                                    28.973605592845, 52.5145321058, 0.0 ];

config const n = probSizes(probClass),
             rcond = 0.1,
             nonzer = nonZeroes(probClass),
             shift = shifts(probClass);


type elemType = real(64);


proc main() {
  writeln("Chapel sparsity pattern is:");
  for ((r,c), v) in makea(elemType, n, nonzer, shift) {
    writeln(" ", r, " ", c, "  ", v);
  }
}
