use MasonTest;

proc main() {
  const args = ["mason", "test" , "1", "3", "--no-update", "--no-checksum"];
  masonTest(args);
}
