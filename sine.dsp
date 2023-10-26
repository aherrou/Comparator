import("stdfaust.lib");

mySine(n) =  +(0.1) ~ _ : /(2*ma.PI*n): sin;
// mySine(n) = +(1/(2*ma.PI*n)) ~ _ : sin;
mySaw(n) = +(1.0) ~ _ : int : /(n); // : ma.frac;

process = proc_out <: _,_
with
{
  proc_out = (mySine(0.25));
  // proc_out = mySine(mySine(512)*2+16);
  // proc_out = mySaw(32);
      
  // proc_out = mySine(div); 
  // div = hslider("div", 16, 1, 64, 1);
};
