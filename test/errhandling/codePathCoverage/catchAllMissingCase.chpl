config const coinFlip = true;

class GoofyError: Error {
}

proc foo(): int throws {
  if (coinFlip) {
    return 42;
  } else {
    throw new owned Error();
  }
}

proc bar() throws {
  try {
    return foo();
  } catch e: GoofyError {
    throw e;
  } catch {
    if coinFlip then
      throw new owned GoofyError();
  }
}
  

var x = bar();
writeln(x);
