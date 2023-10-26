import("stdfaust.lib");

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

mySine(n) =  +(0.1) ~ _ : /(2*ma.PI*n): sin;
// mySine(n) = +(1/(2*ma.PI*n)) ~ _ : sin;
mySaw(n) = +(1.0) ~ _ : int : /(n); // : ma.frac;

process = mySine(20) * myAR(.25, 2*.375, 1 - 1');
//process = mySaw(20) * myAR(.25, 2*.375, 1 - 1');
