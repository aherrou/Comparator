import("stdfaust.lib");

// process = +(1) ~ _ : % (ns) : / (ns) : sin <: _,_
// with {
//   ns = 64;
// };
// // process = +(2) ~ _ : cos;
// process = +(0.125) ~ _;
// process = +(0.125) ~ _;
// process = _ ~ +(0.125); 
// process = 1@44;
// process = +(0.125) ~ _ : %(1) ;//: / (2); // : exp;
// process = +(0.0625) ~ _ %(10) : sin;
// process = +(1) ~ _;

// process = (+ (1) ~ _);// : int : /(2);

process = proc_out <: _,_
with
{
  mySine(n) = _ ~ +(1.0) : int : /(n) : sin;
  mySaw(n) = _ ~ +(1.0): int : /(n) : ma.frac;
  // proc_out = mySine(mySine(512)*2+16);
  // proc_out = mySaw(32);
  // proc_out = mySine(8);
  // proc_out = mySine(2) + mySine(4) + mySine(8) + mySine(16) + mySine(32) + mySine(64);
  proc_out = mySine(8); // : atan;
};

// process = // (+(1/128))
 //  + (1) ~ _: int : /(32);
  // ~ _ : %(128)
/* /(32)
  // ~ ma.frac
  // : * (2*ma.PI)
  : sin;*/
