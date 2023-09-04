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

myAR(at,rt,gate) = AR : max(0)
with {
  // hardcoded sampling rate to work around the inconvenient behaviour of foreign constant
  mySR = 44100;

    // Durations in samples
    an = max(1, at*mySR);
    rn = max(1, rt*mySR);

    // Deltas per samples
    adelta = 1/an;
    rdelta = 1/rn;

    // Attack time (starts at gate upfront and raises infinitely)
    atime = (raise*reset + upfront) ~ _
    with {
        upfront = gate > gate';
        reset = gate <= gate';
        raise(x) = (x + (x > 0));
    };

    // Attack curve
    A = atime * adelta;

    // Release curve
    D0 = 1 + an * rdelta;
    D = D0 - atime * rdelta;

    // AR part
    AR = min(A, D);

};


process = proc_out <: _,_
with
{
  mySine(n) =  +(1.0) ~ _ : int : /(n) : sin;
  mySaw(n) = +(1.0) ~ _ : int : /(n); // : ma.frac;
  // proc_out = mySine(mySine(512)*2+16);
  // proc_out = mySaw(32);
  proc_out = mySine(20) * myAR(.25, 2*.375, 1 - 1');
  // proc_out = mySine(20);
  // proc_out = mySine(2) + mySine(4) + mySine(8) + mySine(16) + mySine(32) + mySine(64);
  // proc_out = mySine(div); // : atan;
  // div = hslider("div", 16, 1, 64, 1);
};

// process = // (+(1/128))
 //  + (1) ~ _: int : /(32);
  // ~ _ : %(128)
/* /(32)
  // ~ ma.frac
  // : * (2*ma.PI)
  : sin;*/
