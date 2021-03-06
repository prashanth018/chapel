import OsetTypeTest;
use OsetTest;


type t = (shared testClass?, shared testClass?);

operator testClass.<(lhs: shared testClass?, rhs: shared testClass?) {
  if lhs == nil && rhs == nil then return false;
  if lhs == nil then return true;
  if rhs == nil then return false;
  return lhs!.dummy < rhs!.dummy;
}

operator testClass.>(lhs: shared testClass?, rhs: shared testClass?) {
  if lhs == nil && rhs == nil then return false;
  if lhs == nil then return false;
  if rhs == nil then return true;
  return lhs!.dummy > rhs!.dummy;
}

OsetTypeTest.testSet(t);
